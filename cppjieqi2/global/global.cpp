#include "global.h"
#include "../board/aiboard4.h"

std::unordered_map<int, std::unordered_map<std::pair<uint32_t, bool>, std::pair<unsigned char, unsigned char>, myhash<uint32_t, bool>>> tp_move_bean;
std::unordered_map<int, std::unordered_map<std::pair<uint32_t, int>, std::pair<short, short>, myhash<uint32_t, int>>> tp_score_bean;