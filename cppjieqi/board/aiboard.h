/*
* Created by Si Miao 2021/07/31
* Copyright (c) 2021. All rights reserved.
* Last modified 2021/07/31
*/
#ifndef aiboard_h
#define aiboard_h
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

extern short pst[123][256];
extern short average[VERSION_MAX][2][2][256];

typedef short(*SCORE)(void* board_pointer);
typedef void(*KONGTOUPAO_SCORE)(void* board_pointer, short* kongtoupao_score, short* kongtoupao_score_opponent);
inline short trivial_score_function(void* self);
inline void trivial_kongtoupao_score_function(void* board_pointer, short* kongtoupao_score, short* kongtoupao_score_opponent);
void register_score_functions();

template <typename K, typename V>
extern V GetWithDefUnordered(const std::unordered_map<K,V>& m, const K& key, const V& defval);


namespace board{
class AIBoard{

public:
   int num_of_legal_moves = 0;
   int version = 0;
   unsigned char che = 0;
   unsigned char che_opponent = 0;
   unsigned char zu = 0;
   unsigned char covered = 0;
   unsigned char covered_opponent = 0;
   unsigned char endline = 0;
   short score_rough = 0;
   unsigned char kongtoupao = 0;
   unsigned char kongtoupao_opponent = 0;
   short kongtoupao_score = 0;
   short kongtoupao_score_opponent=0;
   AIBoard() noexcept;
   AIBoard(const char another_state[MAX], bool turn, int round) noexcept;
   AIBoard(const AIBoard& another_board);
   void Reset() noexcept;
   void SetScoreFunction(std::string function_name, int type);
   std::vector<std::string> GetStateString() const;
   bool GetTurn() const;
   void SetTurn(bool turn);
   bool GetRound() const;
   void Move(const std::pair<int, int> start, const std::pair<int, int> end); //start(x1, y1), end(x2, y2)
   void Move(const std::string ucci); //ucci representation
   void Move(const char* ucci);
   void Move(const int x1, const int y1, const int x2, const int y2);
   void GenMovesWithScore();
   void Scan();
   void KongTouPao(const char* _state_pointer, int pos, bool t);
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
   
private:
   char _state_red[MAX];
   char _state_black[MAX];
   bool _has_initialized = false;
   bool _turn = false; //true红black黑
   int _round = 0; //回合, 从0开始
   static const int _chess_board_size;
   static const char _initial_state[MAX];
   static char _dir[91][8];
   SCORE _score_func = NULL;
   KONGTOUPAO_SCORE _kongtoupao_score_func = NULL;
   void _initialize_dir();
};
}

#endif
