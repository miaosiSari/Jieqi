/*
* Created by Si Miao 2021/07/31
* Copyright (c) 2021. All rights reserved.
* Last modified 2021/07/31
*/
#ifndef aiboard4_h
#define aiboard4_h
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
#define ROOT 0
#define PV 1
#define CUT 2
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
#include <math.h>
#include <time.h>
#include <stdlib.h>
#include <functional>
#include "../global/global.h"
#include "../score/score.h"
#include "thinker.h"
#define CH(X) self->C(X)
#define G0(X) std::get<0>(X)
#define G1(X) std::get<1>(X)
#define G2(X) std::get<2>(X)
#define SELF (*self)
#define PUSH self->debug_flags.push_back(ucci);
#define POP self->debug_flags.pop_back();

#define hashfEXACT 3
#define hashfALPHA 1
#define hashfBETA 2
#define hashfEMPTY 0
#define REDMASK 0x0130b9db
#define BLACKMASK ~REDMASK
#define MASKRED ((zobrist_hash) ^ (REDMASK))
#define MASKBLACK ((zobrist_hash) ^ (BLACKMASK))
#define MASK ((turn) ? MASKRED : MASKBLACK)
#define SELFMASKRED ((self -> zobrist_hash) ^ (REDMASK))
#define SELFMASKBLACK ((self -> zobrist_hash) ^ (BLACKMASK))
#define SELFMASK ((self -> turn) ? SELFMASKRED : SELFMASKBLACK)
#define MAX_ZOBRIST 4194304
#define MASK_ZOBRIST (MAX_ZOBRIST - 1)
#define MATE_UPPER 2600
#define WIN_VALUE 2500
#define BAN_VALUE 2550
#define POS self -> DebugPrintPos()
#define POSCSTR self -> DebugPrintPos().c_str() 
#define SCORE(X) (X.score)
#define SRC(X) (X.src)
#define DST(X) (X.dst)
#define MAKETUPLE(SCORE, SRC, DST) {SCORE, SRC, DST}
#define MAX_CACHE 65536
#define MASK_CACHE (MAX_CACHE - 1)
#define MAKE ((self -> ply << 8)|depth)
#define MIX(ply, depth) ((ply << 8)|depth)

#define INITIALIZELUT4 \
memset(LUT4, 0, sizeof(LUT4)); \
LUT4[195] = 'D'; \
LUT4[196] = 'E'; \
LUT4[197] = 'F'; \
LUT4[198] = 'G'; \
LUT4[200] = 'G'; \
LUT4[201] = 'F'; \
LUT4[202] = 'E'; \
LUT4[203] = 'D'; \
LUT4[164] = 'H'; \
LUT4[170] = 'H'; \
LUT4[147] = 'I'; \
LUT4[149] = 'I'; \
LUT4[151] = 'I'; \
LUT4[153] = 'I'; \
LUT4[155] = 'I'; 

#define SEARCHBODY \
unsigned char t1 = 0, t2 = 0; \
if(type == CUT){\
    val = -alphabeta4(self, -alpha-1, -alpha, depth - !mate, CUT, nullmove, true, t1, t2);\
}else{\
    if(best == -MATE_UPPER){\
        val = -alphabeta4(self, -beta, -alpha, depth - !mate, PV, nullmove, false, t1, t2);\
    }else{ \
        val = -alphabeta4(self, -alpha-1, -alpha, depth - !mate, CUT, nullmove, true, t1, t2); \
        if(val > alpha && val < beta) {\
            val = -alphabeta4(self, -beta, -alpha, depth - !mate, PV, nullmove, false, t1, t2); \
        }\
    }\
}\
ucci2val[ucci] = val; 

