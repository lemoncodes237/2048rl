# 2048rl
All relevant files are in the `cpp` directory.

## Make instructions:
To use the Random MCTS (Policy used for MCTS is random): `make MCTS_TYPE=standard`

To use the Merge MCTS (Policy used for MCTS tries to maximize merges): `make MCTS_TYPE=merge`

To use the Score MCTS (Policy used for MCTS tries to maximize scores): `make MCTS_TYPE=score`

To use the pUCT for Single Games: `make MCTS_TYPE=puct`

To use the pUCT for Multiple Games: `make MCTS_TYPE=puctmult`

# Run instructions:
To run the program, use `./game2048 [num_boards] [num_iterations]`