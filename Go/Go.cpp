#include <queue>
#include <map>
#include <stack>

// Board constants
const int SIZE = 9;	// Size of board
const int E = 0;	//Empty board space
const int B = 1;	// Black piece
const int W = 2;	// White piece

					// Move constants
const int INVALID = -1;	// Move is invalid
const int VALID = 0;	// Placed move, nothing happens

						// MCTS constants
const int initialDepth = 10;
const double LOSE_THRESHOLD = 0.05;
const int INVALID_THRESHOLD = SIZE*SIZE;
const int MOVE = 1;
const int PASS = 0;
const int WIN = 1;
const int LOSS = 0;

/*
Helper methods and classes
*/
int getOpponent(int current) {
	if (current == B) { return W; }
	return B;
}

class Point {
public:
	int x;
	int y;
	Point(int a, int b) {
		x = a;
		y = b;
	}
};

class TreeNode {
public:
	int x;
	int y;
	double winPercent;
	int moveColour;
	TreeNode *parent;
	TreeNode(int a, int b, int move) {
		x = a;
		y = b;
		parent = NULL;
		moveColour = move;
	}
	TreeNode(int a, int b, TreeNode nodeParent) {
		x = a;
		y = b;
		parent = &nodeParent;
		moveColour = getOpponent(nodeParent.moveColour);
	}
};
//Node comparator for Priority Queue '>' used because < is default but we want opposite
bool operator<(const TreeNode& a, const TreeNode& b) {
	return a.winPercent > b.winPercent;
}

//not having this causes a compile error??
bool operator<(const Point& a, const Point& b) {
	return a.x < b.x;
}

// Not used yet
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

int valueAt(int col, int row, int *board) {
	return *(board + row + SIZE * col);
}

