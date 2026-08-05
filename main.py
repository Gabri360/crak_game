import pygame
from src.config import *
from src.classes import *
from src.utils import *



pygame.init()
screen = pygame.display.set_mode((SCREEN_HEIGHT, SCREEN_WIDTH), pygame.RESIZABLE)
clock = pygame.time.Clock()
font = pygame.font.Font(None, 36)

player = Player()
game = Game()

restart = True

#------------------------------------------------------------------------------------------------------------------------

while not game.quit:
    while not game.lose:

        screen.fill("green")

        for event in pygame.event.get():
            if event.type == pygame.QUIT:
                game.lose = True
                game.quit = True

            elif event.type == pygame.KEYDOWN:
                if event.key == pygame.K_a or event.key == pygame.K_LEFT:
                    if player.move(False):
                        game.move_game(player.pos.y)
                elif event.key == pygame.K_d or event.key == pygame.K_RIGHT:
                    if player.move(True):
                        game.move_game(player.pos.y)
                elif event.key == pygame.K_q:
                    game.lose = True
                    game.quit=True


        check_death(game,int(player.pos.y))
        game.draw_rect(screen)
        player.draw(screen)

        game.draw_score(screen, font)


        pygame.display.flip()
        # dt is delta time in seconds since last frame, used for independent physics.
        game.dt = clock.tick(FRAME_RATE) / 1000
#------------------------------------------------------------------------------------------------------------------------

    if not game.quit:
        restart = False

    while not restart:
        for event in pygame.event.get():
            if event.type == pygame.QUIT:
                restart = True
                game.quit = True
            elif event.type == pygame.KEYDOWN:
                if event.key == pygame.K_RETURN:
                    player = Player()
                    game = Game()
                    restart= True
                    game.quit = False
                elif event.key == pygame.K_q:
                    game.quit = True
                    restart = True

        game_over_img = pygame.transform.scale(pygame.image.load("frog_spritesheets/pixelart_skull.png"), (SCREEN_WIDTH*3//4,SCREEN_WIDTH*3//4))
        rect_img = game_over_img.get_rect(center=(SCREEN_WIDTH // 2, SCREEN_HEIGHT // 2))
        screen.blit(game_over_img,rect_img)
        game.draw_rect(screen)
        player.draw(screen)
        player.draw(screen)
        pygame.display.flip()
#------------------------------------------------------------------------------------------------------------------------



pygame.quit()
