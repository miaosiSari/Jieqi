#include "score.h"

std::unordered_map<std::string, SCORE> function_bean;
short pst[123][256 * 256];

inline unsigned short trivial_score_function(const char* board, unsigned char, unsigned char){
   //Singleton<log::Log>::get() -> Write("trivial_scoring_function!");
    return 0;
}

void register_score_functions(){
    function_bean.insert({"trivial_score_function", trivial_score_function});
}


inline std::string trim(const std::string &s)
{
    auto wsfront=std::find_if_not(s.begin(),s.end(),[](int c){return std::isspace(c);});
    auto wsback=std::find_if_not(s.rbegin(),s.rend(),[](int c){return std::isspace(c);}).base();
    return (wsback <= wsfront ? std::string() : std::string(wsfront,wsback));
}

void read_score_table(const char* score_file){
	memset(pst, 0, sizeof(pst));
	std::locale loc = std::locale::global(std::locale(""));
    std::ifstream instream(score_file);
    std::string line = "";
    constexpr int M = 10;
    constexpr int N = 9;
    const std::string MINGZI = "RNBAKCP";
    int state_machine = M;
    char key = '\0';

    if(!instream.is_open()) {
        printf("[FAILED 0]score --> score.cpp --> read_score_table --> Open %s FAILED!\n", score_file);
        RETURN;
    }

    while(std::getline(instream, line)){
    	std::string tmpline = trim(line);
    	int len = tmpline.size();
    	if(len == 0){
    		continue;
    	}else if(len == 1 && isupper(tmpline[0])){
    		if(state_machine == M){
    			if(key != '\0'){
    				printf("score --> score.cpp --> read_score_table --> Read %c finishes!\n", key);
    			}
    			state_machine = 0;
                key = tmpline[0];
                if(MINGZI.find(key) == std::string::npos){
                	printf("[FAILED 1]score --> score.cpp --> read_score_table --> Read key FAILED! state_machine = %d, key = %c\n", state_machine, key);
                	RETURN;
                }
                printf("score --> score.cpp --> read_score_table --> Start reading key %c!\n", key);
                continue;
    		} else{
    			printf("[FAILED 2]score --> score.cpp --> read_score_table --> Read key FAILED! state_machine = %d\n", state_machine);
    			RETURN;
    		}
    	}else{
    		if(state_machine < 0 || state_machine >= M){
    			printf("[FAILED 3]score --> score.cpp --> read_score_table --> Read key FAILED (counter != N)! state_machine = %d\n", state_machine);
    			RETURN;
    		}
            std::stringstream ss;
            ss << tmpline;
            int counter = 0;
            int tmpint = 0;
            while(ss >> tmpint){
                pst[key][ENCODE(state_machine, counter)] = tmpint;
                ++counter;
                if(counter > N){
                	break;
                } //counter > N
            } // ss >> tmpint
            if(counter != N) {
            	printf("[FAILED 4]score --> score.cpp --> read_score_table --> Read key FAILED (counter != N)! state_machine = %d, counter = %d, N = %d\n", state_machine, counter, N);
    			RETURN;
            }
            ++state_machine;
        }
    }

    std::locale::global(loc);
}