#define SEARCHBODYWO \
unsigned char t1 = 0, t2 = 0; \
if(type == CUT){\
    depths[ver] -= 1; \
    val = -alphabeta_doublerecursive4(self, ver, -alpha-1, -alpha, depths, CUT, nullmove, true, uncertainty_dict, needclamp, t1, t2);\
    depths[ver] += 1; \
}else{\
    if(best == -MATE_UPPER){\
        depths[ver] -= 1; \
        val = -alphabeta_doublerecursive4(self, ver, -beta, -alpha, depths, PV, nullmove, false, uncertainty_dict, needclamp, t1, t2); \
        depths[ver] += 1; \
    }else{ \
        depths[ver] -= 1; \
        val = -alphabeta_doublerecursive4(self, ver, -alpha-1, -alpha, depths, CUT, nullmove, true, uncertainty_dict, needclamp, t1, t2); \
        depths[ver] += 1; \
        if(val > alpha && val < beta) {\
            depths[ver] -= 1; \
            val = -alphabeta_doublerecursive4(self, ver, -beta, -alpha, depths, PV, nullmove, false, uncertainty_dict, needclamp, t1, t2); \
            depths[ver] += 1; \
        }\
    }\
}

#define DECISION(SRC, DST, ISKILLER, SCORE) \
if(retval){\
    if(val > best){\
        best = val; \
        argmaxsrc = SRC; \
        argmaxdst = DST; \
        self -> moves[{self -> zobrist_hash, MAKE}] = debugtuple{ucci, val, SCORE, alpha, beta, ISKILLER?5:6, -1, depth, type}; \
        if(val >= beta) { \
            hashf = hashfBETA;\
            if(type != ROOT){\
                break; \
            } \
        } \
        if(val > alpha){ \
            alpha = val; \
            hashf = hashfEXACT; \
        } \
    } \
}

#define DECISIONWO(SRC, DST, ISKILLER, SCORE) \
if(retval){\
    if(val > best){\
        best = val; \
        argmaxsrc = SRC; \
        argmaxdst = DST; \
        if(val >= beta) { \
            hashf = hashfBETA;\
            if(type != ROOT){\
                break; \
            } \
        } \
        if(val > alpha){ \
            alpha = val; \
            hashf = hashfEXACT; \
        } \
    } \
}

#define BANNED \
if(self -> banned[SELFMASK].find(ucci) != self -> banned[SELFMASK].end()){ \
    continue; \
} 

#define DEBUGK \
if(type == ROOT && depth == self -> original_depth){\
    printf("depth=%d, ucci=%s, killer_score=%d, val=%d, retval=%d, best=%d, alpha=%d, beta=%d\n", depth, ucci.c_str(), killer_score, val, retval, best, alpha, beta);\
}
#define DEBUGN \
if(type == ROOT && depth == self -> original_depth){\
    printf("depth=%d, ucci=%s, score=%d, val=%d, retval=%d, best=%d, alpha=%d, beta=%d\n", depth, ucci.c_str(), score, val, retval, best, alpha, beta);\
}\

#define DEBUGH \
if(type==ROOT) printf("Return from here, killer_src=%d, killer_dst=%d, hashval=%d\n", killer_src, killer_dst, hashval);

namespace board{
    class AIBoard4;
}

typedef short(*SCORE4)(board::AIBoard4* bp, const char* state_pointer, unsigned char src, unsigned char dst);
typedef void(*KONGTOUPAO_SCORE4)(board::AIBoard4* bp, short* kongtoupao_score, short* kongtoupao_score_opponent);
typedef std::string(*THINKER4)(board::AIBoard4* bp, int maxdepth);
void register_score_functions4();
std::string SearchScoreFunction4(void* score_func, int type);
extern short pstglobal[5][123][256];
template <typename K, typename V>
extern V GetWithDefUnordered(const std::unordered_map<K,V>& m, const K& key, const V& defval);
extern void copy_pst(short dst[][256], short src[][256]);

struct debugtuple{
    std::string ucci;
    short val;
    short score;
    short alpha;
    short beta;
    int recordplace;
    int recordplacehash;
    int recorddepth;
    int nodetype;
};

struct scoretuple{
    short score;
    unsigned char src;
    unsigned char dst;
};

