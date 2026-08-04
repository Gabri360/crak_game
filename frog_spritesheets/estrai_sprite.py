"""
Estrae 4 sprite (fermo-sx, fermo-dx, salto-sx, salto-dx) dallo spritesheet
del rospo verde. Lo sheet è una griglia di celle 32x32 px (16 colonne x 16 righe).

Coordinate usate (riga, colonna) nella griglia:
  - fermo a destra -> riga 1, colonna 11
  - salto a destra -> riga 1, colonna 9
  - fermo/salto a sinistra -> ottenuti specchiando quelli a destra
    (nello sheet esistono pose "sinistra" disegnate a mano in colonna 0-8,
    ma sono pose diverse frame-per-frame, non lo specchio esatto; per avere
    coppie perfettamente simmetriche è meglio flippare in codice)
"""

from PIL import Image

CELL = 32
SHEET_PATH = "frog_green_spritesheet.png"  # metti qui il tuo file

def get_cell(sheet, row, col):
    x, y = col * CELL, row * CELL
    return sheet.crop((x, y, x + CELL, y + CELL))

def main():
    sheet = Image.open(SHEET_PATH).convert("RGBA")

    idle_right = get_cell(sheet, 1, 11)
    jump_right = get_cell(sheet, 1, 9)

    idle_left = idle_right.transpose(Image.FLIP_LEFT_RIGHT)
    jump_left = jump_right.transpose(Image.FLIP_LEFT_RIGHT)

    idle_right.save("idle_right.png")
    idle_left.save("idle_left.png")
    jump_right.save("jump_right.png")
    jump_left.save("jump_left.png")

if __name__ == "__main__":
    main()
