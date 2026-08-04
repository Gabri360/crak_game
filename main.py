import pygame
from src.config import *
from src.classes import Player, Plat, Game
from src.utils import grid_to_pixel




pygame.init()
screen = pygame.display.set_mode((SCREEN_HEIGHT, SCREEN_WIDTH), pygame.RESIZABLE)
clock = pygame.time.Clock()


player = Player()
game = Game()

while game.quit:

    screen.fill("green")

    for event in pygame.event.get():
        if event.type == pygame.QUIT:
            game.quit = False

        elif event.type == pygame.KEYDOWN:
            if event.key == pygame.K_a or event.key == pygame.K_LEFT:
                player.move(False)

            elif event.key == pygame.K_d or event.key == pygame.K_RIGHT:
                player.move(True)
            elif event.key == pygame.K_q:
                game.quit=False

    game.draw_rect(screen)
    player.draw(screen)
    pygame.display.flip()

    # dt is delta time in seconds since last frame, used for independent physics.
    game.dt = clock.tick(FRAME_RATE) / 1000
pygame.quit()
