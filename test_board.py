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
board, mapping = B.random_board()
B.print_board(board)
B.uncover_board(board, mapping)
B.copy_board(board=board)
print(B.search_kings())
b = B.initialize_another_board()
b[0][5] = 13
b[9][5] = 5
print(B.search_kings(b))