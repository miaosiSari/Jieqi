from board import board
import time
B = board.Board()
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