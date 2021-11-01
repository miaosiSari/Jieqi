#include "global.h"
#include "../board/aiboard3.h"
#include "../board/aiboard4.h"
#include "../board/aiboard5.h"

namespace board{
    std::map<std::string, std::function<Thinker*(const char[], bool, int, const unsigned char [5][2][123], short, std::unordered_map<std::string, bool>*)>> bean; 
    int register_func(std::string x, std::function<Thinker*(const char[], bool, int, const unsigned char [5][2][123], short score, std::unordered_map<std::string, bool>*)> y){
      bean.insert({x, y});
      return 0;
    }
    int aiboard3 = register_func("AIBoard3", [](const char another_state[], bool turn, int round, const unsigned char di[5][2][123], short score, std::unordered_map<std::string, bool>* hist)\
 -> Thinker * {return new AIBoard3(another_state, turn, round, di, score, hist);});
    int aiboard4 = register_func("AIBoard4", [](const char another_state[], bool turn, int round, const unsigned char di[5][2][123], short score, std::unordered_map<std::string, bool>* hist)\
 -> Thinker * {return new AIBoard4(another_state, turn, round, di, score, hist);});
    int aiboard5 = register_func("AIBoard5", [](const char another_state[], bool turn, int round, const unsigned char di[5][2][123], short score, std::unordered_map<std::string, bool>* hist)\
 -> Thinker * {return new AIBoard5(another_state, turn, round, di, score, hist);});
   //这个bean用unordered_map在Windows上会core很奇怪, 有整数除0错误。
	 //之前是用的REGISTER_CLASS宏, 但这个宏有个问题, 就是REGISTER_CLASS通过初始化全局变量的方式往bean中添加, 比如直接bean.add是行不通的, 因为语句不能在函数体外执行, 
	 //只能是int x = bean.add(XXX), 用一个全局变量x去执行bean.add
	 //问题是全局变量的初始化顺序是未定义的, 取决于编译器, 如果编译器不够聪明的话, 调用REGISTER_CLASS的时候bean还没有初始化, Coredump...
}
std::unordered_map<int, std::unordered_map<std::pair<uint32_t, bool>, std::pair<unsigned char, unsigned char>, myhash<uint32_t, bool>>> tp_move_bean;
std::unordered_map<int, std::unordered_map<std::pair<uint32_t, int>, std::pair<short, short>, myhash<uint32_t, int>>> tp_score_bean;
