#include<iostream>
#include<ctime>
#include<SFML/Graphics.hpp>
#include<chrono>
#include<thread>
#include<vector>

#define ROWS 20
#define COLOUMNS 10
#define BLOCK_SIZE 40
#define SPACING_FACTOR 2
#define FRAME_RATE 60
#define DT 0.16

using namespace std;
using namespace sf;
using namespace std::chrono;

// USE A GRID-LIKE APPROACH
// WAY MORE EFFICIENT HONESTLY

enum Piece {I, O, S, Z, L, J, T, NONE};
enum State {ACTIVE, PLACED};

class Block {
private:
	RectangleShape shape;
	Piece piece;
	State state;

public:
	Block() {
		shape.setSize(Vector2f(BLOCK_SIZE, BLOCK_SIZE));
		shape.setFillColor(Color::Color(61, 61, 61));
		piece = NONE;
		state = ACTIVE;
	}

	void setPosition(float x, float y) {
		shape.setPosition(Vector2f(x, y));
	}

	void setPieceState(Piece piece) {
		this->piece = piece;
	}

	void setBlockState(State state) {
		this->state = state;
	}

	void render(RenderWindow* window) {
		window->draw(shape);
	}
	
	void updateColor() {
		switch (piece) {
		case NONE:
			shape.setFillColor(Color::Color(61, 61, 61));
			break;
		
		case O:
			shape.setFillColor(Color::Yellow);
			break;

		case I:
			shape.setFillColor(Color::Cyan);
			break;

		case S:
			shape.setFillColor(Color::Red);
			break;

		case Z:
			shape.setFillColor(Color::Green);
			break;

		case L:
			shape.setFillColor(Color::Color(252, 148, 3));
			break;

		case J:
			shape.setFillColor(Color::Blue);
			break;

		case T:
			shape.setFillColor(Color::Color(252, 3, 252));
			break;
		}
	}

	int getSize_X() {
		return shape.getSize().x;
	}

	int getSize_Y() {
		return shape.getSize().y;
	}

	State getBlockState() {
		return state;
	}

	Piece getPieceState() {
		return piece;
	}
};

class Game {
private:
	Piece piece;
	Block grid[ROWS][COLOUMNS + 2];
	int pos_x, pos_y;
	int pieceLeft, pieceRight, pieceBottom;
	int rotation, previous_rotation;

public:
	Game() {
		for (int i = 0; i < ROWS; i++) {
			for (int j = 0; j < COLOUMNS; j++) {
				grid[i][j].setPosition((BLOCK_SIZE + SPACING_FACTOR) * j, (BLOCK_SIZE + SPACING_FACTOR) * i);
			}
		}
	}

	void selectPiece() {
		int pick = rand()%7;
		switch (pick) {
		case 0:
			piece = I;

			pos_x = 3;
			pos_y = 0;

			pieceLeft = pos_x;
			pieceRight = pos_x + 3;
			pieceBottom = pos_y;
			break;

		case 1:
			piece = O;

			pos_x = 4;
			pos_y = -1;

			pieceLeft = pos_x;
			pieceRight = pos_x + 1;
			pieceBottom = pos_y + 1;
			break;

		case 2:
			piece = S;

			pos_x = 4;
			pos_y = -1;

			pieceLeft = pos_x - 1;
			pieceRight = pos_x + 1;
			pieceBottom = pos_y + 1;
			break;

		case 3:
			piece = Z;

			pos_x = 5;
			pos_y = -1;

			pieceLeft = pos_x - 1;
			pieceRight = pos_x + 1;
			pieceBottom = pos_y + 1;
			break;

		case 4:
			piece = L;

			pos_x = 4;
			pos_y = -2;

			pieceLeft = pos_x;
			pieceRight = pos_x + 1;
			pieceBottom = pos_y + 2;
			break;

		case 5:
			piece = J;

			pos_x = 5;
			pos_y = -2;

			pieceLeft = pos_x - 1;
			pieceRight = pos_x;
			pieceBottom = pos_y + 2;
			break;

		case 6:
			piece = T;

			pos_x = 4;
			pos_y = -1;

			pieceLeft = pos_x - 1;
			pieceRight = pos_x + 1;
			pieceBottom = pos_y + 1;
			break;
		}
		rotation = 0;
		previous_rotation = 0;
	}

	void move_X(int x) {
		static int iterator = 0;
		if (iterator > (FRAME_RATE / 6) * DT) {
			iterator = 0;
			pos_x += x;
			pieceLeft += x;
			pieceRight += x;
		}
		else {
			iterator++;
		}
	}

	void move_Y(int y) {
		static int iterator = 0;
		if (iterator > (FRAME_RATE / 2) * DT) {
			iterator = 0;
			pos_y += y;
			pieceBottom += y;
		}
		else {
			iterator++;
		}
	}

	bool gravity() {
		static int iterator = 0;
		if (iterator > FRAME_RATE * DT * 2) {
			iterator = 0;
			if (pieceBottom < ROWS - 1) {
				if (!check_Y()) {
					pos_y++;
					pieceBottom++;
					return 1;
				}
				else {
					return 0;
				}
			}
			else {
				return 0;
			}
		}
		else {
			iterator++;
		}
	}

