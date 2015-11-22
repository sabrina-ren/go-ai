#include <queue>

// Board constants
const int SIZE = 9;	// Size of board
const int E = 0;	//Empty board space
const int P = 1;	// Player piece
const int C = 2;	// Computer piece

// Move constants
const int INVALID = -1;	// Move is invalid
const int VALID = 0;	// Placed move, nothing happens

// MCTS constants
const int initialDepth = 10;


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

class Component {
public:
	int size;
	int liberties;
	Component(int curSize, int curLiberties) {
		size = curSize;
		liberties = curLiberties;
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
bool isCapture(int(&board)[SIZE][SIZE], bool(&visited)[SIZE][SIZE], Point move, int player) {
	std::queue <Point> checkArea;
	//Point* test;

	visited[move.x][move.y] = true;
	checkArea.push(move);

	while (!checkArea.empty()) {
		Point p = checkArea.front();
		checkArea.pop();

		// Check four neighbours around the point to check
		for (int dx = -1; dx <= 1; dx++) {
			for (int dy = -1; dy <= 1; dy++) {
				if (!(dx == 0) != !(dy == 0)) {
					if (p.x + dx < SIZE && p.x + dx >= 0 && p.y + dy < SIZE && p.y + dy >= 0) {
						Point neighbour = Point(p.x + dx, p.y + dy);

						// An empty space means that pieces have not been captured
						if (board[neighbour.x][neighbour.y] == E) {
							return false;
						}
						if (visited[neighbour.x][neighbour.y] == false && board[p.x][p.y] == player) {
							visited[neighbour.x][neighbour.y] = true;
							checkArea.push(p);
						}
					}
				}
			}
		}
	}

	return true;
}

// Perform a move on the Go Board, checking for validity and capture
int makeMove(int(&board)[SIZE][SIZE], Point move, int player) {
	// Make sure move location isn't currently occupied
	if (board[move.x][move.y] != E) {
		return INVALID;
	}

	// Make sure that move is not a suicide
	bool visited[SIZE][SIZE] = { false };
	board[move.x][move.y] = player;
	if (isCapture(board, visited, move, player)) {
		board[move.x][move.y] = E;
		return INVALID;
	}

	// Make the move and update capture pieces if necessary
	else {
		int opponent;
		if (player == P) { opponent = C; }
		else { opponent = P; }

		reinitializeCheckBoard(visited);

		// Check for an adjacent opponent piece and see if it was part of a capture
		bool result = false;

		for (int dx = -1; dx <= 1; dx++) {
			for (int dy = -1; dy <= 1; dy++) {
				if (!(dx == 0) != !(dy == 0)) {
					if (move.x + dx < SIZE && move.x + dx >= 0 && move.y + dy < SIZE && move.y + dy >= 0) {
						if (board[move.x + dx][move.y + dy] == opponent) {
							result = isCapture(board, visited, Point(move.x + dx, move.y + dy), opponent);
							if (result) {
								removePieces(board, visited);
								reinitializeCheckBoard(visited);
								result = false;
							}
						}
					}
				}
			}
		}
	}
	return VALID;
}


int __stdcall makePlayerMove(int& x, int& y, int& playerScore, int& cpuScore, int *vbaBoard) {
	// Convert VBA 2D array to manageable one
	int board[SIZE][SIZE] = { 0 };
	for (int i = 0; i < SIZE; i++) {
		for (int j = 0; j < SIZE; j++) {
			board[i][j] = valueAt(i, j, vbaBoard);
		}
	}

	int validity = makeMove(board, Point(x, y), P);
	// Return without changing board if move is invalid
	if (validity == INVALID) {
		return INVALID;
	}

	// Update board if necessary and return
	for (int i = 0; i < SIZE; i++) {
		for (int j = 0; j < SIZE; j++) {
			writeAt(i, j, vbaBoard, board[i][j]);
		}
	}

	return VALID;
}

void score(int& playerScore, int& computerScore, int(&board)[SIZE][SIZE]) {
	bool visited[SIZE][SIZE] = { false };

	for (int i = 0; i < SIZE; i++) {
		for (int j = 0; j < SIZE; j++) {
			if (visited[i][j] == false) {
				visited[i][j] = true;
				if (board[i][j] == P) {
					playerScore++;
				}
				else if (board[i][j] == C) {
					computerScore++;
				}
				else {
					// If empty point, look around to see what it's surrounded by
					std::queue <Point> checkArea;
					checkArea.push(Point(i, j));

					bool isNeutralArea = false;
					int surroundingNeighbour = 0;
					int emptyArea = 0;

					while (!checkArea.empty()) {
						Point p = checkArea.front();
						checkArea.pop();
						emptyArea++;

						// Check its four neighbours
						for (int dx = -1; dx <= 1; dx++) {
							for (int dy = -1; dy <= 1; dy++) {
								if (!(dx == 0) != !(dy == 0)) {
									if (p.x + dx < SIZE && p.x + dx >= 0 && p.y + dy < SIZE && p.y + dy >= 0) {
										Point neighbour = Point(p.x + dx, p.y + dy);
										int neighbourValue = board[neighbour.x][neighbour.y];
										
										if (neighbourValue == E) {
											// If empty and unvisited, search around that empty spot as well
											if (!visited[neighbour.x][neighbour.y]) {
												visited[neighbour.x][neighbour.y] = true;
												checkArea.push(neighbour);
											}
										}
										else if (surroundingNeighbour == 0) {
											// If first non-empty neighbour, make this the surrounding neighbour to compare to
											surroundingNeighbour = neighbourValue;
										}
										else if (surroundingNeighbour != neighbourValue) {
											// If this neighbour is not the same, this is a neutral area and we should stahp
											isNeutralArea = true;
										}
									}
								}
							}
						}
					}

					if (!isNeutralArea) {
						// The empty area belongs to the surroundingNeighbour
						if (surroundingNeighbour == P) {
							playerScore += emptyArea;
						}
						else if (surroundingNeighbour == C) {
							computerScore += emptyArea;
						}
					}
				}
			}

		}
	}
}

int simulateToEnd(int(&board)[SIZE][SIZE]) {
	int x = rand() % SIZE;
	int y = rand() % SIZE;

	int player = C;
	for (int i = 0; i < initialDepth; i++) {
		if (i % 2) {
			player = C;
		}
		else {
			player = P;
		}
		int validity = makeMove(board, Point(x, y), player);
	}
	int playerScore = 0;
	int computerScore = 0;
	score(playerScore, computerScore, board);

	return 0;
}

int __stdcall makeComputerMove(int& x, int& y, int& playerScore, int& computerScore, int *vbaBoard) {
	// Convert VBA 2D array to manageable one
	int board[SIZE][SIZE] = { 0 };
	for (int i = 0; i < SIZE; i++) {
		for (int j = 0; j < SIZE; j++) {
			board[i][j] = valueAt(i, j, vbaBoard);
		}
	}
	score(playerScore, computerScore, board);
	//int win = simulateToEnd(board);
	return 0;
}