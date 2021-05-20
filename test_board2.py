from board import board
import time 
import traceback

B = board.Board()
for cnt in range(200):
    rb, mapping, legal_moves = B.generate(turn=True, check=False, file='board_RED_%s.json'%cnt)
    rb, mapping, legal_moves = B.generate(turn=False, check=True, file='board_BLACK_%s.json'%cnt)
#print(legal_moves)