	bool check_Y() {
		switch (piece) {
		case I:
			if (rotation == 0 || rotation == 3) {
				for (int j = 0; j < 4; j++) {
					if (grid[pos_y + 1][pos_x + j].getBlockState() == PLACED) {
						return 1;
					}
				}
			}
			else if (rotation == 1 || rotation == 2) {
				if (grid[pos_y + 4][pos_x].getBlockState() == PLACED) {
					return 1;
				}
			}
			break;

		case O:
			for (int i = 0; i < 2; i++) {
				if (grid[pos_y + 2][pos_x + i].getBlockState() == PLACED)
					return 1;
			}
			break;

		case S:
			if (rotation == 0 || rotation == 3) {
				if (grid[pos_y + 2][pos_x - 1].getBlockState() == PLACED ||
					grid[pos_y + 2][pos_x - 0].getBlockState() == PLACED ||
					grid[pos_y + 1][pos_x + 1].getBlockState() == PLACED)
					return 1;
			}
			else if (rotation == 1 || rotation == 2) {
				if (grid[pieceBottom + 1][pos_x + 1].getBlockState() == PLACED ||
					grid[pieceBottom][pos_x].getBlockState() == PLACED)
					return 1;
			}
			break;

		case Z:
			if (rotation == 0 || rotation == 3) {
				if (grid[pos_y + 2][pos_x + 1].getBlockState() == PLACED ||
					grid[pos_y + 2][pos_x - 0].getBlockState() == PLACED ||
					grid[pos_y + 1][pos_x - 1].getBlockState() == PLACED)
					return 1;
			}
			else if (rotation == 1 || rotation == 2) {
				if (grid[pieceBottom + 1][pos_x - 1].getBlockState() == PLACED ||
					grid[pieceBottom][pos_x].getBlockState() == PLACED)
					return 1;
			}
			break;

		case L:
			if (rotation == 0) {
				if (grid[pos_y + 3][pos_x].getBlockState() == PLACED ||
					grid[pos_y + 3][pos_x + 1].getBlockState() == PLACED) {
					return 1;
				}
			}
			else if (rotation == 1) {
				if (grid[pos_y + 1][pos_x + 1].getBlockState() == PLACED ||
					grid[pos_y + 1][pos_x + 2].getBlockState() == PLACED ||
					grid[pos_y + 2][pos_x].getBlockState() == PLACED)
					return 1;
			}
			else if (rotation == 2) {
				if (grid[pos_y + 2][pos_x].getBlockState() == PLACED ||
					grid[pos_y + 2][pos_x - 1].getBlockState() == PLACED ||
					grid[pos_y + 2][pos_x - 2].getBlockState() == PLACED)
					return 1;
			}
			else if (rotation == 3) {
				if (grid[pos_y + 1][pos_x].getBlockState() == PLACED ||
					grid[pos_y + 3][pos_x + 1].getBlockState() == PLACED)
					return 1;
			}
			break;

		case J:
			if (rotation == 0) {
				if (grid[pos_y + 3][pos_x].getBlockState() == PLACED ||
					grid[pos_y + 3][pos_x - 1].getBlockState() == PLACED) {
					return 1;
				}
			}
			else if (rotation == 1) {
				if (grid[pos_y + 2][pos_x].getBlockState() == PLACED ||
					grid[pos_y + 2][pos_x + 1].getBlockState() == PLACED ||
					grid[pos_y + 2][pos_x + 2].getBlockState() == PLACED)
					return 1;
			}
			else if (rotation == 2) {
				if (grid[pos_y + 1][pos_x].getBlockState() == PLACED ||
					grid[pos_y + 1][pos_x + 1].getBlockState() == PLACED ||
					grid[pos_y + 2][pos_x + 2].getBlockState() == PLACED)
					return 1;
			}
			else if (rotation == 3) {
				if (grid[pos_y][pos_x + 1].getBlockState() == PLACED ||
					grid[pos_y + 3][pos_x].getBlockState() == PLACED)
					return 1;
			}
			break;

		case T:
			if (rotation == 0) {
				if (grid[pos_y + 2][pos_x].getBlockState() == PLACED ||
					grid[pos_y + 2][pos_x - 1].getBlockState() == PLACED ||
					grid[pos_y + 2][pos_x + 1].getBlockState() == PLACED)
					return 1;
			}
			else if (rotation == 1) {
				if (grid[pieceBottom + 1][pos_x].getBlockState() == PLACED ||
					grid[pos_y + 2][pos_x + 1].getBlockState() == PLACED)
					return 1;
			}
			else if (rotation == 2) {
				if (grid[pieceBottom][pos_x].getBlockState() == PLACED ||
					grid[pos_y + 2][pos_x - 1].getBlockState() == PLACED)
					return 1;
			}
			else if (rotation == 3) {
				if (grid[pos_y][pos_x + 1].getBlockState() == PLACED ||
					grid[pos_y + 1][pieceBottom + 1].getBlockState() == PLACED ||
					grid[pos_y + 2][pos_x + 1].getBlockState() == PLACED)
					return 1;
			}
			break;
		}
		return 0;
	}

