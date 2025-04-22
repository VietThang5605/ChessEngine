import pygame
import subprocess
import threading
import chess
import tkinter as tk
from tkinter import filedialog, simpledialog
import os
import sys

pygame.init()

BOARD_WIDTH = 800
HISTORY_PANEL_WIDTH = 380
HEIGHT = 800
WIDTH = BOARD_WIDTH + HISTORY_PANEL_WIDTH

SCREEN = pygame.display.set_mode((WIDTH, HEIGHT))
pygame.display.set_caption("AI vs AI Mode")

WHITE = (255, 255, 255)
BLACK = (0, 0, 0)
LIGHT_BROWN = (222, 184, 135)
DARK_BROWN = (139, 69, 19)
HIGHLIGHT_COLOR = (50, 205, 50)

SQUARE_SIZE = BOARD_WIDTH // 8

font = pygame.font.SysFont(None, 24)

pieces_img = {}
PIECE_NAMES = ["wp", "wr", "wn", "wb", "wq", "wk", "bp", "br", "bn", "bb", "bq", "bk"]
for name in PIECE_NAMES:
    img = pygame.image.load(os.path.join("images", f"{name}.png"))
    pieces_img[name] = pygame.transform.scale(img, (SQUARE_SIZE, SQUARE_SIZE))

def draw_board(board, last_move):
    for row in range(8):
        for col in range(8):
            color = LIGHT_BROWN if (row + col) % 2 == 0 else DARK_BROWN
            rect = pygame.Rect(col * SQUARE_SIZE, row * SQUARE_SIZE, SQUARE_SIZE, SQUARE_SIZE)
            pygame.draw.rect(SCREEN, color, rect)

            if last_move and (row, col) in last_move:
                pygame.draw.rect(SCREEN, HIGHLIGHT_COLOR, rect, 5)

            piece = board.piece_at(row * 8 + col)
            if piece:
                piece_str = piece.symbol()
                key = ('w' if piece_str.isupper() else 'b') + piece_str.lower()
                SCREEN.blit(pieces_img[key], (col * SQUARE_SIZE, row * SQUARE_SIZE))

def draw_move_history(history):
    pygame.draw.rect(SCREEN, BLACK, (BOARD_WIDTH, 0, HISTORY_PANEL_WIDTH, HEIGHT))
    y = 10
    x = BOARD_WIDTH + 5
    fontHis = pygame.font.Font(None, 24)
    
    count = 0
    for idx, move in enumerate(history, 1):
        if count == 4:
            count = 0
            y += 18
            if y > HEIGHT - 18:
                pygame.draw.rect(SCREEN, BLACK, (BOARD_WIDTH, 0, HISTORY_PANEL_WIDTH, HEIGHT))
                y = 10
        move_surface = fontHis.render(f"{idx}. {move}", True, WHITE)
        SCREEN.blit(move_surface, (x + count * 100, y))
        count += 1

