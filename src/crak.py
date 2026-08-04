import pygame

# VARIABLES

FRAME_RATE = 60

SCREEN_HEIGHT = 500
SCREEN_WIDHT = 500

RECT_HEIGHT = 15
RECT_WIDHT = 100
BORDER_RADIUS = 5

# PYGAME SETUP
pygame.init()
screen = pygame.display.set_mode((SCREEN_HEIGHT, SCREEN_WIDHT), pygame.RESIZABLE)
clock = pygame.time.Clock()

#FUNCTION

def grid_to_pixel(pos_grid, is_rect):
    cell_width = SCREEN_WIDHT // 3
    cell_height = SCREEN_HEIGHT // 3
    if is_rect:
        pos_pixel = pygame.Vector2((pos_grid.y * cell_width + cell_width // 2) - RECT_WIDHT/2, (SCREEN_HEIGHT - (pos_grid.x * cell_height + cell_height // 2)) - RECT_HEIGHT/2)
    else:
        pos_pixel = pygame.Vector2(pos_grid.y * cell_height + cell_height // 2 - 48, SCREEN_HEIGHT - (pos_grid.x * cell_width + cell_width // 2 + 100))

    return pos_pixel

# CLASSES
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

class Plat:
    def __init__(self, x, y):
        self.body = pygame.Rect(grid_to_pixel(pygame.Vector2(x, y),1), (RECT_WIDHT, RECT_HEIGHT))
    def draw(self,screen):
        pygame.draw.rect(screen, "orange", self.body, border_radius=BORDER_RADIUS)
    def move(self, x, y):
        new_pos = pygame.Vector2(x, y)
        self.body.topleft = grid_to_pixel(pygame.Vector2(new_pos),1)

class Game:
    def __init__(self):
        self.quit = 1
        self.dt = 0
        self.row = [[0, 1, 0],[1,0,1],[0,1,1]]
    def draw_rect(self, screen):
        for x in range(3):
            for y in range(3):
                if self.row[x][y]:
                    rect=Plat(x,y)
                    rect.draw(screen)




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
