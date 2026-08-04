import pygame
import random
from src.config import *

#------------------------------------------------------------------------------------------------------------------------
def grid_to_pixel(pos_grid, is_rect):
    cell_width = SCREEN_WIDTH // 3
    cell_height = SCREEN_HEIGHT // 3
    if is_rect:
        pos_pixel = pygame.Vector2((pos_grid.y * cell_width + cell_width // 2) - RECT_WIDTH/2, (SCREEN_HEIGHT - (pos_grid.x * cell_height + cell_height // 2)) - RECT_HEIGHT/2)
    else:
        pos_pixel = pygame.Vector2(pos_grid.y * cell_height + cell_height // 2 - 48, SCREEN_HEIGHT - (pos_grid.x * cell_width + cell_width // 2 + 100))

    return pos_pixel
#------------------------------------------------------------------------------------------------------------------------

def row_valid(row, start):
    if (start == 0 or start == 2) and row[1][1]:
        if row[2][0] or row[2][2]:
            return True
    if start == 1:
        if (row[1][0] or row[1][2]) and row[2][1]:
            return True
    return False


def init_row():
    while True:
        row = [[0,1,0]]
        for i in range(2):
            temp_row=[0,0,0]
            for j in range(3):
                rand = random.randint(0, 3)
                if rand == 0:
                    temp_row[j] = 1
            row.append(temp_row)
        if row_valid(row, 1):
            break
    return row
#------------------------------------------------------------------------------------------------------------------------
