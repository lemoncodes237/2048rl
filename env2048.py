import numpy as np
import random

class Game:
    def gen_random(self, board):
        zeros = np.where(board == 0)
        nEmpty = zeros[0].size
        
        new = 2 if np.random.rand(1) < 0.9 else 4
        index = random.randint(0, nEmpty - 1)

        board[zeros[0][index], zeros[1][index]] = new
        return board

    def __init__(self, num_boards):
        self.num_boards = num_boards
        self.boards = []
        for i in range(num_boards):
            self.boards.append(self.gen_random(self.gen_random(np.zeros((4, 4)))))

    def move_line(self, line):
        end = 3
        moved = False
        score = 0
        for i in range(2, -1, -1):
            if line[i] == 0:
                continue
            
            if line[end] == 0:
                moved = True
                line[end] = line[i]
                line[i] = 0
                continue
            elif line[end] == line[i]:
                moved = True
                line[end] *= 2
                line[i] = 0
                score += line[end]
            elif line[end - 1] == 0:
                moved = True
                line[end - 1] = line[i]
                line[i] = 0
            
            end -= 1
            
        return line, moved, score
    
    def game_over(self, board):
        for i in range(4):
            for j in range(4):
                if i != 0 and board[i, j] == board[i - 1, j]:
                    return False
                if i != 3 and board[i, j] == board[i + 1, j]:
                    return False
                if j != 0 and board[i, j] == board[i, j - 1]:
                    return False
                if j != 3 and board[i, j] == board[i, j + 1]:
                    return False
                if board[i, j] == 0:
                    return False
        return True

    def move_without_spawn(self, dir):
        changed_per_board = []
        reward = 0

        for index in range(self.num_boards):
            board = self.boards[index]

            # If the board changed
            changed = False

            for i in range(4):
                if dir == 0:
                    board[::-1,i], moved, score = self.move_line(board[::-1,i])
                if dir == 1:
                    board[:,i], moved, score = self.move_line(board[:,i])
                if dir == 2:
                    board[i,:], moved, score = self.move_line(board[i,:])
                if dir == 3:
                    board[i,::-1], moved, score = self.move_line(board[i,::-1])

                changed = changed or moved
                reward += score
            
            changed_per_board.append(changed)
        
        return changed_per_board, reward
    
    # 0 = Up, 1 = Down, 2 = Right, 3 = Left
    # Returns gameOver, gameOverBoard
    def move(self, dir):
        game_end = False
        game_end_board = None

        changed, reward = self.move_without_spawn(dir)

        for index in range(self.num_boards):
            if changed[index]:
                self.boards[index] = self.gen_random(self.boards[index])

                if self.game_over(self.boards[index]):
                    game_end = True
                    game_end_board = self.boards[index]
        
        return game_end, game_end_board, reward

def main():
    game = Game(2)
    for i in range(1000):
        game_over, _, _ = game.move(random.randint(0, 3))
        if game_over:
            print(game.boards[0], "\n", game.boards[1])
            break


main()