namespace board{
class AIBoard4 : public Thinker{
public:
    static bool has_initialized;
    short aiaverage[VERSION_MAX][2][2][256];
    unsigned char aisumall[VERSION_MAX][2];
    unsigned char aidi[VERSION_MAX][2][123];
    std::vector<bool> lastinserts;\
    int ply = 0;
    int version = 0;
    int round = 0;
    bool turn = true; //true红black黑
    const bool original_turn;//游戏时的红黑
    bool original_turns[VERSION_MAX];
    int original_depth;
    unsigned char protector = 4;
    unsigned char protector_oppo = 4;
    unsigned char all = 0;
    unsigned char che = 0;
    unsigned char che_opponent = 0;
    unsigned char zu = 0;
    unsigned char zu_opponent = 0;
    unsigned char covered = 0;
    unsigned char covered_opponent = 0;
    unsigned char endline = 0;
    short score_rough = 0;
    unsigned char kongtoupao = 0;
    unsigned char kongtoupao_opponent = 0;
    short kongtoupao_score = 0;
    short kongtoupao_score_opponent = 0;
    uint64_t zobrist_hash = 0;
    const float discount_factor;
    char state_red[MAX];
    char state_black[MAX];
    std::stack<std::tuple<unsigned char, unsigned char, char>> cache;
    short score;//局面分数
    short pst[123][256];

    struct gameinfo{
       unsigned char che;
       unsigned char che_opponent;   
       unsigned char zu;
       unsigned char zu_opponent;
       short score;
       uint64_t zobrist_hash;
    };

    std::stack<gameinfo> score_cache;
    std::unordered_set<uint64_t> zobrist_cache;
    tp* tptable;
    std::unordered_map<std::string, bool>* hist;
    std::unordered_map<std::string, std::pair<unsigned char, unsigned char>> kaijuku;
    std::unordered_map<std::pair<uint64_t, int>, debugtuple, myhash<uint64_t, int>> moves;
    std::unordered_map<uint64_t, std::unordered_set<std::string>> banned;
    AIBoard4()=delete;
    AIBoard4(const char another_state[MAX], bool turn, int round, const unsigned char di[5][2][123], short score, tp* tptable, std::unordered_map<std::string, bool>* hist) noexcept;
    AIBoard4(const AIBoard4& another_board) = delete;
    virtual ~AIBoard4()=default;
    void Reset() noexcept;
    void SetScoreFunction(std::string function_name, int type);
    std::string SearchScoreFunction(int type);
    std::string GetName(){
        return _myname;
    }
    bool Move(const unsigned char encode_from, const unsigned char encode_to, short score_step);
    bool NULLMove();
    void UndoMove(int type);
    short ScanProtectors();
    void Scan();
    void KongTouPao(const char* _state_pointer, int pos, bool t);
    template<bool needscore> 
    bool GenMovesWithScore(scoretuple legal_moves[MAX_POSSIBLE_MOVES], int& num_of_legal_moves, std::pair<unsigned char, unsigned char>* killer, short& killer_score, unsigned char& mate_src, unsigned char& mate_dst, bool& killer_is_alive);
    template<bool doublereverse> bool Mate();
    bool Executed(bool* oppo_mate, scoretuple legal_moves_tmp[], int num_of_legal_moves_tmp);
    bool ExecutedDebugger(bool *oppo_mate);
    bool Ismate_After_Move(unsigned char src, unsigned char dst);
    void CalcVersion(const int ver);
    void CopyData(const unsigned char di[5][2][123]);
    virtual std::string Think(int maxdepth);
    void PrintPos(bool turn) const;
    std::string DebugPrintPos() const;
    void print_raw_board(const char* board, const char* hint);
    template<typename... Args> void print_raw_board(const char* board, const char* hint, Args... args);
    uint64_t zobrist[123][256];
    #if DEBUG
    std::vector<std::string> debug_flags;
    int movecounter=0;

