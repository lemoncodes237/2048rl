import env2048 as env
import mcts

game = mcts.MCTSRandom(1, 100)
game_over = False

while not game_over:
    game_over = game.move()
