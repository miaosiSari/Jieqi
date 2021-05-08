from board import board
import time
B = board.Board()
x = time.time()
for cnt in range(1000000):
    B.print_initial_state()
    B.initialize()
