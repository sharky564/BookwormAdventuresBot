import random
from math import ceil

import os
import neat

letters = ['A', 'B', 'C', 'D', 'E', 'F', 'G', 'H', 'I', 'J', 'K', 'L', 'M', 'N', 'O', 'P', 'Q', 'R', 'S', 'T', 'U', 'V', 'W', 'X', 'Y', 'Z']
dictionary = []
with open('dictionary.txt', 'r') as f:
	for line in f:
		dictionary.append(line.strip().replace('QU', 'Q'))
bonus_dict = []
# remove all Us after Qs in dictionary
letter_point_vals = [1, 1.25, 1.25, 1, 1, 1.25, 1, 1.25, 1, 1.75, 1.75, 1, 1.25, 1, 1, 1.25, 2.75, 1, 1, 1, 1, 1.5, 1.5, 2, 1.5, 2]
letter_points = {letters[i]: letter_point_vals[i] for i in range(26)}

max_letter_counts_list = [4, 3, 3, 4, 4, 3, 4, 3, 4, 2, 2, 4, 3, 4, 3, 4, 2, 4, 4, 4, 4, 3, 3, 2, 3, 2]
max_letter_counts = {letters[i]: max_letter_counts_list[i] for i in range(26)}

letter_freq_list = [0.0932, 0.0171, 0.0218, 0.0376, 0.13, 0.0235, 0.0257, 0.0252, 0.0723, 0.0077, 0.0056, 0.0466, 0.0214, 0.0547, 0.0663, 0.0261, 0.0115, 0.0752, 0.0594, 0.0684, 0.0428, 0.0171, 0.0154, 0.006, 0.0252, 0.0042]

random.seed(123)

def rack_generation(curr_rack=[]):
	letter_counts = {letter: 0 for letter in letters}
	if curr_rack != []:
		for letter in curr_rack:
			letter_counts[letter] += 1
	new_rack = list(curr_rack)
	while len(new_rack) < 16:
		x = random.random()
		i = 0
		curr_sum = 0
		while curr_sum <= x and i < 25:
			curr_sum += letter_freq_list[i]
			i += 1
		if curr_sum > x:
			letter = letters[i]
		else:
			letter = letters[25]
		if letter_counts[letter] < max_letter_counts[letter]:
			new_rack.append(letter)
			letter_counts[letter] += 1
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


def eval_genomes(genomes, config):
	for genome_id, genome in genomes:
		genome.fitness = 0.0
		net = neat.nn.FeedForwardNetwork.create(genome, config)
		for games in range(10):
			rack = rack_generation([])
			for pos_rack in range(10):
				input_rack = [ord(i) for i in rack]
				output_word = net.activate(input_rack)
				print(output_word)
				output = ''.join([chr(int(i)) for i in output_word])
				val = word_evaluator(output, [])
				if val > 0:
					for char in output:
						rack.remove(char)
					rack = rack_generation(rack)
					genome.fitness += val
				else:
					genome.fitness -= 1000


def run(config_file):
	# Load configuration.
	config = neat.Config(neat.DefaultGenome, neat.DefaultReproduction,
						 neat.DefaultSpeciesSet, neat.DefaultStagnation,
						 config_file)

	# Create the population, which is the top-level object for a NEAT run.
	p = neat.Population(config)

	# Add a stdout reporter to show progress in the terminal.
	p.add_reporter(neat.StdOutReporter(True))
	stats = neat.StatisticsReporter()
	p.add_reporter(stats)
	p.add_reporter(neat.Checkpointer(5))

	# Run for up to 300 generations.
	winner = p.run(eval_genomes, 300)

	# Display the winning genome.
	print('\nBest genome:\n{!s}'.format(winner))

	# Show output of the most fit genome against training data.
	print('\nOutput:')
	winner_net = neat.nn.FeedForwardNetwork.create(winner, config)
	for games in range(10):
		rack = rack_generation([])
		for pos_rack in range(10):
			output = winner_net.activate(rack)
			val = word_evaluator(output)
			if val > 0:
				for char in output:
					rack.remove(char)
				rack = rack_generation(rack)
		print("Got {!r} points from 1000 racks in 1000 games".format(output))
	
	p = neat.Checkpointer.restore_checkpoint('neat-checkpoint-4')
	p.run(eval_genomes, 10)

if __name__ == '__main__':
    # Determine path to configuration file. This path manipulation is
    # here so that the script will run successfully regardless of the
    # current working directory.
    local_dir = os.path.dirname(__file__)
    config_path = os.path.join(local_dir, 'bookworm_adventures_neat.ini')
    run(config_path)
