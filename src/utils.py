import pygame
from src.config import *

def grid_to_pixel(pos_grid, is_rect):
    cell_width = SCREEN_WIDTH // 3
    cell_height = SCREEN_HEIGHT // 3
    if is_rect:
        pos_pixel = pygame.Vector2((pos_grid.y * cell_width + cell_width // 2) - RECT_WIDTH/2, (SCREEN_HEIGHT - (pos_grid.x * cell_height + cell_height // 2)) - RECT_HEIGHT/2)
    else:
        pos_pixel = pygame.Vector2(pos_grid.y * cell_height + cell_height // 2 - 48, SCREEN_HEIGHT - (pos_grid.x * cell_width + cell_width // 2 + 100))

    return pos_pixel
