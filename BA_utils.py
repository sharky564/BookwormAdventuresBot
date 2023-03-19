import random
import time
from math import ceil
from collections import Counter
from functools import reduce
from termcolor import colored
from typing import Optional, List, Tuple

letters = ['A', 'B', 'C', 'D', 'E', 'F', 'G', 'H', 'I', 'J', 'K', 'L', 'M', 'N', 'O', 'P', 'Q', 'R', 'S', 'T', 'U', 'V', 'W', 'X', 'Y', 'Z']
letters_to_numbers = {'A': 0, 'B': 1, 'C': 2, 'D': 3, 'E': 4, 'F': 5, 'G': 6, 'H': 7, 'I': 8, 'J': 9, 'K': 10, 'L': 11, 'M': 12, 'N': 13, 'O': 14, 'P': 15, 'Q': 16, 'R': 17, 'S': 18, 'T': 19, 'U': 20, 'V': 21, 'W': 22, 'X': 23, 'Y': 24, 'Z': 25}
max_letter_counts = {'A': 4, 'B': 3, 'C': 3, 'D': 4, 'E': 4, 'F': 3, 'G': 4, 'H': 3, 'I': 4, 'J': 2, 'K': 2, 'L': 4, 'M': 3, 'N': 4, 'O': 3, 'P': 4, 'Q': 2, 'R': 4, 'S': 4, 'T': 4, 'U': 4, 'V': 3, 'W': 3, 'X': 2, 'Y': 3, 'Z': 2}
colour_dict = {None: 'white', 'A': 'magenta', 'E': 'green', 'S' : 'blue', 'G': 'yellow', 'R': 'red', 'C': 'magenta', 'D': 'white'}
gem_dmg_dict = {None: 0, 'A': 0.15, 'E': 0.2, 'S' : 0.25, 'G': 0.3, 'R': 0.35, 'C': 0.5, 'D': 1}

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

class Tile():
    def __init__(self, letter: str, gem: Optional[str]=None):
        self.letter = letter # letter on the tile
        self.gem = gem # gem on the tile, if any
    
    def update_gem(self, gem: Optional[str]=None) -> None:
        self.gem = gem
    
    def __eq__(self, other: "Tile") -> bool:
        if self.letter == other.letter and self.gem == other.gem:
            return True
        else:
            return False
    
    def __str__(self) -> str:
        return self.__repr__()
    
    def __repr__(self) -> str:
        out_char = self.letter
        if out_char == 'Q':
            out_char = 'Qu'
        if self.gem not in ['C', 'D']:
            return colored(out_char, colour_dict[self.gem])
        else:
            return colored(out_char, colour_dict[self.gem], attrs=['bold', 'underline'])


def rack_generation(curr_rack: List[Tile]=[], added_gem: Optional[str]=None) -> List[Tile]:
    letter_freq_list = [0.0932, 0.0171, 0.0218, 0.0376, 0.13, 0.0235, 0.0257, 0.0252, 0.0723, 0.0077, 0.0056, 0.0466, 0.0214, 0.0547, 0.0663, 0.0261, 0.0115, 0.0752, 0.0594, 0.0684, 0.0428, 0.0171, 0.0154, 0.006, 0.0252, 0.0042]
    if len(curr_rack) > 16:
        raise ValueError("Rack must contain at most 16 letters")
    else:
        new_rack = [i.letter for i in curr_rack]
        added_rack = []
        out_rack = [i for i in curr_rack]
        x = len(new_rack)
        while x < 16:
            new_letter = random.choices(letters, weights=letter_freq_list)[0]
            # ensuring the maximum frequency of each letter in the rack is bounded by max_letter_counts
            if new_rack.count(new_letter) < max_letter_counts[new_letter]:
                new_rack.append(new_letter)
                added_rack.append(new_letter)
                x += 1
        new_out_rack = [Tile(i) for i in added_rack]
        if added_gem is not None:
            n = len(new_out_rack)
            i = random.randint(0, n-1)
            new_out_rack[i].update_gem(added_gem)
        return out_rack + new_out_rack


