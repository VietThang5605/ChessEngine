import sys
import chess
import pygame

from AI_AI_mode import ai_vs_ai
from config import *

#Init game


#Screen
pygame.display.set_caption('Chess Project')

#color
WHITE = (255, 255, 255)
BLACK = (0, 0, 0)
LIGHT_BROWN = (222, 184, 135)
DARK_BROWN = (139, 69, 19)
BUTTON_COLOR = (0, 128, 0)
HOVER_COLOR = (0, 255, 0)

#board

# start_board = [
#     ["br", "bn", "bb", "bq", "bk", "bb", "bn", "br"],
#     ["bp"] * 8,
#     [""] * 8,
#     [""] * 8,
#     [""] * 8,
#     [""] * 8,
#     ["wp"] * 8,
#     ["wr", "wn", "wb", "wq", "wk", "wb", "wn", "wr"]
# ]

#upload piece_image
# def load_piece(name):
#     image = pygame.image.load(name)
#     return pygame.transform.scale(image, (SQUARE_SIZE, SQUARE_SIZE))

# pieces = {
#     "wp": load_piece("images/white_Pawn.png"),
#     "wr": load_piece("images/white_Rook.png"),
#     "wn": load_piece("images/white_Knight.png"),
#     "wb": load_piece("images/white_Bishop.png"),
#     "wq": load_piece("images/white_Queen.png"),
#     "wk": load_piece("images/white_King.png"),
#     "bp": load_piece("images/black_Pawn.png"),
#     "br": load_piece("images/black_Rook.png"),
#     "bn": load_piece("images/black_Knight.png"),
#     "bb": load_piece("images/black_Bishop.png"),
#     "bq": load_piece("images/black_Queen.png"),
#     "bk": load_piece("images/black_King.png"),
# }

button_font = pygame.font.Font(None, 48)

# def draw_board():
#     for row in range(8):
#         for col in range(8):
#             color = LIGHT_BROWN if (row + col) % 2 == 0 else DARK_BROWN
#             pygame.draw.rect(SCREEN, color, (col * SQUARE_SIZE, row * SQUARE_SIZE, SQUARE_SIZE, SQUARE_SIZE))

# def draw_pieces(board):
#     for row in range(8):
#         for col in range(8):
#             piece = board[row][col]
#             if piece != "":
#                 SCREEN.blit(pieces[piece], (col * SQUARE_SIZE, row * SQUARE_SIZE))

#draw_button
def draw_button(text, x, y, width, height):
    #draw button
    button_rect = pygame.Rect(x, y, width, height)
    pygame.draw.rect(SCREEN, BUTTON_COLOR, button_rect)
    
    #draw text on button
    button_text = button_font.render(text, True, WHITE)
    text_rect = button_text.get_rect(center=button_rect.center)
    SCREEN.blit(button_text, text_rect)
    
    return button_rect

def game_menu():
    while True:
        SCREEN.fill(WHITE)
        title = font.render("Game mode:", True, BLACK)
        SCREEN.blit(title, (WIDTH // 2 - title.get_width() // 2, 60))
        button_width = 300
        button_height = 60
        x = (WIDTH - button_width) // 2

        btn1 = draw_button("Human vs Human", x, 150, button_width, button_height)
        btn2 = draw_button("Human vs AI", x, 250, button_width, button_height)
        btn3 = draw_button("AI vs AI", x, 350, button_width, button_height)

        for event in pygame.event.get():
            if event.type == pygame.QUIT:
                pygame.quit()
                sys.exit()

            if event.type == pygame.MOUSEBUTTONDOWN:
                if btn1.collidepoint(event.pos):
                    pass  # Sau này xử lý Người vs Người
                elif btn2.collidepoint(event.pos):
                    player_vs_ai()
                elif btn3.collidepoint(event.pos):
                    ai_vs_ai()

        pygame.display.update()

#Game Mode
def player_vs_player():
    pass

def player_vs_ai():
    pass

game_menu()
