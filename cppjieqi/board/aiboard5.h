/*
* Created by Si Miao 2021/07/31
* Copyright (c) 2021. All rights reserved.
* Last modified 2021/07/31
*/
#ifndef aiboard5_h
#define aiboard5_h
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
#define GS(x) std::get<0>(x)
extern unsigned char L1[256][256];

#include <cstddef>
#include <vector>
#include <string>
#include <tuple>
#include <set>
#include <unordered_map>
#include <unordered_set>
#include <iostream>
#include <functional>
#include <algorithm>
#include <cctype>
#include <cmath>
#include <random>
#include <chrono>
#include <string.h>
#include <assert.h>
#include <stdio.h>
#include <ctype.h>
#include <stack>
#include <time.h>
#include <stdlib.h>
#include <functional>
#include "../global/global.h"
#include "../score/score.h"
#include "thinker.h"
#define ROOTED 0
#define CLEAR_EVERY_DEPTH false
#define CH(X) self->C(X)

extern std::unordered_map<int, std::unordered_map<std::pair<uint32_t, bool>, std::pair<unsigned char, unsigned char>, myhash<uint32_t, bool>>> tp_move_bean;
extern std::unordered_map<int, std::unordered_map<std::pair<uint32_t, int>, std::pair<short, short>, myhash<uint32_t, int>>> tp_score_bean;
namespace board{
    class AIBoard5;
}

typedef short(*SCORE5)(board::AIBoard5* bp, const char* state_pointer, unsigned char src, unsigned char dst);
typedef void(*KONGTOUPAO_SCORE5)(board::AIBoard5* bp, short* kongtoupao_score, short* kongtoupao_score_opponent);
typedef std::string(*THINKER5)(board::AIBoard5* bp);
void register_score_functions5();
std::string SearchScoreFunction5(void* score_func, int type);
extern short pstglobal[5][123][256];
template <typename K, typename V>
extern V GetWithDefUnordered(const std::unordered_map<K,V>& m, const K& key, const V& defval);
extern void copy_pst(short dst[][256], short src[][256]);


namespace board{
class AIBoard5 : public Thinker{
public:
    short aiaverage[VERSION_MAX][2][2][256];
    unsigned char aisumall[VERSION_MAX][2];
    unsigned char aidi[VERSION_MAX][2][123];
    bool lastinsert = false;
    int version = 0;
    int round = 0;
    bool turn = true; //true红black黑
    const bool original_turn;//游戏时的红黑
    int original_depth;
    unsigned char protector = 4;
    unsigned char protector_oppo = 4;
    unsigned char all = 0;
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
    uint32_t zobrist_hash = 0;
    char state_red[MAX];
    char state_black[MAX];
    std::stack<std::tuple<unsigned char, unsigned char, char>> cache;
    short score;//局面分数
    short pst[123][256];
    std::stack<short> score_cache;
    std::unordered_set<uint32_t> zobrist_cache;
    std::set<unsigned char> rooted_chesses;
    //tp_move: (zobrist_key, turn) --> move
    std::unordered_map<std::pair<uint32_t, bool>, std::pair<unsigned char, unsigned char>, myhash<uint32_t, bool>>* tp_move;
    //tp_score: (zobrist_key, turn, depth <depth * 2 + turn>) --> (lower, upper)
    std::unordered_map<std::pair<uint32_t, int>, std::pair<short, short>, myhash<uint32_t, int>>* tp_score;
    std::unordered_map<std::string, bool>* hist;
    std::unordered_map<std::string, std::pair<unsigned char, unsigned char>> kaijuku;
    AIBoard5() noexcept;
    AIBoard5(const char another_state[MAX], bool turn, int round, const unsigned char di[5][2][123], short score, std::unordered_map<std::string, bool>* hist) noexcept;
    AIBoard5(const AIBoard5& another_board) = delete;
    virtual ~AIBoard5()=default;
    void Reset() noexcept;
    void SetScoreFunction(std::string function_name, int type);
    std::string SearchScoreFunction(int type);
    std::string GetName(){
        return _myname;
    }
    bool Move(const unsigned char encode_from, const unsigned char encode_to, short score_step);
    void NULLMove();
    void UndoMove(int type);
    short ScanProtectors();
    void Scan();
    void KongTouPao(const char* _state_pointer, int pos, bool t);
    template<bool needscore, bool return_after_mate> 
    bool GenMovesWithScore(std::tuple<short, unsigned char, unsigned char> legal_moves[MAX_POSSIBLE_MOVES], int& num_of_legal_moves, std::pair<unsigned char, unsigned char>* killer, short& killer_score, unsigned char& mate_src, unsigned char& mate_dst, bool& killer_is_alive);
    template<bool doublereverse> bool Mate();
    bool Executed(bool* oppo_mate, std::tuple<short, unsigned char, unsigned char> legal_moves_tmp[], int num_of_legal_moves_tmp, bool calc);
    bool ExecutedDebugger(bool *oppo_mate);
    bool Ismate_After_Move(unsigned char src, unsigned char dst);
    void CopyData(const unsigned char di[5][2][123]);
    std::string Kaiju();
    virtual std::string Think();
    void PrintPos(bool turn) const;
    std::string DebugPrintPos(bool turn) const;
    void print_raw_board(const char* board, const char* hint);
    template<typename... Args> void print_raw_board(const char* board, const char* hint, Args... args);
    #if DEBUG
    std::vector<std::string> debug_flags;
    int movecounter=0;
    std::function<uint32_t()> get_theoretical_zobrist = [this]() -> uint32_t {
        uint32_t theoretical_hash = 0;
        for(int j = 51; j <= 203; ++j){
            if(::isalpha(state_red[j])){
                 theoretical_hash ^= _zobrist[(int)state_red[j]][j];
            }
        }
        return theoretical_hash;
    };
    std::function<std::string(std::pair<unsigned char, unsigned char>)> render = [this](std::pair<unsigned char, unsigned char> t) -> std::string {
        return translate_ucci(t.first, t.second);
    };
    bool C(std::vector<std::string> prefix){
        if(debug_flags.size() < prefix.size()){
            return false;
        }
        for(size_t i = 0; i < prefix.size(); ++i){
            if(prefix[i] != debug_flags[i]){
                return false;
            }
        }
        return true;
    }

