import pygame
import subprocess
import threading
import chess
import os
import sys
import time
from tkinter import Tk, filedialog, simpledialog
from config import SCREEN, WIDTH, HEIGHT, BOARD_WIDTH, HISTORY_PANEL_WIDTH, WHITE, BLACK, LIGHT_BROWN, DARK_BROWN, HIGHLIGHT_COLOR, SQUARE_SIZE, font, pieces_img

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

def draw_move_history(history, scroll_offset):
    pygame.draw.rect(SCREEN, BLACK, (BOARD_WIDTH, 0, HISTORY_PANEL_WIDTH, HEIGHT))
    y_start = 10 - scroll_offset
    x = BOARD_WIDTH + 5
    fontHis = pygame.font.Font(None, 24)

    count = 0
    y = y_start
    for idx, move in enumerate(history, 1):
        if count == 4:
            count = 0
            y += 20
        if 0 <= y <= HEIGHT - 18:  # Chỉ vẽ nếu trong khung hiển thị
            move_surface = fontHis.render(f"{idx}. {move}", True, WHITE)
            SCREEN.blit(move_surface, (x + count * 90, y))
        count += 1


def draw_clock(white_time, black_time, white_name, black_name):
    fontClock = pygame.font.SysFont(None, 28)

    # Chuyển đổi thời gian sang phút:giây
    def format_time(ms):
        minutes = ms // 60000
        seconds = (ms // 1000) % 60
        return f"{minutes}:{seconds:02d}"

    white_text = fontClock.render(f"(white) {white_name}: {format_time(white_time)}", True, WHITE)
    black_text = fontClock.render(f"(black) {black_name}: {format_time(black_time)}", True, WHITE)

    # Hiển thị ở góc dưới panel bên phải
    SCREEN.blit(white_text, (BOARD_WIDTH + 10, HEIGHT - 60))
    SCREEN.blit(black_text, (BOARD_WIDTH + 10, HEIGHT - 30))


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
                "engine_white_name": engine_white_name.get(),
                "engine_black_name": engine_black_name.get(),
                "move_time": move_time_val,
                "match_time": match_time_val
            }
        except ValueError:
            print("Thời gian nhập không hợp lệ.")
            return None
    return None


def get_engine_move(engine, board_fen, movetime):
    engine.stdin.write(f"position fen {board_fen}\n".encode())
    engine.stdin.write(f"go movetime {movetime}\n".encode())
    engine.stdin.flush()

    while True:
        line = engine.stdout.readline().decode()
        if line.startswith("bestmove"):
            return line.split()[1]

def ai_vs_ai():
    config = show_engine_selection_dialog()
    if not config:
        print("Engine không được chọn đầy đủ.")
        return

    engine1_path = config["engine_white_path"]
    engine2_path = config["engine_black_path"]
    engine1_name = config["engine_white_name"]
    engine2_name = config["engine_black_name"]
    move_time = config["move_time"]
    match_time_ms = config["match_time"] * 60 * 1000


    white_time = match_time_ms
    black_time = match_time_ms

    board = chess.Board()
    history = []
    last_move_squares = None
    scroll_offset = 0
    max_scroll = 0

    engine1 = subprocess.Popen(engine1_path, stdin=subprocess.PIPE, stdout=subprocess.PIPE)
    engine2 = subprocess.Popen(engine2_path, stdin=subprocess.PIPE, stdout=subprocess.PIPE)

    for engine in [engine1, engine2]:
        engine.stdin.write(b"uci\n")
        engine.stdin.flush()
        while True:
            if engine.stdout.readline().decode().strip() == "uciok":
                break

    running = True
    winner = None
    reason = ""

    while running and not board.is_game_over():
        draw_board(board, last_move_squares)
        draw_move_history(history, scroll_offset)
        draw_clock(white_time, black_time, engine1_name, engine2_name)
        pygame.display.flip()

        current_engine = engine1 if board.turn == chess.WHITE else engine2

        start_time = pygame.time.get_ticks()
        move_uci = get_engine_move(current_engine, board.fen(), move_time)
        end_time = pygame.time.get_ticks()
        elapsed_time = end_time - start_time

        # Cập nhật thời gian mỗi bên
        if board.turn == chess.WHITE:
            white_time -= elapsed_time
            if white_time <= 0:
                winner = engine2_name
                reason = "Trắng hết giờ"
                break
        else:
            black_time -= elapsed_time
            if black_time <= 0:
                winner = engine1_name
                reason = "Đen hết giờ"
                break

        try:
            move = chess.Move.from_uci(move_uci)
            if move in board.legal_moves:
                board.push(move)
                history.append(move.uci())
                last_move_squares = [
                    (move.from_square // 8, move.from_square % 8),
                    (move.to_square // 8, move.to_square % 8),
                ]

                # Cập nhật max_scroll sau khi thêm nước đi
                total_lines = (len(history) + 3) // 4
                needed_height = total_lines * 18 + 10
                max_scroll = max(0, needed_height - HEIGHT)

        except Exception as e:
            print("Lỗi khi thực hiện nước đi:", e)

        for event in pygame.event.get():
            if event.type == pygame.QUIT:
                running = False
            elif event.type == pygame.MOUSEWHEEL:
                scroll_offset -= event.y * 20
                scroll_offset = max(0, min(scroll_offset, max_scroll))

    # Vẽ lại bước cuối cùng
    draw_board(board, last_move_squares)
    draw_move_history(history, scroll_offset)
    draw_clock(white_time, black_time, engine1_name, engine2_name)
    pygame.display.flip()
    pygame.time.wait(1000)

    if not winner:
        if board.is_checkmate():
            winner = engine1_name if board.turn == chess.BLACK else engine2_name
        elif board.is_stalemate():
            winner = "Draw"
        elif board.is_insufficient_material():
            winner = "Draw"
        elif board.is_seventyfive_moves():
            winner = "Draw"
        elif board.is_fivefold_repetition():
            winner = "Draw"
        else:
            winner = "Không xác định"

    show_game_result(f"{winner} win")
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

