#include <queue>

// Board constants
const int SIZE = 9;	// Size of board
const int P = 1;	// Player piece
const int C = 2;	// Computer piece
const int E = 0;	//emprt board space

// Move constants
const int INVALID = -1;	// Move is invalid
const int VALID = 0;	// Placed move, nothing happens

//! How do classes werk with DLLs and VBA? AH!

/*
	Helper methods and classes
*/
class Point {
public:
	int x;
	int y;
	Point(int a, int b) {
		x = a;
		y = b;
	}
};

void removePieces(int(&board)[SIZE][SIZE], bool(&checkBoard)[SIZE][SIZE]) {
	for (int i = 0; i < SIZE; i++) {
		for (int j = 0; j < SIZE; j++) {
			if (checkBoard[i][j] == true) {
				board[i][j] = E;
			}
		}
	}
}

void reinitializeCheckBoard(bool(&checkBoard)[SIZE][SIZE]) {
	for (int i = 0; i < SIZE; i++) {
		for (int j = 0; j < SIZE; j++) {
			checkBoard[i][j] = false;
		}
	}
}

int valueAt(int row, int col, int *board) {
	return *(board + row + SIZE * col);
}

void writeAt(int row, int col, int *board, int value) {
	*(board + row + SIZE * col) = value;
}
	
/* 
	Returns true if valid, false if invalid
*/
bool isCapture(int(&board)[SIZE][SIZE], bool(&checkBoard)[SIZE][SIZE], Point move, int player) {
	std::queue <Point> checkArea;
	Point* test;

	checkBoard[move.x][move.y] = true;
	checkArea.push(move);

	while (!checkArea.empty()) {
		Point next = checkArea.front();
		checkArea.pop();

		//check pos above
		if (next.x != 0) {
			test = new Point(next.x - 1, next.y);
			//an empty space means the pieces haven't been captured
			if (board[test->x][test->y] == E) {
				delete test;
				return false;
			}

			if (checkBoard[test->x][test->y] == false && board[test->x][test->y] == player) {
				checkBoard[test->x][test->y] = true;
				checkArea.push(*test);
			}
			delete test;
		}
		//check pos below
		if (next.x != 8) {
			test = new Point(next.x + 1, next.y);
			//an empty space means the pieces haven't been captured
			if (board[test->x][test->y] == E) {
				delete test;
				return false;
			}

			if (checkBoard[test->x][test->y] == false && board[test->x][test->y] == player) {
				checkBoard[test->x][test->y] = true;
				checkArea.push(*test);
			}
			delete test;
		}
		//check pos left
		if (next.y != 0) {
			test = new Point(next.x, next.y - 1);
			//an empty space means the pieces haven't been captured
			if (board[test->x][test->y] == E) {
				delete test;
				return false;
			}

			if (checkBoard[test->x][test->y] == false && board[test->x][test->y] == player) {
				checkBoard[test->x][test->y] = true;
				checkArea.push(*test);
			}
			delete test;
		}
		//check pos right
		if (next.y != 8) {
			test = new Point(next.x, next.y + 1);
			//an empty space means the pieces haven't been captured
			if (board[test->x][test->y] == E) {
				delete test;
				return false;
			}

			if (checkBoard[test->x][test->y] == false && board[test->x][test->y] == player) {
				checkBoard[test->x][test->y] = true;
				checkArea.push(*test);
			}
			delete test;
		}
	}

	return true;
}

//Perform a move on the GO Board, checking for validity and capture
int moveFunction(int(&board)[SIZE][SIZE], Point move, int player) {
	//make sure move location isn't currently occupied
	bool checkBoard[SIZE][SIZE] = { false };
	if (board[move.x][move.y] != E) {
		return INVALID;
	}

	//make sure that move is not a suicide
	board[move.x][move.y] = player;
	if (isCapture(board, checkBoard, move, player)) {
		board[move.x][move.y] = E;
		return INVALID;
	}
	//make the move and update capture pieces if necessary
	else {
		int opponent;
		if (player = P) { opponent = C; }
		else { opponent = P; }

		reinitializeCheckBoard(checkBoard);

		//check for an adjacent opponent piece and see if it was part of a capture
		bool result = false;
		//check pos above
		if (move.x != 0) {
			if (board[move.x - 1][move.y] == opponent) {
				result = isCapture(board, checkBoard, Point(move.x - 1, move.y), opponent);
				if (result) {
					removePieces(board, checkBoard);
					reinitializeCheckBoard(checkBoard);
					result = false;
				}
			}
		}
		//check pos below
		if (move.x != 8) {
			if (board[move.x + 1][move.y] == opponent) {
				result = isCapture(board, checkBoard, Point(move.x + 1, move.y), opponent);
				if (result) {
					removePieces(board, checkBoard);
					reinitializeCheckBoard(checkBoard);
					result = false;
				}
			}
		}
		//check pos left
		if (move.y != 0) {
			if (board[move.x][move.y - 1] == opponent) {
				result = isCapture(board, checkBoard, Point(move.x, move.y - 1), opponent);
				if (result) {
					removePieces(board, checkBoard);
					reinitializeCheckBoard(checkBoard);
					result = false;
				}
			}
		}
		//check pos right
		if (move.y != 8) {
			if (board[move.x][move.y + 1] == opponent) {
				result = isCapture(board, checkBoard, Point(move.x, move.y + 1), opponent);
				if (result) {
					removePieces(board, checkBoard);
				}
			}
		}

	}
	return VALID;
}


int __stdcall makePlayerMove(int& x, int& y, int& playerScore, int& cpuScore, int *board) {
	//convert vba board into a C++ one
	int newBoard[SIZE][SIZE] = { 0 };
	for (int i = 0; i < SIZE; i++) {
		for (int j = 0; j < SIZE; j++) {
			newBoard[i][j] = valueAt(i, j, board);
		}
	}

	int result = moveFunction(newBoard, Point(x, y), P);
	//return without changing board if move is invalid
	if (result == INVALID) { return INVALID; }

	//update board if necessary and return
	for (int i = 0; i < SIZE; i++) {
		for (int j = 0; j < SIZE; j++) {
			writeAt(i, j, board, newBoard[i][j]);
		}
	}
	return result;
}