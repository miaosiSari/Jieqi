/*
* Created by Si Miao 2021/07/31
* Copyright (c) 2021. All rights reserved.
* Last modified 2021/07/31
*/
#ifndef board_h
#define board_h
#define MAX 400
#define CHESS_BOARD_SIZE 256
#define MAX_POSSIBLE_MOVES 120
#define A0 195 //(0, 0)坐标
#define I0 203 //(0, 8)坐标
#define A9 51 //(9, 0)坐标
#define

#include <cstddef>
#include <vector>
#include <string>
#include <tuple>
#include <unordered_map>
#include <iostream>
#include <functional>
#include <string.h>
#include <assert.h>
#include <stdio.h>
#include "../log/log.h"
#include "../global/global.h"
#include "../score/score.h"

extern std::unordered_map<std::string, SCORE> function_bean;

template <typename K, typename V>
extern V GetWithDefUnordered(const std::unordered_map<K,V>& m, const K& key, const V& defval);

namespace board{
class Board{

public:
   Board() noexcept;
   Board(char another_state[MAX], bool turn, int stage) noexcept;
   Board(const Board& another_board);
   void Reset() noexcept;
   std::vector<int> GetInfo() const;
   const std::vector<std::string>& GetHistory() const;
   std::string GetStateString() const;
   bool GetTurn() const;
   bool GetRound() const;
   std::tuple<int, bool, std::string> GetTuple() const;
   const std::unordered_map<std::string, std::string>& GetUniPieces() const;
   void PrintPos() const;
   void Move(const std::pair<int, int> start, const std::pair<int, int> end, const bool = false); //start(x1, y1), end(x2, y2)
   void Move(const std::string ucci, const bool = false); //ucci representation
   void Move(const char* ucci, const bool = false);
   void Move(const int x1, const int y1, const int x2, const int y2, const bool = false);
   void GenMovesWithScore();
   std::function<int(int)> translate_x = [](const int x) -> int {return 12 - x;};
   std::function<int(int)> translate_y = [](const int y) -> int {return 3 + y;};
   std::function<int(int, int)> translate_x_y = [](const int x, const int y) -> int{return 195 - 16 * x + y;};
   std::function<int(int, int)> encode = [](const int x, const int y) -> int {return 16 * x + y;};  
   std::tuple<unsigned short, unsigned char, unsigned char> moves[MAX_POSSIBLE_MOVES];
   const int get_number_of_legal_moves() const;
   
private:
   char _state[MAX];
   bool _has_initialized;
   bool _turn; //true红black黑
   int _round; //回合, 从0开始
   bool _is_legal_move[MAX][MAX];
   std::vector<std::string> _cur_legal_moves;
   std::vector<std::string> _board_history;
   static const int _chess_board_size;
   static const char _initial_state[MAX];
   static const std::unordered_map<std::string, std::string> _uni_pieces;
   int _num_of_legal_moves;
   std::tuple<unsigned short, unsigned char, unsigned char> moves[MAX_POSSIBLE_MOVES];
   log::Log* _log;
   SCORE *_score_func;
   std::function<std::string(const char)> _getstring = [](const char c) -> std::string {
        std::string ret;
        const std::string c_string(1, c);
        ret = GetWithDefUnordered<std::string, std::string>(_uni_pieces, c_string, c_string);
        return ret;
   };
   std::function<std::string(int, int)> _getstringxy = [this](int x, int y) -> std::string {
        return _getstring(_state[encode(x, y)]);
   };
   inline void _reset_num_of_legal_moves() const;
};
}

#endif