	bool check_X_Left() {
		switch (piece) {
		case I:
			if (rotation == 0 || rotation == 3) {
				if (grid[pos_y][pieceLeft - 1].getBlockState() == PLACED) {
					return 1;
				}
			}
			else if (rotation == 1 || rotation == 2) {
				for (int i = 0; i < 4; i++) {
					if (grid[pos_y + i][pieceLeft - 1].getBlockState() == PLACED) {
						return 1;
					}
				}
			}
			break;

		case O:
			if (grid[pos_y][pieceLeft - 1].getBlockState() == PLACED ||
				grid[pos_y + 1][pieceLeft - 1].getBlockState() == PLACED)
				return 1;
			break;

		case S:
			if (rotation == 0 || rotation == 3) {
				if (grid[pos_y][pos_x - 1].getBlockState() == PLACED ||
					grid[pos_y + 1][pieceLeft - 1].getBlockState() == PLACED)
					return 1;
			}
			else if (rotation == 1 || rotation == 2) {
				if (grid[pos_y][pieceLeft - 1].getBlockState() == PLACED ||
					grid[pos_y + 1][pieceLeft - 1].getBlockState() == PLACED ||
					grid[pos_y + 2][pos_x].getBlockState() == PLACED)
					return 1;
			}
			break;

		case Z:
			if (rotation == 0 || rotation == 3) {
				if (grid[pos_y][pieceLeft - 1].getBlockState() == PLACED ||
					grid[pos_y + 1][pos_x - 1].getBlockState() == PLACED)
					return 1;
			}
			else if (rotation == 1 || rotation == 2) {
				if (grid[pos_y][pos_x - 1].getBlockState() == PLACED ||
					grid[pos_y + 1][pieceLeft - 1].getBlockState() == PLACED ||
					grid[pos_y + 2][pieceLeft - 1].getBlockState() == PLACED)
					return 1;
			}
			break;

		case L:
			if (rotation == 0) {
				if (grid[pos_y][pieceLeft - 1].getBlockState() == PLACED ||
					grid[pos_y + 1][pieceLeft - 1].getBlockState() == PLACED ||
					grid[pos_y + 2][pieceLeft - 1].getBlockState() == PLACED)
					return 1;
			}
			else if (rotation == 1) {
				if (grid[pos_y][pieceLeft - 1].getBlockState() == PLACED ||
					grid[pos_y + 1][pieceLeft - 1].getBlockState() == PLACED)
					return 1;
			}
			else if (rotation == 2) {
				if (grid[pos_y + 1][pieceLeft - 1].getBlockState() == PLACED ||
					grid[pos_y][pos_x - 1].getBlockState() == PLACED)
					return 1;
			}
			else if (rotation == 3) {
				if (grid[pos_y][pieceLeft - 1].getBlockState() == PLACED ||
					grid[pos_y + 1][pos_x].getBlockState() == PLACED ||
					grid[pos_y + 2][pos_x].getBlockState() == PLACED)
					return 1;
			}
			break;

		case J:
			if (rotation == 0) {
				if (grid[pos_y + 2][pieceLeft - 1].getBlockState() == PLACED ||
					grid[pos_y][pos_x - 1].getBlockState() == PLACED ||
					grid[pos_y + 1][pos_x - 1].getBlockState() == PLACED)
					return 1;
			}
			else if (rotation == 1) {
				if (grid[pos_y][pos_x - 1].getBlockState() == PLACED ||
					grid[pos_y + 1][pos_x - 1].getBlockState() == PLACED)
					return 1;
			}
			else if (rotation == 2) {
				if (grid[pos_y][pos_x - 1].getBlockState() == PLACED ||
					grid[pos_y + 1][pos_x + 1].getBlockState() == PLACED)
					return 1;
			}
			else if (rotation == 3) {
				if (grid[pos_y][pos_x - 1].getBlockState() == PLACED ||
					grid[pos_y + 1][pos_x - 1].getBlockState() == PLACED ||
					grid[pos_y + 2][pos_x - 1].getBlockState() == PLACED)
					return 1;
			}
			break;

		case T:
			if (rotation == 0) {
				if (grid[pos_y][pos_x - 1].getBlockState() == PLACED ||
					grid[pos_y + 1][pieceLeft - 1].getBlockState() == PLACED)
					return 1;
			}
			else if (rotation == 1) {
				if (grid[pos_y][pos_x - 1].getBlockState() == PLACED ||
					grid[pos_y + 1][pos_x - 1].getBlockState() == PLACED ||
					grid[pos_y + 2][pos_x - 1].getBlockState() == PLACED)
					return 1;
			}
			else if (rotation == 2) {
				if (grid[pos_y][pos_x - 1].getBlockState() == PLACED ||
					grid[pos_y + 1][pos_x - 2].getBlockState() == PLACED ||
					grid[pos_y + 2][pos_x - 1].getBlockState() == PLACED)
					return 1;
			}
			else if (rotation == 3) {
				if (grid[pos_y][pieceLeft - 1].getBlockState() == PLACED ||
					grid[pos_y + 1][pos_x].getBlockState() == PLACED)
					return 1;
			}
			break;
		}
		return 0;
	}

