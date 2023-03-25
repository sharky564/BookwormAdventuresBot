#ifndef STRING
#define STRING
#include <string>
#endif

// mapping between letters and primes
inline int letterToPrime(char letter) {
    return primes[(int)(letter - 'A')];
}

// function which converts a given word to the product of the primes corresponding to the letters
inline __uint128_t wordToProduct(string word) {
    __uint128_t product = 1;
    for (int i = 0; i < (int)word.size(); i++) {
        product *= letterToPrime(word[i]);
    }
    return product;
}

/* function which reads in all the words from a text file and returns them as a list of strings
 * the number of words is also returned as a pointer
 */
char **readWords(const char *filename, int *numWords) {
    FILE *file = fopen(filename, "r");
    char **words = (char**)malloc(sizeof(char *) * numDictWords); // there is just under 151000 words in the dictionary
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
__uint128_t * dictToHash(char ** dictionary, int numWords) {
    __uint128_t * output = (__uint128_t *)malloc(numDictWords * sizeof(__uint128_t));
    for(int i = 0; i < numWords; i++) {
        output[i] = wordToProduct(dictionary[i]);
    }
    return output;
}