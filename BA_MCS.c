#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <stdbool.h>
#include <time.h>

// Capital letters
const char* capitalLetters = "ABCDEFGHIJKLMNOPQRSTUVWXYZ";

// First 26 primes
const int primes[26] = {2, 3, 5, 7, 11, 13, 17, 19, 23, 29, 31, 37, 41, 43, 47, 53, 59, 61, 67, 71, 73, 79, 83, 89, 97, 101};

// Maximum frequency of each letter
const int max_freqs[26] = {4, 3, 3, 4, 4, 3, 4, 3, 4, 2, 2, 4, 3, 4, 3, 4, 2, 4, 4, 4, 4, 3, 3, 2, 3, 2};

// Probability distribution of each letter
const float letter_dist[26] = {0.0932, 0.0171, 0.0218, 0.0376, 0.13, 0.0235, 0.0257, 0.0252, 0.0723, 0.0077, 0.0056, 0.0466, 0.0214, 0.0547, 0.0663, 0.0261, 0.0115, 0.0752, 0.0594, 0.0684, 0.0428, 0.0171, 0.0154, 0.006, 0.0252, 0.0042};

// Max float for merge sorting
const float float_max = 1e9 + 1;

// mapping between letters and primes
inline int letterToPrime(char letter) {
    return primes[(int)(letter - 'A')];
}

// maximum of two numbers
inline int _max(int a, int b) {
    return a > b ? a : b;
}

// minimum of two numbers
inline int _min(int a, int b) {
    return a < b ? a : b;
}

// function which converts a given word to the product of the primes corresponding to the letters
inline __uint128_t wordToProduct(char *word) {
    __uint128_t product = 1;
    for (int i = 0; i < strlen(word); i++) {
        product *= letterToPrime(word[i]);
    }
    return product;
}

/*function which reads in all the words from a text file and returns them as a list of strings
 * the number of words is also returned as a pointer
 */
char **readWords(const char *filename, int *numWords) {
    FILE *file = fopen(filename, "r");
    char **words = (char**)malloc(sizeof(char *) * 170000); // there is just under 170000 words in the dictionary
    int i = 0;
    while (!feof(file)) {
        words[i] = (char*)malloc(sizeof(char) * 17); // longest word is 16 letters long
        fscanf(file, "%s", words[i++]);
    }
    *numWords = i; // set the number of words
    return words;
}

/* function which converts a list of words to a list of products of primes
 * this is done to speed up the process of checking subanagrams later on
 */
__int128_t * dict_to_nums(char ** dictionary, int numWords) {
    __int128_t * to_ret = (__int128_t *)malloc(170000 * sizeof(__int128_t));
    for(int i = 0; i < numWords; i++) {
        to_ret[i] = wordToProduct(dictionary[i]);
    }
    return to_ret;
}

// function which rounds a number to the nearest integer
inline int _round(float num) {
    return (int)(num + 0.5);
}

/* function which evaluates a word using letterPointConversion, 
 * first taking treasures into account, adjust letterPointConversion list
 * if "Parrot" is a treasure, then all 'R's are worth 2 points
 * if "Bow" is a treasure rather than "Arch", then all 'X's, 'Y's and 'Z's are worth 2.5 points
 * if "Arch" is a treasure rather than "Bow", then all 'X's, 'Y's and 'Z's are worth 3 points
 * finally converts to bookwormHearts
 */
