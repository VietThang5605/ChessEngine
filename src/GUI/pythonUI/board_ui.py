import sys
import chess
import pygame

from AI_AI_mode import ai_vs_ai
from HUMAN_AI_mode import human_vs_ai
# from ai_test import ai_vs_ai
from config import *

#Init game
pygame.init()

#Screen
pygame.display.set_caption('Chess Project')

#color
WHITE = (255, 255, 255)
BLACK = (0, 0, 0)
LIGHT_BROWN = (222, 184, 135)
DARK_BROWN = (139, 69, 19)
BUTTON_COLOR = (0, 128, 0)
HOVER_COLOR = (0, 255, 0)

button_font = pygame.font.Font(None, 48)

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

        # btn1 = draw_button("Human vs Human", x, 150, button_width, button_height)
        btn2 = draw_button("Human vs AI", x, 250, button_width, button_height)
        btn3 = draw_button("AI vs AI", x, 350, button_width, button_height)

        for event in pygame.event.get():
            if event.type == pygame.QUIT:
                pygame.quit()
                sys.exit()

            if event.type == pygame.MOUSEBUTTONDOWN:
                # if btn1.collidepoint(event.pos):
                #     pass  # Sau này xử lý Người vs Người
                if btn2.collidepoint(event.pos):
                    human_vs_ai()
                elif btn3.collidepoint(event.pos):
                    ai_vs_ai()

        pygame.display.update()

#Game Mode

game_menu()
