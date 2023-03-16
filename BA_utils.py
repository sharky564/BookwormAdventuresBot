import random
from math import ceil
import time
from collections import Counter
from functools import reduce

letters = ['A', 'B', 'C', 'D', 'E', 'F', 'G', 'H', 'I', 'J', 'K', 'L', 'M', 'N', 'O', 'P', 'Q', 'R', 'S', 'T', 'U', 'V', 'W', 'X', 'Y', 'Z']
letters_to_numbers = {'A': 0, 'B': 1, 'C': 2, 'D': 3, 'E': 4, 'F': 5, 'G': 6, 'H': 7, 'I': 8, 'J': 9, 'K': 10, 'L': 11, 'M': 12, 'N': 13, 'O': 14, 'P': 15, 'Q': 16, 'R': 17, 'S': 18, 'T': 19, 'U': 20, 'V': 21, 'W': 22, 'X': 23, 'Y': 24, 'Z': 25}
max_letter_counts = {'A': 4, 'B': 3, 'C': 3, 'D': 4, 'E': 4, 'F': 3, 'G': 4, 'H': 3, 'I': 4, 'J': 2, 'K': 2, 'L': 4, 'M': 3, 'N': 4, 'O': 3, 'P': 4, 'Q': 2, 'R': 4, 'S': 4, 'T': 4, 'U': 4, 'V': 3, 'W': 3, 'X': 2, 'Y': 3, 'Z': 2}

dictionary = []
# with open('ba1-wordlist.txt', 'r') as f:
#     for line in f:
#         # remove all Us after Qs in dictionary, check all words have at most 16 characters, and check all words can be obtained in game from frequency of letters
#         word = line.strip().upper()
#         if len(word) >= 3:
#             if 'Q' in word:
#                 if 'QU' not in word:
#                     pass
#                 else:
#                     word = word.replace('QU', 'Q')
#                     if len(word) <= 16:
#                         freq = Counter(word)
#                         if all(freq[letter] <= max_letter_counts[letter] for letter in freq):
#                             dictionary.append(word)
#             else:
#                 if len(word) <= 16:
#                     freq = Counter(word)
#                     if all(freq[letter] <= max_letter_counts[letter] for letter in freq):
#                         dictionary.append(word)
#
# # sort the dictionary by length in reverse
# dictionary.sort(key=len, reverse=True)
with open('ba1-dictionary-playable.txt', 'r') as f:
    for line in f:
        word = line.strip()
        dictionary.append(word)

# list of bonus words, currently not being used
bonus_dict = []
random.seed(123)

def rack_generation(curr_rack=[]):
    letter_freq_list = [0.0932, 0.0171, 0.0218, 0.0376, 0.13, 0.0235, 0.0257, 0.0252, 0.0723, 0.0077, 0.0056, 0.0466, 0.0214, 0.0547, 0.0663, 0.0261, 0.0115, 0.0752, 0.0594, 0.0684, 0.0428, 0.0171, 0.0154, 0.006, 0.0252, 0.0042]
    if len(curr_rack) > 16:
        raise ValueError("Rack must contain at most 16 letters")
    else:
        new_rack = [i for i in curr_rack]
        x = len(new_rack)
        while x < 16:
            new_letter = random.choices(letters, weights=letter_freq_list)[0]
            # ensuring the maximum frequency of each letter in the rack is bounded by max_letter_counts
            if new_rack.count(new_letter) < max_letter_counts[new_letter]:
                new_rack.append(new_letter)
                x += 1
        return new_rack
    
