#include "god.h"

template<typename T>
bool isT(std::string s, T* i){
    //用于判断字符串是否为整数/浮点型
    std::stringstream ss;
    ss << s;
    ss >> *i;
    if(!ss.fail() && ss.eof()) { 
        return true; 
    }else{
        *i = 0;//由于有无符号数, = 0 更好
        return false;
    }
    return false;
}


God::God(const char* file): ok(true), redwin(0), blackwin(0), draw(0), file(file), logfile(""){
    tptable = new (std::nothrow) tp[MAX_ZOBRIST];
    if(!file || (this -> file).size() == 0){
        ok = false;
        return;
    }
    board_pointer = Singleton<board::Board>::get();
    std::ifstream instream(file);
    int counter = 0;
    std::string line;
    if(!instream.is_open()){
        ok = false;
        assert(false);
        return;
    }
    while(std::getline(instream, line)){
        if(counter >= 4){
            ok = false;
            break;
        }
        line = trim(line);

        if(counter == 0 && !line.empty()){
            if(!isT<int>(line, &type1)){
                ok = false;
            }
        }

        else if(counter == 1 && !line.empty()){
            if(!isT<int>(line, &type2)){
                ok = false;
            }
        }

        else if(counter == 2 && !line.empty()){
            if(!isT<size_t>(line, &winning_threshold_class)){
                ok = false;
            }
        }

        else if(counter == 3 && !line.empty()){
            if(line[0] == '@'){
                logfile = line.substr(1, line.size() - 1);
                std::ofstream(logfile, std::ios_base::trunc);
            }else{
                logfile = line;
            }
        }

        ++counter;
    }
    if(counter < 2){
        ok = false;
    }
    printf("I am the referee. You two must listen to me! player1 = %d, player2 = %d\n", type1, type2);
    assert(ok);
    ok = true;
    instream.close();
}

God::~God(){
    if(tptable){
       delete[] tptable;
       tptable = NULL;
    }
    Singleton<board::Board>::deleteT();
    if(thinker1) thinker1.reset();
    if(thinker2) thinker2.reset();
}

int God::StartThinker(std::ofstream* of){
    if(!ok) return -1;
    board_pointer -> GenMovesWithScore();
    if(board_pointer -> turn){
        if(type1 == 0){
            printf("红方行棋!\n");
            std::cout << PrintEat(board_pointer -> turn, false) << std::endl;
            board_pointer -> PrintPos(board_pointer -> turn, true, false, true);
            thinker1.reset(new board::Human(board_pointer -> turn, board_pointer -> round));
        }else{
            thinker1.reset(NEWRED);
        }
        if(!thinker1){
            printf("红空指针!\n");
            return BLACK_WIN;
        }
        thinker1 -> thinker_type = type1;
        thinker1 -> retry_num = thinker1 -> thinker_type?1:5;
        for(int i = 0; i < thinker1 -> retry_num; ++i){
            std::string think_result = thinker1 -> Think(8); // This function might cost a lot of time!
            std::string trim_think_result = trim(think_result);
            if(trim_think_result == "R" || trim_think_result == "r"){
                return BLACK_WIN;
            }else if(trim_think_result == "W" || trim_think_result == "w"){
                return RED_WIN;
            }else if(trim_think_result == "D" || trim_think_result == "d"){
                return DRAW;
            }else if(trim_think_result == "P" || trim_think_result == "p"){
                return WASTE;
            }
            if(!check_legal(think_result)) continue;
            std::shared_ptr<InfoDict> p = board_pointer -> Move(think_result, true);
            if(p && p -> islegal){
                if(of){
                    (*of) << think_result << "\n";
                }
                red_eat_black.push_back({p -> eat, p -> eat_type, 195 - 16 * p -> dst_x + p -> dst_y, p -> eat_check});
                board_pointer -> PrintPos(!board_pointer -> turn, true, false, true);
                printf("第%d轮红方行棋结束\n========================================\n\n", board_pointer -> round);
                return p -> win ? RED_WIN : NORMAL;
            }
        } 
        printf("红方连续%d个无效棋步, 判输!\n", thinker1 -> retry_num);
        return BLACK_WIN;
    }else{
        if(type2 == 0){
            printf("黑方行棋!\n");
            std::cout << PrintEat(board_pointer -> turn, false) << std::endl;
            board_pointer -> PrintPos(board_pointer -> turn, true, false, true);
            thinker2.reset(new board::Human(board_pointer -> turn, board_pointer -> round));
        }else{
            thinker2.reset(NEWBLACK);
        }
        if(!thinker2){
            printf("黑空指针!\n");
            return RED_WIN;
        }
        thinker2 -> thinker_type = type2;
        thinker2 -> retry_num = thinker2 -> thinker_type?1:5;
        for(int i = 0; i < thinker2 -> retry_num; ++i){
            std::string think_result = thinker2 -> Think(8); // This function might cost a lot of time!
            std::string trim_think_result = trim(think_result);
            if(trim_think_result == "R" || trim_think_result == "r"){
                return RED_WIN;
            }else if(trim_think_result == "W" || trim_think_result == "w"){
                return BLACK_WIN;
            }else if(trim_think_result == "D" || trim_think_result == "d"){
                return DRAW;
            }else if(trim_think_result == "P" || trim_think_result == "p"){
                return WASTE;
            }
            if(!check_legal(think_result)) continue;
            std::shared_ptr<InfoDict> p = board_pointer -> Move(think_result, true);
            if(p && p -> islegal){
                if(of){
                    (*of) << think_result << "\n";
                }
                black_eat_red.push_back({p -> eat, p -> eat_type, 195 - 16 * p -> dst_x + p -> dst_y, p -> eat_check});
                board_pointer -> PrintPos(!board_pointer -> turn, true, false, true);
                printf("第%d轮黑方行棋结束\n========================================\n\n", board_pointer -> round);
                return p -> win ? BLACK_WIN : NORMAL;
            }
        }
        printf("黑方连续%d个无效棋步, 判输!\n", thinker2 -> retry_num);
        return RED_WIN;
   }
   return NORMAL;
}

