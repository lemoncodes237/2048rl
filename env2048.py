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
            elif line[end - 1] == 0:
                moved = True
                line[end - 1] = line[i]
                line[i] = 0
            
            end -= 1
            
        return line, moved
    
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
    
    # 0 = Up, 1 = Down, 2 = Right, 3 = Left
    # Returns gameOver, gameOverBoard
    def move(self, dir):
        game_end = False
        game_end_board = None

        for index in range(self.num_boards):
            board = self.boards[index]

            # If the board changed
            changed = False

            for i in range(4):
                if dir == 0:
                    board[::-1,i], moved = self.move_line(board[::-1,i])
                if dir == 1:
                    board[:,i], moved = self.move_line(board[:,i])
                if dir == 2:
                    board[i,:], moved = self.move_line(board[i,:])
                if dir == 3:
                    board[i,::-1], moved = self.move_line(board[i,::-1])

                changed = changed or moved
            
            if not changed:
                continue
            
            self.boards[index] = self.gen_random(board)

            if self.game_over(board):
                game_end = True
                game_end_board = board
        
        return game_end, game_end_board

def main():
    game = Game(1)
    for i in range(1000):
        game_over, _ = game.move(random.randint(0, 3))
        if game_over:
            print(_)
            break


main()