def word_evaluator(word, treasures):
    letter_points = {'A': 1, 'B': 1.25, 'C': 1.25, 'D': 1, 'E': 1, 'F': 1.25, 'G': 1, 'H': 1.25, 'I': 1, 'J': 1.75, 'K': 1.75, 'L': 1, 'M': 1.25, 'N': 1, 'O': 1, 'P': 1.25, 'Q': 2.75, 'R': 1, 'S': 1, 'T': 1, 'U': 1, 'V': 1.5, 'W': 1.5, 'X': 2, 'Y': 1.5, 'Z': 2}
    if "Parrot" in treasures:
        letter_points['R'] = 2
    if "Bow" in treasures:
        letter_points['X'] = 2.5
        letter_points['Y'] = 2.5
        letter_points['Z'] = 2.5
    if "Arch" in treasures:
        letter_points['X'] = 3
        letter_points['Y'] = 3
        letter_points['Z'] = 3
    lp = 0
    i = 0
    while i < len(word):
        letter = word[i]
        val = letter_points[letter]
        lp += val
        i += 1
    
    lp_bh = {2: 0.25, 3: 0.5, 4: 0.75, 5: 1, 6: 1.5, 7: 2, 8: 2.75, 9: 3.5, 10: 4.5, 11: 5.5, 12: 6.75, 13: 8, 14: 9.5, 15: 11, 16: 13}
    
    def ba_round(x):
        return int(x + 0.5)
    
    lp_bh_rel = max(2, min(16, ba_round(lp)))
    bh = lp_bh[lp_bh_rel]
    return bh

anagram_dict = {}
anagram_scores = {}
for word in dictionary:
    anagram = ''.join(sorted(word))
    if anagram in anagram_dict:
        anagram_dict[anagram].append(word)
    else:
        anagram_dict[anagram] = [word]
        anagram_scores[anagram] = word_evaluator(word, [])

'''
Create a dictionary that maps each letter to a prime number
'''
primes = [2, 3, 5, 7, 11, 13, 17, 19, 23, 29, 31, 37, 41, 43, 47, 53, 59, 61, 67, 71, 73, 79, 83, 89, 97, 101]
letter_primes = {}
for i, letter in enumerate(letters):
    letter_primes[letter] = primes[i]

def word_hash_conversion(word):
    '''
    Returns the product of the prime numbers corresponding to the letters in the word
    '''
    return reduce(lambda x, y: x * y, [letter_primes[letter] for letter in word])

word_hashes = {word: word_hash_conversion(word) for word in anagram_dict}

def word_score(word, treasures, sorted_flag=False):
    if treasures == []:
        if sorted_flag:
            return anagram_scores[word]
        else:
            return anagram_scores[''.join(sorted(word))]
    else:
        return word_evaluator(word, treasures)
    
def pos_words2(rack, treasures, max_words=50):
    '''
    Creates a list of all words in the rack that can be found in the dictionary, 
    taking frequency of letters into account, along with their evaluation from 
    word_evaluator, sorted by evaluation.
    '''
    words = []
    min_score = 0
    rack_hash = word_hash_conversion(rack)
    word_efficiencies = {2: 0.376, 3: 0.501, 4: 0.501, 5: 0.701, 6: 0.751, 7: 0.786, 8: 0.844, 9: 0.889, 10: 1.101, 11: 1.182, 12: 1.084, 13: 1.001, 14: 0.929, 15: 0.867, 16: 0.813}
    for letters in anagram_dict:
        if rack_hash % word_hashes[letters] == 0 and word_efficiencies[len(letters)] * len(letters) > min_score:
            s = word_score(letters, treasures)
            if len(words) < max_words:
                words += [(word, s) for word in anagram_dict[letters]]
            else:
                if s > min_score:
                    words += [(word, s) for word in anagram_dict[letters]]
                    words.sort(key=lambda x: x[1], reverse=True)
                    words = words[:max_words]
                    min_score = words[-1][1]
    words.sort(key=lambda x: x[1], reverse=True)
    return words[:max_words]

def play(word, rack):
    '''
    Removes the letters of the word from the rack, and returns a new rack
    '''
    new_rack = rack.copy()
    for letter in word:
        new_rack.remove(letter)
    return rack_generation(new_rack)