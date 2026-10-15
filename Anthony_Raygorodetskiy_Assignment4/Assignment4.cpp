#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <array>
using namespace std;

class SudokuSolver {
private:
    int grid[9][9];

    // Checks whether a number can be placed in a cell.
    bool isValid(int row, int col, int num) {
        // Check the row and column.
        for (int i = 0; i < 9; i++) {
            if (grid[row][i] == num)
                return false;

            if (grid[i][col] == num)
                return false;
        }

        // Find the top-left corner of the 3x3 block.
        int blockRow = (row / 3) * 3;
        int blockCol = (col / 3) * 3;

        // Check the 3x3 block.
        for (int r = blockRow; r < blockRow + 3; r++) {
            for (int c = blockCol; c < blockCol + 3; c++) {
                if (grid[r][c] == num)
                    return false;
            }
        }

        return true;
    }

    // Recursive depth-first search with backtracking.
    void solveRecursive(int row, int col,
                        vector<array<int, 81>>& solutions,
                        int& solutionCount) {
        // Stop at the end of the grid.
        if (row == 9) {
            array<int, 81> solution;
            for (int r = 0; r < 9; r++) {
                for (int c = 0; c < 9; c++) {
                    solution[r * 9 + c] = grid[r][c];
                }
            }

            solutions.push_back(solution);
            solutionCount++;
            return;
        }

        // Move to the next cell.
        int nextRow = row;
        int nextCol = col + 1;

        if (nextCol == 9) {
            nextRow++;
            nextCol = 0;
        }

        // If the cell is already filled, skip it.
        if (grid[row][col] != 0) {
            solveRecursive(nextRow, nextCol,
                           solutions, solutionCount);
            return;
        }

        // Try every possible digit in the empty cell.
        for (int num = 1; num <= 9; num++) {
            if (isValid(row, col, num)) {
                grid[row][col] = num;

                solveRecursive(nextRow, nextCol,
                               solutions, solutionCount);

                // Backtrack to try another candidate.
                grid[row][col] = 0;
            }
        }
    }

public:
    // Constructor initializes all cells as empty.
    SudokuSolver() {
        for (int r = 0; r < 9; r++) {
            for (int c = 0; c < 9; c++) {
                grid[r][c] = 0;
            }
        }
    }

    // Reads a puzzle from a text file.
    bool loadPuzzle(const string& filename) {
        ifstream inputFile(filename);

        if (!inputFile.is_open()) {
            cout << "Could not open " << filename << endl;
            return false;
        }

        char ch;
        int count = 0;

        while (inputFile.get(ch) && count < 81) {
            if (ch >= '1' && ch <= '9') {
                grid[count / 9][count % 9] = ch - '0';
                count++;
            }
            else if (ch == '_') {
                grid[count / 9][count % 9] = 0;
                count++;
            }
            // Ignore spaces, line breaks, and other separators.
        }

        inputFile.close();

        if (count != 81) {
            cout << "Invalid puzzle format in "
                 << filename << endl;
            return false;
        }

        return true;
    }

    // Prints the current grid.
    void printPuzzle() {
        for (int r = 0; r < 9; r++) {
            for (int c = 0; c < 9; c++) {
                if (grid[r][c] == 0)
                    cout << "_ ";
                else
                    cout << grid[r][c] << " ";

                if (c == 2 || c == 5)
                    cout << "| ";
            }

            cout << endl;

            if (r == 2 || r == 5)
                cout << "------+-------+------" << endl;
        }
    }

    // Finds and prints every solution.
    void solvePuzzle() {
        vector<array<int, 81>> solutions;
        int solutionCount = 0;

        solveRecursive(0, 0, solutions, solutionCount);

        if (solutionCount == 0) {
            cout << "No solution found." << endl;
            return;
        }

        cout << "Number of solutions found: "
             << solutionCount << endl;

        for (int s = 0; s < solutionCount; s++) {
            cout << "\nSolution " << s + 1 << ":" << endl;

            for (int r = 0; r < 9; r++) {
                for (int c = 0; c < 9; c++) {
                    cout << solutions[s][r * 9 + c] << " ";

                    if (c == 2 || c == 5)
                        cout << "| ";
                }

                cout << endl;

                if (r == 2 || r == 5)
                    cout << "------+-------+------" << endl;
            }
        }
    }
};

int main() {
    string filenames[] = {
        "puzzle1.txt",
        "puzzle2.txt",
        "puzzle3.txt",
        "puzzle4.txt",
        "puzzle5.txt"
    };

    // Process all five puzzle files.
    for (int i = 0; i < 5; i++) {
        cout << "\n================================" << endl;
        cout << "Puzzle file: " << filenames[i] << endl;
        cout << "================================" << endl;

        SudokuSolver solver;

        if (solver.loadPuzzle(filenames[i])) {
            cout << "\nOriginal puzzle:" << endl;
            solver.printPuzzle();

            cout << "\nResults:" << endl;
            solver.solvePuzzle();
        }
    }

    return 0;
}
