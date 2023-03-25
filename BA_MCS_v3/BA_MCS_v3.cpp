#pragma GCC optimize("Ofast,unroll-loops")
#pragma GCC target("avx,avx2,sse,sse2")

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <stdbool.h>
#include <time.h>

#include "constants.h"
#include "utils.h"
#include "datatypes.h"
#include "preprocessing.h"

#include <cassert>
#include <algorithm>
#include <deque>
#include <iostream>
#include <unordered_map>

#ifndef VECTOR
#include <vector>
#endif

#ifndef STRING
#include <string>
#endif

using namespace std;

// convert list of tiles to word
string tilesToWord(const vector<tile>& tiles) {
    string word;
    for(tile t: tiles) {
        word.push_back(t.letter);
    }
    return word;
}

// convert word to list of tiles in rack
bool used_idx[16];

vector<tile> wordToTiles(const vector<tile>& sortedRackTiles, string word) {
    vector<tile> output;
    
    memset(used_idx, false, 16 * sizeof(bool));
    
    for (int k = 0; k < (int)word.size(); k++) {
        for (int l = 0; l < 16; l++) {
            if (!used_idx[l] and sortedRackTiles[l].letter == word[k]) {
                output.push_back(sortedRackTiles[l]);
                used_idx[l] = true;
                break;
            }
        }
    }
    return output;
}

// We do another implementation of wordToTiles using a deque
vector<tile> wordToTiles2(const vector<tile>& sortedRackTiles, string word) {
    vector<tile> output;
    // create a dictionary which maps each letter of a tile in a rack to all the tiles with that letter
    unordered_map<char, deque<tile>> rackTiles;
    for (int i = 0; i < 16; i++) {
        rackTiles[sortedRackTiles[i].letter].push_back(sortedRackTiles[i]);
    }
    // use the dictionary to find the tiles in the rack which correspond to the letters in the word
    for (int k = 0; k < (int)word.size(); k++) {
        output.push_back(rackTiles[word[k]].front());
        rackTiles[word[k]].pop_front();
    }
    return output;
}

/* function which outputs the number of letter points in a word, taking into account treasures
 * the function also returns the damage boost from gems
 */
float evaluateEquivalentLength(const vector<tile>& word, int *treasure, float * totalGemBoost) {
    
    float equivalentLetterLength[26] = {1, 1.25, 1.25, 1, 1, 1.25, 1, 1.25, 1, 1.75, 1.75, 1, 1.25, 1, 1, 1.25, 2.75, 1, 1, 1, 1, 1.5, 1.5, 2, 1.5, 2};
    float wordEquivalentLength = 0;
    float currGemBoost = 0;
    if (treasure[0] == 1) { // parrot
        equivalentLetterLength['R' - 'A'] = 2;
    }
    if (treasure[1] == 1) { // bow
        equivalentLetterLength['X' - 'A'] = 2.5;
        equivalentLetterLength['Y' - 'A'] = 2.5;
        equivalentLetterLength['Z' - 'A'] = 2.5;
    }
    else if (treasure[1] == 2) { // arch
        equivalentLetterLength['X' - 'A'] = 3;
        equivalentLetterLength['Y' - 'A'] = 3;
        equivalentLetterLength['Z' - 'A'] = 3;
    }
    // calculate letterPoints by iterating through each tile in word
    for (int i = 0; i < (int)word.size(); i++) {

        wordEquivalentLength += equivalentLetterLength[word[i].letter - 'A'];
        currGemBoost += gemBoost(word[i].gem);
    }
    *totalGemBoost = currGemBoost;
    return wordEquivalentLength;
}

/* function which evaluates a word using letterPointConversion, 
 * first taking treasures into account, adjust letterPointConversion list
 * if "Parrot" is a treasure, then all 'R's are worth 2 points
 * if "Bow" is a treasure rather than "Arch", then all 'X's, 'Y's and 'Z's are worth 2.5 points
 * if "Arch" is a treasure rather than "Bow", then all 'X's, 'Y's and 'Z's are worth 3 points
 * finally converts to bookwormHearts
 */

