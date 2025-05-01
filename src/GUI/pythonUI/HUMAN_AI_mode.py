import pygame
import subprocess
import threading
import chess
import os
import sys
import time
import tkinter as tk
from tkinter import filedialog, simpledialog

from config import SCREEN, WIDTH, HEIGHT, BOARD_WIDTH, HISTORY_PANEL_WIDTH, SQUARE_SIZE
from config import WHITE, BLACK, LIGHT_BROWN, DARK_BROWN, BLUE, HIGHLIGHT_COLOR, SQUARE_SIZE, font, pieces_img
from config import move_sound, capture_sound, PIECE_SIZE, captured_white, captured_black
from general import draw_board, draw_captured_pieces, show_game_result, draw_info
from AI_AI_mode import engine_play
# from logic import check_options, check_valid_moves, draw_valid, white_pieces, white_locations, black_pieces, black_locations


# black_options = check_options(black_pieces, black_locations, 'black')    
# white_options = check_options(white_pieces, white_locations, 'white')
board = chess.Board()

def show_human_vs_ai_dialog():
    root = tk.Tk()
    root.title("Cài đặt Human vs AI")

    engine_path = tk.StringVar()
    engine_name = tk.StringVar(value="AI Engine")
    move_time = tk.StringVar(value="1000")   # default 1s
    match_time = tk.StringVar(value="5")      # default 5 phút
    human_color = tk.StringVar(value="white") # default human đi trắng
    confirmed = tk.BooleanVar(value=False)

    def browse_engine():
        path = filedialog.askopenfilename(title="Chọn Engine", filetypes=[("Executable files", "*.exe")])
        engine_path.set(path)

    def confirm():
        confirmed.set(True)
        root.quit()
        root.destroy()

    # Engine
    tk.Label(root, text="Chọn Engine cho AI:").grid(row=0, column=0, sticky="w", pady=5)
    tk.Entry(root, textvariable=engine_path, width=40).grid(row=0, column=1, padx=5)
    tk.Button(root, text="Browse", command=browse_engine).grid(row=0, column=2, padx=5)

    # Engine Name
    tk.Label(root, text="Đặt tên cho AI:").grid(row=1, column=0, sticky="w", pady=5)
    tk.Entry(root, textvariable=engine_name, width=40).grid(row=1, column=1, columnspan=2, padx=5)

    # Move Time
    tk.Label(root, text="Thời gian suy nghĩ mỗi lượt (ms):").grid(row=2, column=0, sticky="w", pady=5)
    tk.Entry(root, textvariable=move_time).grid(row=2, column=1, columnspan=2, padx=5)

    # Match Time
    tk.Label(root, text="Tổng thời gian trận đấu (phút):").grid(row=3, column=0, sticky="w", pady=5)
    tk.Entry(root, textvariable=match_time).grid(row=3, column=1, columnspan=2, padx=5)

    # Human Color
    tk.Label(root, text="Bạn muốn chơi bên:").grid(row=4, column=0, sticky="w", pady=5)
    tk.Radiobutton(root, text="Trắng", variable=human_color, value="white").grid(row=4, column=1, sticky="w")
    tk.Radiobutton(root, text="Đen", variable=human_color, value="black").grid(row=4, column=2, sticky="w")

    # OK Button
    tk.Button(root, text="OK", command=confirm).grid(row=5, column=1, pady=10)

    root.mainloop()

    if confirmed.get():
        try:
            move_time_val = int(move_time.get())
            match_time_val = int(match_time.get())
            return {
                "engine_path": engine_path.get(),
                "engine_name": engine_name.get(),
                "move_time": move_time_val,
                "match_time": match_time_val,
                "human_color": human_color.get()
            }
        except ValueError:
            print("Thời gian nhập không hợp lệ.")
            return None
    return None

