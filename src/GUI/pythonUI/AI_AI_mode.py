import pygame
import subprocess
import threading
import chess
import os
import sys
import time
from tkinter import Tk, filedialog, simpledialog

from config import SCREEN, WIDTH, HEIGHT, BOARD_WIDTH, HISTORY_PANEL_WIDTH
from config import WHITE, BLACK, LIGHT_BROWN, DARK_BROWN, BLUE, HIGHLIGHT_COLOR, SQUARE_SIZE, font, pieces_img
from config import move_sound, capture_sound, PIECE_SIZE, captured_white, captured_black

from general import draw_board, draw_captured_pieces, show_game_result, get_engine_move, draw_info

INFO_HEIGHT = 150
HISTORY_Y_START = INFO_HEIGHT
timer = pygame.time.Clock()
fps = 60
board = chess.Board()

# def draw_clock(white_time, black_time, white_name, black_name):
#     fontClock = pygame.font.SysFont(None, 28)

#     # Chuyển đổi thời gian sang phút:giây
#     def format_time(ms):
#         minutes = ms // 60000
#         seconds = (ms // 1000) % 60
#         return f"{minutes}:{seconds:02d}"

#     white_text = fontClock.render(f"(white) {white_name}: {format_time(white_time)}", True, WHITE)
#     black_text = fontClock.render(f"(black) {black_name}: {format_time(black_time)}", True, WHITE)

#     # Hiển thị ở góc dưới panel bên phải
#     SCREEN.blit(white_text, (BOARD_WIDTH + 10, HEIGHT - 60))
#     SCREEN.blit(black_text, (BOARD_WIDTH + 10, HEIGHT - 30))


def show_engine_selection_dialog():
    import tkinter as tk

    root = tk.Tk()
    root.title("Chọn Engine và Cài đặt Trận đấu")

    engine_white_path = tk.StringVar()
    engine_black_path = tk.StringVar()
    engine_white_name = tk.StringVar(value="White Engine")
    engine_black_name = tk.StringVar(value="Black Engine")
    move_time = tk.StringVar(value="1000")  # default 1s
    match_time = tk.StringVar(value="5")    # default 5 phút
    confirmed = tk.BooleanVar(value=False)

    def browse_white():
        path = filedialog.askopenfilename(title="Chọn Engine bên Trắng", filetypes=[("Executable files", "*.exe")])
        engine_white_path.set(path)

    def browse_black():
        path = filedialog.askopenfilename(title="Chọn Engine bên Đen", filetypes=[("Executable files", "*.exe")])
        engine_black_path.set(path)

    def confirm():
        confirmed.set(True)
        root.quit()
        root.destroy()

    # Giao diện chọn engine trắng
    tk.Label(root, text="Engine bên Trắng:").grid(row=0, column=0)
    tk.Entry(root, textvariable=engine_white_path, width=40).grid(row=0, column=1)
    tk.Button(root, text="Browse", command=browse_white).grid(row=0, column=2)

    tk.Label(root, text="Tên Engine Trắng:").grid(row=1, column=0)
    tk.Entry(root, textvariable=engine_white_name, width=40).grid(row=1, column=1, columnspan=2)

    # Giao diện chọn engine đen
    tk.Label(root, text="Engine bên Đen:").grid(row=2, column=0)
    tk.Entry(root, textvariable=engine_black_path, width=40).grid(row=2, column=1)
    tk.Button(root, text="Browse", command=browse_black).grid(row=2, column=2)

    tk.Label(root, text="Tên Engine Đen:").grid(row=3, column=0)
    tk.Entry(root, textvariable=engine_black_name, width=40).grid(row=3, column=1, columnspan=2)

    # Thời gian
    tk.Label(root, text="Thời gian suy nghĩ mỗi lượt (ms):").grid(row=4, column=0)
    tk.Entry(root, textvariable=move_time).grid(row=4, column=1, columnspan=2)

    tk.Label(root, text="Tổng thời gian trận đấu (phút):").grid(row=5, column=0)
    tk.Entry(root, textvariable=match_time).grid(row=5, column=1, columnspan=2)

    tk.Button(root, text="OK", command=confirm).grid(row=6, column=1)

    root.mainloop()

    if confirmed.get():
        try:
            move_time_val = int(move_time.get())
            match_time_val = int(match_time.get())
            return {
                "engine_white_path": engine_white_path.get(),
                "engine_black_path": engine_black_path.get(),
                "white_name": engine_white_name.get(),
                "black_name": engine_black_name.get(),
                "move_time": move_time_val,
                "match_time": match_time_val
            }
        except ValueError:
            print("Thời gian nhập không hợp lệ.")
            return None
    return None