void God::Play(std::string logfile){
    printf("\n对局回顾!\n");
    std::ifstream in(logfile);
    if(!in.is_open()){
        std::cout << logfile << " is NOT open!\n";
        return;
    }
    std::string line;
    int state = 0;
    size_t gamecounter = 0;
    bool blocked = false;
    std::unordered_map<bool, std::unordered_map<unsigned char, char>> random_map;
    while(std::getline(in, line)){
        line = trim(line);
        if(line.find("CLAIM") != std::string::npos){
            continue;
        }
        if(line.empty()) {
            break;
        }
        if(line[0] == '#'){
            blocked = false;
            std::string s;
            int i = 0;
            int pos = 0;
            char c = ' ';
            std::istringstream ss(line);
            while(std::getline(ss, s, ' ')){
                if(i >= 1){
                    if(i & 1){
                        if(!isT<int>(s, &pos)){
                            assert(false);
                            break;
                        }
                    }
                    else{
                        assert(!s.empty());
                        c = s[0];
                        assert(c == 'R' || c == 'N' || c == 'B' || c == 'A' || c == 'C' || c == 'P' || c == 'r' || c == 'n' || c == 'b' || c == 'a' || c == 'c' || c == 'p');
                        if(state == 0){
                            random_map[true][(unsigned char)pos] = c;   
                        }else if(state == 1){
                            random_map[false][(unsigned char)pos] = c;
                        }
                    }
                }
                i++;
            }
            state++;
        }

        if(state == 2){
            board_pointer -> Reset(&random_map);
            assert(board_pointer -> CheckRandomMap());
            printf("\n对局No.%zu:\n", gamecounter);
            red_eat_black.clear();
            black_eat_red.clear();
            board_pointer -> PrintPos(true, true, false, true);
            state = 0;
            ++gamecounter;
        }

        if(line.size() == 4 && !blocked){
            board_pointer -> GenMovesWithScore();
            std::string turn = board_pointer -> turn ? "红方: " : "黑方: ";
            std::cout << turn << line << "\n";
            getchar();
            std::shared_ptr<InfoDict> p = board_pointer -> Move(line, true);
            assert(p && p -> islegal);
            if(p -> win){
                blocked = true;
                if(board_pointer -> turn){
                    printf("黑胜!\n");
                }else{
                    printf("红胜!\n");
                }
            }
            if(!board_pointer -> turn){
                red_eat_black.push_back({p -> eat, p -> eat_type, 195 - 16 * p -> dst_x + p -> dst_y, p -> eat_check});
            }
            else{
                black_eat_red.push_back({p -> eat, p -> eat_type, 195 - 16 * p -> dst_x + p -> dst_y, p -> eat_check});
            }
            std::cout << PrintEat(true, true) << "\n";
            board_pointer -> PrintPos(true, true, false, true);
        }

    }
}

