#ifndef STRING
#define STRING
#include <string>
#endif

using namespace std;

string addQU(string word) {
    string outWord;
    for(char c: word) {
        outWord.push_back(c);
        if (c == 'Q') {
            outWord.push_back('U');
        }
    }
    return outWord;
}

// maximum of two numbers
inline int _max(int a, int b) {
    return a > b ? a : b;
}

// minimum of two numbers
inline int _min(int a, int b) {
    return a < b ? a : b;
}

// function which rounds a number to the nearest integer
inline int _round(float num) {
    return (int)(num + 0.5);
}

// function which outputs the ceiling of a float
inline int _ceil(float num) {
    if (abs((int)num - num) < 1e-4) {
        return (int)num;
    }
    return (int)num + 1;
}

// convert tile to priority
int tileToPriority(char gem) {
    switch (gem) {
        case 'D':
            return 7;
        case 'C':
            return 6;
        case 'R':
            return 5;
        case 'S':
            return 4;
        case 'G':
            return 3;
        case 'E':
            return 2;
        case 'A':
            return 1;
        default:
            return 0;
    }
}

float gemBoost(char gem) {
    switch (gem) {
        case 'D':
            return 1;
        case 'C':
            return 0.5;
        case 'R':
            return 0.35;
        case 'S':
            return 0.25;
        case 'G':
            return 0.3;
        case 'E':
            return 0.2;
        case 'A':
            return 0.15;
        default:
            return 0;
    }
}

char scoreToGem(float score) {
    char gem;
    if (score < 5) {
        gem = ' ';
    }
    else if (score < 6) {
        gem = 'A';
    }
    else if (score < 7) {
        gem = 'E';
    }
    else if (score < 8) {
        gem = 'G';
    }
    else if (score < 9) {
        gem = 'S';
    }
    else if (score < 10) {
        gem = 'R';
    }
    else if (score < 11) {
        gem = 'C';
    }
    else {
        gem = 'D';
    }
    return gem;
}