float evaluateWord(char *word, int *treasure) {
    int letterPointConversion[27] = {4, 5, 5, 4, 4, 5, 4, 5, 4, 7, 7, 4, 5, 4, 4, 5, 11, 4, 4, 4, 4, 6, 6, 8, 6, 8};
    int letterPoints = 0;
    if (treasure[0] == 1) { // parrot
        letterPointConversion['R' - 'A'] = 8;
    }
    if (treasure[1] == 1) { // bow
        letterPointConversion['X' - 'A'] = 10;
        letterPointConversion['Y' - 'A'] = 10;
        letterPointConversion['Z' - 'A'] = 10;
    }
    else if (treasure[1] == 2) { // arch
        letterPointConversion['X' - 'A'] = 12;
        letterPointConversion['Y' - 'A'] = 12;
        letterPointConversion['Z' - 'A'] = 12;
    }
    for (int i = 0; i < strlen(word); i++) {
        letterPoints += letterPointConversion[word[i] - 'A'];
    }
    /* take maximum of 2 and minimum of 16 and letterPoints/4*/
    int lp_bh_rel = _round((float)_max(8, _min(64, letterPoints)) / 4); // letterPoints to bookwormHearts relative
    float bookwormHearts = 0;
    switch (lp_bh_rel) { // convert to bookwormHearts
        case 2:
            bookwormHearts = 0.25;
            break;
        case 3:
            bookwormHearts = 0.5;
            break;
        case 4:
            bookwormHearts = 0.75;
            break;
        case 5:
            bookwormHearts = 1;
            break;
        case 6:
            bookwormHearts = 1.5;
            break;
        case 7:
            bookwormHearts = 2;
            break;
        case 8:
            bookwormHearts = 2.75;
            break;
        case 9:
            bookwormHearts = 3.5;
            break;
        case 10:
            bookwormHearts = 4.5;
            break;
        case 11:
            bookwormHearts = 5.5;
            break;
        case 12:
            bookwormHearts = 6.75;
            break;
        case 13:
            bookwormHearts = 8;
            break;
        case 14:
            bookwormHearts = 9.5;
            break;
        case 15:
            bookwormHearts = 11;
            break;
        case 16:
            bookwormHearts = 13;
            break;
    }
    return bookwormHearts;
}

/* function which finds all the subanagrams of a given word
 * returns a list of strings and the number of matches
 */
char** findSubanagrams(
    char *rack, 
    char **dictionary,
    __int128_t * dict_nums,
    int numWords,
    int * num_matches
) {
    char ** to_ret = (char **)malloc(10000 * sizeof(char *)); // there are less than 10000 subanagrams in any given rack
    int idx = 0;

    __uint128_t product1 = wordToProduct(rack);
    for (int i = 0; i < numWords; i++) {
        __uint128_t product2 = dict_nums[i];
        if (product1 % product2 == 0) { // if product1 is divisible by product2, then product2 is a subanagram of product1
            to_ret[idx++] = dictionary[i];
        }
    }
    *num_matches = idx; // set the number of matches
    return to_ret;
}

/* struct which stores a word and its score
 * used to sort the words by score
 */
typedef struct word_score {
    char * word;
    float score;

} word_score;

// for each Q in the word, add U after it
char * addQU(char * word) {
    char * out_word = (char *)malloc(100 * sizeof(char));
    int idx = 0;
    for (int i = 0; i < strlen(word); i++) {
        out_word[idx++] = word[i];
        if (word[i] == 'Q') {
            out_word[idx++] = 'U';
        }
    }
    out_word[idx] = '\0';
    return out_word;
}

// prints a word_score struct
void _print_word_score(word_score word_score) {
    printf("Word = %s, score = %f\n", addQU(word_score.word), word_score.score);
}

// subroutine for merge sort
inline void merge(word_score arr[], int l, int m, int r)
{
    int n1 = m - l + 1;
	int n2 = r - m;
    word_score L[n1 + 1], R[n2 + 1];
    for(int i = 0; i < n1; i++) {
        L[i] = arr[l + i]; // copy first half of array
	}
    for(int j = 0; j < n2; j++) {
        R[j] = arr[m + 1 + j]; // copy second half of array
	}
    const word_score MIN = {.score = -float_max};
	L[n1] = MIN;
	R[n2] = MIN;
    int i = 0, j = 0, k = l;
	while(i + j < n1 + n2) {
		arr[k++] = (L[i].score > R[j].score) ? L[i++] : R[j++]; // merge the two arrays
    }
}

