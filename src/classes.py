import pygame
from src.config import *
from src.utils import *

#------------------------------------------------------------------------------------------------------------------------
class Player:
    def __init__(self):
        self.pos = 1
        self.old_pos = 0
        self.is_moving_right = 1
        self.idle_right = pygame.transform.scale(pygame.image.load("frog_spritesheets/idle_right.png"), (96,96))
        self.idle_left = pygame.transform.scale(pygame.image.load("frog_spritesheets/idle_left.png"), (96,96))
        self.jump_right = pygame.transform.scale(pygame.image.load("frog_spritesheets/jump_right.png"), (96,96))
        self.jump_left = pygame.transform.scale(pygame.image.load("frog_spritesheets/jump_left.png"), (96,96))
    def draw(self, screen):
        if self.is_moving_right:
            screen.blit(self.idle_right, ((self.pos * SCREEN_WIDTH // 3 + SCREEN_WIDTH // 6) - 48 , SCREEN_HEIGHT*5//6 - 100 ))
        else:
            screen.blit(self.idle_left,  ((self.pos * SCREEN_WIDTH // 3 + SCREEN_WIDTH // 6) - 48 , SCREEN_HEIGHT*5//6 - 100 ))
    def move(self, is_right):
        if is_right and self.pos != 2:
            self.old_pos = self.pos
            new_pos = self.pos+1
            self.is_moving_right = 1
            self.pos = new_pos
            return True
        if (not is_right) and self.pos != 0:
            self.old_pos = self.pos
            new_pos = self.pos-1
            self.is_moving_right = 0
            self.pos = new_pos
            return True
        return False
    def render_animation(self, screen, game):

        if self.is_moving_right:
            screen.blit(self.jump_right, ((((self.pos * game.animation_time * ANIMATION_RATE + self.old_pos * (1-game.animation_time * ANIMATION_RATE)) * SCREEN_WIDTH) // 3 + SCREEN_WIDTH // 6) - 48 , SCREEN_HEIGHT*5//6 - 100 ))
        else:
            screen.blit(self.jump_left, ((((self.pos * game.animation_time * ANIMATION_RATE + self.old_pos * (1-game.animation_time * ANIMATION_RATE)) * SCREEN_WIDTH) // 3 + SCREEN_WIDTH // 6) - 48 , SCREEN_HEIGHT*5//6 - 100 ))

#------------------------------------------------------------------------------------------------------------------------

class Plat:
    def __init__(self, x, y):
        self.body = pygame.Rect(grid_to_pixel(pygame.Vector2(x, y)), (RECT_WIDTH, RECT_HEIGHT))
    def draw(self,screen):
        pygame.draw.rect(screen, "orange", self.body, border_radius=BORDER_RADIUS)

class Cloud:
    def __init__(self):
        self.body = [pygame.image.load("frog_spritesheets/Cloud-2.png"),  pygame.image.load("frog_spritesheets/Cloud-31.png")]
        self.pos_y = [SCREEN_HEIGHT//6, SCREEN_HEIGHT*2//3]
        self.pos_x = [SCREEN_WIDTH//3 -58, SCREEN_WIDTH*2//3 -94]

#------------------------------------------------------------------------------------------------------------------------
class Game:
    def __init__(self):
        self.quit = False
        self.lose = False
        self.is_animated = False
        self.animation_time = 0
        self.score = 0
        self.dt = 0
        self.row = init_row()
        self.old_row = self.row.copy()
        self.cloud = Cloud()

    def draw_rect(self, screen):
        for i in range(2):
            screen.blit(self.cloud.body[i], (self.cloud.pos_x[i],self.cloud.pos_y[i]))
        for x in range(3):
            for y in range(3):
                if self.row[x][y]:
                    rect=Plat(x,y)
                    rect.draw(screen)

    def draw_score(self, screen, font):
        text = font.render(f"  Score: {self.score}  ", True, (255, 255, 255))
        padding = 7
        bg_rect = text.get_rect(topleft=(11, 11))
        bg_rect.inflate_ip(padding * 2, padding * 2)
        pygame.draw.rect(screen, "orange", bg_rect, border_radius = BORDER_RADIUS)

        screen.blit(text, (bg_rect.x + padding, bg_rect.y + padding))

    def move_game(self, start):
        add_row(self, start)
        if self.is_animated:
            self.animation_time = 0
            self.old_row.pop(0)
        self.is_animated = True
        self.score += 1

    def render_animation(self, screen):
        for i in range(2):
            if self.cloud.pos_y[i] > SCREEN_HEIGHT:
                self.cloud.pos_y[i] = -100
            self.cloud.pos_y[i] += CLOUD_VELOCITY * self.animation_time * ANIMATION_RATE
            screen.blit(self.cloud.body[i], (self.cloud.pos_x[i],self.cloud.pos_y[i]))

        for x in range(4):
            for y in range(3):
                if self.old_row[x][y]:
                    rect=Plat(x,y)
                    pygame.draw.rect(screen, "orange", (rect.body.x , rect.body.y + ((SCREEN_HEIGHT)//3)*self.animation_time*ANIMATION_RATE, RECT_WIDTH, RECT_HEIGHT), border_radius=BORDER_RADIUS)
        self.animation_time += self.dt
