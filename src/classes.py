import pygame
from src.config import *
from src.utils import *

#------------------------------------------------------------------------------------------------------------------------
class Player:
    def __init__(self):
        self.pos = pygame.Vector2(0,1)
        self.is_moving_right = 1
        self.idle_right = pygame.transform.scale(pygame.image.load("frog_spritesheets/idle_right.png"), (96,96))
        self.idle_left = pygame.transform.scale(pygame.image.load("frog_spritesheets/idle_left.png"), (96,96))
        self.jump_right = pygame.transform.scale(pygame.image.load("frog_spritesheets/jump_right.png"), (96,96))
        self.jump_left = pygame.transform.scale(pygame.image.load("frog_spritesheets/jump_left.png"), (96,96))
    def draw(self, screen):
        if self.is_moving_right:
            screen.blit(self.idle_right, grid_to_pixel(self.pos, 0))
        else:
            screen.blit(self.idle_left, grid_to_pixel(self.pos, 0))
    def move(self, is_right):
        if is_right:
            new_pos = pygame.Vector2(self.pos.x, min(self.pos.y+1,2))
            self.is_moving_right = 1
        else:
            new_pos = pygame.Vector2(self.pos.x, max(self.pos.y-1,0))
            self.is_moving_right = 0
        self.pos = new_pos

#------------------------------------------------------------------------------------------------------------------------

class Plat:
    def __init__(self, x, y):
        self.body = pygame.Rect(grid_to_pixel(pygame.Vector2(x, y),1), (RECT_WIDTH, RECT_HEIGHT))
    def draw(self,screen):
        pygame.draw.rect(screen, "orange", self.body, border_radius=BORDER_RADIUS)

#------------------------------------------------------------------------------------------------------------------------
class Game:
    def __init__(self):
        self.quit = 1
        self.dt = 0
        self.row = init_row()
    def draw_rect(self, screen):
        for x in range(3):
            for y in range(3):
                if self.row[x][y]:
                    rect=Plat(x,y)
                    rect.draw(screen)
