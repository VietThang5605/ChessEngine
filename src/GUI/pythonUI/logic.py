# import pygame
# from config import SCREEN
# from HUMAN_AI_mode import color, black_options, white_options, counter

# en_passant_possible = ()

# white_pieces = ['rook', 'knight', 'bishop', 'king', 'queen', 'bishop', 'knight', 'rook',
#                 'pawn', 'pawn', 'pawn', 'pawn', 'pawn', 'pawn', 'pawn', 'pawn']
# white_locations = [(0, 0), (1, 0), (2, 0), (3, 0), (4, 0), (5, 0), (6, 0), (7, 0),
#                    (0, 1), (1, 1), (2, 1), (3, 1), (4, 1), (5, 1), (6, 1), (7, 1)]
# black_pieces = ['rook', 'knight', 'bishop', 'king', 'queen', 'bishop', 'knight', 'rook',
#                 'pawn', 'pawn', 'pawn', 'pawn', 'pawn', 'pawn', 'pawn', 'pawn']
# black_locations = [(0, 7), (1, 7), (2, 7), (3, 7), (4, 7), (5, 7), (6, 7), (7, 7),
#                    (0, 6), (1, 6), (2, 6), (3, 6), (4, 6), (5, 6), (6, 6), (7, 6)]

# # function to check all pieces valid options on board
# def check_options(pieces, locations, turn):
#     moves_list = []
#     all_moves_list = []
#     for i in range((len(pieces))):
#         location = locations[i]
#         piece = pieces[i]
#         if piece == 'pawn':
#             moves_list = check_pawn(location, turn)
#         elif piece == 'rook':
#             moves_list = check_rook(location, turn)
#         elif piece == 'knight':
#             moves_list = check_knight(location, turn)
#         elif piece == 'bishop':
#             moves_list = check_bishop(location, turn)
#         elif piece == 'queen':
#             moves_list = check_queen(location, turn)
#         elif piece == 'king':
#             moves_list = check_king(location, turn)
#         all_moves_list.append(moves_list)
#     return all_moves_list

# # check king valid moves
# def check_king(position, color):
#     moves_list = []
#     if color == 'white':
#         enemies_list = black_locations
#         friends_list = white_locations
#     else:
#         friends_list = black_locations
#         enemies_list = white_locations
#     # 8 squares to check for kings, they can go one square any direction
#     targets = [(1, 0), (1, 1), (1, -1), (-1, 0),
#                (-1, 1), (-1, -1), (0, 1), (0, -1)]
#     for i in range(8):
#         target = (position[0] + targets[i][0], position[1] + targets[i][1])
#         if target not in friends_list and 0 <= target[0] <= 7 and 0 <= target[1] <= 7:
#             moves_list.append(target)
#     return moves_list

# # check queen valid moves
# def check_queen(position, color):
#     moves_list = check_bishop(position, color)
#     second_list = check_rook(position, color)
#     for i in range(len(second_list)):
#         moves_list.append(second_list[i])
#     return moves_list

# # check bishop moves
# def check_bishop(position, color):
#     moves_list = []
#     if color == 'white':
#         enemies_list = black_locations
#         friends_list = white_locations
#     else:
#         friends_list = black_locations
#         enemies_list = white_locations
#     for i in range(4):  # up-right, up-left, down-right, down-left
#         path = True
#         chain = 1
#         if i == 0:
#             x = 1
#             y = -1
#         elif i == 1:
#             x = -1
#             y = -1
#         elif i == 2:
#             x = 1
#             y = 1
#         else:
#             x = -1
#             y = 1
#         while path:
#             if (position[0] + (chain * x), position[1] + (chain * y)) not in friends_list and \
#                     0 <= position[0] + (chain * x) <= 7 and 0 <= position[1] + (chain * y) <= 7:
#                 moves_list.append(
#                     (position[0] + (chain * x), position[1] + (chain * y)))
#                 if (position[0] + (chain * x), position[1] + (chain * y)) in enemies_list:
#                     path = False
#                 chain += 1
#             else:
#                 path = False
#     return moves_list

# # check rook moves
# def check_rook(position, color):
#     moves_list = []
#     if color == 'white':
#         enemies_list = black_locations
#         friends_list = white_locations
#     else:
#         friends_list = black_locations
#         enemies_list = white_locations
#     for i in range(4):  # down, up, right, left
#         path = True
#         chain = 1
#         if i == 0:
#             x = 0
#             y = 1
#         elif i == 1:
#             x = 0
#             y = -1
#         elif i == 2:
#             x = 1
#             y = 0
#         else:
#             x = -1
#             y = 0
#         while path:
#             if (position[0] + (chain * x), position[1] + (chain * y)) not in friends_list and \
#                     0 <= position[0] + (chain * x) <= 7 and 0 <= position[1] + (chain * y) <= 7:
#                 moves_list.append(
#                     (position[0] + (chain * x), position[1] + (chain * y)))
#                 if (position[0] + (chain * x), position[1] + (chain * y)) in enemies_list:
#                     path = False
#                 chain += 1
#             else:
#                 path = False
#     return moves_list