	bool check_X_Right() {
		switch (piece) {
		case I:
			if (rotation == 0 || rotation == 3) {
				if (grid[pos_y][pieceRight + 1].getBlockState() == PLACED) {
					return 1;
				}
			}
			else if (rotation == 1 || rotation == 2) {
				for (int i = 0; i < 4; i++) {
					if (grid[pos_y + i][pieceRight + 1].getBlockState() == PLACED) {
						return 1;
					}
				}
			}
			break;

		case O:
			if (grid[pos_y][pieceRight + 1].getBlockState() == PLACED ||
				grid[pos_y + 1][pieceRight + 1].getBlockState() == PLACED)
				return 1;
			break;

		case S:
			if (rotation == 0 || rotation == 3) {
				if (grid[pos_y][pieceRight + 1].getBlockState() == PLACED ||
					grid[pos_y + 1][pos_x + 1].getBlockState() == PLACED)
					return 1;
			}
			else if (rotation == 1 || rotation == 2) {
				if (grid[pos_y + 1][pieceRight + 1].getBlockState() == PLACED ||
					grid[pos_y + 2][pieceRight + 1].getBlockState() == PLACED ||
					grid[pos_y][pos_x + 1].getBlockState() == PLACED)
					return 1;
			}
			break;

		case Z:
			if (rotation == 0 || rotation == 3) {
				if (grid[pos_y][pos_x + 1].getBlockState() == PLACED ||
					grid[pos_y + 1][pieceRight + 1].getBlockState() == PLACED)
					return 1;
			}
			else if (rotation == 1 || rotation == 2) {
				if (grid[pos_y][pieceRight + 1].getBlockState() == PLACED ||
					grid[pos_y + 1][pieceRight + 1].getBlockState() == PLACED ||
					grid[pos_y + 2][pos_x].getBlockState() == PLACED)
					return 1;
			}
			break;

		case L:
			if (rotation == 0) {
				if (grid[pos_y][pos_x + 1].getBlockState() == PLACED ||
					grid[pos_y + 1][pos_x + 1].getBlockState() == PLACED ||
					grid[pos_y + 2][pieceRight + 1].getBlockState() == PLACED)
					return 1;
			}
			else if (rotation == 1) {
				if (grid[pos_y][pieceRight + 1].getBlockState() == PLACED ||
					grid[pos_y + 1][pos_x + 1].getBlockState() == PLACED)
					return 1;
			}
			else if (rotation == 2) {
				if (grid[pos_y][pieceRight + 1].getBlockState() == PLACED ||
					grid[pos_y + 1][pieceRight + 1].getBlockState() == PLACED)
					return 1;
			}
			else if (rotation == 3) {
				if (grid[pos_y][pos_x + 2].getBlockState() == PLACED ||
					grid[pos_y + 1][pos_x + 2].getBlockState() == PLACED ||
					grid[pos_y + 2][pos_x + 2].getBlockState() == PLACED)
					return 1;
			}
			break;

		case J:
			if (grid[pos_y][pieceRight + 1].getBlockState() == PLACED ||
				grid[pos_y + 1][pieceRight + 1].getBlockState() == PLACED ||
				grid[pos_y + 2][pieceRight + 1].getBlockState() == PLACED)
				return 1;
			break;

		case T:
			if (rotation == 0) {
				if (grid[pos_y][pos_x + 1].getBlockState() == PLACED ||
					grid[pos_y + 1][pieceRight + 1].getBlockState() == PLACED)
					return 1;
			}
			else if (rotation == 1) {
				if (grid[pos_y][pos_x + 1].getBlockState() == PLACED ||
					grid[pos_y + 1][pos_x + 2].getBlockState() == PLACED ||
					grid[pos_y + 2][pos_x + 1].getBlockState() == PLACED)
					return 1;
			}
			else if (rotation == 2) {
				if (grid[pos_y][pos_x + 1].getBlockState() == PLACED ||
					grid[pos_y + 1][pos_x + 1].getBlockState() == PLACED ||
					grid[pos_y + 2][pos_x + 1].getBlockState() == PLACED)
					return 1;
			}
			else if (rotation == 3) {
				if (grid[pos_y][pieceRight + 1].getBlockState() == PLACED ||
					grid[pos_y + 1][pos_x + 2].getBlockState() == PLACED)
					return 1;
			}
			break;
		}
		return 0;

	}

