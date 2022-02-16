#ifndef score_h
#define score_h

#include <unordered_map>
#include <algorithm>
#include <locale>
#include <sstream>
#include <fstream>
#include <locale>
#include <cmath>
#include <string>
#include <string_view>
#include <regex>
#include <stdio.h>
#include <string.h>
#include "../global/global.h"
#define INTR (int)'R'
#define INTN (int)'N'
#define INTB (int)'B'
#define INTA (int)'A'
#define INTC (int)'C'
#define INTP (int)'P'
#define INTr (int)'r'
#define INTn (int)'n'
#define INTb (int)'b'
#define INTa (int)'a'
#define INTc (int)'c'
#define INTp (int)'p'
#define RETURN memset(pst, 0, 123*256); return false;
#define ENCODE(x, y) ((16 * (x)) + (y) + 51)
#define VERSION_MAX 2
#define SUM(VERSION) \
short numr = 0, numb = 0; \
numr += di[VERSION][1][INTR]; numr += di[VERSION][1][INTN];  numr += di[VERSION][1][INTB];  numr += di[VERSION][1][INTA];  numr += di[VERSION][1][INTC]; numr += di[VERSION][1][INTP]; \
numb += di[VERSION][0][INTr]; numb += di[VERSION][0][INTn];  numb += di[VERSION][0][INTb];  numb += di[VERSION][0][INTa];  numb += di[VERSION][0][INTc]; numb += di[VERSION][0][INTp]; \
sumall[VERSION][1] = numr; sumall[VERSION][0] = numb;

const std::string MINGZI = "RNBAKCP";
bool read_score_table(const char* score_file, short pst[][256]);
void copy_pst(short dst[][256], short src[][256]);
bool read_kaijuku(const char* kaijuku_file, std::unordered_map<std::string, std::pair<unsigned char, unsigned char>>& kaijuku);
void IntializeL1();

#endif
