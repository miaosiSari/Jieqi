#include "score.h"

short pst[123][256];
short average[VERSION_MAX][2][2][256];
unsigned char sumall[VERSION_MAX][2];
unsigned char di[VERSION_MAX][2][123];

bool read_score_table(const char* score_file){
    std::unordered_map<char, bool> is_read;
    is_read['R'] = is_read['N'] = is_read['B'] = is_read['A'] = is_read['K'] = is_read['C'] = is_read['P'] = false;

    memset(pst, 0, sizeof(pst));
    std::ifstream instream(score_file);
    std::string line = "";
    constexpr int M = 10;
    constexpr int N = 9;
    int state_machine = M;
    char key = '\0';

    if(!instream.is_open()) {
        printf("[FAILED 0]score --> score.cpp --> read_score_table --> Open %s FAILED!\n", score_file);
        RETURN;
    }

    while(std::getline(instream, line)){
        std::string tmpline = subtrim(line);
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
                is_read[key] = true;
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
                pst[(int)key][ENCODE(state_machine, counter)] = tmpint;
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
    bool ret = true;
    for(auto it = is_read.begin(); it != is_read.end(); ++it){
        if(!it -> second){
            ret = false;
            printf("%c not initialized!\n", it -> first);
        }
    }
    if(ret) { printf("SUCCESSFUL reading score conf!\n"); } else { RETURN; }
    return true;
}

void initialize_score_table(float discount_factor=1.5){
    for(const char c : MINGZI){
        int intc = (int)c;
        pst[intc][0] = 0;
        for(int i = 51; i <= 203; ++i){
            pst[intc][0] += pst[intc][i];
        }
        pst[intc][0] /= 90;
    }
    memset(di, 0, sizeof(di));
    memset(average, 0, sizeof(average));
    for(int version = 0; version < VERSION_MAX; ++version){
        di[version][0][INTr] = 2;
        di[version][0][INTn] = 2;
        di[version][0][INTb] = 2;
        di[version][0][INTa] = 2;
        di[version][0][INTc] = 2;
        di[version][0][INTp] = 5;
        di[version][1][INTR] = 2;
        di[version][1][INTN] = 2;
        di[version][1][INTB] = 2;
        di[version][1][INTA] = 2;
        di[version][1][INTC] = 2;
        di[version][1][INTP] = 5;
        SUM(version);
        if(numr > 0){
            double sumr = 0.0;
            for(const char c : MINGZI){
                sumr += pst[(int)c][0] * di[version][1][(int)c] / discount_factor;
            }
            average[version][1][0][0] = (short)sumr / numr;
            for(int i = 51; i <= 203; ++i){
                sumr = 0.0;
                for(const char c : MINGZI){
                    sumr += pst[(int)c][i] * di[version][1][(int)c];
                }
                average[version][1][1][i] = (short)sumr / numr;
            }
        }
        if(numb > 0){
            double sumb = 0.0;
            for(const char c : MINGZI){
                sumb += pst[(int)c][0] * di[version][0][(int)c] / discount_factor;
            }
            average[version][0][0][0] = ::round(sumb / numb);
            for(int i = 51; i <= 203; ++i){
                sumb = 0.0;
                for(const char c : MINGZI){
                    sumb += pst[(int)c][i] * di[version][0][((int)c)^32];
                }
                average[version][0][1][i] = ::round(sumb / numb);
            }
        }
    }
}

bool debug(const char* debug_output_file){
    std::ofstream out_trunc(debug_output_file, std::ios::trunc|std::ios::out);
    std::ofstream out(debug_output_file, std::ios::app|std::ios::out);
    if(!out.is_open()){
       return false;
    }
    out << "pst\n";
    for(const char c : MINGZI){
        int intc = (int)c;
        out << c << " " << pst[intc][0] << "\n";
        for(int i = 51; i <= 203; ++i){
            if((i & 15) >= 3 && (i & 15) <= 11){
               out << pst[intc][i] << " ";
               if((i & 15) == 11) {out << "\n";}
            }
        }
    }

    constexpr int version = 0;
    out << "di&sumall\n";
    out << (int)di[version][1][INTR] << " " << (int)di[version][1][INTN] << " " << (int)di[version][1][INTB] << " " << (int)di[version][1][INTA] << " " << (int)di[version][1][INTC] << " " << (int)di[version][1][INTP] << " ";
    out << (int)di[version][0][INTr] << " " << (int)di[version][0][INTn] << " " << (int)di[version][0][INTb] << " " << (int)di[version][0][INTa] << " " << (int)di[version][0][INTc] << " " << (int)di[version][0][INTp] << " ";
    out << (int)sumall[version][1] << " " << (int)sumall[version][0] << "\n";

    out << "average\n";
    out << average[version][1][0][0] << " " << average[version][0][0][0] << "\n";
    out << "red\n";
    for(int i = 51; i <= 203; ++i){
        if((i & 15) >= 3 && (i & 15) <= 11){
           out << average[version][1][1][i] << " ";
           if((i & 15) == 11) {out << "\n";}
        }
    }
    out << "black\n";
    for(int i = 51; i <= 203; ++i){
        if((i & 15) >= 3 && (i & 15) <= 11){
           out << average[version][0][1][i] << " ";
           if((i & 15) == 11) {out << "\n";}
        }
    }
    out.close();
    return true;
}

bool initialize_wrapper(const char* score_file, const char* debug_output_file, float discount_factor=1.5){
    printf("STEP 1: Read the score table:\n");
    if(!read_score_table(score_file)){
        printf("Read %s failed!\n", score_file);
        return false;
    }
    printf("STEP 2： Initialize the score table (discount_factor = %f)!\n", discount_factor);
    initialize_score_table(discount_factor);
    if(debug_output_file && strlen(debug_output_file) > 0){
        printf("STEP 3： Output debug message to %s!\n", debug_output_file);
        if(!debug(debug_output_file)){
            return false;
        }
    }else{
        printf("Skip debug step!\n");
    }
    return true;
}
