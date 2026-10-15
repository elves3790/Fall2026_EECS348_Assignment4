// Project Name: EECS 348 Assignment 4
// Author Name: Anthony Raygorodetskiy(Ku ID: 3159281)
// Brief Description: The goal of this code is to use File I/O to 
// take in different sudoku puzzles (9X9 squares with numbers 1-9) and 
// to fill in the empty spaces so that no identical number is in the same row,
// column, or diagonal while also having each square have one of each number.
// This code uses recursion in order to check every possible solution to the puzzle
// untill it matches with the given rules and prints out every possible solution that it
// might find. 
// Collaborators: Gemini, ChatGPT
// First Revision: 10/14/2026
// Most Recent Revision: 10/15/2026
#include <iostream>  // Provides cout for displaying output in the console.
#include <fstream>   // Provides ifstream for reading puzzle files.
#include <string>    // Provides the string type for filenames and text.
#include <vector>    // Provides vector for storing candidate digits.
#include <cctype>    // Provides isspace() for identifying whitespace characters.

using namespace std; // Allows us to use standard library names without writing std::.

// Defines the class that contains the Sudoku grid and its solving operations.
class SudokuSolver {
private: // Members in this section can only be accessed directly by this class.

    // IMPROVEMENT: Named constants make the code easier to read and maintain.
    static constexpr int SIZE = 9; // The Sudoku grid has 9 rows and 9 columns.
    static constexpr int BLOCK_SIZE = 3; // Each Sudoku block has 3 rows and 3 columns.
    static constexpr int CELL_COUNT = SIZE * SIZE; // The grid contains 81 cells.

    int grid[SIZE][SIZE]; // Stores the puzzle; 0 represents an empty cell.

    // Checks whether a digit can legally be placed in a specified cell.
    bool isValid(int row, int col, int num) const {
        // Start checking the row and column at index 0 and continue through index 8.
        for (int i = 0; i < SIZE; i++) {

            // If the digit already appears in this row, it cannot be placed here.
            if (grid[row][i] == num)
                return false; // Reject this digit because it violates the row rule.

            // If the digit already appears in this column, it cannot be placed here.
            if (grid[i][col] == num)
                return false; // Reject this digit because it violates the column rule.
        }

        // Calculate the first row of the 3x3 block containing this cell.
        int blockRow = (row / BLOCK_SIZE) * BLOCK_SIZE;

        // Calculate the first column of the 3x3 block containing this cell.
        int blockCol = (col / BLOCK_SIZE) * BLOCK_SIZE;

        // Visit all three rows of the relevant 3x3 block.
        for (int r = blockRow; r < blockRow + BLOCK_SIZE; r++) {

            // Visit all three columns of the relevant 3x3 block.
            for (int c = blockCol; c < blockCol + BLOCK_SIZE; c++) {

                // Check whether the candidate digit already appears in this block.
                if (grid[r][c] == num)
                    return false; // Reject the digit because it violates the block rule.
            }
        }

        return true; // The digit does not conflict with the row, column, or block.
    }

    // IMPROVEMENT: Checks the original clues before starting the search.
    // This detects repeated digits in any original row, column, or 3x3 block.
    bool validateInitialPuzzle() {

        // Examine every row in the Sudoku grid.
        for (int r = 0; r < SIZE; r++) {

            // Examine every column in the current row.
            for (int c = 0; c < SIZE; c++) {

                // Save the current cell's value in a variable.
                int value = grid[r][c];

                // Skip empty cells because they cannot contain conflicting clues.
                if (value == 0)
                    continue; // Move to the next cell.

                // Temporarily clear this cell so isValid() can check its value
                // against the other cells without comparing the cell to itself.
                grid[r][c] = 0;

                // Check whether the original digit conflicts with another clue.
                bool valid = isValid(r, c, value);

                // Restore the original digit regardless of the validation result.
                grid[r][c] = value;

                // Stop validation immediately if the clue violates a Sudoku rule.
                if (!valid)
                    return false; // The original puzzle is invalid.
            }
        }

        return true; // All original clues satisfy the Sudoku constraints.
    }

    // IMPROVEMENT: Prints a solution immediately instead of storing it in a vector.
    // This reduces memory use when a puzzle has many solutions.
    void printSolution() const {

        // Process each row of the completed Sudoku grid.
        for (int r = 0; r < SIZE; r++) {

            // Process each cell in the current row.
            for (int c = 0; c < SIZE; c++) {

                // Display the digit stored in the current cell.
                cout << grid[r][c] << " ";

                // Print a vertical separator after the third and sixth columns.
                if (c == 2 || c == 5)
                    cout << "| "; // Visually separate the three blocks.
            }

            cout << '\n'; // Start a new output line after printing a row.

            // Print a horizontal separator after the third and sixth rows.
            if (r == 2 || r == 5)
                cout << "------+-------+------\n"; // Separate the three block rows.
        }
    }