    std::function<uint64_t()> get_theoretical_zobrist = [this]() -> uint64_t {
        uint64_t theoretical_hash = 0;
        for(int j = 51; j <= 203; ++j){
            if(::isalpha(state_red[j])){
                theoretical_hash ^= zobrist[(int)state_red[j]][j];
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

    std::function<uint64_t(void)> randU64 = []() -> uint64_t{
	   //BUG: 在Windows上每次生成同样的随机数
       std::mt19937_64 gen(std::random_device{}());
       uint64_t randomNumber = gen();
       return randomNumber;
    };

    template<typename T>
    inline T div(T x, T y){
        if(std::is_floating_point<T>::value){
            return fabs(y) < 1e-7 ? (x/y) : 0.0;
        }
        return y != 0 ? (x / y) : 0;
    }

    std::function<short()> evaluate = [this]() -> short{
        return score;
    };

    void RecordHash(int depth, int val, int score, int hashf, unsigned char src, unsigned char dst, int recordplace){
        tp* phashe = tptable + (int)(MASK & MASK_ZOBRIST);
        bool originnull = false, movenotnull = (src != 0 && dst != 0);
        if(phashe -> key == zobrist_hash && phashe -> turn == turn){
            //原先的HashItem存在的情况
            originnull = (phashe -> src == 0 || phashe -> dst == 0);
            if((hashf & hashfALPHA) != 0 && (phashe -> alphadepth <= depth || phashe -> alphadepth >= val)){
                phashe -> alphadepth = depth;
                phashe -> alphaval = val;
            }
            if((hashf & hashfBETA) != 0 && (phashe -> betadepth <= depth || phashe -> betadepth <= val) && \
                (originnull || movenotnull)){
                    phashe -> betadepth = depth;
                    phashe -> betaval = val;
            }
            if(movenotnull){
                phashe -> src = src;
                phashe -> dst = dst;
            }
            phashe -> score = score;
            return;
        }
        phashe -> alphaval = phashe -> betaval = 0;
        if((hashf & hashfALPHA) != 0){
            phashe -> alphadepth = depth;
            phashe -> alphaval = val;
        }
        if((hashf & hashfBETA) != 0){
            phashe -> betadepth = depth;
            phashe -> betaval = val;
        }
        phashe -> key = zobrist_hash;
        phashe -> turn = turn;
        phashe -> score = score;
        phashe -> src = src;
        phashe -> dst = dst;
        phashe -> recordplace = recordplace;
    }

    short ProbeHash(int depth, int alpha, int beta, bool nonull, tp** hashnode){

        auto ValueAdjust = [](bool& matenode, short val, int ply) -> short{
            matenode = false;
            if(val > WIN_VALUE){
                matenode = true;
                val -= ply;
            }else if(val < -WIN_VALUE){
                matenode = true;
                val += ply;
            }
            return val;
        };

        if(!hashnode){
            return -MATE_UPPER;
        }
        *hashnode = NULL;
        tp* phashe = tptable + (int)(MASK & MASK_ZOBRIST);
        if(phashe -> key == zobrist_hash && phashe -> turn == turn){
            *hashnode = phashe;
            bool originnull = (phashe -> src == 0 || phashe -> dst == 0);
            if(phashe -> betadepth > 0){
                bool betamatenode = false;
                short val = ValueAdjust(betamatenode, phashe -> betaval, ply);
                if(!(originnull && nonull) && (phashe -> betadepth >= depth || betamatenode) && (val >= beta)){
                    return val;
                }
            }
            if(phashe -> alphadepth > 0){
                bool alphamatenode = false;
                short val = ValueAdjust(alphamatenode, phashe -> alphaval, ply);
                if((phashe -> alphadepth >= depth || alphamatenode) && (val <= alpha)){
                    return val;
                }
            }
        }
        return -MATE_UPPER;
    }

    void Trace(int depth_limit, std::vector<std::string> hints){
        int j = 0, depth = depth_limit; //move times
        std::vector<int> movetype;
        Scan();
        while(1){
            bool oppomate = false;
            ExecutedDebugger(&oppomate);
            if((size_t)j < hints.size()){
                std::cout << DebugPrintPos() << "ply == " << ply << ", j == " << j << ", hints.size()==" << hints.size() << ", hints[j]=="  << hints[j] << ", depth==" << depth << "\n\n";
                Move(f(hints[j].substr(0, 2)), f(hints[j].substr(2, 2)), 0);
                if(!oppomate) --depth;
                movetype.push_back(1);
                j++;
                continue;
            }
            bool find = true;
            std::string ucci;
            debugtuple score;
            if(moves.find({zobrist_hash, MIX(ply, depth)}) == moves.end()){
                find = false;
                printf("<%d %d> does not exist! hash=%zu, turn=%d, depth=%d\n", ply, depth, zobrist_hash, turn, depth);
                tp* hashnode = NULL;
                ProbeHash(depth, 0, 0, false, &hashnode);
                if(hashnode){
                    ucci = translate_ucci(hashnode -> src, hashnode -> dst);
                    std::cout << "find in hashnode: " << ucci << " " << hashnode -> alphaval << " " << hashnode -> betaval << "\n";
                }else{
                    break;
                }
            }
            if(find){
                debugtuple score = moves[{zobrist_hash, MIX(ply, depth)}];
                ucci = score.ucci;
                std::cout << DebugPrintPos() << std::boolalpha << "<ply, depth> == <" << ply << ", " << depth << ">, hash == " << zobrist_hash << ", ucci == " << score.ucci << ", val == " << score.val << ", score == " << score.score << ", alpha==" << score.alpha << \
                ", beta==" << score.beta << ", recordplace==" << score.recordplace << ", recordplacehash==" << score.recordplacehash  << ", nodetype==" << (score.nodetype == ROOT ? "ROOT" : score.nodetype == PV ? "PV" : "CUT") << "\n\n";
            }else{
                std::cout << std::boolalpha << "<ply, depth> == <" << ply << ", " << depth << ">" << DebugPrintPos() << " :not find, taken from hashtable\n\n";
            }
            bool retval = false;
            if(!ucci.empty() && ucci != "^<^<") {
                retval = Move(f(ucci.substr(0, 2)), f(ucci.substr(2, 2)), 0);
                depth -= (oppomate ? 0 : 1);
                movetype.push_back(1);
            }
            else{
                retval = NULLMove();
                depth -= 3;
                movetype.push_back(0);
            } 
            assert(retval || oppomate);
            if(score.recordplace == 3 || depth == 0){
                break;
            }
        }
        printf("FINISHED: depth=%d, %s, hints.size()==%zu\n", depth, DebugPrintPos().c_str(), hints.size());
        for(int k = (int)(movetype.size()) - 1; k >= 0; --k){
            UndoMove(movetype[k]);
        }
    }
   
private:
    const char* _kaijuku_file;
    std::string _myname;
    bool _has_initialized = false;
    static const int _chess_board_size;
    static const char _initial_state[MAX];
    static const std::unordered_map<std::string, std::string> _uni_pieces;
    static char _dir[91][8];
    SCORE4 _score_func = NULL;
    KONGTOUPAO_SCORE4 _kongtoupao_score_func = NULL;
    THINKER4 _thinker_func = NULL;
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
                    zobrist[i][j] = randU64();
				}
                else{
                    zobrist[i][j] = 0;
                }
            }
        }
        for(int j = 51; j <= 203; ++j){
            if(::isalpha(state_red[j])){
                zobrist_hash ^= zobrist[(int)state_red[j]][j];
            }
        }
    };
    void _initialize_dir();
};
}

