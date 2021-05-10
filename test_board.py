from board import board
import time
B = board.Board()
B.print_initial_state()
'''
x = time.time()
for cnt in range(1000000):
    B.print_initial_state()
    B.initialize()
print(time.time() - x)
B.copy_board(**{'turn': False})
print(B)
'''
#board, mapping = B.random_board()
'''
B.print_board(board)
B.uncover_board(board, mapping)
B.copy_board(board=board)
print(B.search_kings())
b = B.initialize_another_board()
b[0][5] = 13
b[9][5] = 5
print(B.search_kings(b))
'''
result = B.check_legal_and_jiangjun((0, 1), (2, 0))
print(result)
result = B.check_legal_and_jiangjun((2, 1), (8, 1))
print(result)
B.stupid_print_all_legal_moves()
board, mapping = B.random_board()
B.copy_board(board=board, mapping=mapping)
B.stupid_print_all_legal_moves()
B.turn = not B.turn
B.stupid_print_all_legal_moves()
board = B.initialize_another_board()
board[0][2] = 2
board[1][2] = 9
board[2][4] = 13
board[7][3] = 5
B.copy_board(board=board)
B.print_board()
B.stupid_print_all_legal_moves()