    // IMPROVEMENT: Selects the empty cell with the fewest legal candidates.
    // This technique is called Minimum Remaining Values (MRV).
    bool findBestCell(int& bestRow, int& bestCol,
                      vector<int>& bestCandidates) {

        // Initially, the smallest candidate count is larger than any possible count.
        int fewestCandidates = SIZE + 1;

        // -1 indicates that no empty cell has been selected yet.
        bestRow = -1;

        // -1 indicates that no empty cell has been selected yet.
        bestCol = -1;

        // Remove any candidates left over from a previous search step.
        bestCandidates.clear();

        // Examine every row to search for an empty cell.
        for (int r = 0; r < SIZE; r++) {

            // Examine every column in the current row.
            for (int c = 0; c < SIZE; c++) {

                // Skip cells that already contain a digit.
                if (grid[r][c] != 0)
                    continue; // Move to the next cell.

                // Create a list for the legal digits in this empty cell.
                vector<int> candidates;

                // Try all possible Sudoku digits, from 1 through 9.
                for (int num = 1; num <= SIZE; num++) {

                    // Check whether the current digit is legal in this cell.
                    if (isValid(r, c, num))
                        candidates.push_back(num); // Save the legal candidate.
                }

                // If an empty cell has no legal candidates, this search path fails.
                if (candidates.empty())
                    return false; // Tell the recursive solver that this path is impossible.

                // Check whether this cell has fewer candidates than the best one so far.
                if (static_cast<int>(candidates.size()) < fewestCandidates) {

                    // Update the smallest candidate count found so far.
                    fewestCandidates = static_cast<int>(candidates.size());

                    // Record the row of the best cell.
                    bestRow = r;

                    // Record the column of the best cell.
                    bestCol = c;

                    // Save the legal candidates for the selected cell.
                    bestCandidates = candidates;

                    // If only one candidate exists, no other cell can improve this count.
                    if (fewestCandidates == 1)
                        return true; // Return the forced choice immediately.
                }
            }
        }

        // Return true if an empty cell was found.
        // Return false if no empty cell remains, meaning the grid is complete.
        return bestRow != -1;
    }

    // Performs recursive depth-first search with backtracking.
    // The solution counter is passed by reference so every recursive call
    // updates the same counter.
    void solveRecursive(unsigned long long& solutionCount) {

        // Variables that will hold the coordinates of the selected empty cell.
        int row, col;

        // Stores the legal candidates for the selected cell.
        vector<int> candidates;

        // Find the empty cell with the fewest legal candidates.
        bool foundCell = findBestCell(row, col, candidates);

        // If no usable cell was found, the grid may be complete or this path may fail.
        if (!foundCell) {

            // Assume the grid is complete until an empty cell is discovered.
            bool complete = true;

            // Examine every row to check whether all cells are filled.
            for (int r = 0; r < SIZE; r++) {

                // Examine every column in the current row.
                for (int c = 0; c < SIZE; c++) {

                    // A zero indicates that at least one cell remains empty.
                    if (grid[r][c] == 0) {

                        // Mark the grid as incomplete.
                        complete = false;

                        // Stop checking this row because an empty cell was found.
                        break;
                    }
                }

                // Stop checking further rows if the grid is incomplete.
                if (!complete)
                    break;
            }

            // If no empty cells remain, a complete valid solution has been found.
            if (complete) {

                // Increase the total number of solutions found.
                solutionCount++;

                // Display the solution number.
                cout << "\nSolution " << solutionCount << ":\n";

                // Print the completed Sudoku grid immediately.
                printSolution();
            }

            // Return to the previous recursive call.
            // If the grid was incomplete, this branch was a dead end.
            return;
        }

        // Try each legal candidate for the selected empty cell.
        for (int num : candidates) {

            // Place the candidate digit into the selected cell.
            grid[row][col] = num;

            // Recursively try to solve the rest of the puzzle.
            solveRecursive(solutionCount);

            // BACKTRACK: clear the cell after the recursive call returns.
            // This allows the next candidate to be tested.
            grid[row][col] = 0;
        }
    }

public: // Methods in this section can be called from main().

    // Constructor: runs automatically when a SudokuSolver object is created.
    SudokuSolver() {

        // Visit all rows to initialize the grid.
        for (int r = 0; r < SIZE; r++) {

            // Visit all columns in the current row.
            for (int c = 0; c < SIZE; c++) {

                // Set every cell to zero to represent an empty cell.
                grid[r][c] = 0;
            }
        }
    }