void God::Play(){
    Play(logfile);
}

int God::StartGame(){
    red_eat_black.clear();
    black_eat_red.clear();
    bool write = false;
    std::ofstream of(logfile, std::ios::app);
    if(of.is_open()){
        write = true;
    }
    int result = NORMAL;
    board_pointer -> Reset(NULL);
    if(write){
        of << "# ";
        for(auto it = board_pointer -> random_map[true].begin(); it != board_pointer -> random_map[true].end(); ++it){
            of << (int)it -> first << " " << it -> second << " ";
        }
        of << "\n# ";
        for(auto it = board_pointer -> random_map[false].begin(); it != board_pointer -> random_map[false].end(); ++it){
            of << (int)it -> first << " " << it -> second << " "; 
        }
        of << "\n";
    }
    while(result == NORMAL && board_pointer -> round < MAX_ROUNDS){
        result = StartThinker(write ? &of : NULL);
        if(result == -1) {
            printf("BUG!\n");
            return -1;
        } 
        if(result == RED_WIN){
            printf("红胜!\n");
            ++redwin;
            of << "CLAIM " << RED_WIN << "\n";
            return RED_WIN;
        }else if(result == BLACK_WIN){
            printf("黑胜!\n");
            ++blackwin;
            of << "CLAIM " << BLACK_WIN << "\n";
            return BLACK_WIN;
        }
        if(result == WASTE){
            return WASTE;
            of << "CLAIM " << WASTE << "\n";
        }
        if(result == DRAW){
            ++draw;
            of << "CLAIM " << DRAW << "\n";
            return DRAW;
        }
    }
    ++draw;
    return DRAW;
}

int God::StartGameLoop(size_t winning_threshold){
   if(winning_threshold == 0) {
      return DRAW;
   }
   size_t i = 0;
   redwin = 0;
   draw = 0;
   blackwin = 0;
   size_t maxgame = 2 * winning_threshold - 1;
   while(i < maxgame && draw < maxgame){
        int state = StartGame();
        printf("红%zu : 和%zu : 黑%zu\n", redwin, draw, blackwin);
        if(state != DRAW && state != WASTE){
            ++i;
        }
        if(redwin >= winning_threshold){
            printf("红方最终胜利!\n");
            printf("红%zu : 和%zu : 黑%zu\n", redwin, draw, blackwin);
            std::ofstream of(logfile, std::ios::app);
            if(of.is_open()){
                of << "CLAIM RED-DRAW-BLACK " << redwin << "-" << draw << "-" << blackwin << "\n";
            }
            return RED_WIN;
        }
        if(blackwin >= winning_threshold){
            printf("黑方最终胜利!\n");
            printf("红%zu : 和%zu : 黑%zu\n", redwin, draw, blackwin);
            std::ofstream of(logfile, std::ios::app);
            if(of.is_open()){
                of << "CLAIM RED-DRAW-BLACK " << redwin << "-" << draw << "-" << blackwin << "\n";
            }
            return BLACK_WIN;
        }
   }
   printf("红%zu : 和%zu : 黑%zu\n", redwin, draw, blackwin);
   std::ofstream of(logfile, std::ios::app);
   if(of.is_open()){
      of << "CLAIM RED-DRAW-BLACK " << redwin << "-" << draw << "-" << blackwin << "\n";
   }
   printf("握手言和\n");
   return DRAW;
}

int God::StartGameLoop(){
   return StartGameLoop(this -> winning_threshold_class);
}