    bool _C(std::vector<std::string>& prefix){
        return C(prefix);
    }

    template <typename... Args>
    bool _C(std::vector<std::string>& prefix, std::string now, Args... args){
        prefix.push_back(now);
        return _C(prefix, args...);
    }
 
    template <typename... Args>
    bool C(Args... args){
        if(sizeof...(args) == 0) {
            return true;
        }
        std::vector<std::string> prefix;
        return _C(prefix, args...);
    }

    bool IAMDebugger(std::string s){
        assert(s.size() == 4);
        return Ismate_After_Move(f(s.substr(0, 2)), f(s.substr(2, 2)));
    }

    #endif
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
	   const char* _state_pointer = (this -> turn? this -> state_red : this -> state_black);
       return _state_pointer;
    };

    std::function<unsigned char(std::string)> f = [](std::string s) -> unsigned char {
        if(s.size() != 2) return 0;
        unsigned char x = s[1] - '0';
        unsigned char y = s[0] - 'a';
        return 195 - 16 * x + y;
    };

    char operator[](std::string s){
        return state_red[f(s)];
    }

    std::function<std::string(unsigned char)> translate_single = [](unsigned char i) -> std::string{
       int x1 = 12 - (i >> 4);
       int y1 = (i & 15) - 3;
       std::string ret = "  ";
       ret[0] = 'a' + y1;
       ret[1] = '0' + x1;
       return ret;
    };

    std::function<std::string(unsigned char, unsigned char)> translate_ucci = [this](unsigned char src, unsigned char dst) -> std::string{
       return translate_single(src) + translate_single(dst);
    };

    std::function<std::string(std::tuple<short, unsigned char, unsigned char>)> translate_tuple = \
       [this](std::tuple<short, unsigned char, unsigned char> t) -> std::string{ 
       return translate_single(std::get<1>(t)) + translate_single(std::get<2>(t));
    };

    std::function<uint32_t(void)> randU32 = []() -> uint32_t{
	   //BUG: 在Windows上每次生成同样的随机数
	   #ifdef WIN32
	   //Windows RAND_MAX 0x7fff
	   int a = rand();
	   unsigned b = ((a & 1) << 15) | a; //符号位随机
	   int c = rand();
	   unsigned d = ((c & 1) << 15) | c; //符号位随机
	   return (b << 16) | d;
	   #else
       std::mt19937 gen(std::random_device{}());
       uint32_t randomNumber = gen();
       return randomNumber;
	   #endif
    };

private:
    const char* _kaijuku_file;
    std::string _myname;
    uint32_t _zobrist[123][256];
    bool _has_initialized = false;
    static const int _chess_board_size;
    static const char _initial_state[MAX];
    static const std::unordered_map<std::string, std::string> _uni_pieces;
    static char _dir[91][8];
    SCORE5 _score_func = NULL;
    KONGTOUPAO_SCORE5 _kongtoupao_score_func = NULL;
    THINKER5 _thinker_func = NULL;
    std::function<std::string(const char)> _getstring = [](const char c) -> std::string {
        std::string ret;
        const std::string c_string(1, c);
        ret = GetWithDefUnordered<std::string, std::string>(_uni_pieces, c_string, c_string);
        return ret;
    };
    std::function<std::string(int, int, bool)> _getstringxy = [this](int x, int y, bool turn) -> std::string {
        std::string ret =  turn?_getstring(state_red[encode(x, y)]):_getstring(state_black[encode(x, y)]);
        return ret;
    };
    std::function<void(void)> _initialize_zobrist = [this](){
        for(int i = 0; i < 123; ++i){
            for(int j = 0; j < 256; ++j){
                if(i != '.'){
                    _zobrist[i][j] = randU32();
				}
                else
                    _zobrist[i][j] = 0;
            }
        }
        for(int j = 51; j <= 203; ++j){
            if(::isalpha(state_red[j])){
                zobrist_hash ^= _zobrist[(int)state_red[j]][j];
            }
        }
    };
    void _initialize_dir();
};
}

std::string mtd_thinker5(board::AIBoard5* self);
void complicated_kongtoupao_score_function5(board::AIBoard5* board_pointer, short* kongtoupao_score, short* kongtoupao_score_opponent);
short complicated_score_function5(board::AIBoard5* bp, const char* state_pointer, unsigned char src, unsigned char dst);
short mtd_quiescence5(board::AIBoard5* self, const short gamma, int quiesc_depth, const bool root);
short mtd_alphabeta5(board::AIBoard5* self, const short gamma, int depth, const bool root, const bool nullmove, const bool lastmate, const int quiesc_depth, const bool traverse_all_strategy);

#endif
