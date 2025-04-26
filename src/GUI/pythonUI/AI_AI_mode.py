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
from config import move_sound, capture_sound, PIECE_SIZE

font = pygame.font.SysFont(None, 24)
INFO_HEIGHT = 150
HISTORY_Y_START = INFO_HEIGHT

# pieces_img = {}
# PIECE_NAMES = ["bp", "br", "bn", "bb", "bq", "bk", "wp", "wr", "wn", "wb", "wq", "wk"]
# for name in PIECE_NAMES:
#     img = pygame.image.load(os.path.join("images", f"{name}.png"))
#     pieces_img[name] = pygame.transform.scale(img, (SQUARE_SIZE, SQUARE_SIZE))

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



def draw_engine_info(config, white_time, black_time):

    pygame.draw.rect(SCREEN, BLUE, (BOARD_WIDTH, 0, HISTORY_PANEL_WIDTH, INFO_HEIGHT))

    name_font = pygame.font.SysFont(None, 22)
    time_font = pygame.font.SysFont(None, 20)

    white_text = name_font.render(f"{config['white']['name']} (White)", True, WHITE)
    black_text = name_font.render(f"{config['black']['name']} (Black)", True, WHITE)

    white_clock = time_font.render(f"{white_time//60:02d}:{white_time%60:02d}", True, WHITE)
    black_clock = time_font.render(f"{black_time//60:02d}:{black_time%60:02d}", True, WHITE)

    SCREEN.blit(white_text, (BOARD_WIDTH + 10, 10))
    SCREEN.blit(white_clock, (BOARD_WIDTH + 10, 35))
    SCREEN.blit(black_text, (BOARD_WIDTH + 10, 70))
    SCREEN.blit(black_clock, (BOARD_WIDTH + 10, 95))
    
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


def get_engine_move(engine, fen, white_time, black_time, move_time):
    engine.stdin.write(f"position fen {fen}\n".encode())
    
    engine.stdin.write(f"go wtime {white_time * 1000} btime {black_time * 1000} winc 0 binc 0\n".encode())
    engine.stdin.flush()
    
    time.sleep(0.1)
    response = engine.stdout.readline().decode().strip()
    
    while not response.startswith("bestmove"):
        response = engine.stdout.readline().decode().strip()
    best_move = response.split()[1]
    
    return best_move

def ai_vs_ai():
    config = show_engine_selection_dialog()
    if not config:
        print("Engine không được chọn đầy đủ.")
        return

    board = chess.Board()
    max_scroll = 0
    last_move_squares = None
    captured_white = []
    captured_black = []

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

    clock = pygame.time.Clock()
    scroll_offset = 0
    pause_button_rect = pygame.Rect(BOARD_WIDTH + 20, HEIGHT - 60, 100, 30)
    resume_button_rect = pygame.Rect(BOARD_WIDTH + 130, HEIGHT - 60, 100, 30)
    is_paused = False

    font_button = pygame.font.SysFont(None, 24)
    last_time_check = time.time()  # Thời gian lần cuối kiểm tra để cập nhật clock

    running = True
    while running and not board.is_game_over(claim_draw=True):
        current_time = time.time()
        elapsed_real = current_time - last_time_check

        # Giảm thời gian theo thời gian thực nếu không tạm dừng
        if not is_paused and elapsed_real >= 1:
            if board.turn == chess.WHITE:
                white_remaining = max(0, white_remaining - int(elapsed_real))
            else:
                black_remaining = max(0, black_remaining - int(elapsed_real))
            last_time_check = current_time

        if is_paused:
            pygame.time.wait(100)
            for event in pygame.event.get():
                if event.type == pygame.QUIT:
                    running = False
                elif event.type == pygame.MOUSEBUTTONDOWN:
                    if resume_button_rect.collidepoint(event.pos):
                        is_paused = False
                        last_time_check = time.time()
            continue

        draw_board(board, last_move_squares)
        draw_engine_info({
            "white": {"name": config["engine_white_name"]},
            "black": {"name": config["engine_black_name"]},
        }, white_remaining, black_remaining)
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

        for event in pygame.event.get():
            if event.type == pygame.QUIT:
                running = False
            elif event.type == pygame.MOUSEBUTTONDOWN:
                if pause_button_rect.collidepoint(event.pos):
                    is_paused = True
                elif resume_button_rect.collidepoint(event.pos):
                    is_paused = False
                    last_time_check = time.time()
            elif event.type == pygame.MOUSEWHEEL:
                # Cuộn thanh trượt lịch sử nước đi
                scroll_offset = max(0, min(scroll_offset - event.y, max_scroll))

        clock.tick(60)

    # Hiển thị bàn cờ cuối cùng nếu chiếu hết
    if board.is_checkmate():
        draw_board(board, last_move_squares)
        draw_engine_info({
            "white": {"name": config["engine_white_name"]},
            "black": {"name": config["engine_black_name"]},
        }, white_remaining, black_remaining)
        pygame.display.flip()
        pygame.time.wait(500)

    # Kết thúc trận đấu
    if board.is_checkmate():
        winner = "White" if board.turn == chess.BLACK else "Black"
    elif board.is_stalemate() or board.is_insufficient_material() or board.is_seventyfive_moves() or board.is_fivefold_repetition():
        winner = "Draw"
    else:
        if (white_remaining == 0 or black_remaining == 0): winner = "Drawn"
        else: winner = "Unknown"

    engine1.kill()
    engine2.kill()
    print("Game Over:", board.result())
    result = show_game_result(winner)
    captured_white.clear()
    captured_black.clear()
    if result == 'new_game':
        ai_vs_ai()



# def get_move_time_from_input():
#     root = tk.Tk()
#     root.withdraw()
#     time_str = simpledialog.askstring("Thời gian suy nghĩ", "Nhập thời gian suy nghĩ mỗi lượt (ms):")
#     try:
#         return int(time_str)
#     except:
#         return None

