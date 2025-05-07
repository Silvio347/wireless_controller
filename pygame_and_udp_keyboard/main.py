import pygame
from core.config import WIDTH, HEIGHT
from core.udp import receive_udp, sock
from core.game_state import reset_game, spawn_enemy, create_particles
from assets import load_assets
from ui.ui import wait_udp_message
from core.discovery import start_discovery_server

pygame.init()
start_discovery_server()
screen = pygame.display.set_mode((WIDTH, HEIGHT))
pygame.display.set_caption("\U0001F3AE Wireless Controller ESP32")
clock = pygame.time.Clock()
font = pygame.font.SysFont(None, 36)

background_img, player_img, enemy_img, fire_img = load_assets()

wait_udp_message(screen, font, "Press any button to start", sock)
state = reset_game()
paused = False

running = True
while running:
    screen.blit(background_img, (0, 0))

    msg = receive_udp()

    # Reset movement cada frame
    state["movement"] = None

    if not msg:
        pass
    elif msg.startswith("x:"):
        try:
            data = msg.split(',')
            dx = float(data[1].split(':')[1])
            dy = float(data[2].split(':')[1])

            movement = []
            if dx < -20:
                movement.append('a')
            elif dx > 20:
                movement.append('d')

            if dy < -20:
                movement.append('w')
            elif dy > 20:
                movement.append('s')

            # Concatena os movimentos (por ex: ['w', 'a'] -> 'wa')
            state["movement"] = ''.join(movement) if movement else None

        except Exception as e:
            print(f"[ERRO] Analógico inválido: {msg} -> {e}")
    else:
        for char in msg:
            if char in ['w', 'a', 's', 'd'] and not paused:
                state["movement"] = char
            if char == 'f' and not paused:
                bullet_rect = pygame.Rect(state["player"].x + 40, state["player"].y + 15, 20, 20)
                state["bullets"].append({"rect": bullet_rect, "img": fire_img})
            if char == 'p':
                paused = not paused


    if paused:
        pause_text = font.render("Paused - press 'p' or 'start' to continue", True, (255, 255, 255))
        screen.blit(pause_text, pause_text.get_rect(center=(WIDTH // 2, HEIGHT // 2)))
        pygame.display.flip()
        clock.tick(60)
        continue

    screen.blit(player_img, state["player"])
    score_text = font.render(f"Pontos: {state['score']}", True, (255, 255, 255))
    screen.blit(score_text, (10, 10))

    for b in state["bullets"][:]:
        b["rect"].x += 10
        screen.blit(b["img"], b["rect"])
        if b["rect"].x > WIDTH:
            state["bullets"].remove(b)

    enemy_speed = 3 + (state["score"] // 10)
    for e in state["enemies"][:]:
        e.x -= enemy_speed
        screen.blit(enemy_img, e)

    for e in state["enemies"][:]:
        hit = False
        for b in state["bullets"][:]:
            if e.colliderect(b["rect"]):
                state["explosions"].extend(create_particles(e.centerx, e.centery))
                state["bullets"].remove(b)
                hit = True
                state["score"] += 1
                break
        if hit and e in state["enemies"]:
            state["enemies"].remove(e)
        elif state["player"].colliderect(e):
            state["gameover"] = True
            if e in state["enemies"]:
                state["enemies"].remove(e)

    new_list = []
    for p in state["explosions"]:
        pygame.draw.circle(screen, (255, 80, 80), (int(p["x"]), int(p["y"])), 4)
        p["x"] += p["dx"]
        p["y"] += p["dy"]
        p["life"] -= 1
        if p["life"] > 0:
            new_list.append(p)
    state["explosions"] = new_list

    p = state["player"]
    m = state["movement"] or ''
    if 'w' in m and p.top > 0:
        p.y -= 5
    if 's' in m and p.bottom < HEIGHT:
        p.y += 5
    if 'a' in m and p.left > 0:
        p.x -= 5
    if 'd' in m and p.right < WIDTH:
        p.x += 5
    if m == 'w' and p.top > 0:
        p.y -= 5
    elif m == 's' and p.bottom < HEIGHT:
        p.y += 5
    elif m == 'a' and p.left > 0:
        p.x -= 5
    elif m == 'd' and p.right < WIDTH:
        p.x += 5

    state["enemy_timer"] += 1
    if state["enemy_timer"] >= 60:
        state["enemies"].append(spawn_enemy())
        state["enemy_timer"] = 0

    if state["gameover"]:
        wait_udp_message(screen, font, f"GAME OVER - Points: {state['score']} - Press any button to restart", sock)
        state = reset_game()

    pygame.display.flip()
    clock.tick(60)

pygame.quit()