// function which sorts a list of word_score structs by score
void mergeSort(word_score arr[], int l, int r)
{
    if(l < r) {
        int m = l + (r - l)/2;
        mergeSort(arr, l, m); // sort first half
        mergeSort(arr, m + 1, r); // sort second half
        merge(arr, l, m, r); // merge the two halves
    }
}

/* function which finds the best subanagrams of a given word
 * returns a list of word_score structs and the number of matches
 */
word_score * bestSubanagrams(
    char * rack,
    int * treasures,
    int max_words,
    char ** dictionary,
    __int128_t * dict_nums,
    int numWords, // size of dict
    int * num_matches
) {
    word_score * to_ret = (word_score *)malloc((max_words + 1) * sizeof(word_score));
    float word_efficiencies[17] = {0, 0, 0.376, 0.501, 0.501, 0.701, 0.751, 0.786, 0.844, 0.889, 1.101, 1.182, 1.084, 1.001, 0.929, 0.867, 0.813};

    int idx = 0;
    float min_score = 0; // min score out of the largest max_words words
    __uint128_t rack_hash = wordToProduct(rack);
    
    for (int i = 0; i < numWords; i++) {
        int word_length = strlen(dictionary[i]);
        if (rack_hash % dict_nums[i] == 0 // if the word is a subanagram of the rack
            && word_efficiencies[word_length] * word_length > min_score // if the word is long enough to achieve the min score
        ) {
            word_score curr_word = {
                .word = dictionary[i], 
                .score = evaluateWord(dictionary[i], treasures)
            };

            if (idx < max_words) {
                to_ret[idx++] = curr_word; // add the word to the list if there is space
            }
            else {
                if (curr_word.score > min_score) {
                    to_ret[idx] = curr_word; // replace the min score with the current word
                    mergeSort(to_ret, 0, idx); // sort the list
                    min_score = to_ret[idx].score; // update the min score
                }
            }
        }
    }
    mergeSort(to_ret, 0, idx - 1); // sort the list
    *num_matches = idx; // set the number of matches
    return to_ret;
}

// count function for counting the number of times a character appears in a string
int count(char * str, char * end, char c) {
    int count = 0;
    while (str != end) {
        if (*str == c) {
            count++;
        }
        str++;
    }
    return count;
}

// function which generates a new rack given an old rack
char * rackGeneration(char * old_rack) {
    char * new_rack = (char *)malloc(17 * sizeof(char));
    int x = strlen(old_rack);
    for (int i = 0; i < x; i++) {
        new_rack[i] = old_rack[i]; // copy the old rack
    }
    while (x < 16) {
        // We want to use letter_dist to generate a letter as a weighted random variable
        int letter_idx = 0;
        float rand_num = (float)rand() / (float)RAND_MAX;
        while (rand_num > letter_dist[letter_idx]) { // find the letter which corresponds to the random number
            rand_num -= letter_dist[letter_idx];
            letter_idx++;
        }
        char new_letter = letter_idx + 'A';
        if (count(new_rack, new_rack + x, new_letter) < max_freqs[new_letter - 'A']) {
            new_rack[x++] = new_letter; // add the letter to the rack if the max frequency is not exceeded
        }
    }
    new_rack[x] = '\0';
    return new_rack;
}

// function which removes letters from a rack
char * removeLetters(char * rack, char * word) {
    char * new_rack = (char *)malloc(17 * sizeof(char));
    int x = strlen(rack);
    for (int i = 0; i < x; i++) {
        new_rack[i] = rack[i]; // copy the old rack
    }
    int y = strlen(word);
    for (int i = 0; i < y; i++) {
        int idx = 0;
        while (new_rack[idx] != word[i]) {
            idx++;
        }
        new_rack[idx] = new_rack[x - 1]; // remove the letter from the rack
        x--;
    }
    new_rack[x] = '\0';
    return new_rack;
}

