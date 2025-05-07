import time
from pynput.keyboard import Controller as KeyboardController, Key
from pynput.mouse import Controller as MouseController, Button
from core.udp import receive_udp  # usando seu módulo udp.py
from core.discovery import start_discovery_server

# === Setup Input Controllers ===
keyboard = KeyboardController()
mouse = MouseController()
last_press = {}
DEBOUNCE_DELAY = 0.2  # segundos

# Estado das teclas analógicas
held_keys = {
    'w': False,
    'a': False,
    's': False,
    'd': False
}

# Acúmulo para suavizar movimento do mouse
mouse_accumulator = {'x': 0.0, 'y': 0.0}
MOUSE_SENSITIVITY = 0.25
MOUSE_DEADZONE = 5.0  # ignora movimentos muito pequenos

# Inicia o servidor de descoberta
start_discovery_server()

# === Funções utilitárias ===

# Atualiza o estado de uma tecla
def update_key_state(key, should_hold):
    if should_hold and not held_keys[key]:
        keyboard.press(key)
        held_keys[key] = True
        # print(f"[INFO] Tecla '{key}' PRESSIONADA")
    elif not should_hold and held_keys[key]:
        keyboard.release(key)
        held_keys[key] = False
        # print(f"[INFO] Tecla '{key}' LIBERADA")

# Função para mover o mouse com suavização
def handle_mouse_move(dx, dy):
    try:
        mouse_accumulator['x'] += dx * MOUSE_SENSITIVITY
        mouse_accumulator['y'] += dy * MOUSE_SENSITIVITY

        move_x = int(mouse_accumulator['x'])
        move_y = int(mouse_accumulator['y'])

        if abs(move_x) > MOUSE_DEADZONE or abs(move_y) > MOUSE_DEADZONE:
            mouse.move(move_x, move_y)
            mouse_accumulator['x'] -= move_x
            mouse_accumulator['y'] -= move_y
            # print(f"[INFO] Mouse movido: {move_x}, {move_y}")

    except Exception as e:
        print(f"[ERRO] Falha ao mover o mouse: {e}")

# Função para lidar com os comandos do joystick analógico
def handle_analog_command(msg, joystick):
    try:
        data = msg.split(',')
        dx = float(data[1].split(':')[1])
        dy = float(data[2].split(':')[1])
        print(f"[INFO] joystick - {joystick} dx: {dx}, dy: {dy}")

        if joystick == 'left':
            threshold = 30
            update_key_state('a', dx < -threshold)
            update_key_state('d', dx > threshold)
            update_key_state('w', dy < -threshold)
            update_key_state('s', dy > threshold)
        elif joystick == 'right':
            handle_mouse_move(dx, dy)

    except Exception as e:
        print(f"[ERRO] Falha ao interpretar analógico: {e}")

# Função para lidar com os comandos de pressionamento de teclas
def handle_command(cmd):
    now = time.time()
    print(f"[INFO] Comando recebido: {cmd}")
    if cmd not in last_press or (now - last_press[cmd]) > DEBOUNCE_DELAY:
        if cmd == 'p':
            keyboard.press(Key.esc)
            keyboard.release(Key.esc)
        if cmd == 's':
            keyboard.press(Key.space)
            keyboard.release(Key.space)
        elif cmd == 'f':  
            mouse.click(Button.left)
        last_press[cmd] = now

# === Loop Principal ===
print("Aguardando comandos UDP do ESP32...")

while True:
    msg = receive_udp()

    if not msg:
        continue

    if msg.startswith("x:"):
        if "left" in msg:
            handle_analog_command(msg, 'left')
        elif "right" in msg:
            handle_analog_command(msg, 'right')
    else:
        for c in msg:
            handle_command(c)
