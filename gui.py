import subprocess
import sys
from subprocess import PIPE
from threading import Thread
from queue import Queue
from collections import namedtuple

import pygame
import pygame.freetype
import sys
from pygame.locals import QUIT, KEYDOWN, MOUSEBUTTONDOWN, MOUSEBUTTONUP

RED_CHESS_COLOR = [255, 255, 255]
BLACK_CHESS_COLOR = [0, 0, 0]
CHESS_NAMES = {'车', '马', '炮', '象', '士', '兵', '帅',
               '俥', '傌', '相', '仕', '卒', '将', '暗'}
if sys.platform == 'linux':
    font_path = '/usr/share/fonts/truetype/arphic/ukai.ttc'
else:
    font_path = 'C:/Windows/Fonts/simkai.ttf'

play_process = subprocess.Popen(
    ['python',
     'C:\\Users\\anpro\\Desktop\\Jieqi-main\\musesfish_pvs_20210815.py'],
    stdin=PIPE, stdout=PIPE)


class ChessInfo:
    def __init__(self, rect, draw_metadata, cmd_pos):
        self.rect = rect
        self.draw_metadata = draw_metadata
        self.cmd_pos = cmd_pos


class Board:
    def __init__(self, stdout, font, screen, screen_color, line_color,
                 width=500):
        self.stdout = stdout
        self.font = font
        self.screen = screen
        self.screen_color = screen_color
        self.line_color = line_color
        self.width = width
        self.row_spacing = width / 10
        self.col_spacing = width / 8
        self.start_point = 60, 60
        self.start_point_x = self.start_point[0]
        self.start_point_y = self.start_point[1]
        self.chesses = []
        self.empty_chess_rects = []
        self.current_select_chess = None

    def draw_board(self):
        x, y = self.start_point
        for i in range(10):
            pygame.draw.line(self.screen, self.line_color, [x, y],
                             [x + self.width, y])
            y += self.row_spacing

        x, y = self.start_point
        for i in range(9):
            pygame.draw.line(self.screen, self.line_color, [x, y],
                             [x, y + self.row_spacing * 9])
            x += self.col_spacing

        x, y = self.start_point_x + self.col_spacing, \
               self.start_point_y + self.row_spacing * 4
        for i in range(7):
            pygame.draw.line(self.screen, self.screen_color, [x, y],
                             [x, y + self.row_spacing])
            x += self.col_spacing

    def get_chess_pos(self, row, col):
        center = [self.start_point_x + self.col_spacing * (col - 1),
                  self.start_point_y + self.row_spacing * (row - 1)]
        radius = self.row_spacing / 2
        return center, radius

    def draw_a_chess(self, row, col, chess_color, chess_name):
        center, radius = self.get_chess_pos(row, col)
        rect = pygame.draw.circle(self.screen, chess_color, center, radius)

        if chess_color == BLACK_CHESS_COLOR:
            font_color = [255, 255, 255]
        elif chess_color == RED_CHESS_COLOR:
            font_color = [255, 0, 0]
        else:
            font_color = [0, 0, 0]
        self.font.render_to(self.screen, [center[0]-5,center[1]-5], chess_name, font_color)
        return rect

    def draw(self):

        while 1:
            line = self.stdout.get().decode().strip()
            print(line)

            if line == '电脑吃子:':
                self.screen.fill(self.screen_color)
                self.draw_board()
                self.chesses = []
                self.empty_chess_rects = []
                self.current_select_chess = None

            if not line or line[0] not in {
                '0', '1', '2', '3', '4', '5', '6', '7', '8', '9'}:
                continue

            chess_color = RED_CHESS_COLOR if '\033[31m' in line else BLACK_CHESS_COLOR
            row = 10 - int(line[0])
            col = 1
            for name in line:
                if name == '．':
                    center, radius = self.get_chess_pos(row, col)
                    self.empty_chess_rects.append(
                        ChessInfo(
                            pygame.Rect(center[0] - radius,
                                        center[1] - radius,
                                        radius * 2,
                                        radius * 2),
                            None,
                            f'{chr(col + 96)}{line[0]}'))
                    col += 1
                    continue

                if name not in CHESS_NAMES:
                    continue

                params = row, col, chess_color, name
                chess_rect = self.draw_a_chess(*params)
                self.chesses.append(
                    ChessInfo(chess_rect, params, f'{chr(col + 96)}{line[0]}'))
                col += 1

            pygame.display.update()

    def select(self, chess):
        params = list(chess.draw_metadata)
        params[2] = [128, 128, 128]
        self.draw_a_chess(*params)
        self.current_select_chess = chess

    def move(self, pos):
        global play_process

        play_process.stdin.write((pos + '\n').encode())
        play_process.stdin.flush()


def read_stdout(result: Queue, play_process):
    while 1:
        if play_process.poll():
            print('Read_stdout quit')
            return

        result.put(play_process.stdout.readline(), timeout=3)


def main():
    stdout = Queue(1024)
    Thread(target=read_stdout, args=(stdout, play_process),
           daemon=True).start()

    # 初始化pygame
    pygame.init()
    # 获取对显示系统的访问，并创建一个窗口screen
    # 窗口大小为670x670
    font = pygame.freetype.Font(font_path, 20)
    width = 670
    height = 670
    screen = pygame.display.set_mode((width, height))
    screen_color = [238, 154, 73]  # 设置画布颜色,[238,154,73]对应为棕黄色
    line_color = [0, 0, 0]  # 设置线条颜色，[0,0,0]对应黑色

    board = Board(stdout, font, screen, screen_color, line_color)
    screen.fill(screen_color)  # 清屏
    Thread(target=board.draw, daemon=True).start()

    while 1:
        for event in pygame.event.get():
            if event.type in (QUIT, KEYDOWN):
                play_process.kill()
                sys.exit()
            elif event.type == MOUSEBUTTONDOWN:
                if event.button != 1:
                    continue

                for chess in board.chesses:
                    if chess.rect.collidepoint(event.pos):
                        # 鼠标在棋子上
                        if not board.current_select_chess:
                            # 选中棋子
                            board.select(chess)
                        else:
                            # 取消之前棋子的选中，再选中棋子
                            # board.draw_a_chess(
                            #     *board.current_select_chess.draw_metadata)
                            # board.select(chess)
                            board.move(
                                f'{board.current_select_chess.cmd_pos}'
                                f'{chess.cmd_pos}')
                        break
                else:
                    # 鼠标不在棋子上
                    if board.current_select_chess:
                        for chess in board.empty_chess_rects:
                            if chess.rect.collidepoint(event.pos):
                                board.move(
                                    f'{board.current_select_chess.cmd_pos}'
                                    f'{chess.cmd_pos}')

            pygame.display.update()


if __name__ == '__main__':
    main()