	void placePiece() {
		switch (piece) {
		case O:
			for (int i = 0; i < 2; i++) {
				for (int j = 0; j < 2; j++) {
					grid[pos_y + i][pos_x + j].setBlockState(PLACED);
				}
			}
			break;

		case I:
			if (rotation == 0 || rotation == 3) {
				for (int i = 0; i < 1; i++) {
					for (int j = 0; j < 4; j++) {
						grid[pos_y + i][pos_x + j].setBlockState(PLACED);
					}
				}
			}
			else if (rotation == 1 || rotation == 2) {
				for (int i = 0; i < 4; i++) {
					grid[pos_y + i][pos_x].setBlockState(PLACED);
				}
			}
			break;

		case S:
			if (rotation == 0 || rotation == 3) {
				grid[pos_y][pos_x].setBlockState(PLACED);
				grid[pos_y][pos_x + 1].setBlockState(PLACED);
				grid[pos_y + 1][pos_x - 1].setBlockState(PLACED);
				grid[pos_y + 1][pos_x - 0].setBlockState(PLACED);
			}
			else if (rotation == 1 || rotation == 2) {
				grid[pos_y][pos_x].setBlockState(PLACED);
				grid[pos_y + 1][pos_x].setBlockState(PLACED);
				grid[pos_y + 1][pos_x + 1].setBlockState(PLACED);
				grid[pos_y + 2][pos_x + 1].setBlockState(PLACED);
			}
			break;

		case Z:
			if (rotation == 0 || rotation == 3) {
				grid[pos_y][pos_x].setBlockState(PLACED);
				grid[pos_y][pos_x - 1].setBlockState(PLACED);
				grid[pos_y + 1][pos_x + 1].setBlockState(PLACED);
				grid[pos_y + 1][pos_x - 0].setBlockState(PLACED);
			}
			else if (rotation == 1 || rotation == 2) {
				grid[pos_y][pos_x].setBlockState(PLACED);
				grid[pos_y + 1][pos_x].setBlockState(PLACED);
				grid[pos_y + 1][pos_x - 1].setBlockState(PLACED);
				grid[pos_y + 2][pos_x - 1].setBlockState(PLACED);
			}
			break;

		case L:
			if (rotation == 0) {
				grid[pos_y][pos_x].setBlockState(PLACED);
				grid[pos_y + 1][pos_x].setBlockState(PLACED);
				grid[pos_y + 2][pos_x].setBlockState(PLACED);
				grid[pos_y + 2][pos_x + 1].setBlockState(PLACED);
			}
			else if (rotation == 1) {
				grid[pos_y][pos_x].setBlockState(PLACED);
				grid[pos_y][pos_x + 1].setBlockState(PLACED);
				grid[pos_y][pos_x + 2].setBlockState(PLACED);
				grid[pos_y + 1][pos_x].setBlockState(PLACED);
			}
			else if (rotation == 2) {
				grid[pos_y][pos_x].setBlockState(PLACED);
				grid[pos_y + 1][pos_x].setBlockState(PLACED);
				grid[pos_y + 1][pos_x - 1].setBlockState(PLACED);
				grid[pos_y + 1][pos_x - 2].setBlockState(PLACED);
			}
			else if (rotation == 3) {
				grid[pos_y][pos_x].setBlockState(PLACED);
				grid[pos_y][pos_x + 1].setBlockState(PLACED);
				grid[pos_y + 1][pos_x + 1].setBlockState(PLACED);
				grid[pos_y + 2][pos_x + 1].setBlockState(PLACED);
			}
			break;

		case J:
			if (rotation == 0) {
				grid[pos_y][pos_x].setBlockState(PLACED);
				grid[pos_y + 1][pos_x].setBlockState(PLACED);
				grid[pos_y + 2][pos_x].setBlockState(PLACED);
				grid[pos_y + 2][pos_x - 1].setBlockState(PLACED);
			}
			else if (rotation == 1) {
				grid[pos_y][pos_x].setBlockState(PLACED);
				grid[pos_y + 1][pos_x].setBlockState(PLACED);
				grid[pos_y + 1][pos_x + 1].setBlockState(PLACED);
				grid[pos_y + 1][pos_x + 2].setBlockState(PLACED);
			}
			else if (rotation == 2) {
				grid[pos_y][pos_x].setBlockState(PLACED);
				grid[pos_y][pos_x + 1].setBlockState(PLACED);
				grid[pos_y][pos_x + 2].setBlockState(PLACED);
				grid[pos_y + 1][pos_x + 2].setBlockState(PLACED);
			}
			else if (rotation == 3) {
				grid[pos_y][pos_x].setBlockState(PLACED);
				grid[pos_y][pos_x + 1].setBlockState(PLACED);
				grid[pos_y + 1][pos_x].setBlockState(PLACED);
				grid[pos_y + 2][pos_x].setBlockState(PLACED);
			}
			break;

		case T:
			if (rotation == 0) {
				grid[pos_y][pos_x].setBlockState(PLACED);
				grid[pos_y + 1][pos_x].setBlockState(PLACED);
				grid[pos_y + 1][pos_x - 1].setBlockState(PLACED);
				grid[pos_y + 1][pos_x + 1].setBlockState(PLACED);
			}
			else if (rotation == 1) {
				grid[pos_y][pos_x].setBlockState(PLACED);
				grid[pos_y + 1][pos_x].setBlockState(PLACED);
				grid[pos_y + 2][pos_x].setBlockState(PLACED);
				grid[pos_y + 1][pos_x + 1].setBlockState(PLACED);
			}
			else if (rotation == 2) {
				grid[pos_y][pos_x].setBlockState(PLACED);
				grid[pos_y + 1][pos_x].setBlockState(PLACED);
				grid[pos_y + 2][pos_x].setBlockState(PLACED);
				grid[pos_y + 1][pos_x - 1].setBlockState(PLACED);
			}
			else if (rotation == 3) {
				grid[pos_y][pos_x].setBlockState(PLACED);
				grid[pos_y][pos_x + 1].setBlockState(PLACED);
				grid[pos_y][pos_x + 2].setBlockState(PLACED);
				grid[pos_y + 1][pos_x + 1].setBlockState(PLACED);
			}
			break;
		}
	}

	void setRotationLeft() {
		if (rotation == 0) {
			previous_rotation = 0;
			rotation = 2;
		}
		else if (rotation == 1) {
			previous_rotation = 1;
			rotation = 0;
		}
		else if (rotation == 2) {
			previous_rotation = 2;
			rotation = 3;
		}
		else if (rotation == 3) {
			previous_rotation = 3;
			rotation = 1;
		}
		rotate();
	}

	void setRotationRight() {
		if (rotation == 0) {
			previous_rotation = 0;
			rotation = 1;
		}
		else if (rotation == 1) {
			previous_rotation = 1;
			rotation = 3;
		}
		else if (rotation == 2) {
			previous_rotation = 2;
			rotation = 0;
		}
		else if (rotation == 3) {
			previous_rotation = 3;
			rotation = 2;
		}
		rotate();
	}

	void input() {
		static bool rotationLeftPressed = false;
		static bool rotationRightPressed = false;
		static bool hardDropPressed = false;

		Event *event = new Event;
		if (Keyboard::isKeyPressed(Keyboard::Quote) && pieceRight < COLOUMNS - 1) {
			if (!check_X_Right()) {
				move_X(1);
			}
		}
		if (Keyboard::isKeyPressed(Keyboard::L) && pieceLeft > 0) {
			if (!check_X_Left()) {
				move_X(-1);
			}
		}
		if (Keyboard::isKeyPressed(Keyboard::SemiColon) && pieceBottom < ROWS - 1){
			if (!check_Y()) {
				move_Y(1);
			}
		}
		if (Keyboard::isKeyPressed(Keyboard::A) || Keyboard::isKeyPressed(Keyboard::Left)) {
			if (!rotationLeftPressed) {
				setRotationLeft();
				rotationLeftPressed = true;
			}
		}
		else {
			rotationLeftPressed = false;
		}

		if (Keyboard::isKeyPressed(Keyboard::D) || Keyboard::isKeyPressed(Keyboard::Right)) {
			if (!rotationRightPressed) {
				setRotationRight();
				rotationRightPressed = true;
			}
		}
		else {
			rotationRightPressed = false;
		}

		if (Keyboard::isKeyPressed(Keyboard::Space)) {
			if (!hardDropPressed) {
				
				hardDropPressed = true;
			}
		}
		else {
			hardDropPressed = false;
		}
	}

