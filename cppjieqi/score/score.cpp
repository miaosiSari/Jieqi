#include "score.h"

std::unordered_map<std::string, SCORE> function_bean;

unsigned short trivial_scoring_function(unsigned char, unsigned char){
   Singleton<log::Log>::get() -> Write("trivial_scoring_function!");
   return 0;
}

void register_scoring_functions(){
   function_bean.insert({"trivial_scoring_function", trivial_scoring_function});
}
