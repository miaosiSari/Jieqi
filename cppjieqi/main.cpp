// copyright 2021 miaosi@all rights reserved.
//#include <QCoreApplication>
#include <stdio.h>
#include <iostream>
#include <time.h>
#include "global/global.h"
#include "board/god.h"
#include "score/score.h"

extern bool read_score_table(const char* score_file, short pst[][256]);
extern void copy_pst(short dst[][256], short src[][256]);
extern short pstglobal[2][123][256];

int main(void) {
    srand(time(NULL));
    assert(read_score_table("../score.conf", pstglobal[0]));
    assert(read_score_table("../score.conf", pstglobal[1]));
    God g("../players.conf");
    DEBUG ? g.StartGame() : g.StartGameLoopAlternatively(5);
    return 0;
}
