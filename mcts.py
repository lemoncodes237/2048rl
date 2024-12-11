import env2048 as env
import copy
import random
import numpy as np

class MCTSRandom:

    def __init__(self, n, B):
        self.game = env.Game(n)
        self.B = B
        self.points = 0

    def random_to_end(self, move):
        game_copy = copy.deepcopy(self.game)
        game_over, _, score = game_copy.move(move)

        while not game_over:
            game_over, _, reward = game_copy.move(random.randint(0, 3))
            score += reward
        
        return score

    def move(self):
        rewards = []
        for i in range(4):
            game = copy.deepcopy(self.game)
            _, _, changed = game.move_without_spawn(i)
            if not changed:
                rewards.append(-1)
                continue
            
            score_sum = 0

            for _ in range(self.B):
                score_sum += self.random_to_end(i)
            rewards.append(score_sum)
        
        move = np.argmax(np.asarray(rewards))

        game_over, _, points = self.game.move(move)
        self.points += points

        if game_over:
            print("Game over, total points", self.points)
            print(self.game.boards)
        return game_over
    