def show_game_result(winner):
    font = pygame.font.SysFont(None, 48)
    text = font.render(f"Game Over! Winner: {winner}", True, WHITE)
    SCREEN.blit(text, (WIDTH // 2 - text.get_width() // 2, HEIGHT // 2 - text.get_height() // 2))
    pygame.display.update()

    waiting_for_key = True
    while waiting_for_key:
        for event in pygame.event.get():
            if event.type == pygame.QUIT:
                pygame.quit()
                sys.exit()
            if event.type == pygame.KEYDOWN:
                waiting_for_key = False

def show_engine_selection_dialog():
    root = tk.Tk()
    root.title("Chọn Engine")

    engine1_path = tk.StringVar()
    engine2_path = tk.StringVar()
    confirmed = tk.BooleanVar(value=False)

    def browse_engine1():
        path = filedialog.askopenfilename(title="Chọn Engine 1", filetypes=[("Executable files", "*.exe")])
        engine1_path.set(path)

    def browse_engine2():
        path = filedialog.askopenfilename(title="Chọn Engine 2", filetypes=[("Executable files", "*.exe")])
        engine2_path.set(path)

    def confirm():
        confirmed.set(True)
        root.quit()
        root.destroy()

    tk.Label(root, text="Engine 1:").grid(row=0, column=0)
    tk.Entry(root, textvariable=engine1_path, width=40).grid(row=0, column=1)
    tk.Button(root, text="Browse", command=browse_engine1).grid(row=0, column=2)

    tk.Label(root, text="Engine 2:").grid(row=1, column=0)
    tk.Entry(root, textvariable=engine2_path, width=40).grid(row=1, column=1)
    tk.Button(root, text="Browse", command=browse_engine2).grid(row=1, column=2)

    tk.Button(root, text="OK", command=confirm).grid(row=2, column=1)

    root.mainloop()

    if confirmed.get():
        return engine1_path.get(), engine2_path.get()
    return None, None

def get_engine_move(engine, board_fen, movetime):
    engine.stdin.write(f"position fen {board_fen}\n".encode())
    engine.stdin.write(f"go movetime {movetime}\n".encode())
    engine.stdin.flush()

    while True:
        line = engine.stdout.readline().decode()
        if line.startswith("bestmove"):
            return line.split()[1]

# def show_game_over_popup(winner):
#     font = pygame.font.SysFont(None, 48)
#     screen_width, screen_height = SCREEN.get_size()
    
#     # Tạo màn hình pop-up
#     popup_width = 400
#     popup_height = 200
#     popup_x = (screen_width - popup_width) // 2
#     popup_y = (screen_height - popup_height) // 2

#     # Tạo cửa sổ pop-up
#     popup_rect = pygame.Rect(popup_x, popup_y, popup_width, popup_height)

#     # Nội dung thông báo
#     game_over_text = font.render("Game Over!", True, WHITE)
#     result_text = font.render(f"Winner: {winner}", True, WHITE)
#     button_text = font.render("Press OK to return", True, WHITE)

#     while True:
#         SCREEN.fill((0, 0, 0))  # Màu nền của pop-up
#         pygame.draw.rect(SCREEN, (50, 50, 50), popup_rect)

#         # Hiển thị thông tin lên pop-up
#         SCREEN.blit(game_over_text, (popup_x + 20, popup_y + 20))
#         SCREEN.blit(result_text, (popup_x + 20, popup_y + 80))
#         SCREEN.blit(button_text, (popup_x + 20, popup_y + 140))

#         # Kiểm tra sự kiện người dùng nhấn nút
#         for event in pygame.event.get():
#             if event.type == pygame.QUIT:
#                 pygame.quit()
#                 sys.exit()
#             if event.type == pygame.MOUSEBUTTONDOWN:
#                 if popup_rect.collidepoint(event.pos):
#                     return  # Đóng pop-up và quay lại menu

#         pygame.display.update()

def ai_vs_ai():
    engine1_path, engine2_path = show_engine_selection_dialog()

    if not engine1_path or not engine2_path:
        print("Engine không được chọn đầy đủ.")
        return

    move_time = get_move_time_from_input()
    if move_time is None:
        print("Thời gian không hợp lệ.")
        return

    board = chess.Board()
    history = []
    last_move_squares = None

    engine1 = subprocess.Popen(engine1_path, stdin=subprocess.PIPE, stdout=subprocess.PIPE)
    engine2 = subprocess.Popen(engine2_path, stdin=subprocess.PIPE, stdout=subprocess.PIPE)

    for engine in [engine1, engine2]:
        engine.stdin.write(b"uci\n")
        engine.stdin.flush()
        while True:
            if engine.stdout.readline().decode().strip() == "uciok":
                break

    running = True
    while running and not board.is_game_over():
        draw_board(board, last_move_squares)
        draw_move_history(history)
        pygame.display.flip()

        current_engine = engine1 if board.turn == chess.WHITE else engine2
        move_uci = get_engine_move(current_engine, board.fen(), move_time)

        try:
            move = chess.Move.from_uci(move_uci)
            if move in board.legal_moves:
                board.push(move)
                history.append(move.uci())
                last_move_squares = [(move.from_square // 8, move.from_square % 8), (move.to_square // 8, move.to_square % 8)]
        except Exception as e:
            print("Lỗi khi thực hiện nước đi:", e)

        for event in pygame.event.get():
            if event.type == pygame.QUIT:
                running = False

    # Xử lý khi game kết thúc
    if board.is_checkmate():
        winner = "White" if board.turn == chess.BLACK else "Black"
    elif board.is_stalemate():
        winner = "Draw"
    elif board.is_insufficient_material():
        winner = "Draw"
    elif board.is_seventyfive_moves():
        winner = "Draw"
    elif board.is_fivefold_repetition():
        winner = "Draw"
    else:
        winner = "Unknown"
        
    show_game_result(winner)

    # Kết thúc game
    engine1.kill()
    engine2.kill()
    print("Game Over:", board.result())

def get_move_time_from_input():
    root = tk.Tk()
    root.withdraw()
    time_str = simpledialog.askstring("Thời gian suy nghĩ", "Nhập thời gian suy nghĩ mỗi lượt (ms):")
    try:
        return int(time_str)
    except:
        return None
