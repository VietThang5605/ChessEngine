import pygame
import subprocess
import threading
import os
import sys
import time
from tkinter import Tk, filedialog, simpledialog

from config import SCREEN, WIDTH, HEIGHT, BOARD_WIDTH, HISTORY_PANEL_WIDTH
from config import WHITE, BLACK, LIGHT_BROWN, DARK_BROWN, BLUE, HIGHLIGHT_COLOR, SQUARE_SIZE, font, pieces_img
from config import PIECE_SIZE

INFO_HEIGHT = 150

def draw_board(board, last_move):
    for row in range(8):
        for col in range(8):
            color = LIGHT_BROWN if (row + col) % 2 == 0 else DARK_BROWN
            rect = pygame.Rect(col * SQUARE_SIZE, row * SQUARE_SIZE, SQUARE_SIZE, SQUARE_SIZE)
            pygame.draw.rect(SCREEN, color, rect)

            board_row = 7 - row
            board_col = col


            if last_move:
                last_move_from_row = 7 - (last_move[0][0])
                last_move_from_col = last_move[0][1] 

                last_move_to_row = 7 - (last_move[1][0])
                last_move_to_col = last_move[1][1]

                if (board_row == last_move_from_row and board_col == last_move_from_col) or \
                   (board_row == last_move_to_row and board_col == last_move_to_col):
                    pygame.draw.rect(SCREEN, HIGHLIGHT_COLOR, rect, 5)

            piece = board.piece_at(board_row * 8 + board_col)
            if piece:
                piece_str = piece.symbol()
                key = ('w' if piece_str.isupper() else 'b') + piece_str.lower()
                SCREEN.blit(pieces_img[key], (col * SQUARE_SIZE, row * SQUARE_SIZE))

def draw_captured_pieces(captured_white, captured_black):
    panel_rect = pygame.Rect(BOARD_WIDTH, INFO_HEIGHT, HISTORY_PANEL_WIDTH, HEIGHT - INFO_HEIGHT - 80)
    pygame.draw.rect(SCREEN, BLUE, panel_rect)

    x_center = BOARD_WIDTH + (HISTORY_PANEL_WIDTH // 10)
    y_center_white = INFO_HEIGHT + 20
    y_center_black = HEIGHT // 2

    for i, piece in enumerate(captured_white):
        key = 'w' + piece.lower()
        img = pieces_img.get(key)
        img_resized = pygame.transform.scale(img, (PIECE_SIZE, PIECE_SIZE))
        if (i < 8):
            SCREEN.blit(img_resized, (x_center + i * (PIECE_SIZE + 5), y_center_white))
        else:
            SCREEN.blit(img_resized, (x_center + i * (PIECE_SIZE + 5) - 264, y_center_white + 50))

    for i, piece in enumerate(captured_black):
        key = 'b' + piece.lower() 
        img = pieces_img.get(key)
        img_resized = pygame.transform.scale(img, (PIECE_SIZE, PIECE_SIZE))
        if (i< 8):
            SCREEN.blit(img_resized, (x_center + i * (PIECE_SIZE + 5), y_center_black))
        else:
            SCREEN.blit(img_resized, (x_center + i * (PIECE_SIZE + 5) - 264, y_center_black + 50))
            
def show_game_result(winner):
    font_big = pygame.font.SysFont(None, 48)
    font_small = pygame.font.SysFont(None, 32)

    # Hiện dòng kết quả
    text = font_big.render(f"Game Over! Winner: {winner}", True, WHITE)
    instruction = font_small.render("Press 1 to New Game, 2 to Exit", True, WHITE)

    SCREEN.blit(text, (WIDTH // 2 - text.get_width() // 2, HEIGHT // 2 - text.get_height() // 2 - 20))
    SCREEN.blit(instruction, (WIDTH // 2 - instruction.get_width() // 2, HEIGHT // 2 + 40))
    pygame.display.update()

    waiting_for_key = True
    while waiting_for_key:
        for event in pygame.event.get():
            if event.type == pygame.QUIT:
                pygame.quit()
                sys.exit()
            if event.type == pygame.KEYDOWN:
                if event.key == pygame.K_1:
                    return "new_game"
                elif event.key == pygame.K_2:
                    return "exit"
                

def draw_info(white_name, black_name, white_time, black_time):

    pygame.draw.rect(SCREEN, BLUE, (BOARD_WIDTH, 0, HISTORY_PANEL_WIDTH, INFO_HEIGHT))

    name_font = pygame.font.SysFont('timesnewroman', 22)
    time_font = pygame.font.SysFont('timesnewroman', 20)

    white_text = name_font.render(f"{white_name} (White)", True, WHITE)
    black_text = name_font.render(f"{black_name} (Black)", True, WHITE)

    white_clock = time_font.render(f"{white_time//60:02d}:{white_time%60:02d}", True, WHITE)
    black_clock = time_font.render(f"{black_time//60:02d}:{black_time%60:02d}", True, WHITE)

    SCREEN.blit(white_text, (BOARD_WIDTH + 10, 10))
    SCREEN.blit(white_clock, (BOARD_WIDTH + 10, 35))
    SCREEN.blit(black_text, (BOARD_WIDTH + 10, 70))
    SCREEN.blit(black_clock, (BOARD_WIDTH + 10, 95))
                
def get_engine_move(engine, fen, white_time, black_time, move_time):
    engine.stdin.write(f"position fen {fen}\n".encode())
    
    # engine.stdin.write(f"go wtime {white_time * 1000} btime {black_time * 1000} winc 0 binc 0\n".encode())
    engine.stdin.write(f"go movetime {move_time}\n".encode())
    engine.stdin.flush()
     
    while True:
        response = engine.stdout.readline().decode().strip()
        if response.startswith("bestmove"):
            best_move = response.split()[1]
            return best_move