std::string thinker4(board::AIBoard4* bp, int maxdepth);
void complicated_kongtoupao_score_function4(board::AIBoard4* bp, short* kongtoupao_score, short* kongtoupao_score_opponent);
short complicated_score_function4(board::AIBoard4* self, const char* state_pointer, unsigned char src, unsigned char dst);
short alphabeta4(board::AIBoard4* self, const short alpha, const short beta, int depth, int type, const bool nullmove, const bool nullmovenow, unsigned char& src, unsigned char& dst);
short alphabeta_doublerecursive4(board::AIBoard4* self, const int ver, short alpha, short beta, std::vector<int>& depths, const int type, const bool nullmove, const bool nullmovenow, std::unordered_map<unsigned char, char>& uncertainty_dict, bool* needclamp, unsigned char& argmaxsrc, unsigned char& argmaxdst);
void _inner_recur(board::AIBoard4* self, const int ver, std::unordered_map<unsigned char, char>& uncertainty_dict, std::vector<unsigned char>& uncertainty_keys, \
    std::unordered_map<std::pair<int, int>, short, myhash<int, int>>& result_dict, std::unordered_map<std::pair<int, int>, short, myhash<int, int>>& counter_dict, \
    const int index, const int me, const int op, const short score, const short alpha, const short beta, \
    std::vector<int>& depths, const bool nullmove, unsigned char& argmaxsrc, unsigned char& argmaxdst);
short eval4(board::AIBoard4* self, const int ver, const short alpha, const short beta, std::vector<int>& depths, const bool nullmove, unsigned char& argmaxsrc, unsigned char& argmaxdst);
short calleval4(board::AIBoard4* self, short alpha, short beta, std::vector<int> depths, const bool nullmove, unsigned char& argmaxsrc, unsigned char& argmaxdst);
void debugset(board::AIBoard4* self);
#endif
