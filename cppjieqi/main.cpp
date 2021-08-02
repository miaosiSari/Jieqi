// copyright 2021 miaosi@all rights reserved.
//#include <QCoreApplication>
#include <stdio.h>
#include <iostream>
#include "board/board.h"
#include "log/log.h"
#include "global/global.h"
#include "score/score.h"

extern void register_scoring_functions();

int main(void) {
    //QCoreApplication a(argc, argv);
    register_scoring_functions();
    printf("Hello Jieqi!\n");
    board::Board b;
    std::vector<int> res = b.GetInfo();
    int cnt = 0;
    for(auto i = res.begin(); i < res.end(); ++i){
        std::cout << (char)*i << " " << *i << " " << cnt << std::endl;
        ++cnt;
    }
    std::cout << b.GetStateString() << " " << b.GetStateString().size() << std::endl;
    std::cout << std::get<2>(b.GetTuple()) << "\n";
    b.PrintPos();
    b.Move(std::make_pair<int, int>(0, 0), std::make_pair<int, int>(1, 0));
    b.PrintPos();
    log::Log* l = Singleton<log::Log>::get();
    l -> SetConfig("F");
    l -> Write("Hello world!");
    Singleton<log::Log>::deleteT();
    return 0;
}