	void reset() {
		for (int i = 0; i < ROWS; i++) {
			for (int j = 0; j < COLOUMNS; j++) {
				if (grid[i][j].getBlockState() == ACTIVE) {
					grid[i][j].setPieceState(NONE);
				}
			}
		}
	}

	void terminateLine(int &index) {
		for (int j = 0; j < COLOUMNS; j++) {
			grid[index][j].setBlockState(ACTIVE);
			grid[index][j].setPieceState(NONE);
		}
		for (int i = index; i >= 0; i--) {
			for (int j = 0; j < COLOUMNS; j++) {
				if (grid[i][j].getBlockState() == PLACED) {
					grid[i + 1][j].setPieceState(grid[i][j].getPieceState());
					grid[i + 1][j].setBlockState(PLACED);
					grid[i][j].setBlockState(ACTIVE);
					grid[i][j].setPieceState(NONE);
				}
			}
		}
	}

	bool lineCheck() {
		int counter = 0;
		for (int i = 0; i < ROWS; i++) {
			for (int j = 0; j < COLOUMNS; j++) {
				if (grid[i][j].getBlockState() == PLACED) {
					counter++;
				}
				else {
					break;
				}
			}
			if (counter == COLOUMNS) {
				terminateLine(i);
				return true;
			}
			counter = 0;
		}
		return false;
	}

