// copyright 2021 miaosi@all rights reserved.
//#include <QCoreApplication>
#include <stdio.h>
#include <iostream>
#include <time.h>
#include "board/board.h"
#include "board/aiboard.h"
#include "log/log.h"
#include "global/global.h"
#include "score/score.h"
#include "helper/helper.h"

extern void register_scoring_functions();
extern void read_score_table(const char* score_file);

const char test[MAX] = 
                    "                "
                    "                "
                    "                "
                    "   ....k....    "
                    "   .........    "
                    "   .........    "
                    "   .........    "
                    "   .........    "
                    "   .........    "
                    "   .........    "
                    "   .H.......    "
                    "   .........    "
                    "   ....K....    "
                    "                "
                    "                "
                    "                ";

int main(void) {
    helper::Helper h;
    h.Read("../../log/log.txt");
    printf("h.Compare() == %d\n", h.Compare());
    register_score_functions();
    read_score_table("../score.conf");
    board::AIBoard b = board::AIBoard();
    b.GenMovesWithScore();
    /*
    b.Move("b2c3", true);
    b.PrintPos(true);
    b.GenMovesWithScore();
    printf("%d\n", b.num_of_legal_moves);
    b.SetTurn(true);
    b.GenMovesWithScore();
    b.PrintAllMoves();
    */
	
	/*
    printf("%d\n", b.num_of_legal_moves);
    std::cout << subtrim("abc,d ,\r\n") << "\n" << subtrim("abc,d ,\r\n").size() << std::endl;
    std::vector<std::string> statestring = b.GetStateString();
    std::cout << statestring[0] << std::endl << std::endl << statestring[1] << std::endl;
	*/

    logclass::Log* l = Singleton<logclass::Log>::get();
    l -> SetConfig((std::string)"F");
    l -> Write("Fuck!");
    size_t start = (size_t)clock();
    b.SetScoreFunction((std::string)"trivial_score_function", 0);
    for(int i = 0; i < 40000000; ++i){
        b.GenMovesWithScore();
    }
    printf("%d\n", b.num_of_legal_moves);
    size_t end = (size_t)clock();
    printf("%d %lf\n", b.num_of_legal_moves, (double)(end - start)/CLOCKS_PER_SEC);
    Singleton<logclass::Log>::deleteT();
    return 0;
}