float evaluateWord(const vector<tile>& word, int *treasure, float power) {
    float gemBoost;
    float equivalentLength = evaluateEquivalentLength(word, treasure, &gemBoost);
    // take maximum of 2 and minimum of 16 and equivalentLength
    int equivLenBookHeartsConv = _max(2, _min(16, _round(equivalentLength))); // equivalentLength to bookwormHearts conversion
    float bookwormHearts = equivLenBookHeartsRel[equivLenBookHeartsConv]; 
    float damage = bookwormHearts * (1 + power) + _ceil(bookwormHearts * gemBoost);
    // round to nearest quarter
    damage = (int)(damage * 4) / 4.0;
    return damage;
}

/* function which finds all the subanagrams of a given word
 * returns a list of strings and the number of matches
 */
vector<vector<tile>> findSubanagrams(
    const vector<tile>& sortedRackTiles, 
    char **dictionary,
    __uint128_t * dictHashes,
    int numWords,
    int * numMatches
) {
    vector<string> wordList;

    // create a list of characters of the letters of the rack
    string rack = tilesToWord(sortedRackTiles);

    __uint128_t product1 = wordToProduct(rack);

    for (int i = 0; i < numWords; i++) {
        __uint128_t product2 = dictHashes[i];
        if (product1 % product2 == 0) {
            // if product1 is divisible by product2, then product2 is a subanagram of product1
            wordList.push_back(dictionary[i]);
        }
    }
    *numMatches = wordList.size(); // set the number of matches

    // for each word in word_list, add the corresponding tiles for the word
    vector<vector<tile>> output(*numMatches);

    for (int i = 0; i < *numMatches; i++) {
        // cout << "Word to be converted: " << wordList[i] << "\n";
        output[i] = wordToTiles(sortedRackTiles, wordList[i]);
        // output[i] = wordToTiles2(sortedRackTiles, wordList[i]);
        // cout << "Converted word: " << tilesToWord(output[i]) << "\n";
    }
    
    
    return output;
}


/* function which finds the best subanagrams of a given word
 * returns a list of word_score structs and the number of matches
 */
vector<wordScore> bestSubanagrams(
    const vector<tile>& rack,
    int * treasures,
    float power,
    int maxWords,
    char ** dictionary,
    __uint128_t * dictHashes,
    int numWords, // size of dict
    int * numMatches,
    bool gemFlag
) {
    vector<wordScore> output(maxWords + 1);

    float minScore = 0; // min score out of the largest max_words words

    // make a copy of the rack, and then subsequently sort it
    vector<tile> sortedRack(rack.begin(), rack.end());
    sort(sortedRack.begin(), sortedRack.end(), [&](tile a, tile b) {
        return tileToPriority(a.gem) > tileToPriority(b.gem);
    });

    vector<vector<tile>> candidateWords = findSubanagrams(
        sortedRack, 
        dictionary, 
        dictHashes, 
        numWords, 
        numMatches
    );

    int idx = 0;

    for (int i = 0; i < *numMatches; i++) {

        vector<tile> candidateWord = candidateWords[i];

        int wordLength = (int)candidateWord.size();
        if (!(!gemFlag and (wordEfficiencies[wordLength] * wordLength <= minScore))) {
            
            wordScore currWord = {
                .word = candidateWord, 
                .score = evaluateWord(candidateWord, treasures, power)
            };

            if (idx < maxWords) {
                output[idx++] = currWord; // add the word to the list if there is space
            }
            else if (currWord.score > minScore) {
                output[idx] = currWord; // replace the min score with the current word
                sort(output.begin(), output.end(), [&](wordScore a, wordScore b) {
                    return a.score > b.score;
                });
                minScore = output[idx].score; // update the min score
                assert((int)output.size() == maxWords + 1);
            }
        }
    }
    output.resize(_min(idx, maxWords)); // resize the list to the correct size
    
    sort(output.begin(), output.end(), [&](wordScore a, wordScore b) {
        return a.score > b.score;
    });
    return output;
}


