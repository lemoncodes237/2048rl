import numpy as np
import random
import pygame
import time
import argparse
from copy import deepcopy
import math

COLORS = {
    0: (205, 193, 180),     # Empty cell
    2: (238, 228, 218),
    4: (237, 224, 200),
    8: (242, 177, 121),
    16: (245, 149, 99),
    32: (246, 124, 95),
    64: (246, 94, 59),
    128: (237, 207, 114),
    256: (237, 204, 97),
    512: (237, 200, 80),
    1024: (237, 197, 63),
    2048: (237, 194, 46)
}

TEXT_COLORS = {
    0: (205, 193, 180),
    2: (119, 110, 101),
    4: (119, 110, 101),
    8: (249, 246, 242),
    16: (249, 246, 242),
    32: (249, 246, 242),
    64: (249, 246, 242),
    128: (249, 246, 242),
    256: (249, 246, 242),
    512: (249, 246, 242),
    1024: (249, 246, 242),
    2048: (249, 246, 242)
}

class Game:

    '''
    @def gen_random
    @params: self, board
        Generates a random tile on `board`. The random tile is 2 with 9/10 probability and 4 with 1/10 probability.

        Return:
            board: The new board after the random tile was placed.
    '''
    def gen_random(self, board):
        zeros = np.where(board == 0)
        nEmpty = zeros[0].size
        
        new = 2 if np.random.rand(1) < 0.9 else 4
        index = random.randint(0, nEmpty - 1)

        board[zeros[0][index], zeros[1][index]] = new
        return board

    def __init__(self, num_boards, animate=False):
        self.num_boards = num_boards
        self.animate = animate
        self.boards = []
        self.previous_boards = []
        for i in range(num_boards):
            self.boards.append(self.gen_random(self.gen_random(np.zeros((4, 4)))))
            self.previous_boards.append(np.zeros((4, 4)))

    '''
    @def move_line
    @params: self, line
        Takes in a length 4 list and moves everything in the list to the right, merging as necessary.

        Return:
            line: The new line after the move
            moved: Whether the move changed anything on the line
            score: Total scores earned by the move
    '''
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
    
    '''
    @def game_over
    @params: self, board
        Checks if no more moves can be made on `board`.

        Returns True if the board is in a game over state, False otherwise.
    '''
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

    '''
    @def move_without_spawn
    @params: self, dir
        Moves the every board in `self.board` according to the direction indicated by `dir`.
        The direction is encoded as 0 = Up, 1 = Down, 2 = Right, 3 = Left.
        This function does not spawn a random tile after a move.

        Return:
            changed_per_board: Returns list containing whether each board has changed from the move or not.
            reward: Total points scored from the move
            change_occurred: Boolean representing whether any board changed from the move or not.
    '''
    def move_without_spawn(self, dir):
        changed_per_board = []
        reward = 0
        change_occurred = False

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
            change_occurred = change_occurred or changed
        
        return changed_per_board, reward, change_occurred
    
    '''
    @def move
    @params: self, dir
        Moves the every board in `self.board` according to the direction indicated by `dir`.
        The direction is encoded as 0 = Up, 1 = Down, 2 = Right, 3 = Left.
        This function spawns a random tile after the move.

        Return:
            game_end: Boolean indicating whether the game is over
            game_end_board: If `game_end` is true, contains a board that caused the game over
            reward: Total points gained from the move.
    '''
    def move(self, dir):
        game_end = False
        game_end_board = None

        changed, reward, _ = self.move_without_spawn(dir)

        for index in range(self.num_boards):
            if changed[index]:
                self.boards[index] = self.gen_random(self.boards[index])

                if self.game_over(self.boards[index]):
                    game_end = True
                    game_end_board = self.boards[index]
        
        return game_end, game_end_board, reward

    def calculate_board_size(self, screen_width, screen_height, num_boards):
        """Calculate the optimal board size and layout based on screen dimensions"""
        min_padding = 20  # Minimum padding between boards
        aspect_ratio = 1.0  # Board aspect ratio (square)
        
        # Calculate number of rows and columns for the grid
        num_cols = math.ceil(math.sqrt(num_boards))
        num_rows = math.ceil(num_boards / num_cols)
        
        # Calculate maximum possible board size that fits the screen
        max_board_width = (screen_width - (num_cols + 1) * min_padding) / num_cols
        max_board_height = (screen_height - (num_rows + 1) * min_padding) / num_rows
        
        # Use the smaller dimension to maintain aspect ratio
        board_size = min(max_board_width, max_board_height)
        
        # Calculate actual padding to center everything
        total_width = num_cols * board_size
        total_height = num_rows * board_size
        
        h_padding = (screen_width - total_width) / (num_cols + 1)
        v_padding = (screen_height - total_height) / (num_rows + 1)
        
        return board_size, h_padding, v_padding, num_rows, num_cols

    def draw_grid(self, screen, board_size, x_offset, y_offset):
        """Draw the background grid"""
        cell_size = board_size / 4.5  # Divide board size by 4.5 to leave room for padding
        padding = cell_size * 0.15    # 15% of cell size for padding
        
        # Draw background rectangle
        background_rect = pygame.Rect(x_offset, y_offset, board_size, board_size)
        pygame.draw.rect(screen, (187, 173, 160), background_rect, border_radius=int(board_size * 0.02))
        
        # Draw empty cells
        for i in range(4):
            for j in range(4):
                rect = pygame.Rect(
                    x_offset + padding + j * (cell_size + padding),
                    y_offset + padding + i * (cell_size + padding),
                    cell_size,
                    cell_size
                )
                pygame.draw.rect(screen, COLORS[0], rect, border_radius=int(cell_size * 0.08))

    def draw_tile(self, screen, value, rect, alpha=255):
        """Draw a single tile with optional transparency"""
        color = COLORS.get(value, (237, 194, 46))
        s = pygame.Surface((rect.width, rect.height))
        s.fill(color)
        s.set_alpha(alpha)
        
        # Draw rounded rectangle
        pygame.draw.rect(s, color, s.get_rect(), border_radius=int(rect.width * 0.08))
        screen.blit(s, rect)
        
        if value != 0:
            # Scale font size based on tile size and number length
            font_size = int(rect.width * (0.45 if value < 1000 else 0.35))
            font = pygame.font.Font(None, font_size)
            text_color = TEXT_COLORS.get(value, (249, 246, 242))
            text = font.render(str(value), True, text_color)
            text_rect = text.get_rect(center=rect.center)
            screen.blit(text, text_rect)

    def draw_board(self, screen, board, prev_board, board_size, x_offset, y_offset, progress=1.0):
        """Draw a single board with animations"""
        cell_size = board_size / 4.5
        padding = cell_size * 0.15
        
        # Draw the grid background first
        self.draw_grid(screen, board_size, x_offset, y_offset)
        
        for i in range(4):
            for j in range(4):
                value = int(board[i][j])
                prev_value = int(prev_board[i][j])
                
                rect = pygame.Rect(
                    x_offset + padding + j * (cell_size + padding),
                    y_offset + padding + i * (cell_size + padding),
                    cell_size,
                    cell_size
                )
                
                if self.animate and value != prev_value:
                    if prev_value != 0:
                        self.draw_tile(screen, prev_value, rect, alpha=int(255 * (1 - progress)))
                    if value != 0:
                        self.draw_tile(screen, value, rect, alpha=int(255 * progress))
                else:
                    self.draw_tile(screen, value, rect)

    def draw_game(self, screen, progress=1.0):
        """Draw all boards in a grid layout"""
        screen.fill((250, 248, 239))  # Light cream background
        
        # Calculate optimal board size and layout
        board_size, h_padding, v_padding, num_rows, num_cols = self.calculate_board_size(
            screen.get_width(), 
            screen.get_height(), 
            self.num_boards
        )
        
        for i, (board, prev_board) in enumerate(zip(self.boards, self.previous_boards)):
            row = i // num_cols
            col = i % num_cols
            
            x_offset = h_padding + col * (board_size + h_padding)
            y_offset = v_padding + row * (board_size + v_padding)
            
            self.draw_board(
                screen,
                board,
                prev_board,
                board_size,
                x_offset,
                y_offset,
                progress
            )

