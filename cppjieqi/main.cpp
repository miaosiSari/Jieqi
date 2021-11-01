// copyright 2021 miaosi@all rights reserved.
#include <stdio.h>
#include <iostream>
#include <time.h>
#include "global/global.h"
#include "board/god.h"
#include "score/score.h"

extern bool read_score_table(const char* score_file, short pst[][256]);
extern void copy_pst(short dst[][256], short src[][256]);
extern void IntializeL1();
extern short pstglobal[5][123][256];
extern unsigned char L1[256][256];

int main(void) {
    srand(time(NULL));
    IntializeL1();
    memset(pstglobal, 0, sizeof(pstglobal));
    assert(read_score_table("../score.conf", pstglobal[2]));
    assert(read_score_table("../score.conf", pstglobal[3]));
    assert(read_score_table("../score.conf", pstglobal[4]));
    God g("../players.conf");
    DEBUG ? g.StartGame() : g.StartGameLoopAlternatively();
    #if !DEBUG
    g.Play();
    #endif
    return 0;
}
