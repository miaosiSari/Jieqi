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
#include <set>
#include <unordered_map>
#include <iostream>
#include <functional>
#include <algorithm>
#include <cctype>
#include <cmath>
#include <random>
#include <string.h>
#include <assert.h>
#include <stdio.h>
#include <ctype.h>
#include <stack>
#include <time.h>
#include <stdlib.h>
#include "../log/log.h"
#include "../global/global.h"
#include "../score/score.h"
#include "thinker.h"

#define MATE_LOWER 1304
#define MATE_UPPER 3696
#define CLEAR_STACK(STACK) \
while(!STACK.empty()){ \
   STACK.pop(); \
} 
#define AISUM(VERSION) \
short numr = 0, numb = 0; \
numr += aidi[VERSION][1][INTR]; numr += aidi[VERSION][1][INTN];  numr += di[VERSION][1][INTB];  numr += aidi[VERSION][1][INTA];  numr += aidi[VERSION][1][INTC]; numr += aidi[VERSION][1][INTP]; \
numb += aidi[VERSION][0][INTr]; numb += aidi[VERSION][0][INTn];  numb += di[VERSION][0][INTb];  numb += aidi[VERSION][0][INTa];  numb += aidi[VERSION][0][INTc]; numb += aidi[VERSION][0][INTp]; \
aisumall[VERSION][1] = numr; aisumall[VERSION][0] = numb;

template <typename T, typename U, typename V>
bool GreaterTuple(const std::tuple<T, U, V> &i, const std::tuple<T, U, V> &j) {
        return i > j;
}

extern short pst[123][256];
extern short average[VERSION_MAX][2][2][256];
extern unsigned char sumall[VERSION_MAX][2];
extern unsigned char di[VERSION_MAX][2][123];
extern std::unordered_map<std::string_view, std::pair<unsigned char, unsigned char>> kaijuku;

typedef short(*SCORE)(void* board_pointer, const char* state_pointer, unsigned char src, unsigned char dst);
typedef void(*KONGTOUPAO_SCORE)(void* board_pointer, short* kongtoupao_score, short* kongtoupao_score_opponent);
typedef std::string(*THINKER)(void* board_pointer, const char* state_pointer);
std::string trivial_thinker(void* self, const char* state_pointer);
std::string random_thinker(void* self, const char* state_pointer);
inline short trivial_score_function(void* self, const char* state_pointer, unsigned char src, unsigned char dst);
inline void trivial_kongtoupao_score_function(void* board_pointer, short* kongtoupao_score, short* kongtoupao_score_opponent);
void register_score_functions();
std::string SearchScoreFunction(void* score_func, int type);

template <typename K, typename V>
extern V GetWithDefUnordered(const std::unordered_map<K,V>& m, const K& key, const V& defval);


namespace board{
class AIBoard : public Thinker{

public:
    short aiaverage[VERSION_MAX][2][2][256];
    unsigned char aisumall[VERSION_MAX][2];
    unsigned char aidi[VERSION_MAX][2][123];
    int num_of_legal_moves = 0;
    int num_of_legal_moves2 = 0;
    int version = 0;
    int round = 0;
    bool turn = true; //true红black黑
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
    short kongtoupao_score_opponent = 0;
    uint64_t zobrist_hash = 0;
    std::stack<std::tuple<unsigned char, unsigned char, char>> cache;
    std::tuple<short, unsigned char, unsigned char> legal_moves[MAX_POSSIBLE_MOVES];
    std::tuple<short, unsigned char, unsigned char> legal_moves2[MAX_POSSIBLE_MOVES];
    std::set<unsigned char> rooted_chesses;
    AIBoard() noexcept;
    AIBoard(const char another_state[MAX], bool turn, int round, const unsigned char di[5][2][123]) noexcept;
    AIBoard(const AIBoard& another_board) = delete;
    virtual ~AIBoard();
    void Reset() noexcept;
    void SetScoreFunction(std::string function_name, int type);
    std::string SearchScoreFunction(int type);
    std::vector<std::string> GetStateString() const;
    void Move(const std::string ucci); //ucci representation
    void Move(const char* ucci);
    void Move(const unsigned char encode_from, const unsigned char encode_to);
    void UndoMove();
    void Scan();
    void KongTouPao(const char* _state_pointer, int pos, bool t);
    void Rooted();
    bool GenMovesWithScore(int type);
    void OppoRootedMate(bool* mate_by_oppo,std::vector<unsigned char>* rooted);
    void CopyData(const unsigned char di[5][2][123]);
    std::string Kaiju();
    virtual std::string Think();
    void PrintPos(bool turn) const;
    std::string DebugPrintPos(bool turn) const;
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
   
    std::function<const char*(void)> getstatepointer = [this](){
	   const char* _state_pointer = (this -> turn? this -> _state_red : this -> _state_black);
       return _state_pointer;
    };

    std::function<std::string(int)> translate_single = [](int i){
       int x1 = 12 - (i >> 4);
       int y1 = (i & 15) - 3;
       std::string ret = "  ";
       ret[0] = 'a' + y1;
       ret[1] = '0' + x1;
       return ret;
    };

    std::function<std::string(int, int)> translate_ucci = [this](int src, int dst){
       return translate_single(src) + translate_single(dst);
    };

    std::function<uint64_t(void)> randU64 = []() -> uint64_t{
       std::mt19937_64 gen(std::random_device{}());
       uint64_t randomNumber = gen();
       return randomNumber;
    };
   
private:
    char _state_red[MAX];
    char _state_black[MAX];
    uint64_t _zobrist[123][256];
    bool _has_initialized = false;
    static const int _chess_board_size;
    static const char _initial_state[MAX];
    static const std::unordered_map<std::string, std::string> _uni_pieces;
    static char _dir[91][8];
    SCORE _score_func = NULL;
    KONGTOUPAO_SCORE _kongtoupao_score_func = NULL;
    THINKER _thinker_func = NULL;
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
    std::function<void(void)> _initialize_zobrist = [this](){
        for(int i = 0; i < 123; ++i){
            for(int j = 0; j < 256; ++j){
                _zobrist[i][j] = randU64();
            }
        }
        for(int j = 51; j <= 203; ++j){
            if(::isalpha(_state_red[j])){
                zobrist_hash ^= _zobrist[(int)_state_red[j]][j];
            }
        }
    };
    void _initialize_dir();
};
}

#endif
