import socket
import threading
import time
from pynput.keyboard import Controller as KeyboardController, Key
from pynput.mouse import Controller as MouseController, Button

# ================= CONFIG =================

DISCOVERY_PORT = 4211
COMMAND_PORT = 4210

DEBOUNCE_DELAY = 0.2
MOUSE_SENSITIVITY = 0.25
MOUSE_DEADZONE = 5.0

# ==========================================

keyboard = KeyboardController()
mouse = MouseController()

last_press = {}

held_keys = {
    'w': False,
    'a': False,
    's': False,
    'd': False
}

mouse_acc = {"x": 0.0, "y": 0.0}


# ================= DISCOVERY SERVER =================

def discovery_server():

    sock = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)
    sock.bind(('', DISCOVERY_PORT))

    print(f"[DISCOVERY] Escutando porta {DISCOVERY_PORT}")

    while True:

        data, addr = sock.recvfrom(1024)
        msg = data.decode().strip()

        if msg == "who-is-pc":
            print(f"[DISCOVERY] Pedido de {addr}")
            sock.sendto(b"esp32-discovery", addr)


# ================= UDP SERVER =================

def udp_server():

    sock = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)
    sock.bind(('', COMMAND_PORT))

    print(f"[UDP] Escutando comandos na porta {COMMAND_PORT}")

    while True:

        data, addr = sock.recvfrom(1024)
        msg = data.decode().strip()

        handle_message(msg)


# ================= INPUT HANDLERS =================

def update_key_state(key, hold):

    if hold and not held_keys[key]:
        keyboard.press(key)
        held_keys[key] = True

    elif not hold and held_keys[key]:
        keyboard.release(key)
        held_keys[key] = False


def handle_mouse_move(dx, dy):

    mouse_acc["x"] += dx * MOUSE_SENSITIVITY
    mouse_acc["y"] += dy * MOUSE_SENSITIVITY

    move_x = int(mouse_acc["x"])
    move_y = int(mouse_acc["y"])

    if abs(move_x) > MOUSE_DEADZONE or abs(move_y) > MOUSE_DEADZONE:

        mouse.move(move_x, move_y)

        mouse_acc["x"] -= move_x
        mouse_acc["y"] -= move_y


def handle_analog(msg):

    try:

        parts = msg.split(",")

        joystick = parts[0].split(":")[1]
        dx = float(parts[1].split(":")[1])
        dy = float(parts[2].split(":")[1])

        print(f"[ANALOG] {joystick} dx:{dx} dy:{dy}")

        if joystick == "left":

            threshold = 30

            update_key_state("a", dx < -threshold)
            update_key_state("d", dx > threshold)

            update_key_state("w", dy < -threshold)
            update_key_state("s", dy > threshold)

        elif joystick == "right":

            handle_mouse_move(dx, dy)

    except Exception as e:
        print("Erro analog:", e)


def handle_button(cmd):

    now = time.time()

    if cmd not in last_press or now - last_press[cmd] > DEBOUNCE_DELAY:

        print("[BTN]", cmd)

        if cmd == "p":
            keyboard.press(Key.esc)
            keyboard.release(Key.esc)

        elif cmd == "s":
            keyboard.press(Key.space)
            keyboard.release(Key.space)

        elif cmd == "f":
            mouse.click(Button.left)

        last_press[cmd] = now


def handle_message(msg):

    msg = msg.lower()

    if msg.startswith("x:"):
        handle_analog(msg)
    else:
        for c in msg:
            handle_button(c)


# ================= MAIN =================

print("Gamepad UDP Server iniciado")

threading.Thread(target=discovery_server, daemon=True).start()
threading.Thread(target=udp_server, daemon=True).start()

while True:
    time.sleep(1)