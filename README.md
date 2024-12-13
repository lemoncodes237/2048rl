# 2048rl
All relevant files are in the `cpp` directory.

## Make instructions:
To use Monte Carlo with Random Policy: `make MCTS_TYPE=standard`

To use Monte Carlo with Merge Policy: `make MCTS_TYPE=merge`

To use Monte Carlo with Score Policy (does not work well): `make MCTS_TYPE=score`

To use pUCT for Single Games: `make MCTS_TYPE=puct`

To use pUCT for Multiple Games: `make MCTS_TYPE=puctmult`

To use Oblivious pUCT for Multiple Games: `make MCTS_TYPE=puctcombmult`

# Run instructions:
To run the program, use `./game2048 [num_boards] [num_iterations]`