vector<wordScore> bestSubanagrams2(
    const vector<tile>& rack,
    int * treasures,
    float power,
    int maxWords,
    char ** dictionary,
    __uint128_t * dictHashes,
    int numWords, // size of dict
    int * numMatches,
    bool gemFlag
) {
    vector<wordScore> output(maxWords + 1);

    float minScore = 0; // min score out of the largest max_words words
    
    __uint128_t rackHash = wordToProduct(tilesToWord(rack));

    // make a copy of the rack, and then subsequently sort it
    vector<tile> sortedRack(rack.begin(), rack.end());
    sort(sortedRack.begin(), sortedRack.end(), [&](tile a, tile b) {
        return tileToPriority(a.gem) > tileToPriority(b.gem);
    });

    int idx = 0;

    for (int i = 0; i < numWords; i++) {

        string candidateActualWord = dictionary[i];

        int wordLength = (int)candidateActualWord.size();
        if ((gemFlag or (wordEfficiencies[wordLength] * wordLength > minScore)) and (rackHash % dictHashes[i] == 0)) {
            
            vector<tile> candidateWord = wordToTiles(sortedRack, candidateActualWord);

            wordScore currWord = {
                .word = candidateWord, 
                .score = evaluateWord(candidateWord, treasures, power)
            };

            if (idx < maxWords) {
                output[idx++] = currWord; // add the word to the list if there is space
            }
            else if (currWord.score > minScore) {
                output[idx] = currWord; // replace the min score with the current word
                sort(output.begin(), output.end(), [&](wordScore a, wordScore b) {
                    return a.score > b.score;
                });
                minScore = output[idx].score; // update the min score
                assert((int)output.size() == maxWords + 1);
            }
        }
    }
    output.resize(_min(idx, maxWords)); // resize the list to the correct size
    
    sort(output.begin(), output.end(), [&](wordScore a, wordScore b) {
        return a.score > b.score;
    });
    return output;
}


// count function for counting the number of times a character appears in a string
int count(string str, char c) {
    int count = 0;
    for(char ch: str)
        count += (int)(ch == c);
    return count;
}

// function which generates a new rack given an old rack
vector<tile> rackGeneration(
    const vector<tile>& oldRack, 
    char gem
) {
    vector<tile> newRack(16);
    tile * addedTiles = (tile *)malloc(17 * sizeof(tile));
    string currLetters = tilesToWord(oldRack);
    int x = (int)oldRack.size();
    int y = x;
    int numAddedTiles = 0;
    
    while (y < 16) {
        // We want to use letter_dist to generate a letter as a weighted random variable
        int idx = 0;
        float randNum = (float)rand() / (float)RAND_MAX;
        while (randNum > letterDist[idx]) { // find the letter which corresponds to the random number
            randNum -= letterDist[idx++];
        }
        assert(idx < 26);
        
        char new_letter = idx + 'A';
        if (count(currLetters, new_letter) < maxFreqs[idx]) {
            currLetters[y++] = new_letter; // add the letter to the rack if the max frequency is not exceeded
            addedTiles[numAddedTiles++] = (tile){.letter = new_letter, .gem = ' '};
        }
    }
    if (numAddedTiles > 0) {
        int randNum = rand() % numAddedTiles;
        addedTiles[randNum].gem = gem; // add a gem to the rack
    }
    // combine old rack and added tiles to get new rack
    for (int i = 0; i < 16; i++) {
        if (i < 16 - numAddedTiles)
            newRack[i] = oldRack[i];
        else
            newRack[x++] = addedTiles[i - 16 + numAddedTiles];
    }
    return newRack;
}

// function which removes letters from a rack
vector<tile> removeLetters(
    const vector<tile>& rack,
    const vector<tile>& word
) {
    vector<bool> used_idx(16, false);

    for (int k = 0; k < (int)word.size(); k++) {
        for (int l = 0; l < 16; l++) {
            if (!used_idx[l] && rack[l].letter == word[k].letter && rack[l].gem == word[k].gem) {
                used_idx[l] = true;
                break;
            }
        }
    }
    vector<tile> newRack;
    for (int i = 0; i < 16; i++) {
        if (!used_idx[i])
            newRack.push_back(rack[i]);
    }

    return newRack;
}



