from board import board
import time 
import traceback

B = board.Board()
#rb, mapping, legal_moves = B.generate(turn=True, check=False, file='board_RED.json')
rb, mapping, legal_moves = B.generate(turn=False, check=True, file='board_BLACK.json')
#print(legal_moves)
