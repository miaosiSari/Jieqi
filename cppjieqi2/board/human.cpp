#include "human.h"

board::Human::Human():turn(true), round(0){
   
}

board::Human::Human(bool turn, int round):turn(turn), round(round){

}

board::Human::~Human(){

}

std::string board::Human::Think(int maxdepth){
	do { (void)(maxdepth); } while (0);
	std::string think_result;
	if(turn){
        printf("第%d轮, 红方行棋: \n", round);
	}else{
        printf("第%d轮, 黑方行棋: \n", round);
	}
	getline(std::cin, think_result);
	trim(think_result);
	return think_result;
}