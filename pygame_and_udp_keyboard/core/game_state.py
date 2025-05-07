import pygame
from .config import WIDTH, HEIGHT

def reset_game():
    return {
        "player": pygame.Rect(WIDTH // 2, HEIGHT // 2, 40, 40),
        "color": (0, 150, 255),
        "bullets": [],
        "explosions": [],
        "enemies": [],
        "enemy_timer": 0,
        "movement": None,
        "score": 0,
        "gameover": False
    }

def spawn_enemy():
    import random
    y = random.randint(0, HEIGHT - 60)
    return pygame.Rect(WIDTH, y, 60, 60)


def create_particles(x, y):
    import math, random
    return [{
        "x": x,
        "y": y,
        "dx": math.cos(a) * random.uniform(2, 5),
        "dy": math.sin(a) * random.uniform(2, 5),
        "life": 15
    } for a in [i * math.pi / 8 for i in range(16)]]