def letter_points(word: List[Tile], treasures: List[str]) -> Tuple[float, float]:
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
    gem_dmg = 0
    while i < len(word):
        letter, gem = word[i].letter, word[i].gem
        val = letter_points[letter]
        gem_dmg += gem_dmg_dict[gem]
        lp += val
        i += 1
    return lp, gem_dmg
    
def word_evaluator(word: List[Tile], treasures: List[str], power: float=0) -> float:
    lp, gem_dmg = letter_points(word, treasures)
    
    lp_bh = {2: 0.25, 3: 0.5, 4: 0.75, 5: 1, 6: 1.5, 7: 2, 8: 2.75, 9: 3.5, 10: 4.5, 11: 5.5, 12: 6.75, 13: 8, 14: 9.5, 15: 11, 16: 13}
    
    def ba_round(x):
        return int(x + 0.5)
    
    lp_bh_rel = max(2, min(16, ba_round(lp)))
    bh = lp_bh[lp_bh_rel]
    dmg = bh + bh * power + ceil(bh * gem_dmg)

    return int(4 * dmg)/4

anagram_dict = {}
# anagram_scores = {}
for word in dictionary:
    anagram = ''.join(sorted(word))
    if anagram in anagram_dict:
        # anagram_dict[anagram].append(word)
        continue
    else:
        anagram_dict[anagram] = word
        # anagram_dict[anagram] = [word]
        # anagram_scores[anagram] = word_evaluator([Tile(i) for i in word], [])

'''
Create a dictionary that maps each letter to a prime number
'''
primes = [2, 3, 5, 7, 11, 13, 17, 19, 23, 29, 31, 37, 41, 43, 47, 53, 59, 61, 67, 71, 73, 79, 83, 89, 97, 101]
letter_primes = {}
for i, letter in enumerate(letters):
    letter_primes[letter] = primes[i]

def word_hash_conversion(word: str) -> int:
    '''
    Returns the product of the prime numbers corresponding to the letters in the word
    '''
    return reduce(lambda x, y: x * y, [letter_primes[letter] for letter in word])

word_hashes = {word: word_hash_conversion(word) for word in anagram_dict}

def pos_words(rack: List[Tile], treasures: List[str], power: float=0, max_words: int=50) -> List[Tuple[str, float]]:
    '''
    Creates a list of all words in the rack that can be found in the dictionary, 
    taking frequency of letters into account, along with their evaluation from 
    word_evaluator, sorted by evaluation.
    '''
    words = []
    min_score = 0
    rack_letters = [i.letter for i in rack]
    # sort the rack by the gem of each tile, with highest priority going to 'D', then 'C', then 'R', then 'S', then 'G', then 'E', then 'A', then None
    gems = ['D', 'C', 'R', 'S', 'G', 'E', 'A', None]
    sorted_rack = sorted(rack, key=lambda x: gems.index(x.gem))
    # print(sorted_rack)
    rack_hash = word_hash_conversion(rack_letters)
    word_efficiencies = {2: 0.376, 3: 0.501, 4: 0.501, 5: 0.701, 6: 0.751, 7: 0.786, 8: 0.844, 9: 0.889, 10: 1.101, 11: 1.182, 12: 1.084, 13: 1.001, 14: 0.929, 15: 0.867, 16: 0.813}
    for letters in anagram_dict:
        if rack_hash % word_hashes[letters] == 0 and word_efficiencies[len(letters)] * len(letters) > min_score:
            letter_tiles_idx = set()
            for letter in letters:
                for i in range(16):
                    if i not in letter_tiles_idx and letter == sorted_rack[i].letter:
                        letter_tiles_idx.add(i)
                        break
            letter_tiles = [sorted_rack[i] for i in letter_tiles_idx]
            s = word_evaluator(letter_tiles, treasures, power)
            if len(words) < max_words:
                word = anagram_dict[letters]
                word_tiles_idx = set()
                out = []
                for letter in word:
                    for i in range(16):
                        if i not in word_tiles_idx and letter == sorted_rack[i].letter:
                            word_tiles_idx.add(i)
                            out += [sorted_rack[i]]
                            break
                # print("Unfilled", out, s)
                words.append((out, s))
            else:
                if s > min_score:
                    word = anagram_dict[letters]
                    word_tiles_idx = set()
                    out = []
                    for letter in word:
                        for i in range(16):
                            if i not in word_tiles_idx and letter == sorted_rack[i].letter:
                                word_tiles_idx.add(i)
                                out += [sorted_rack[i]]
                                break
                    # print("Filled", out, s)
                    words.append((out, s))
                    words.sort(key=lambda x: x[1], reverse=True)
                    words = words[:max_words]
                    min_score = words[-1][1]
    words.sort(key=lambda x: x[1], reverse=True)
    return words[:max_words]

