import random
from math import ceil

letters = ['A', 'B', 'C', 'D', 'E', 'F', 'G', 'H', 'I', 'J', 'K', 'L', 'M', 'N', 'O', 'P', 'Q', 'R', 'S', 'T', 'U', 'V', 'W', 'X', 'Y', 'Z']
letters_to_numbers = {letters[i]: i for i in range(26)}

dictionary = []


with open('Bookworm_Dictionary.txt', 'r') as f:
    for line in f:
        dictionary.append(line.strip().upper().replace('QU', 'Q'))

bonus_dict = []
# remove all Us after Qs in dictionary
letter_point_vals = [1, 1.25, 1.25, 1, 1, 1.25, 1, 1.25, 1, 1.75, 1.75, 1, 1.25, 1, 1, 1.25, 2.75, 1, 1, 1, 1, 1.5, 1.5, 2, 1.5, 2]
letter_points = {letters[i]: letter_point_vals[i] for i in range(26)}

max_letter_counts_list = [4, 3, 3, 4, 4, 3, 4, 3, 4, 2, 2, 4, 3, 4, 3, 4, 2, 4, 4, 4, 4, 3, 3, 2, 3, 2]
max_letter_counts = {letters[i]: max_letter_counts_list[i] for i in range(26)}

letter_freq_list = [0.0932, 0.0171, 0.0218, 0.0376, 0.13, 0.0235, 0.0257, 0.0252, 0.0723, 0.0077, 0.0056, 0.0466, 0.0214, 0.0547, 0.0663, 0.0261, 0.0115, 0.0752, 0.0594, 0.0684, 0.0428, 0.0171, 0.0154, 0.006, 0.0252, 0.0042]

random.seed(123)

def rack_generation(curr_rack=[]):
    # print(curr_rack)
    # print(len(curr_rack))
    if len(curr_rack) > 16:
        raise ValueError("Rack must contain at most 16 letters")
    else:
        new_rack = curr_rack.copy()
        x = len(new_rack)
        while x < 16:
            new_letter = random.choices(letters, weights=letter_freq_list)[0]
            # ensuring the maximum frequency of each letter in the rack is bounded by max_letter_counts
            if new_rack.count(new_letter) < max_letter_counts[new_letter]:
                new_rack.append(new_letter)
                x += 1
        return new_rack

def word_evaluator(word, treasures):
    lp = 0
    if word in dictionary:
        i = 0
        while i < len(word):
            letter = word[i]
            val = letter_points[letter]
            if letter == 'R':
                if "Parrot" in treasures:
                    val += 1
            if letter in ['X', 'Y', 'Z']:
                if "Bow" in treasures:
                    val += 1.5
                elif "Arch" in treasures:
                    val += 2
            lp += val
            i += 1
    
    lp_bh_rel = ceil(lp)
    bh = 0.25 * (lp_bh_rel > 0) + 0.25 * max(0, min(lp_bh_rel, 16) - 2) + 0.25 * max(0, min(lp_bh_rel, 16) - 5) + 0.25 * max(0, min(lp_bh_rel, 16) - 7) + 0.25 * max(0, min(lp_bh_rel, 16) - 9) + 0.25 * max(0, min(lp_bh_rel, 16) - 11) + 0.25 * max(0, min(lp_bh_rel, 16) - 13) + 0.5 * max(0, min(lp_bh_rel, 16) - 15)
    return bh

def pos_words(rack, treasures):
	'''
	Creates a list of all words in the rack that can be found in the dictionary, 
	taking frequency of letters into account, along with their evaluation from 
	word_evaluator
	'''
	words = []
	for word in dictionary:
		if len(word) > len(rack):
			continue
		word_letters = [letter for letter in word]
		rack_letters = [letter for letter in rack]
		for letter in word_letters:
			if letter in rack_letters:
				rack_letters.remove(letter)
			else:
				break
		else:
			words.append((word, word_evaluator(word, treasures)))
	return words


def play(word, rack):
    '''
    Removes the letters of the word from the rack, and returns a new rack
    '''
    new_rack = rack.copy()
    for letter in word:
        new_rack.remove(letter)
    return rack_generation(new_rack)


'''
We now want to create an AI that uses a Monte Carlo search to search up to 2 layers deep and determine 
which word is the best word to play to maximise their score over the next 2 turns. 
'''

def monte_carlo_search(rack, treasures, num_simulations=50):
    '''
    Uses a Monte Carlo search to determine the best word to play out of the top 20 words, 
    taking into account the next 2 turns
    '''
    # First, we want to generate all possible words that can be played
    possible_words = pos_words(rack, treasures)
    possible_words.sort(key=lambda x: x[1], reverse=True)
    possible_words = possible_words[:5]
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
            new_rack = rack_generation(play(word[0], rack))
            new_words = pos_words(new_rack, treasures)
            new_words.sort(key=lambda x: x[1], reverse=True)
            new_word = new_words[0]
            total_score += new_word[1]
            # print("NEW", new_rack, new_word)
        avg_score = word[1] + total_score / num_simulations
        if avg_score > best_score:
            best_score = avg_score
            best_word = word
        print(rack, word, avg_score)
    return best_word

def main():
    rack = rack_generation()
    treasures = []
    print("Rack:", rack)
    print("Treasures:", treasures)
    print("Best word to play:", monte_carlo_search(rack, treasures))

if __name__ == "__main__":
    main()