def main():

    parser = argparse.ArgumentParser(description='2048 Game Visualization')
    parser.add_argument('--animate', action='store_true', help='Enable animations')
    parser.add_argument('--num_boards', type=int, default=4, help='Number of boards to display')
    args = parser.parse_args()

    pygame.init()
    
    # Set up display to be 90% of the screen size
    display_info = pygame.display.Info()
    window_width = int(display_info.current_w * 0.9)
    window_height = int(display_info.current_h * 0.9)
    
    screen = pygame.display.set_mode((window_width, window_height))
    pygame.display.set_caption('2048 Game Visualization')
    
    game = Game(args.num_boards, animate=args.animate)
    
    clock = pygame.time.Clock()
    running = True
    animation_time = 0
    animation_duration = 200  # milliseconds
    
    while running:
        current_time = pygame.time.get_ticks()
        
        for event in pygame.event.get():
            if event.type == pygame.QUIT:
                running = False
            elif event.type == pygame.KEYDOWN:
                if event.key in [pygame.K_UP, pygame.K_DOWN, pygame.K_RIGHT, pygame.K_LEFT]:
                    game.previous_boards = [board.copy() for board in game.boards]
                    
                    if event.key == pygame.K_UP:
                        game_over, _, _ = game.move(0)
                    elif event.key == pygame.K_DOWN:
                        game_over, _, _ = game.move(1)
                    elif event.key == pygame.K_RIGHT:
                        game_over, _, _ = game.move(2)
                    elif event.key == pygame.K_LEFT:
                        game_over, _, _= game.move(3)
                    
                    if game.animate:
                        animation_time = current_time
                    
                    if game_over:
                        print("Game Over!")
                elif event.key == pygame.K_r:
                    game = Game(args.num_boards, animate=args.animate)
                elif event.key == pygame.K_f:  # Toggle fullscreen
                    pygame.display.toggle_fullscreen()
        
        # Calculate animation progress
        if game.animate and current_time - animation_time < animation_duration:
            progress = (current_time - animation_time) / animation_duration
        else:
            progress = 1.0
        
        game.draw_game(screen, progress)
        pygame.display.flip()
        clock.tick(60)
    
    pygame.quit()

main()