def play(word: List[Tile], treasures: List[str], rack: List[Tile]) -> List[Tile]:
    '''
    Removes the letters of the word from the rack, and returns a new rack
    '''
    new_rack = rack.copy()
    for letter in word:
        new_rack.remove(letter)
    score = ceil(4 * letter_points(word, treasures)[0])/4
    if score <= 5:
        added_gem = None
    elif score <= 6:
        added_gem = 'A'
    elif score <= 7:
        added_gem = 'E'
    elif score <= 8:
        added_gem = 'G'
    elif score <= 9:
        added_gem = 'S'
    elif score <= 10:
        added_gem = 'R'
    elif score <= 11:
        added_gem = 'C'
    else:
        added_gem = 'D'
    return rack_generation(new_rack, added_gem)


'''
We now want to create an AI that uses a Monte Carlo search to search up to 2 layers deep and determine 
which word is the best word to play to maximise their score over the next 2 turns. 
'''

def monte_carlo_search(rack, treasures, power=0, num_simulations=100, max_words=5, depth=2):
    '''
    Uses a Monte Carlo search to determine the best word to play out of the top words, 
    taking into account the next few turns
    '''
    # First, we want to generate all possible words that can be played
    possible_words = pos_words(rack, treasures, power, max_words=max_words)
    # print(possible_words)
    # Now, we want to simulate the next 2 turns for each word, and determine the average score
    # We will do this by generating a new rack, and then generating a new word to play
    # We will then repeat this num_simulations times
    # We will then return the word with the highest average score
    best_word = None
    best_score = 0
    for word in possible_words:
        total_score = 0
        for i in range(num_simulations):
            new_rack = play(word[0], treasures, rack)
            if depth > 1:
                new_word, new_score = monte_carlo_search(new_rack, treasures, power, num_simulations=1, max_words=max(1, max_words//2), depth=depth-1)
            else:
                new_words = pos_words(new_rack, treasures, power, max_words=1)
                if len(new_words) == 0:
                    new_word, new_score = '', 0
                else:
                    new_word, new_score = new_words[0]
            total_score += new_score
        avg_score = word[1] + total_score / num_simulations
        if avg_score > best_score:
            best_score = avg_score
            best_word = word
        if depth == 2:
            print(''.join([str(tile) for tile in word[0]]), avg_score)
    return best_word, best_score

rack = [Tile('B'), Tile('M'), Tile('I', 'G'), Tile('O'), Tile('R'), Tile('V'), Tile('F'), Tile('R', 'D'), Tile('L'), Tile('V'), Tile('A'), Tile('O'), Tile('A'), Tile('L'), Tile('A'), Tile('A')]
emote_rack = ''
for tile in rack:
    str_letter = tile.letter
    if str_letter == 'Q':
        str_letter = 'Qu'
    emote_rack += ':tile_' + str_letter + ': '
treasures = []
print("Emote Rack:", emote_rack)
print("Rack:", rack)
print("Treasures:", treasures)
s = pos_words(rack, treasures, max_words=5)
print("Best scoring words:")
for i in s:
    print(''.join(str(tile) for tile in i[0]), i[1])
print("Word performances:")
out = monte_carlo_search(rack, treasures, max_words=5, depth=2)
print("Best word to play:", ''.join(str(tile) for tile in out[0][0]), "Score:", out[1])