#include "god.h"
#include "board.h"

God::God(const char* file){
   board_pointer = Singleton<board::Board>::get();
   initialize_di();
   std::ifstream instream(file);
   int counter = 0;
   std::string line;
   if(!instream.is_open()){
      ok = false;
      return;
   }
   while(std::getline(instream, line)){
      if(counter >= 2){
         ok = false;
         return;
      }
      line = trim(line);
      if(line.size() != 1) {
         ok = false;
         return;
      }
      if(line[0] != '0' && line[0] != '1'){
         ok = false;
         return;
      }else if(line[0] == '0'){
         if(counter == 0){
            type1 = false;
         }else{
            type2 = false;
         }
      }else if(line[0] == '1'){
         if(counter == 0){
            type1 = true;
         }else{
            type2 = true;
         }
      }
      ++counter;
   }
   printf("I am the referee. You two must listen to me!\n");
   ok = true;
   instream.close();
}

God::~God(){
   Singleton<board::Board>::deleteT();
   if(thinker1) thinker1.reset();
   if(thinker2) thinker2.reset();
}

void God::initialize_di(){
   memmove(this -> di, ::di, sizeof(::di));
   memmove(this -> di_red, ::di, sizeof(::di));
   memmove(this -> di_black, ::di, sizeof(::di));
}

int God::StartThinker(){
   if(!ok) return -1;
   board_pointer -> GenMovesWithScore();
   board_pointer -> CopyToIsLegalMove();
   if(board_pointer -> turn){
      if(type1){
         std::cout << PrintEat(board_pointer -> turn) << std::endl;
         board_pointer -> PrintPos(board_pointer -> turn, true, false, true);
         thinker1.reset(new board::Human());
      }else{
         thinker1.reset(new board::AIBoard(board_pointer -> state_red, board_pointer -> turn, board_pointer -> round));
      }
      thinker1 -> thinker_type = type1;
      thinker1 -> retry_num = thinker1 -> thinker_type?3:1;
      thinker1 -> CopyData(this -> di_red);
      for(int i = 0; i < thinker1 -> retry_num; ++i){
         std::string think_result = thinker1 -> Think(); // This function might cost a lot of time!
         if(!check_legal(think_result)) continue;
         std::shared_ptr<InfoDict> p = board_pointer -> Move(think_result, true);
         if(p && p -> islegal){
            red_eat_black.push_back({p -> eat, p -> eat_type, 16 * p -> dst_x + p -> dst_y});
            board_pointer -> PrintPos(!board_pointer -> turn, true, false, true);
            return p -> win ? RED_WIN : NORMAL;
         }
      }
      printf("红方连续%d个无效棋步, 判输!\n", thinker1 -> retry_num);
      return BLACK_WIN;
   }else{
      if(type2){
         std::cout << PrintEat(board_pointer -> turn) << std::endl;
         board_pointer -> PrintPos(board_pointer -> turn, true, false, true);
         thinker2.reset(new board::Human(board_pointer -> turn, board_pointer -> round));
      }else{
         thinker2.reset(new board::AIBoard(board_pointer -> state_black, board_pointer -> turn, board_pointer -> round));
      }
      thinker2 -> thinker_type = type1;
      thinker2 -> retry_num = thinker1 -> thinker_type?3:1;
      thinker2 -> CopyData(this -> di_black);
      for(int i = 0; i < thinker2 -> retry_num; ++i){
         std::string think_result = thinker2 -> Think(); // This function might cost a lot of time!
         if(!check_legal(think_result)) continue;
         std::shared_ptr<InfoDict> p = board_pointer -> Move(think_result, true);
         if(p && p -> islegal){
            black_eat_red.push_back({p -> eat, p -> eat_type, 16 * p -> dst_x + p -> dst_y});
            board_pointer -> PrintPos(!board_pointer -> turn, true, false, true);
            return p -> win ? BLACK_WIN : NORMAL;
         }
      }
      printf("黑方连续%d个无效棋步, 判输!\n", thinker2 -> retry_num);
      return RED_WIN;
   }
   return NORMAL;
}

int God::StartGame(){
   int result = NORMAL;
   board_pointer -> Reset();
   while(result == NORMAL && board_pointer -> round < MAX_ROUNDS){
      result = StartThinker();
      if(result == RED_WIN){
         printf("红胜!\n");
         return RED_WIN;
      }else if(result == BLACK_WIN){
         printf("黑胜!\n");
         return BLACK_WIN;
      }
   }
   return DRAW;
}

bool God::GetTurn(){
   return board_pointer -> turn;
}

std::string God::PrintEat(bool turn){
   std::string ret;
   if(turn){
      ret += "红吃黑: ";
      for(const auto& tuple : red_eat_black){
         ret += std::get<0>(tuple);
      }
      ret += '\n';
      ret += "黑吃红: ";
      for(const auto& tuple: black_eat_red){
         if(std::get<1>(tuple) == 2){
            TRANSLATE_PLACE_TO_COVERED(std::get<2>(tuple));
            ret += covered;
            ret += "(DARK), ";
         }else{
            ret += std::get<0>(tuple);
            ret += ", ";
         }
      }
   }else{
      ret += "黑吃红: ";
      for(const auto& tuple : black_eat_red){
         ret += std::get<0>(tuple);
      }
      ret += "红吃黑: ";
      for(const auto& tuple: red_eat_black){
         if(std::get<1>(tuple) == 2){
            TRANSLATE_PLACE_TO_COVERED(std::get<2>(tuple));
            ret += covered;
            ret += "(DARK), ";
         }else{
            ret += std::get<0>(tuple);
            ret += ", ";
         }
      }
   }
   return ret;
}