// function which performs a monte carlo search
wordScore monteCarloSearch(
    const vector<tile>& rack, 
    int * treasures, 
    float power,
    int numSimulations, 
    int maxWords, 
    int depth, 
    char **dictionary, 
    __uint128_t *dictHashes, 
    int numWords, 
    int *numMatches,
    bool gemFlag
) {
    vector<wordScore> possibleWords = bestSubanagrams2(
        rack, 
        treasures,
        power,
        maxWords, 
        dictionary, 
        dictHashes, 
        numWords, 
        numMatches,
        gemFlag
    ); // list of candidate words
    wordScore bestWord = {.score = 0}; // best word found so far
    for (int i = 0; i < maxWords; i++) {
        wordScore currWord = possibleWords[i]; // current word
        float totalScore = 0; // total score of the simulations
        for (int i = 0; i < numSimulations; i++) {
            int newNumMatches;
            char gem = ' '; // gem to be added to the rack
            if (gemFlag) {
                float temp;
                gem = scoreToGem(evaluateEquivalentLength(currWord.word, treasures, &temp));
            }
            vector<tile> removedRack = removeLetters(rack, currWord.word); // remove the letters from the rack
            vector<tile> newRack = rackGeneration(removedRack, gem); // generate a new rack after playing the word
            wordScore newWord; // initialise new word to be specified later
            if (depth > 1) { // if we are not at the bottom of the tree
                newWord = monteCarloSearch(
                    newRack, 
                    treasures, 
                    power,
                    1, 
                    maxWords, 
                    depth - 1, 
                    dictionary, 
                    dictHashes, 
                    numWords, 
                    &newNumMatches,
                    gemFlag
            ); // perform a monte carlo search on the new rack
            }
            else {
                vector<wordScore> currWords = bestSubanagrams2(
                    newRack, 
                    treasures, 
                    power,
                    maxWords, 
                    dictionary, 
                    dictHashes, 
                    numWords, 
                    &newNumMatches,
                    gemFlag
                ); // find the best word on the new rack
                if (newNumMatches == 0) {
                    newWord = (wordScore){.score = 0}; // if there are no words, set the score to 0
                }
                else {
                    newWord = currWords[0]; // otherwise, set the score to the best word
                }
            }
            totalScore += newWord.score; // add the score of the new word to the total score
        }
        float avgScore = currWord.score + totalScore / numSimulations; // calculate the average score
        if (avgScore > bestWord.score) {
            bestWord.word = currWord.word; // update the best word
            bestWord.score = avgScore; // update the best score
        }
        if (depth == 2) {
            printf("%s: %.3f\n", addQU(tilesToWord(currWord.word)).c_str(), avgScore);
        }
    }
    return bestWord;
}

// reads the ba1-dictionary-playable-anagram.txt file, and print the best word to play for the provided rack
int main() {
    srand(time(NULL)); // seed the random number generator

    int numWords; // number of words in the dictionary
    const char *filename = "../ba1-dictionary-playable-anagram.txt"; // file name
    int treasures[2] = {1, 2}; // treasures array
    char **dictionary = readWords(filename, &numWords); // read the dictionary
    __uint128_t* dictHashes = dictToHash(dictionary, numWords); // convert the dictionary to numbers
    
    int numMatches; // number of matches

    vector<tile> rack = {(tile){.letter = 'R', .gem = ' '}, (tile){.letter = 'E', .gem = ' '}, (tile){.letter = 'G', .gem = ' '}, (tile){.letter = 'E', .gem = ' '}, 
                         (tile){.letter = 'N', .gem = ' '}, (tile){.letter = 'A', .gem = ' '}, (tile){.letter = 'O', .gem = ' '}, (tile){.letter = 'O', .gem = ' '}, 
                         (tile){.letter = 'E', .gem = ' '}, (tile){.letter = 'T', .gem = ' '}, (tile){.letter = 'N', .gem = ' '}, (tile){.letter = 'G', .gem = ' '}, 
                         (tile){.letter = 'R', .gem = ' '}, (tile){.letter = 'I', .gem = ' '}, (tile){.letter = 'L', .gem = ' '}, (tile){.letter = 'E', .gem = ' '}};
    string word = tilesToWord(rack);
    printf("Rack: %s %d\n", word.c_str(), (int)word.size()); // print the rack
    float power = 0;
    bool gemFlag = false;

    clock_t start_time = clock(); // start the timer
    wordScore bestWord = monteCarloSearch( // perform a monte carlo search
        rack,
        treasures,
        power,
        100,
        10,
        2,
        dictionary,
        dictHashes,
        numWords,
        &numMatches,
        gemFlag
    );
    bestWord._print_word_score(); // print the best word
    printf("Time taken: %.3fs", (float)(clock() - start_time) / CLOCKS_PER_SEC); // print the time taken
    return 0;
}