// Capital letters
const char* capitalLetters = "ABCDEFGHIJKLMNOPQRSTUVWXYZ";

// First 26 primes
const int primes[26] = {2, 3, 5, 7, 11, 13, 17, 19, 23, 29, 31, 37, 41, 43, 47, 53, 59, 61, 67, 71, 73, 79, 83, 89, 97, 101};

// Maximum frequency of each letter
const int maxFreqs[26] = {4, 3, 3, 4, 4, 3, 4, 3, 4, 2, 2, 4, 3, 4, 3, 4, 2, 4, 4, 4, 4, 3, 3, 2, 3, 2};

// Probability distribution of each letter
const float letterDist[26] = {0.0932, 0.0171, 0.0218, 0.0376, 0.13, 0.0235, 0.0257, 0.0252, 0.0723, 0.0077, 0.0056, 0.0466, 0.0214, 0.0547, 0.0663, 0.0261, 0.0115, 0.0752, 0.0594, 0.0684, 0.0428, 0.0171, 0.0154, 0.006, 0.0252, 0.0042};

const float wordEfficiencies[17] = {0, 0, 0.376, 0.501, 0.501, 0.701, 0.751, 0.786, 0.844, 0.889, 1.101, 1.182, 1.084, 1.001, 0.929, 0.867, 0.813};

const float equivLenBookHeartsRel[17] = {0, 0, 0.25, 0.5, 0.75, 1, 1.5, 2, 2.75, 3.5, 4.5, 5.5, 6.75, 8, 9.5, 11, 13};

// Max float for merge sorting
const float floatMax = 1e9 + 1;

// Number of words in the dictionary
const int numDictWords = 151000;