	void rotate() {
		switch (piece) {
		case I:
			if (previous_rotation == 0 && rotation == 1 || previous_rotation == 3 && rotation == 2 ||
				previous_rotation == 0 && rotation == 2 || previous_rotation == 3 && rotation == 1) {
				pos_x = pos_x + 2;
				pos_y = pos_y - 1;

				pieceLeft = pos_x;
				pieceRight = pos_x;
				pieceBottom = pos_y + 3;
			}
			else if (previous_rotation == 1 && rotation == 3 || previous_rotation == 2 && rotation == 0 ||
					 previous_rotation == 2 && rotation == 3 || previous_rotation == 1 && rotation == 0) {
				pos_x = pos_x - 2;
				pos_y = pos_y + 1;

				pieceLeft = pos_x;
				pieceRight = pos_x + 3;
				pieceBottom = pos_y;
			}
			break;

		case O:
			break;

		case S:
			if (previous_rotation == 0 && rotation == 1 || previous_rotation == 3 && rotation == 2 ||
				previous_rotation == 0 && rotation == 2 || previous_rotation == 3 && rotation == 1) {
				pos_x = pos_x - 1;
				pos_y = pos_y - 1;

				pieceLeft = pos_x;
				pieceRight = pos_x + 1;
				pieceBottom = pos_y + 2;
			}
			else if (previous_rotation == 1 && rotation == 3 || previous_rotation == 2 && rotation == 0 ||
					 previous_rotation == 1 && rotation == 0 || previous_rotation == 2 && rotation == 3) {

				pos_x = pos_x + 1;
				pos_y = pos_y + 1;

				pieceLeft = pos_x - 1;
				pieceRight = pos_x + 1;
				pieceBottom = pos_y + 1;
			}
			break;

		case Z:
			if (previous_rotation == 0 && rotation == 1 || previous_rotation == 3 && rotation == 2 ||
				previous_rotation == 0 && rotation == 2 || previous_rotation == 3 && rotation == 1) {
				pos_x = pos_x + 2;
				pos_y = pos_y - 1;

				pieceLeft = pos_x - 1;
				pieceRight = pos_x;
				pieceBottom = pos_y + 2;
			}
			else if (previous_rotation == 1 && rotation == 3 || previous_rotation == 2 && rotation == 0 ||
				previous_rotation == 1 && rotation == 0 || previous_rotation == 2 && rotation == 3) {

				pos_x = pos_x - 2;
				pos_y = pos_y + 1;

				pieceLeft = pos_x;
				pieceRight = pos_x + 2;
				pieceBottom = pos_y + 1;
			}
			break;

		case L:
			// CW
			if (previous_rotation == 0 && rotation == 1) {
				pos_x = pos_x - 1;
				pos_y = pos_y + 1;

				pieceLeft = pos_x;
				pieceRight = pos_x + 2;
				pieceBottom = pos_y + 1;
			}
			else if (previous_rotation == 1 && rotation == 3) {
				pos_x = pos_x;
				pos_y = pos_y - 1;

				pieceLeft = pos_x;
				pieceRight = pos_x + 1;
				pieceBottom = pos_y + 2;
			}
			else if (previous_rotation == 3 && rotation == 2) {
				pos_x = pos_x + 2;
				pos_y = pos_y + 1;

				pieceLeft = pos_x - 2;
				pieceRight = pos_x;
				pieceBottom = pos_y + 1;
			}
			else if (previous_rotation == 2 && rotation == 0) {
				pos_x = pos_x - 1;
				pos_y = pos_y - 1;

				pieceLeft = pos_x;
				pieceRight = pos_x + 1;
				pieceBottom = pos_y + 2;
			}
			// CCW
			if (previous_rotation == 0 && rotation == 2) {
				pos_x = pos_x;
				pos_y = pos_y + 1;

				pieceLeft = pos_x - 2;
				pieceRight = pos_x;
				pieceBottom = pos_y + 1;
			}
			else if (previous_rotation == 2 && rotation == 3) {
				pos_x = pos_x - 2;
				pos_y = pos_y - 1;

				pieceLeft = pos_x;
				pieceRight = pos_x + 1;
				pieceBottom = pos_y + 2;
			}
			else if (previous_rotation == 3 && rotation == 1) {
				pos_x = pos_x;
				pos_y = pos_y + 1;

				pieceLeft = pos_x;
				pieceRight = pos_x + 2;
				pieceBottom = pos_y + 1;
			}
			else if (previous_rotation == 1 && rotation == 0) {
				pos_x = pos_x + 1;
				pos_y = pos_y - 1;

				pieceLeft = pos_x;
				pieceRight = pos_x + 1;
				pieceBottom = pos_y + 2;
			}
			break;

		case J:			
			// CW
			if (previous_rotation == 0 && rotation == 1) {
				pos_x = pos_x - 1;
				pos_y = pos_y + 1;

				pieceLeft = pos_x;
				pieceRight = pos_x + 2;
				pieceBottom = pos_y + 1;
			}
			else if (previous_rotation == 1 && rotation == 3) {
				pos_x = pos_x + 1;
				pos_y = pos_y - 1;

				pieceLeft = pos_x;
				pieceRight = pos_x + 1;
				pieceBottom = pos_y + 2;
			}
			else if (previous_rotation == 3 && rotation == 2) {
				pos_x = pos_x - 1;
				pos_y = pos_y + 1;

				pieceLeft = pos_x;
				pieceRight = pos_x + 2;
				pieceBottom = pos_y + 1;
			}
			else if (previous_rotation == 2 && rotation == 0) {
				pos_x = pos_x + 1;
				pos_y = pos_y - 1;

				pieceLeft = pos_x - 1;
				pieceRight = pos_x;
				pieceBottom = pos_y + 2;
			}
			// CCW
			if (previous_rotation == 0 && rotation == 2) {
				pos_x = pos_x - 1;
				pos_y = pos_y + 1;

				pieceLeft = pos_x;
				pieceRight = pos_x + 2;
				pieceBottom = pos_y + 1;
			}
			else if (previous_rotation == 2 && rotation == 3) {
				pos_x = pos_x + 1;
				pos_y = pos_y - 1;

				pieceLeft = pos_x;
				pieceRight = pos_x + 1;
				pieceBottom = pos_y + 2;
			}
			else if (previous_rotation == 3 && rotation == 1) {
				pos_x = pos_x - 1;
				pos_y = pos_y + 1;

				pieceLeft = pos_x;
				pieceRight = pos_x + 2;
				pieceBottom = pos_y + 1;
			}
			else if (previous_rotation == 1 && rotation == 0) {
				pos_x = pos_x + 1;
				pos_y = pos_y - 1;

				pieceLeft = pos_x + 1;
				pieceRight = pos_x;
				pieceBottom = pos_y + 2;
			}
			break;

		case T:
			// CW
			if (previous_rotation == 0 && rotation == 1) {
				pos_x = pos_x;
				pos_y = pos_y - 1;
				
				pieceLeft = pos_x;
				pieceRight = pos_x + 1;
				pieceBottom = pos_y + 2;
			}
			else if (previous_rotation == 1 && rotation == 3) {
				pos_x = pos_x - 1;
				pos_y = pos_y + 1;

				pieceLeft = pos_x;
				pieceRight = pos_x + 2;
				pieceBottom = pos_y + 1;
			}
			else if (previous_rotation == 3 && rotation == 2) {
				pos_x = pos_x + 1;
				pos_y = pos_y - 1;

				pieceLeft = pos_x - 1;
				pieceRight = pos_x;
				pieceBottom = pos_y + 2;
			}
			else if (previous_rotation == 2 && rotation == 0) {
				pos_x = pos_x;
				pos_y = pos_y + 1;

				pieceLeft = pos_x - 1;
				pieceRight = pos_x + 1;
				pieceBottom = pos_y + 1;
			}

			// CCW
			if (previous_rotation == 0 && rotation == 2) {
				pos_x = pos_x;
				pos_y = pos_y - 1;

				pieceLeft = pos_x - 1;
				pieceRight = pos_x;
				pieceBottom = pos_y + 2;
			}
			else if (previous_rotation == 2 && rotation == 3) {
				pos_x = pos_x - 1;
				pos_y = pos_y + 1;

				pieceLeft = pos_x;
				pieceRight = pos_x + 2;
				pieceBottom = pos_y + 1;
			}
			else if (previous_rotation == 3 && rotation == 1) {
				pos_x = pos_x + 1;
				pos_y = pos_y - 1;

				pieceLeft = pos_x;
				pieceRight = pos_x + 1;
				pieceBottom = pos_y + 2;
			}
			else if (previous_rotation == 1 && rotation == 0) {
				pos_x = pos_x;
				pos_y = pos_y + 1;

				pieceLeft = pos_x - 1;
				pieceRight = pos_x + 1;
				pieceBottom = pos_y + 1;
			}
			break;
		}
	}

