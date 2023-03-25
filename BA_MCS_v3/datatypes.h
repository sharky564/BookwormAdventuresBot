#ifndef VECTOR
#define VECTOR
#include <vector>
#endif

#ifndef STRING
#define STRING
#include <string>
#endif

using namespace std;

/* struct which stores
 individual tiles, storing their letter 
 * used to store any gem they have
 */
typedef struct tile {
    char letter;
    char gem;
} tile;

/* struct which stores a word and its score
 * used to sort the words by score
 */
typedef struct wordScore {
    vector<tile> word;
    float score;

    // prints a word_score struct
    void _print_word_score() {
        int len = (int)this->word.size();
        string word(len, ' '), gem(len, ' ');

        for (int i = 0; i < len; i++) {
            word[i] = this->word[i].letter;
            gem[i] = this->word[i].gem;
        }
        printf("Word = %s, gem = %s, score = %.3f\n", 
            addQU(word).c_str(), 
            gem.c_str(), 
            this->score
        );
    }

    bool operator<(const wordScore& other) {
        return score > other.score;
    }
} wordScore;