selected_square = None
def get_human_move():
    global selected_square, last_move_squares
    
    for event in pygame.event.get():
        if event.type == pygame.MOUSEBUTTONDOWN and event.button == 1:
            cols = event.pos[0] // SQUARE_SIZE
            rows = 7 - (event.pos[1] // SQUARE_SIZE)
            clicked_square = chess.square(cols, rows)
            pygame.draw.rect(SCREEN, HIGHLIGHT_COLOR, (cols * SQUARE_SIZE, rows * SQUARE_SIZE, SQUARE_SIZE, SQUARE_SIZE))
            print(f"Click tại ô: {chess.square_name(clicked_square)}")

            if board.turn != player_color:
                continue

            if selected_square is None:
                piece = board.piece_at(clicked_square)
                if piece is not None and piece.color == player_color:
                    selected_square = clicked_square
            else:
                move = chess.Move(selected_square, clicked_square)
                if move in board.legal_moves:
                    print(f"Người chơi di chuyển: {board.san(move)}")
                    is_captured = board.is_capture(move)
                    
                    if(is_captured):
                        captured_piece = board.piece_at(move.to_square)
                        if captured_piece:
                            if captured_piece.color == chess.WHITE:
                                captured_white.append(captured_piece.symbol())
                            else:
                                captured_black.append(captured_piece.symbol())
                        capture_sound.play()
                    else:
                        move_sound.play()   
                                
                    board.push(move)
                    last_move_squares = [selected_square, clicked_square]
                    selected_square = None
                else:
                    selected_square = None

def human_vs_ai():
    setting = show_human_vs_ai_dialog()
    if not setting:
        print("Chưa đủ thông tin.")
        return
    
    move_time = setting['move_time']
    white_remaining = setting['match_time'] * 60
    black_remaining = setting['match_time'] * 60
    last_move_squares = None
    
    font_button = pygame.font.SysFont('timesnewroman', 24)
    timer = pygame.time.Clock()
    fps = 60    
    
    #start engine
    engine = subprocess.Popen(setting["engine_path"], stdin=subprocess.PIPE, stdout=subprocess.PIPE)
    engine.stdin.write(b"uci\n")
    engine.stdin.flush()
    while True:
        if engine.stdout.readline().decode().strip() == "uciok":
            break
    
    name_white = ""
    name_black = ""
    global player_color 
    
    if (setting["human_color"] == 'white'):
        name_black = setting['engine_name']
        name_white = 'Human'
        player_color = chess.WHITE
        print("Human is white")
    else:
        name_white = setting['engine_name']
        name_black = 'Human'
        player_color = chess.BLACK
        print("Human is black")
        
    clock = pygame.time.Clock()
    pause_button_rect = pygame.Rect(BOARD_WIDTH + 20, HEIGHT - 60, 100, 30)
    resume_button_rect = pygame.Rect(BOARD_WIDTH + 130, HEIGHT - 60, 100, 30)
    is_paused = False
    
    running = True
    while running and not board.is_game_over(claim_draw=True):
        timer.tick(fps)
        
        events = pygame.event.get()
        for event in events:
            if event.type == pygame.QUIT:
                running = False
            elif event.type == pygame.MOUSEBUTTONDOWN:
                if pause_button_rect.collidepoint(event.pos):
                    is_paused = True
                elif resume_button_rect.collidepoint(event.pos):
                    is_paused = False
        
        draw_board(board, last_move_squares)
        draw_info(name_white, name_black, white_remaining, black_remaining)
        draw_captured_pieces(captured_white, captured_black)
        
        pygame.draw.rect(SCREEN, (100, 100, 100), pause_button_rect)
        pygame.draw.rect(SCREEN, (0, 100, 0), resume_button_rect)
        pause_text = font_button.render("Pause", True, WHITE)
        resume_text = font_button.render("Resume", True, WHITE)
        SCREEN.blit(pause_text, (pause_button_rect.x + 10, pause_button_rect.y + 5))
        SCREEN.blit(resume_text, (resume_button_rect.x + 10, resume_button_rect.y + 5))

        pygame.display.flip()
        
        for event in pygame.event.get():
            if event.type == pygame.QUIT:
                running = False
            elif event.type == pygame.MOUSEBUTTONDOWN:
                if pause_button_rect.collidepoint(event.pos):
                    is_paused = True
                elif resume_button_rect.collidepoint(event.pos):
                    is_paused = False
                    
        if board.turn == chess. WHITE:
            if (player_color == chess.WHITE):
                get_human_move()
            else:
                engine_play(engine, white_remaining, black_remaining, move_time, last_move_squares)
        else:
            if (player_color == chess.BLACK):
                get_human_move()
            else:
                 engine_play(engine, white_remaining, black_remaining, move_time, last_move_squares)
        
    if board.is_checkmate():
        draw_board(board, last_move_squares)
        draw_info(name_white, name_black, white_remaining, black_remaining)
        pygame.display.flip()
        pygame.time.wait(500)

    if board.is_checkmate():
        winner = "White" if board.turn == chess.BLACK else "Black"
    elif board.is_stalemate() or board.is_insufficient_material() or board.is_seventyfive_moves() or board.is_fivefold_repetition():
        winner = "Draw"
    else:
        if (white_remaining == 0 or black_remaining == 0): winner = "Drawn"
        else: winner = 'Unknown'

    engine.kill()
    
    print("Game Over:", board.result())
    result = show_game_result(winner)
    captured_white.clear()
    captured_black.clear()
    last_move_squares = None
    if result == 'new_game':
        human_vs_ai()