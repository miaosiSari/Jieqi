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
#include <string.h>
#include <assert.h>
#include <stdio.h>
#include <ctype.h>
#include "../log/log.h"
#include "../global/global.h"
#include "../score/score.h"

template <typename T, typename U, typename V>
bool GreaterTuple(const std::tuple<T, U, V> &i, const std::tuple<T, U, V> &j) {
        return i > j;
}

extern std::unordered_map<std::string, SCORE> function_bean;

template <typename K, typename V>
extern V GetWithDefUnordered(const std::unordered_map<K,V>& m, const K& key, const V& defval);


namespace board{
class Board{

public:
   int num_of_legal_moves = 0;
   Board() noexcept;
   Board(const char another_state[MAX], bool turn, int round) noexcept;
   Board(const Board& another_board);
   void Reset() noexcept;
   void SetScoreFunction(std::string function_name);
   const std::vector<std::string>& GetHistory() const;
   std::vector<std::string> GetStateString() const;
   bool GetTurn() const;
   void SetTurn(bool turn);
   bool GetRound() const;
   std::tuple<int, bool, std::string, std::string> GetTuple() const;
   const std::unordered_map<std::string, std::string>& GetUniPieces() const;
   void PrintPos(bool turn) const;
   void Move(const std::pair<int, int> start, const std::pair<int, int> end, const bool = false); //start(x1, y1), end(x2, y2)
   void Move(const std::string ucci, const bool = false); //ucci representation
   void Move(const char* ucci, const bool = false);
   void Move(const int x1, const int y1, const int x2, const int y2, const bool = false);
   void GenMovesWithScore();
   void CopyToIsLegalMove();
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
       for(int i = 256; i < MAX; ++i){
           p[i] = '\0';
       }
   };

   std::tuple<unsigned short, unsigned char, unsigned char> legal_moves[MAX_POSSIBLE_MOVES];
   static void Translate(unsigned char i, unsigned char j, char ucci[5]);
   static void Print_ij_ucci(unsigned char i, unsigned char j);
   void PrintAllMoves();
   
private:
   char _state_red[MAX];
   char _state_black[MAX];
   bool _has_initialized = false;
   bool _turn = false; //true红black黑
   int _round = 0; //回合, 从0开始
   bool _is_legal_move[MAX][MAX];
   std::vector<std::string> _cur_legal_moves;
   std::vector<std::string> _board_history;
   static const int _chess_board_size;
   static const char _initial_state[MAX];
   static const std::unordered_map<std::string, std::string> _uni_pieces;
   static char _dir[91][8];
   logclass::Log* _log  = NULL;
   SCORE _score_func = NULL;
   std::function<std::string(const char)> _getstring = [](const char c) -> std::string {
        std::string ret;
        const std::string c_string(1, c);
        ret = GetWithDefUnordered<std::string, std::string>(_uni_pieces, c_string, c_string);
        return ret;
   };
   std::function<std::string(int, int, bool)> _getstringxy = [this](int x, int y, bool turn) -> std::string {
        std::string ret =  turn?_getstring(_state_red[encode(x, y)]):_getstring(_state_black[encode(x, y)]);
        return ret;
   };
   void _initialize_dir();
};
}

#endif