int God::StartGameLoopAlternatively(size_t winning_threshold){
    redwin = 0;
    draw = 0;
    blackwin = 0;
    if(winning_threshold == 0) {
        return DRAW;
    }
    size_t player1win = 0;
    size_t player2win = 0;
    size_t i = 0;
    bool cnt = true;
    size_t maxgame = 2 * winning_threshold - 1;
    while(i < maxgame && draw < maxgame){
        printf("cnt = %d. Player1_win %zu : Draw %zu : Player2_win %zu\n", cnt, player1win, draw, player2win);
        int state = StartGame();
        if(cnt){
            if(state == RED_WIN){
                ++player1win;
                ++i;
            }else if(state == BLACK_WIN){
                ++player2win;
                ++i;
            }
        }else{
            if(state == RED_WIN){
                ++player2win;
                ++i;
            }else if(state == BLACK_WIN){
                ++player1win;
                ++i;
            }
        }
        if(player1win >= winning_threshold){
            printf("玩家1获胜!\n");
            printf("cnt = %d. Player1_win %zu : Draw %zu : Player2_win %zu\n", cnt, player1win, draw, player2win);
            std::ofstream of(logfile, std::ios::app);
            if(of.is_open()){
                of << "CLAIM PLAYER1-DRAW-PLAYER2 " << player1win << "-" << draw << "-" << player2win << "\n";
            }
            return 1;
        }else if(player2win >= winning_threshold){
            printf("玩家2获胜!\n");
            printf("cnt = %d. Player1_win %zu : Draw %zu : Player2_win %zu\n", cnt, player1win, draw, player2win);
            std::ofstream of(logfile, std::ios::app);
            if(of.is_open()){
                of << "CLAIM PLAYER1-DRAW-PLAYER2 " << player1win << "-" << draw << "-" << player2win << "\n";
            }
            return 2;
        }
        cnt = !cnt;
        std::swap(type1, type2);
    }
    printf("握手言和!\n");
    printf("cnt = %d. Player1_win %zu : Draw %zu : Player2_win %zu\n", cnt, player1win, draw, player2win);
    std::ofstream of(logfile, std::ios::app);
    if(of.is_open()){
        of << "CLAIM PLAYER1-DRAW-PLAYER2 " << player1win << "-" << draw << "-" << player2win << "\n";
    }
    return 0;
}

int God::StartGameLoopAlternatively(){
   return StartGameLoopAlternatively(winning_threshold_class);
}

bool God::GetTurn(){
   return board_pointer -> turn;
}

std::string God::PrintEat(bool turn, bool SHOWDARK){
    std::string ret;
    if(turn){
        ret += "黑吃红: ";
        for(size_t i = 0; i < black_eat_red.size(); ++i){
            if(SHOWDARK){
                auto tuple = black_eat_red[i];
                if(std::get<1>(tuple) == 2){
                    char covered = board_pointer -> random_map[!turn][std::get<2>(tuple)];
                    assert(covered == std::get<3>(tuple));
                    ret += isdot(covered, true, true);
                }else{
                    ret += isdot(std::get<0>(tuple), false, true);
                }
            }
            else{
                ret += isdot(std::get<0>(black_eat_red[i]), false, true);
            }
        }
        ret += '\n';
        ret += "红吃黑: ";
        for(size_t i = 0; i < red_eat_black.size(); ++i){
            auto tuple = red_eat_black[i];
            if(std::get<1>(tuple) == 2){
                char covered = board_pointer -> random_map[turn][std::get<2>(tuple)];
                assert(covered == std::get<3>(tuple));
                ret += isdot(covered, true, false);
            }else{
                ret += isdot(std::get<0>(tuple), false, false);
            }
        }
    }else{
        ret += "红吃黑: ";
        for(size_t i = 0; i < red_eat_black.size(); ++i){
            if(SHOWDARK){
                auto tuple = red_eat_black[i];
                if(std::get<1>(tuple) == 2){
                    char covered = board_pointer -> random_map[!turn][std::get<2>(tuple)];
                    assert(covered == std::get<3>(tuple));
                    ret += isdot(covered, true, false);
                }else{
                    ret += isdot(std::get<0>(tuple), false, false);
                }
            }else{
                ret += isdot(std::get<0>(red_eat_black[i]), false, false);
            }
        }
        ret += '\n';
        ret += "黑吃红: ";
        for(size_t i = 0; i < black_eat_red.size(); ++i){
            auto tuple = black_eat_red[i];
            if(std::get<1>(tuple) == 2){
                char covered = board_pointer -> random_map[turn][std::get<2>(tuple)];
                assert(covered == std::get<3>(tuple));
                ret += isdot(covered, true, true);
            }else{
                ret += isdot(std::get<0>(tuple), false, true);
            }
        }
   }
   return ret;
}
