#ifndef score_h
#define score_h

#include <unordered_map>
#include "../log/log.h"
#include "../global/global.h"

typedef unsigned short(*SCORE)(unsigned char, unsigned char);

unsigned short trivial_score_function(unsigned char, unsigned char);
void register_score_functions();

#endif