void writeAt(int col, int row, int *board, int value) {
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
int makeMove(int(&board)[SIZE][SIZE], Point move, int player) {
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
		int opponent = getOpponent(player);

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

/*
Scores board based on number of stones on the board and captured area.
*/
void score(int& blackScore, int& whiteScore, int(&board)[SIZE][SIZE]) {
	blackScore = 0;
	whiteScore = 0;

	bool visited[SIZE][SIZE] = { false };

	for (int i = 0; i < SIZE; i++) {
		for (int j = 0; j < SIZE; j++) {
			if (visited[i][j] == false) {
				visited[i][j] = true;
				if (board[i][j] == B) {
					blackScore++;
				}
				else if (board[i][j] == W) {
					whiteScore++;
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
						if (surroundingNeighbour == B) {
							blackScore += emptyArea;
						}
						else if (surroundingNeighbour == W) {
							whiteScore += emptyArea;
						}
					}
				}
			}

		}
	}
}

/*
Makes a move on the board in the specified colour.
Modifies the board and scores for each colour.
*/
int __stdcall makePlayerMove(int& x, int& y, int& blackScore, int& whiteScore, int& colour, int *vbaBoard) {
	// Convert VBA 2D array to manageable one
	int board[SIZE][SIZE] = { 0 };
	for (int i = 0; i < SIZE; i++) {
		for (int j = 0; j < SIZE; j++) {
			board[i][j] = valueAt(i, j, vbaBoard);
		}
	}

	int validity = makeMove(board, Point(x, y), colour);
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
	score(blackScore, whiteScore, board);

	return VALID;
}

void genInitialBoard(int(&board)[SIZE][SIZE], int(&newBoard)[SIZE][SIZE]) {
	for (int i = 0; i < SIZE; i++) {
		for (int j = 0; j < SIZE; j++) {
			newBoard[i][j] = board[i][j];
		}
	}
}

//we can add a valid move board checker if we need to make this more efficient
int simulateOneGame(int(&board)[SIZE][SIZE], int colour, int nextMoveColour) {
	int invalidCount = 0;
	int newY;
	int newX;
	while (invalidCount < INVALID_THRESHOLD) {
		newY = rand() % SIZE;
		newX = rand() % SIZE;
		int result = makeMove(board, Point(newX, newY), nextMoveColour);
		if (result == INVALID) { invalidCount++; }
		else {
			nextMoveColour = getOpponent(nextMoveColour);
			invalidCount = 0;
		}
	}

	int blackScore, whiteScore;
	score(blackScore, whiteScore, board);
	if ((blackScore >= whiteScore && colour == B) || (whiteScore >= blackScore && colour == W)) {
		return WIN;
	}
	else {
		return LOSS;
	}
}

void roundFunction(int(&board)[SIZE][SIZE], std::priority_queue<TreeNode> &treeQueue, int colour, int round, int simNum, int eCount) {

	int newX = rand() % SIZE;
	int newY = rand() % SIZE;
	TreeNode highestWin = treeQueue.top();
	treeQueue.pop();

	std::map<Point, bool> visitedMap;
	for (int i = 0; i < ((1.1 - (0.1*round))*eCount); i++) {
		while (visitedMap[Point(newX, newY)] || board[newX][newY] != E || (newX == highestWin.x && newY == highestWin.y)) {
			newX = rand() % SIZE;
			newY = rand() % SIZE;
		}
		visitedMap[Point(newX, newY)] = true;
		TreeNode newTree(newX, newY, highestWin);
		TreeNode currentNode = newTree;

		for (int i = 0; i < simNum; i++) {
			std::stack<TreeNode> parentStack;

			int newBoard[SIZE][SIZE];
			genInitialBoard(board, newBoard);

			while (currentNode.parent != NULL) {
				parentStack.push(*currentNode.parent);
				currentNode = *currentNode.parent;
			}
			while (!parentStack.empty()) {
				currentNode = parentStack.top();
				parentStack.pop();
				makeMove(newBoard, Point(currentNode.x, currentNode.y), currentNode.moveColour);
			}

			newTree.winPercent += simulateOneGame(newBoard, colour, newTree.moveColour);
		}
		newTree.winPercent = newTree.winPercent / simNum;
		treeQueue.push(newTree);
	}
}

int simulateAll(int(&board)[SIZE][SIZE], int& colour) {
	std::priority_queue<TreeNode> treeQueue;

	//count number of empty spaces to determine number of simulations per move
	int eCount = 0;
	for (int i = 0; i < SIZE; i++) {
		for (int j = 0; j < SIZE; j++) {
			if (board[i][j] == E) {
				eCount++;
			}
		}
	}
	int simNum = 10 + ((SIZE*SIZE) - eCount);

	//first round of simulations
	for (int i = 0; i < SIZE; i++) {
		for (int j = 0; j < SIZE; j++) {
			if (board[i][j] == E) {
				TreeNode newTree = TreeNode(i, j, colour);

				//starts at 10 simulations but grows with fewer moves
				for (int i = 0; i < simNum; i++) {
					int newBoard[SIZE][SIZE];
					genInitialBoard(board, newBoard);
					int result = makeMove(newBoard, Point(i, j), colour);

					if (result == VALID) {
						//adds 1 if it wins, 0 if not
						newTree.winPercent += simulateOneGame(newBoard, colour, getOpponent(colour));
					}
				}
				//gets actual win percent value and adds to priority queue
				newTree.winPercent = newTree.winPercent / simNum;
				treeQueue.push(newTree);
			}
		}
	}

	//conduct rounds 2 through 10 (or less if there isn't enough spaces)
	for (int i = 2; i < 10 && i < eCount; i++) {

		roundFunction(board, treeQueue, colour, i, simNum, eCount);
		//TODO: i don't know what you are supposed to do between rounds so that's not incorporated yet
	}

	TreeNode winner = treeQueue.top();
	if (winner.winPercent < LOSE_THRESHOLD) {
		return PASS;
	}
	else {
		while (winner.parent != NULL) {
			winner = *winner.parent;
		}
		makeMove(board, Point(winner.x, winner.y), winner.moveColour);
		return MOVE;
	}

}

/*
Determine computer's next best move. Will take a while to run.
*/
int __stdcall determineComputerMove(int& blackScore, int& whiteScore, int& colour, int *vbaBoard) {
	// Convert VBA 2D array to manageable one
	int board[SIZE][SIZE] = { 0 };
	for (int i = 0; i < SIZE; i++) {
		for (int j = 0; j < SIZE; j++) {
			board[i][j] = valueAt(i, j, vbaBoard);
		}
	}

	int result = simulateAll(board, colour);

	for (int i = 0; i < SIZE; i++) {
		for (int j = 0; j < SIZE; j++) {
			writeAt(i, j, vbaBoard, board[i][j]);
		}
	}
	score(blackScore, whiteScore, board);
	return result;
}