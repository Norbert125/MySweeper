#include <random>
#include <iostream>
#include "Board.h"

void Board::deploy_bombs() {
    int counter = 0;
    random_device rd;
    mt19937 gen(rd());
    uniform_int_distribution<> distx(0, rows - 1);
    uniform_int_distribution<> disty(0, coll - 1);
    while (counter != bombs) {
        int x = distx(gen);
        int y = disty(gen);
        if (!game_board[x][y]) { game_board[x][y] = -1; }
        else { continue; }
        counter++;
    }
}

const vector<vector<int>> &Board::getGameBoard() const {
    return game_board;
}

ostream &operator<<(ostream &os, const Board &board) {
    for (int i = 0; i < board.rows; ++i) {
        for (int j = 0; j < board.coll; ++j) {
            if (board.mask[i][j] == 0) {
                os << "# "; // Hidden
            } else if (board.game_board[i][j] == -1) {
                os << "* "; // Bomb
            } else {
                os << board.game_board[i][j] << " "; // Hint number
            }
        }
        os << endl;
    }
    return os;
}

void Board::hints() {
    for (int r = 0; r < rows; ++r) {
        for (int c = 0; c < coll; ++c) {
            if (game_board[r][c] == -1) continue;
            int count = 0;
            for (int i = -1; i <= 1; ++i) {
                for (int j = -1; j <= 1; ++j) {
                    int ni = r + i;
                    int nj = c + j;
                    if (ni >= 0 && ni < rows && nj >= 0 && nj < coll) {
                        if (game_board[ni][nj] == -1) {
                            count++;
                        }
                    }
                }
            }
            game_board[r][c] = count;
        }
    }
}

void Board::reveal(const int r, const int c) {
    if (r < 0 || r >= rows || c < 0 || c >= coll || mask[r][c] == 1 || mask[r][c] == 2) return;
    mask[r][c] = 1; // Mark as revealed
    if (game_board[r][c] == -1) {
        gameOver = true;
        revealAllBombs();
        return;
    }
    if (game_board[r][c] == 0) { // Flood fill for empty spaces
        for (int i = -1; i <= 1; i++) {
            for (int j = -1; j <= 1; j++) reveal(r + i, c + j);
        }
    }
}

void Board::toggleFlag(const int r, const int c) {
    if (mask[r][c] == 0) mask[r][c] = 2;
    else if (mask[r][c] == 2) mask[r][c] = 0;
}

void Board::revealAllBombs() {
    for (int r = 0; r < rows; ++r) {
        for (int c = 0; c < coll; ++c) {
            if (game_board[r][c] == -1) {
                mask[r][c] = 1; // Force reveal bombs
            }
        }
    }
}

bool Board::checkWin() const {
    int revealedCount = 0;
    for (int r = 0; r < rows; ++r) {
        for (int c = 0; c < coll; ++c) {
            if (mask[r][c] == 1) revealedCount++;
        }
    }
    // Win if: Total cells - Revealed cells == Number of bombs
    return (rows * coll - revealedCount == bombs);
}

void Board::chord(int r, int c) {
    // Only chord if the cell is already revealed and has a number > 0
    if (mask[r][c] != 1 || game_board[r][c] <= 0) return;

    int targetNumber = game_board[r][c];
    int flagCount = 0;

    // Count flags in 3x3 area
    for (int i = -1; i <= 1; i++) {
        for (int j = -1; j <= 1; j++) {
            int nr = r + i;
            int nc = c + j;
            if (nr >= 0 && nr < rows && nc >= 0 && nc < coll) {
                if (mask[nr][nc] == 2) flagCount++;
            }
        }
    }
    // If flags match the number, reveal all non-flagged neighbors
    if (flagCount == targetNumber) {
        for (int i = -1; i <= 1; i++) {
            for (int j = -1; j <= 1; j++) {
                int nr = r + i;
                int nc = c + j;
                if (nr >= 0 && nr < rows && nc >= 0 && nc < coll) {
                    // Only reveal if it's not already revealed and not flagged
                    if (mask[nr][nc] == 0) {
                        reveal(nr, nc); // This will trigger game over if it's a bomb!
                    }
                }
            }
        }
    }
}