# # check valid pawn moves
# def check_pawn(position, color):
#     moves_list = []
#     if color == 'white':
#         if (position[0], position[1] + 1) not in white_locations and \
#                 (position[0], position[1] + 1) not in black_locations and position[1] < 7:
#             moves_list.append((position[0], position[1] + 1))
#         if (position[0], position[1] + 2) not in white_locations and \
#                 (position[0], position[1] + 2) not in black_locations and position[1] == 1:
#             moves_list.append((position[0], position[1] + 2))
#         if (position[0] + 1, position[1] + 1) in black_locations:
#             moves_list.append((position[0] + 1, position[1] + 1))
#         if (position[0] - 1, position[1] + 1) in black_locations:
#             moves_list.append((position[0] - 1, position[1] + 1))
#     else:
#         if (position[0], position[1] - 1) not in white_locations and \
#                 (position[0], position[1] - 1) not in black_locations and position[1] > 0:
#             moves_list.append((position[0], position[1] - 1))
#         if (position[0], position[1] - 2) not in white_locations and \
#                 (position[0], position[1] - 2) not in black_locations and position[1] == 6:
#             moves_list.append((position[0], position[1] - 2))
#         if (position[0] + 1, position[1] - 1) in white_locations:
#             moves_list.append((position[0] + 1, position[1] - 1))
#         if (position[0] - 1, position[1] - 1) in white_locations:
#             moves_list.append((position[0] - 1, position[1] - 1))
#     return moves_list

# # check valid knight moves
# def check_knight(position, color):
#     moves_list = []
#     if color == 'white':
#         enemies_list = black_locations
#         friends_list = white_locations
#     else:
#         friends_list = black_locations
#         enemies_list = white_locations
#     # 8 squares to check for knights, they can go two squares in one direction and one in another
#     targets = [(1, 2), (1, -2), (2, 1), (2, -1),
#                (-1, 2), (-1, -2), (-2, 1), (-2, -1)]
#     for i in range(8):
#         target = (position[0] + targets[i][0], position[1] + targets[i][1])
#         if target not in friends_list and 0 <= target[0] <= 7 and 0 <= target[1] <= 7:
#             moves_list.append(target)
#     return moves_list

# def check_promotion():
#     # White promotion
#     for i in range(len(white_pieces)):
#         if white_pieces[i] == 'pawn' and white_locations[i][1] == 7:
#             white_pieces[i] = 'queen'
    
#     # Black promotion
#     for i in range(len(black_pieces)):
#         if black_pieces[i] == 'pawn' and black_locations[i][1] == 0:
#             black_pieces[i] = 'queen'

# def check_castling(selection, destination):
#     global white_locations, black_locations

#     # White King Side Castling
#     if turn_step < 2:
#         if white_pieces[selection] == 'king':
#             start = white_locations[selection]
#             if destination == (start[0] + 2, start[1]):
#                 # Move rook
#                 rook_index = white_locations.index((7, 0))
#                 white_locations[rook_index] = (5, 0)
#             elif destination == (start[0] - 2, start[1]):
#                 rook_index = white_locations.index((0, 0))
#                 white_locations[rook_index] = (3, 0)

#     # Black King Side Castling
#     else:
#         if black_pieces[selection] == 'king':
#             start = black_locations[selection]
#             if destination == (start[0] + 2, start[1]):
#                 rook_index = black_locations.index((7, 7))
#                 black_locations[rook_index] = (5, 7)
#             elif destination == (start[0] - 2, start[1]):
#                 rook_index = black_locations.index((0, 7))
#                 black_locations[rook_index] = (3, 7)

# def check_en_passant(selection, destination):
#     global white_locations, black_locations, white_pieces, black_pieces
#     if turn_step < 2:  # White turn
#         if white_pieces[selection] == 'pawn':
#             if destination == en_passant_possible:
#                 for i in range(len(black_locations)):
#                     if black_locations[i][0] == destination[0] and black_locations[i][1] == destination[1] - 1:
#                         black_locations.pop(i)
#                         black_pieces.pop(i)
#                         break
#     else:  # Black turn
#         if black_pieces[selection] == 'pawn':
#             if destination == en_passant_possible:
#                 for i in range(len(white_locations)):
#                     if white_locations[i][0] == destination[0] and white_locations[i][1] == destination[1] + 1:
#                         white_locations.pop(i)
#                         white_pieces.pop(i)
#                         break

# # check for valid moves for just selected piece
# def check_valid_moves():
#     if turn_step < 2:
#         options_list = white_options
#     else:
#         options_list = black_options
#     valid_options = options_list[selection]
#     return valid_options

# # draw valid moves on screen
# def draw_valid(moves):
#     color = 'red'
#     for i in range(len(moves)):
#         pygame.draw.circle(
#             SCREEN, color, (moves[i][0] * 100 + 50, moves[i][1] * 100 + 50), 5)
        
# # draw a flashing square around king if in check
# def draw_check():
#     if color == 0:
#         if 'king' in white_pieces:
#             king_index = white_pieces.index('king')
#             king_location = white_locations[king_index]
#             for i in range(len(black_options)):
#                 if king_location in black_options[i]:
#                     if counter < 15:
#                         pygame.draw.rect(SCREEN, 'dark red', [white_locations[king_index][0] * 100 + 1,
#                                                               white_locations[king_index][1] * 100 + 1, 100, 100], 5)
#     else:
#         if 'king' in black_pieces:
#             king_index = black_pieces.index('king')
#             king_location = black_locations[king_index]
#             for i in range(len(white_options)):
#                 if king_location in white_options[i]:
#                     if counter < 15:
#                         pygame.draw.rect(SCREEN, 'dark blue', [black_locations[king_index][0] * 100 + 1,
#                                                                black_locations[king_index][1] * 100 + 1, 100, 100], 5)