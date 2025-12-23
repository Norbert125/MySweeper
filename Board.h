#ifndef MYSWEEPER_BOARD_H
#define MYSWEEPER_BOARD_H

#include <istream>
#include <vector>
#include <ostream>

using namespace std;

class Board {
private:
    vector<vector<int>> game_board;
    vector<vector<int>> mask;
    int bombs{}, rows{}, coll{};
    bool gameOver = false;
public:
    Board() = default;

    Board(const int bombs, const int rows, const int coll) : bombs(bombs), rows(rows), coll(coll) {
        game_board.resize(rows, vector<int>(coll, 0));
        mask.resize(rows, vector<int>(coll, 0));
        deploy_bombs();
        hints();
    }

    void deploy_bombs();

    void hints();

    void reveal(int r, int c);

    void toggleFlag(int r, int c);

    void revealAllBombs();

    [[nodiscard]] bool checkWin() const;

    void chord(int r, int c);

    [[nodiscard]] const vector<vector<int>> &getGameBoard() const;

    [[nodiscard]] bool isGameOver() const { return gameOver; }

    [[nodiscard]] int getMaskValue(int r, int c) const {return mask[r][c];}

    friend ostream &operator<<(ostream &os, const Board &board);

};


#endif //MYSWEEPER_BOARD_H