	void update() {
		if (!gravity()) {
			placePiece();
			while (lineCheck()) {
				this_thread::sleep_for(20ms);
			}
			selectPiece();
		}
		else {
			input();
		}
		reset();
		switch (piece) {
		case O:
			for (int i = 0; i < 2; i++) {
				for (int j = 0; j < 2; j++) {
					grid[pos_y + i][pos_x + j].setPieceState(O);
				}
			}
			break;

		case I:
			if (rotation == 0 || rotation == 3) {
				for (int i = 0; i < 1; i++) {
					for (int j = 0; j < 4; j++) {
						grid[pos_y + i][pos_x + j].setPieceState(I);
					}
				}
			}
			else if (rotation == 1 || rotation == 2) {
				for (int j = 0; j < 1; j++) {
					for (int i = 0; i < 4; i++) {
						grid[pos_y + i][pos_x + j].setPieceState(I);
					}
				}
			}
			break;

		case S:
			if (rotation == 0 || rotation == 3) {
				grid[pos_y][pos_x].setPieceState(S);
				grid[pos_y][pos_x + 1].setPieceState(S);
				grid[pos_y + 1][pos_x - 1].setPieceState(S);
				grid[pos_y + 1][pos_x - 0].setPieceState(S);
			}
			else if (rotation == 1 || rotation == 2) {
				grid[pos_y][pos_x].setPieceState(S);
				grid[pos_y + 1][pos_x].setPieceState(S);
				grid[pos_y + 1][pos_x + 1].setPieceState(S);
				grid[pos_y + 2][pos_x + 1].setPieceState(S);
			}
			break;

		case Z:
			if (rotation == 0 || rotation == 3) {
				grid[pos_y][pos_x].setPieceState(Z);
				grid[pos_y][pos_x - 1].setPieceState(Z);
				grid[pos_y + 1][pos_x + 1].setPieceState(Z);
				grid[pos_y + 1][pos_x - 0].setPieceState(Z);
			}
			else if (rotation == 1 || rotation == 2) {
				grid[pos_y][pos_x].setPieceState(Z);
				grid[pos_y + 1][pos_x].setPieceState(Z);
				grid[pos_y + 1][pos_x - 1].setPieceState(Z);
				grid[pos_y + 2][pos_x - 1].setPieceState(Z);
			}
			break;

		case L:
			if (rotation == 0) {
				grid[pos_y][pos_x].setPieceState(L);
				grid[pos_y + 1][pos_x].setPieceState(L);
				grid[pos_y + 2][pos_x].setPieceState(L);
				grid[pos_y + 2][pos_x + 1].setPieceState(L);
			}
			else if (rotation == 1) {
				grid[pos_y][pos_x].setPieceState(L);
				grid[pos_y][pos_x + 1].setPieceState(L);
				grid[pos_y][pos_x + 2].setPieceState(L);
				grid[pos_y + 1][pos_x].setPieceState(L);
			}
			else if (rotation == 2) {
				grid[pos_y][pos_x].setPieceState(L);
				grid[pos_y + 1][pos_x].setPieceState(L);
				grid[pos_y + 1][pos_x - 1].setPieceState(L);
				grid[pos_y + 1][pos_x - 2].setPieceState(L);
			}
			else if (rotation == 3) {
				grid[pos_y][pos_x].setPieceState(L);
				grid[pos_y][pos_x + 1].setPieceState(L);
				grid[pos_y + 1][pos_x + 1].setPieceState(L);
				grid[pos_y + 2][pos_x + 1].setPieceState(L);
			}
			break;

		case J:
			if (rotation == 0) {
				grid[pos_y][pos_x].setPieceState(J);
				grid[pos_y + 1][pos_x].setPieceState(J);
				grid[pos_y + 2][pos_x].setPieceState(J);
				grid[pos_y + 2][pos_x - 1].setPieceState(J);
			}
			else if (rotation == 1) {
				grid[pos_y][pos_x].setPieceState(J);
				grid[pos_y + 1][pos_x].setPieceState(J);
				grid[pos_y + 1][pos_x + 1].setPieceState(J);
				grid[pos_y + 1][pos_x + 2].setPieceState(J);
			}
			else if (rotation == 2) {
				grid[pos_y][pos_x].setPieceState(J);
				grid[pos_y][pos_x + 1].setPieceState(J);
				grid[pos_y][pos_x + 2].setPieceState(J);
				grid[pos_y + 1][pos_x + 2].setPieceState(J);
			}
			else if (rotation == 3) {
				grid[pos_y][pos_x].setPieceState(J);
				grid[pos_y][pos_x + 1].setPieceState(J);
				grid[pos_y + 1][pos_x].setPieceState(J);
				grid[pos_y + 2][pos_x].setPieceState(J);
			}
			break;

		case T:
			if (rotation == 0) {
				grid[pos_y][pos_x].setPieceState(T);
				grid[pos_y + 1][pos_x].setPieceState(T);
				grid[pos_y + 1][pos_x - 1].setPieceState(T);
				grid[pos_y + 1][pos_x + 1].setPieceState(T);
			}
			else if (rotation == 1) {
				grid[pos_y][pos_x].setPieceState(T);
				grid[pos_y + 1][pos_x].setPieceState(T);
				grid[pos_y + 2][pos_x].setPieceState(T);
				grid[pos_y + 1][pos_x + 1].setPieceState(T);
			}
			else if (rotation == 2) {
				grid[pos_y][pos_x].setPieceState(T);
				grid[pos_y + 1][pos_x].setPieceState(T);
				grid[pos_y + 2][pos_x].setPieceState(T);
				grid[pos_y + 1][pos_x -1 ].setPieceState(T);
			}
			else if (rotation == 3) {
				grid[pos_y][pos_x].setPieceState(T);
				grid[pos_y][pos_x + 1].setPieceState(T);
				grid[pos_y][pos_x + 2].setPieceState(T);
				grid[pos_y + 1][pos_x + 1].setPieceState(T);
			}
			break;
		}
	}

	void render(RenderWindow* window) {
		for (int i = 0; i < ROWS; i++) {
			for (int j = 0; j < COLOUMNS; j++) {
				grid[i][j].updateColor();
				grid[i][j].render(window);
			}
		}
	}
};

int main() {
	srand(time(0));
	RenderWindow window(VideoMode(COLOUMNS * (BLOCK_SIZE + SPACING_FACTOR), ROWS * (BLOCK_SIZE + SPACING_FACTOR)), "Tetris", Style::Default);
	Event event;
	Game game;
	game.selectPiece();
	window.setFramerateLimit(60);
	while (window.isOpen()) {
		while (window.pollEvent(event)) {
			if (event.type == Event::Closed) {
				window.close();
			}
		}
		game.update();
		window.clear(Color::Black);
		game.render(&window);
		window.display();
	}
}