// function which performs a monte carlo search
word_score monteCarloSearch(
    char * rack, 
    int * treasures, 
    int num_simulations, 
    int max_words, 
    int depth, 
    char **dictionary, 
    __int128_t *dict_nums, 
    int numWords, 
    int *num_matches
) {
    word_score * possible_words = bestSubanagrams(
        rack, 
        treasures,
        max_words, 
        dictionary, 
        dict_nums, 
        numWords, 
        num_matches
    ); // list of candidate words
    word_score best_word = {.score = 0}; // best word found so far
    for (int i = 0; i < *num_matches; i++) {
        word_score curr_word = possible_words[i]; // current word
        float total_score = 0; // total score of the simulations
        for (int i = 0; i < num_simulations; i++) {
            int new_num_matches;
            char * new_rack = rackGeneration(removeLetters(rack, curr_word.word)); // generate a new rack after playing the word
            word_score new_word; // initialise new word to be specified later
            if (depth > 1) { // if we are not at the bottom of the tree
                new_word = monteCarloSearch(
                    new_rack, 
                    treasures, 
                    1, 
                    _max(1, max_words/2), 
                    depth - 1, 
                    dictionary, 
                    dict_nums, 
                    numWords, 
                    &new_num_matches
            ); // perform a monte carlo search on the new rack
            }
            else {
                word_score *curr_words = bestSubanagrams(
                    new_rack, 
                    treasures, 
                    1, 
                    dictionary, 
                    dict_nums, 
                    numWords, 
                    &new_num_matches
                ); // find the best word on the new rack
                if (new_num_matches == 0) {
                    new_word = (word_score){.score = 0}; // if there are no words, set the score to 0
                }
                else {
                    new_word = curr_words[0]; // otherwise, set the score to the best word
                }
            }
            total_score += new_word.score; // add the score of the new word to the total score
        }
        float avg_score = curr_word.score + total_score / num_simulations; // calculate the average score
        if (avg_score > best_word.score) {
            best_word.word = curr_word.word; // update the best word
            best_word.score = avg_score; // update the best score
        }
        if (depth == 2) {
            printf("%s: %f\n", addQU(curr_word.word), avg_score);
        }
    }
    return best_word;
}


/*reads the ba1-dictionary-playable.txt file and outputs the subanagrams of a random rack*/
int main() {
    srand(time(NULL)); // seed the random number generator

    int numWords; // number of words in the dictionary
    const char *file_name = "ba1-dictionary-playable.txt"; // file name
    int treasures[2] = {0, 0}; // treasures array
    char **dictionary = readWords(file_name, &numWords); // read the dictionary
    __int128_t* dict_nums = dict_to_nums(dictionary, numWords); // convert the dictionary to numbers
    
    int num_matches; // number of matches
    char* rack = "\0"; // initialise the rack
    rack = rackGeneration(rack); // generate a random rack

    printf("Rack: %s\n", rack); // print the rack

    // print the best 10 words just by score
    // word_score * possible_words = bestSubanagrams(
    //     rack, 
    //     treasures, 
    //     10, 
    //     dictionary, 
    //     dict_nums, 
    //     numWords, 
    //     &num_matches
    // );
    // for (int i = 0; i < num_matches; i++) {
    //     _print_word_score(possible_words[i]);
    // }

    // start the time
    double startTime = (double)clock() / CLOCKS_PER_SEC;

    word_score best_word = monteCarloSearch(
        rack,
        treasures,
        100,
        10,
        2,
        dictionary,
        dict_nums,
        numWords,
        &num_matches
    ); // perform a monte carlo search
    _print_word_score(best_word); // print the best word

    double endTime = (double)clock() / CLOCKS_PER_SEC;

    double time_elapsed = endTime - startTime; // calculate the time elapsed
    printf("Amount of time taken = %.10lfs\n", time_elapsed);
    return 0;
}