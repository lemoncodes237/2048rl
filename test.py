import env2048 as env
import mcts

game = mcts.MCTSRandom(2, 10)
game_over = False

while not game_over:
    game_over = game.move()
