import pygame
import subprocess
import threading
import chess
import os
import sys
import time
import tkinter as tk
from tkinter import filedialog, simpledialog

from config import SCREEN, WIDTH, HEIGHT, BOARD_WIDTH, HISTORY_PANEL_WIDTH
from config import WHITE, BLACK, LIGHT_BROWN, DARK_BROWN, BLUE, HIGHLIGHT_COLOR, SQUARE_SIZE, font, pieces_img
from config import move_sound, capture_sound, PIECE_SIZE

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

def human_vs_ai():
    setting = show_human_vs_ai_dialog()
    if not setting:
        print("Chưa đủ thông tin.")
        return
    
    board = chess.Board()
    last_move_squares = None
    captured_white = []
    captured_black = []
    
    engine = subprocess.Popen(setting["engine_path"], stdin=subprocess.PIPE, stdout=subprocess.PIPE)