    // Reads a puzzle file and loads its 81 cells into the grid.
    bool loadPuzzle(const string& filename) {

        // Attempt to open the specified file for reading.
        ifstream inputFile(filename);

        // Check whether the file was successfully opened.
        if (!inputFile.is_open()) {

            // Tell the user which file could not be opened.
            cout << "Error: Could not open " << filename << ".\n";

            // Return false to indicate that loading failed.
            return false;
        }

        // Clear the grid so it does not retain values from another puzzle.
        for (int r = 0; r < SIZE; r++) {

            // Visit each column in the current row.
            for (int c = 0; c < SIZE; c++) {

                // Initialize the cell as empty.
                grid[r][c] = 0;
            }
        }

        // A character variable used to read the file one character at a time.
        char ch;

        // Counts the number of puzzle cells that have been read.
        int count = 0;

        // Continue reading until the file reaches its end or an error occurs.
        while (inputFile.get(ch)) {

            // Ignore spaces, tabs, and newline characters in the file.
            if (isspace(static_cast<unsigned char>(ch)))
                continue; // Do not count whitespace as a Sudoku cell.

            // Accept only digits 1-9 and underscores representing blank cells.
            if ((ch < '1' || ch > '9') && ch != '_') {

                // Report an invalid character.
                cout << "Error: Invalid character in " << filename << ".\n";

                // Stop loading this file.
                return false;
            }

            // Reject the file if it contains more than 81 cells.
            if (count >= CELL_COUNT) {

                // Explain why the file format is invalid.
                cout << "Error: Too many cells in " << filename << ".\n";

                // Stop loading this file.
                return false;
            }

            // Convert the cell number into a row index from 0 through 8.
            int row = count / SIZE;

            // Convert the cell number into a column index from 0 through 8.
            int col = count % SIZE;

            // An underscore represents an empty cell, so store zero.
            if (ch == '_')
                grid[row][col] = 0;

            // Convert a character digit into its corresponding integer value.
            else
                grid[row][col] = ch - '0';

            // Record that one more cell has been loaded.
            count++;
        }

        // Check whether a low-level input error occurred while reading the file.
        if (inputFile.bad()) {

            // Report the reading error.
            cout << "Error: Failed while reading " << filename << ".\n";

            // Indicate that loading failed.
            return false;
        }

        // Reject the file if it contains fewer than 81 cells.
        if (count != CELL_COUNT) {

            // Tell the user that exactly 81 cells are required.
            cout << "Error: " << filename
                 << " must contain exactly 81 cells.\n";

            // Indicate that loading failed.
            return false;
        }

        // IMPROVEMENT: Check that the starting clues do not conflict.
        if (!validateInitialPuzzle()) {

            // Report that the original clues violate Sudoku rules.
            cout << "Error: Conflicting clues in " << filename << ".\n";

            // Do not attempt to solve an invalid puzzle.
            return false;
        }

        // The file was read successfully and its original clues are valid.
        return true;
    }

    // Displays the original puzzle before the solving process begins.
    void printPuzzle() const {

        // Process each row of the grid.
        for (int r = 0; r < SIZE; r++) {

            // Process each cell in the current row.
            for (int c = 0; c < SIZE; c++) {

                // Print an underscore if the cell is empty.
                if (grid[r][c] == 0)
                    cout << "_ ";

                // Otherwise, print the digit already in the cell.
                else
                    cout << grid[r][c] << " ";

                // Print a vertical separator after columns 3 and 6.
                if (c == 2 || c == 5)
                    cout << "| ";
            }

            // Move to the next line after printing the current row.
            cout << '\n';

            // Print a horizontal separator after rows 3 and 6.
            if (r == 2 || r == 5)
                cout << "------+-------+------\n";
        }
    }

    // Starts solving the puzzle and reports the total number of solutions.
    void solvePuzzle() {

        // Initialize the solution counter to zero.
        unsigned long long solutionCount = 0;

        // Start the recursive search at the current puzzle state.
        solveRecursive(solutionCount);

        // If the recursive search found no completed grids, no solution exists.
        if (solutionCount == 0) {

            // Print the required no-solution message.
            cout << "No solution found.\n";
        }

        // Otherwise, report the total number of solutions discovered.
        else {

            // Display the total number of solutions.
            cout << "\nTotal solutions found: " << solutionCount << '\n';
        }
    }
};

// The main() function is where program execution begins.
int main() {

    // List the five puzzle filenames required by the assignment.
    const string filenames[] = {
        "puzzle1.txt", // First Sudoku puzzle file.
        "puzzle2.txt", // Second Sudoku puzzle file.
        "puzzle3.txt", // Third Sudoku puzzle file.
        "puzzle4.txt", // Fourth Sudoku puzzle file.
        "puzzle5.txt"  // Fifth Sudoku puzzle file.
    };

    // IMPROVEMENT: Calculate the number of filenames automatically.
    // This avoids hardcoding the number 5 in the loop condition.
    const int fileCount = sizeof(filenames) / sizeof(filenames[0]);

    // Process every puzzle filename in the array.
    for (int i = 0; i < fileCount; i++) {

        // Print a separator to distinguish the current puzzle's output.
        cout << "\n================================\n";

        // Display the filename currently being processed.
        cout << "Puzzle file: " << filenames[i] << '\n';

        // Print another separator below the filename.
        cout << "================================\n";

        // Create a new SudokuSolver object for this puzzle.
        SudokuSolver solver;

        // Attempt to load the current puzzle file.
        if (solver.loadPuzzle(filenames[i])) {

            // Print a heading for the original puzzle.
            cout << "\nOriginal puzzle:\n";

            // Display the puzzle before solving it.
            solver.printPuzzle();

            // Print a heading for the solving process.
            cout << "\nSolving puzzle...\n";

            // Find and print every valid solution for this puzzle.
            solver.solvePuzzle();
        }

        // If loading fails, the if statement skips solving and the loop
        // continues to the next puzzle file.
    }

    // Return zero to indicate that the program completed normally.
    return 0;
}
