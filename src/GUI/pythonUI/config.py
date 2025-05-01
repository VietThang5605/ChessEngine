# config.py
import pygame
import os
import sys

pygame.init()

BOARD_WIDTH = 800
SQUARE_SIZE = BOARD_WIDTH // 8
WIDTH = 1150
HEIGHT = 8 * SQUARE_SIZE + 40
HISTORY_PANEL_WIDTH = 350
SCREEN = pygame.display.set_mode((WIDTH, HEIGHT))

WHITE = (255, 255, 255)
BLACK = (0, 0, 0)
LIGHT_BROWN = (222, 184, 135)
DARK_BROWN = (139, 69, 19)
BLUE = (100, 139, 186)

HIGHLIGHT_COLOR = (50, 205, 50)
font = pygame.font.SysFont('timesnewroman', 48)

captured_white = []
captured_black = []

move_sound = pygame.mixer.Sound("sound/move.wav")
capture_sound = pygame.mixer.Sound("sound/capture.wav")

PIECE_NAMES = ["wp", "wr", "wn", "wb", "wq", "wk", "bp", "br", "bn", "bb", "bq", "bk"]

pieces_img = {}
for name in PIECE_NAMES:
    img = pygame.image.load(os.path.join("images", f"{name}.png"))
    pieces_img[name] = pygame.transform.scale(img, (SQUARE_SIZE, SQUARE_SIZE))
    
PIECE_SIZE = 28