// copyright 2021 miaosi@all rights reserved.
#include <QCoreApplication>
#include <stdio.h>
#include <iostream>
#include "board/board.h"

int main(int argc, char *argv[]) {
    QCoreApplication a(argc, argv);
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
    return 0;
}