def engine_play(current_engine, white_remaining, black_remaining, move_time, last_move_squares):
    move_uci = get_engine_move(current_engine, board.fen(), white_remaining, black_remaining, move_time)
    
    try:
        move = chess.Move.from_uci(move_uci)
        if move in board.legal_moves:
            piece_captured = board.is_capture(move)
                
            if piece_captured:
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
            last_move_squares = [
                (7 - move.from_square // 8, move.from_square % 8),
                (7 - move.to_square // 8, move.to_square % 8),
            ]
    except Exception as e:
        print("Lỗi khi thực hiện nước đi:", e)
    

def ai_vs_ai():
    config = show_engine_selection_dialog()
    if not config:
        print("Engine không được chọn đầy đủ.")
        return

    engine1 = subprocess.Popen(config["engine_white_path"], stdin=subprocess.PIPE, stdout=subprocess.PIPE)
    engine2 = subprocess.Popen(config["engine_black_path"], stdin=subprocess.PIPE, stdout=subprocess.PIPE)

    for engine in [engine1, engine2]:
        engine.stdin.write(b"uci\n")
        engine.stdin.flush()
        while True:
            if engine.stdout.readline().decode().strip() == "uciok":
                break

    move_time = config["move_time"]
    match_time = config["match_time"] * 60  # phút -> giây

    white_remaining = match_time
    black_remaining = match_time
    last_move_squares = None

    clock = pygame.time.Clock()
    scroll_offset = 0
    pause_button_rect = pygame.Rect(BOARD_WIDTH + 20, HEIGHT - 60, 100, 30)
    resume_button_rect = pygame.Rect(BOARD_WIDTH + 130, HEIGHT - 60, 100, 30)
    is_paused = False

    font_button = pygame.font.SysFont('timesnewroman', 24)
    
    running = True
    while running and not board.is_game_over(claim_draw=True):
        timer.tick(fps)

        if is_paused:
            pygame.time.wait(100)
            for event in pygame.event.get():
                if event.type == pygame.QUIT:
                    running = False
                elif event.type == pygame.MOUSEBUTTONDOWN:
                    if resume_button_rect.collidepoint(event.pos):
                        is_paused = False

            continue

        draw_board(board, last_move_squares)
        draw_info(config['white_name'], config['black_name'], white_remaining, black_remaining)
        # draw_clock(white_remaining, black_remaining, config['white_name'], config['black_name'])
        draw_captured_pieces(captured_white, captured_black)

        # Nút tạm dừng / tiếp tục
        pygame.draw.rect(SCREEN, (100, 100, 100), pause_button_rect)
        pygame.draw.rect(SCREEN, (0, 100, 0), resume_button_rect)
        pause_text = font_button.render("Pause", True, WHITE)
        resume_text = font_button.render("Resume", True, WHITE)
        SCREEN.blit(pause_text, (pause_button_rect.x + 10, pause_button_rect.y + 5))
        SCREEN.blit(resume_text, (resume_button_rect.x + 10, resume_button_rect.y + 5))

        pygame.display.flip()

        current_engine = engine1 if board.turn == chess.WHITE else engine2
        
        start_move_time = time.time()
        engine_play(current_engine, white_remaining, black_remaining, move_time, last_move_squares)
        
        elapsed_time = time.time() - start_move_time
        elapsed_seconds = int(elapsed_time)
        
        # if board.turn == chess.WHITE:
        #     white_remaining = max(0, white_remaining - elapsed_seconds)

        # else:
        #     black_remaining = max(0, black_remaining - elapsed_seconds)

        for event in pygame.event.get():
            if event.type == pygame.QUIT:
                running = False
            elif event.type == pygame.MOUSEBUTTONDOWN:
                if pause_button_rect.collidepoint(event.pos):
                    is_paused = True
                elif resume_button_rect.collidepoint(event.pos):
                    is_paused = False

    # Hiển thị bàn cờ cuối cùng nếu chiếu hết
    if board.is_checkmate():
        draw_board(board, last_move_squares)
        draw_info(config['white_name'], config['black_name'], white_remaining, black_remaining)
        pygame.display.flip()
        pygame.time.wait(500)

    # Kết thúc trận đấu
    if board.is_checkmate():
        winner = "White" if board.turn == chess.BLACK else "Black"
    elif board.is_stalemate() or board.is_insufficient_material() or board.is_seventyfive_moves() or board.is_fivefold_repetition():
        winner = "Draw"
    else:
        if (white_remaining == 0 or black_remaining == 0): winner = "Drawn"
        else: winner = 'nknown'

    engine1.kill()
    engine2.kill()
    print("Game Over:", board.result())
    result = show_game_result(winner)
    captured_white.clear()
    captured_black.clear()
    last_move_squares = None
    if result == 'new_game':
        ai_vs_ai()

