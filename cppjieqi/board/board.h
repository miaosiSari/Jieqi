/*
* Created by Si Miao 2021/07/31
* Copyright (c) 2021. All rights reserved.
* Last modified 2021/07/31
*/
#ifndef board_h
#define board_h
#define MAX 257
#define CHESS_BOARD_SIZE 256
#define MAX_POSSIBLE_MOVES 120
#define A0 195 //(0, 0)坐标
#define I0 203 //(0, 8)坐标
#define A9 51 //(9, 0)坐标
#define I9 59 //(9, 8)坐标
#define NORTH -16
#define EAST 1
#define SOUTH 16
#define WEST -1

#include <cstddef>
#include <vector>
#include <string>
#include <tuple>
#include <unordered_map>
#include <iostream>
#include <functional>
#include <algorithm>
#include <random>
#include <chrono>
#include <memory>
#include <string.h>
#include <assert.h>
#include <stdio.h>
#include <ctype.h>
#include "../log/log.h"
#include "../global/global.h"
#include "../score/score.h"


#define TXY(x, y) (unsigned char)translate_x_y(x, y)
#define SV(vector) shuffle(vector.begin(), vector.end(), std::default_random_engine(std::chrono::system_clock::now().time_since_epoch().count()))
#define FIND(c, place, perspective) \
if(c == '.'){ \
    eat_type_tmp = 0; \
}else if(std::string("DEFGHIdefghi").find(c) != std::string::npos){ \
    c = random_map[perspective][place]; \
    eat_type_tmp = 2; \
}else{ \
    eat_type_tmp = 1; \
}
#define TRANSLATE_PLACE_TO_COVERED(place) \
char covered = '.'; \
switch(place){ \
   case 195: \
   case 203: covered = 'D'; break; \
   case 196: \
   case 202: covered = 'E'; break; \
   case 197: \
   case 201: covered = 'F'; break; \
   case 198: \
   case 200: covered = 'G'; break; \
   case 164: \
   case 170: covered = 'H'; break; \
   case 147: \
   case 149: \
   case 151: \
   case 153: \
   case 155: covered = 'I'; break; \
   case 59: \
   case 51: covered = 'D'; break; \
   case 58: \
   case 52: covered = 'e'; break; \
   case 57: \
   case 53: covered = 'f'; break; \
   case 56: \
   case 54: covered = 'g'; break; \
   case 90: \
   case 84: covered = 'h'; break; \
   case 107: \
   case 105: \
   case 103: \
   case 101: \
   case 99: covered = 'i'; break; \
} \

namespace board{
class Board{
public:
    bool finished = false;
    int num_of_legal_moves = 0;
    char state_red[MAX];
    char state_black[MAX];
    bool turn; //true红black黑
    int round; //回合, 从0开始
    Board() noexcept;
    Board(const char another_state[MAX], bool turn, int round) noexcept;
    Board(const Board& another_board);
    void Reset() noexcept;
    const std::vector<std::string>& GetHistory() const;
    std::vector<std::string> GetStateString() const;
    bool GetTurn() const;
    void SetTurn(bool turn);
    bool GetRound() const;
    std::tuple<int, bool, std::string, std::string> GetTuple() const;
    const std::unordered_map<std::string, std::string>& GetUniPieces() const;
    void PrintPos(bool turn, bool iscovered, bool god, bool swapcasewhenblack) const;
    std::shared_ptr<InfoDict> Move(const std::pair<int, int> start, const std::pair<int, int> end, const bool = false); //start(x1, y1), end(x2, y2)
    std::shared_ptr<InfoDict> Move(const std::string ucci, const bool = false); //ucci representation
    std::shared_ptr<InfoDict> Move(const char* ucci, const bool = false);
    std::shared_ptr<InfoDict> Move(const int x1, const int y1, const int x2, const int y2, const bool = false);
    void GenMovesWithScore();
    void CopyToIsLegalMove();
    void GenerateRandomMap();
    void PrintRandomMap(bool turn);
    std::function<int(int)> translate_x = [](const int x) -> int {return 12 - x;};
    std::function<int(int)> translate_y = [](const int y) -> int {return 3 + y;};
    std::function<int(int, int)> translate_x_y = [](const int x, const int y) -> int{return 195 - 16 * x + y;};
    std::function<int(int, int)> encode = [](const int x, const int y) -> int {return 16 * x + y;};  
    std::function<int(int)> reverse = [](const int x) -> int {return 254 - x;};
    std::function<char(char)> swapcase = [](const char c) -> char{
       if(isalpha(c)) {
           return c ^ 32;
       }
       return c;
    };

    std::function<void(char*)> rotate = [this](char* p){
        std::reverse(p, p+255);
        std::transform(p, p+255, p, this -> swapcase);
        p[255] = ' ';
        memset(p + 256, 0, (MAX - 256) * sizeof(char));
    };

    std::function<std::string(int, int, bool, bool, bool)> _getstringxy = [this](int x, int y, bool turn, bool iscovered, bool swapcasewhenblack) -> std::string {
        return iscovered?_getstringxy_covered(x, y, turn, swapcasewhenblack):_getstringxy_uncovered(x, y, turn, swapcasewhenblack);
    };

    std::tuple<unsigned short, unsigned char, unsigned char> legal_moves[MAX_POSSIBLE_MOVES];
    std::unordered_map<bool, std::unordered_map<unsigned char, char>> random_map;
    static void Translate(unsigned char i, unsigned char j, char ucci[5]);
    static void TranslateSingle(unsigned char i, char ucci[3]);
    static void Print_ij_ucci(unsigned char i, unsigned char j);
    void PrintAllMoves();
   
private:
    bool _has_initialized;
    bool _is_legal_move[MAX][MAX];
    std::vector<std::string> _cur_legal_moves;
    std::vector<std::string> _board_history;
    static const int _chess_board_size;
    static const char _initial_state[MAX];
    static const std::unordered_map<std::string, std::string> _uni_pieces;
    static char _dir[91][8];
    std::function<std::string(const char)> _getstring = [](const char c) -> std::string {
        std::string ret;
        const std::string c_string(1, c);
        ret = GetWithDefUnordered<std::string, std::string>(_uni_pieces, c_string, c_string);
        return ret;
    };
    std::function<std::string(int, int, bool, bool)> _getstringxy_covered = [this](int x, int y, bool turn, bool swapcasewhenblack) -> std::string {
        char c = turn?state_red[encode(x, y)]:state_black[encode(x, y)];
        c = (swapcasewhenblack && !turn)?swapcase(c):c;
        std::string ret =  _getstring(c);
        return ret;
    };
    std::function<std::string(int, int, bool, bool)> _getstringxy_uncovered = [this](int x, int y, bool turn, bool swapcasewhenblack) -> std::string {
        char c = turn?state_red[encode(x, y)]:state_black[encode(x, y)];
        if(std::string("DEFGHIdefghi").find(c) != std::string::npos){
            c = random_map[turn][encode(x, y)];
        }
        c = (swapcasewhenblack && !turn)?swapcase(c):c;
        return _getstring(c);
    };
    void _initialize_dir();
};
}

#endif
