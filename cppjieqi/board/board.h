/*
* Created by Si Miao 2021/07/31
* Copyright (c) 2021. All rights reserved.
* Last modified 2021/07/31
*/
#ifndef board_h
#define board_h
#define MAX 400

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
#include "../global/global.h"

template <typename K, typename V>
extern V GetWithDefUnordered(const std::unordered_map<K,V>& m, const K& key, const V& defval);

namespace board{
class Board{

public:
   Board() noexcept;
   Board(char another_state[MAX], bool turn, int stage) noexcept;
   Board(const Board& another_board);
   std::vector<int> GetInfo() const;
   const std::vector<std::string>& GetHistory() const;
   std::string GetStateString() const;
   bool GetTurn() const;
   bool GetRound() const;
   std::tuple<int, bool, std::string> GetTuple() const;
   const std::unordered_map<std::string, std::string>& GetUniPieces() const;
   void PrintPos() const;
   void move(std::pair<int, int> start, std::pair<int, int> end, bool check); //start(x1, y1), end(x2, y2)
   void move(std::string ucci, bool check); //ucci representation
   void move(char* ucci, bool check);
   void move(int x1, int y1, int x2, int y2, bool check);
   std::function<int(int)> translate_x = [](const int x) -> int {return 12 - x;};
   std::function<int(int)> translate_y = [](const int y) -> int {return 3 + y;};
   std::function<int(int, int)> encode = [](const int x, const int y) -> int {return 16 * x + y;};
   std::function<int(int, int)> translate_x_y = [](const int x, const int y) -> int{return 195 - 16 * x + y;};
   
private:
   char _state[MAX];
   bool _has_initialized;
   bool _turn; //true红black黑
   int _round; //回合, 从0开始
   std::vector<std::string> _cur_legal_moves;
   std::vector<std::string> _board_history;
   static const int _chess_board_size;
   static const char _initial_state[MAX];
   static const std::unordered_map<std::string, std::string> _uni_pieces;
   std::function<std::string(const char)> _getstring = [](const char c) -> std::string {
        std::string ret;
        const std::string c_string(1, c);
        ret = GetWithDefUnordered<std::string, std::string>(_uni_pieces, c_string, c_string);
        return ret;
   };
   std::function<std::string(int, int)> _getstringxy = [this](int x, int y) -> std::string {
        return _getstring(_state[encode(x, y)]);
   };
};
}

#endif