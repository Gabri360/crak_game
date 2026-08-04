import pygame

# VARIABLES

FRAME_RATE = 60

SCREEN_HEIGHT = 500
SCREEN_WIDHT = 500

PLAYER_SPEED = 500
GRAVITY = 10


# PYGAME SETUP
pygame.init()
screen = pygame.display.set_mode((SCREEN_HEIGHT, SCREEN_WIDHT), pygame.RESIZABLE)
clock = pygame.time.Clock()

# CLASSES
class Player:
    def __init__(self):
        self.pos = pygame.Vector2(SCREEN_WIDHT/2, SCREEN_HEIGHT/2)
    def draw(self,screen):
        pygame.draw.circle(screen, "red", self.pos, 40)


class Game:
    def __init__(self):
        self.quit = 1
        self.dt = 0



player = Player()
game = Game()


while game.quit:

    keys = pygame.key.get_pressed()
    for event in pygame.event.get():
        if event.type == pygame.QUIT or keys[pygame.K_q]:
            game.quit = False

    screen.fill("green")

    # RENDER YOUR GAME HERE

    if keys[pygame.K_w] or keys[pygame.K_UP]:
        player.pos.y -= PLAYER_SPEED * game.dt
    if keys[pygame.K_s] or keys[pygame.K_DOWN]:
        player.pos.y += PLAYER_SPEED * game.dt
    if keys[pygame.K_a] or keys[pygame.K_LEFT]:
        player.pos.x -= PLAYER_SPEED * game.dt
    if keys[pygame.K_d] or keys[pygame.K_RIGHT]:
        player.pos.x += PLAYER_SPEED * game.dt



    player.draw(screen)
    pygame.display.flip()

    # dt is delta time in seconds since last frame, used for independent physics.
    game.dt = clock.tick(FRAME_RATE) / 1000
pygame.quit()
