// Board constants
const int SIZE = 9;	// Size of board
const int P = 1;	// Player piece
const int C = 2;	// Computer piece

// Move constants
const int INVALID = -1;	// Move is invalid
const int VALID = 0;	// Placed move, nothing happens

//! How do classes werk with DLLs and VBA? AH!

/*
	Helper methods
*/
int valueAt(int row, int col, int *board) {
	return *(board + row + SIZE * col);
}

void writeAt(int row, int col, int *board, int value) {
	*(board + row + SIZE * col) = value;
}
	
/* 
	Returns true if valid, false if invalid
*/
bool validateMove(int x, int y, int *board, bool isComputer) {

	if (valueAt(x, y, board) != 0) {
		// Can only place piece at empty point
		return false;
	} else {
		// Check if it is an eye (surrounded by opponent or wall on 4 sides)

		int opponent = 2;
		if (isComputer) {
			opponent = 1;
		}

		if (x > 0 && valueAt(x - 1, y, board) != opponent) {
			return true;
		}
		else if (y > 0 && valueAt(x, y - 1, board) != opponent) {
			return true;
		}
		else if (x < SIZE - 1 && valueAt(x + 1, y, board) != opponent) {
			return true;
		}
		else if (y < SIZE - 1 && valueAt(x, y + 1, board) != opponent) {
			return true;
		}
		return false;
	}
}

/*
	Adds piece to the passed board and handles eaten opponents.
	Move should already be validated.
*/
int makeMove(int& x, int& y, int& numPiecesEaten, int *board, bool isComputer) {
	if (!validateMove(x, y, board, isComputer)) {
		return INVALID;
	}

	// TODO: this just sets the value to what it currently is / does nothing right now
	for (int i = 0; i < 9; i++) {
		for (int j = 0; j < 9; j++) {
			writeAt(i, j, board, valueAt(i, j, board));
		}
	}

	// Pretend we ate things. Use negative # if we were eaten.
	numPiecesEaten = 6;
	return VALID;
}

int __stdcall makePlayerMove(int& x, int& y, int& numPiecesEaten, int *board) {
	return makeMove(x, y, numPiecesEaten, board, false);
}