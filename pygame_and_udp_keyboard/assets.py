# assets.py
import os
import pygame
from core.config import WIDTH, HEIGHT

ASSET_DIR = os.path.join(os.path.dirname(__file__), "assets")

def load_assets():
    background_img = pygame.transform.scale(
        pygame.image.load(os.path.join(ASSET_DIR, "bg.png")).convert(),
        (WIDTH, HEIGHT)
    )
    player_img = pygame.transform.scale(
        pygame.image.load(os.path.join(ASSET_DIR, "airplane.png")).convert_alpha(),
        (60, 40)
    )
    enemy_img = pygame.transform.scale(
        pygame.image.load(os.path.join(ASSET_DIR, "enemy.png")).convert_alpha(),
        (60, 60)
    )
    fire_img = pygame.transform.scale(
        pygame.image.load(os.path.join(ASSET_DIR, "fire.png")).convert_alpha(),
        (60, 20)
    )
    return background_img, player_img, enemy_img, fire_img
