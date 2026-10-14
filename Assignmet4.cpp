#include <iostream>
#include <fstream>
#include <vector>
#include <string>

class SudokuSolver {
public:
    static const int SIZE = 9;
    static const int SUBGRID = 3;

    SudokuSolver() : solutionCount(0) {
        grid.assign(SIZE, std::vector<char>(SIZE, '_'));
    }

    // Loads puzzle grid from the specified file
    bool loadFromFile(const std::string& filename) {
        std::ifstream inFile(filename);
        if (!inFile) {
            std::cerr << "Error: Could not open file " << filename << "\n";
            return false;
        }

        std::string line;
        int row = 0;
        while (row < SIZE && inFile >> line) {
            // Read valid non-whitespace characters representing cells
            for (size_t col = 0; col < line.size() && row < SIZE; ++col) {
                char ch = line[col];
                if (ch == '_' || (ch >= '1' && ch <= '9')) {
                    grid[row][col] = ch;
                }
            }
            row++;
        }

        inFile.close();
        return (row == SIZE);
    }

    // Prints the current state of the grid
    void displayGrid() const {
        for (int r = 0; r < SIZE; ++r) {
            for (int c = 0; c < SIZE; ++c) {
                std::cout << grid[r][c] << (c == SIZE - 1 ? "" : " ");
            }
            std::cout << "\n";
        }
    }

    // Executes the recursive depth-first search to find all solutions
    void solve(const std::string& filename) {
        std::cout << "========================================\n";
        std::cout << "Puzzle File: " << filename << "\n";
        std::cout << "----------------------------------------\n";
        std::cout << "Initial Puzzle:\n";
        displayGrid();
        std::cout << "----------------------------------------\n";

        solutionCount = 0;
        backtrack(0, 0);

        if (solutionCount == 0) {
            std::cout << "No solution found\n";
        } else {
            std::cout << "Total Solutions Found: " << solutionCount << "\n";
        }
        std::cout << "========================================\n\n";
    }

private:
    std::vector<std::vector<char>> grid;
    int solutionCount;

    // Checks row, column, and 3x3 block constraints
    bool isValid(int row, int col, char num) const {
        // Check row and column
        for (int i = 0; i < SIZE; ++i) {
            if (grid[row][i] == num || grid[i][col] == num) {
                return false;
            }
        }

        // Check 3x3 block
        int startRow = (row / SUBGRID) * SUBGRID;
        int startCol = (col / SUBGRID) * SUBGRID;
        for (int r = 0; r < SUBGRID; ++r) {
            for (int c = 0; c < SUBGRID; ++c) {
                if (grid[startRow + r][startCol + c] == num) {
                    return false;
                }
            }
        }

        return true;
    }

    // Returns all candidates (1-9) that can legally be placed at (row, col)
    std::vector<char> getCandidates(int row, int col) const {
        std::vector<char> candidates;
        for (char num = '1'; num <= '9'; ++num) {
            if (isValid(row, col, num)) {
                candidates.push_back(num);
            }
        }
        return candidates;
    }

    // Recursive depth-first search with backtracking
    void backtrack(int row, int col) {
        // End of row reached: move to the next row
        if (col == SIZE) {
            row++;
            col = 0;
        }

        // Base case: all 81 cells traversed successfully
        if (row == SIZE) {
            solutionCount++;
            std::cout << "Solution #" << solutionCount << ":\n";
            displayGrid();
            std::cout << "\n";
            return; // Backtrack to find any other solutions
        }

        // Skip cells already filled with clues
        if (grid[row][col] != '_') {
            backtrack(row, col + 1);
            return;
        }

        // Deduce candidates for current empty cell
        std::vector<char> candidates = getCandidates(row, col);

        // Exhaustively try each candidate
        for (char candidate : candidates) {
            grid[row][col] = candidate;       // Place candidate
            backtrack(row, col + 1);          // Recurse to next cell
            grid[row][col] = '_';             // Backtrack / Undo choice
        }
    }
};

int main() {
    std::vector<std::string> puzzleFiles = {
        "puzzle1.txt",
        "puzzle2.txt",
        "puzzle3.txt",
        "puzzle4.txt",
        "puzzle5.txt"
    };

    for (const std::string& filename : puzzleFiles) {
        SudokuSolver solver;
        if (solver.loadFromFile(filename)) {
            solver.solve(filename);
        }
    }

    return 0;
}