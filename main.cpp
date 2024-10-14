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
			for (int i = 0; i < 4; i++) {
				if (grid[pos_y + 1][pos_x + i].getBlockState() == PLACED) {
					return 1;
				}
			}
			break;

		case S:
			if (grid[pos_y + 2][pos_x - 1].getBlockState() == PLACED ||
				grid[pos_y + 2][pos_x - 0].getBlockState() == PLACED ||
				grid[pos_y + 1][pos_x + 1].getBlockState() == PLACED) {
				return 1;
			}
			break;

		case O:
			for (int i = 0; i < 2; i++) {
				if (grid[pos_y + 2][pos_x + i].getBlockState() == PLACED)
					return 1;
			}
			break;

		case Z:
			if (grid[pos_y + 2][pos_x + 1].getBlockState() == PLACED ||
				grid[pos_y + 2][pos_x - 0].getBlockState() == PLACED ||
				grid[pos_y + 1][pos_x - 1].getBlockState() == PLACED) {
				return 1;
			}
			break;

		case L:
			if (grid[pos_y + 3][pos_x].getBlockState() == PLACED ||
				grid[pos_y + 3][pos_x + 1].getBlockState() == PLACED) {
				return 1;
			}
			break;

		case J:
			if (grid[pos_y + 3][pos_x].getBlockState() == PLACED ||
				grid[pos_y + 3][pos_x - 1].getBlockState() == PLACED) {
				return 1;
			}
			break;

		case T:
			if (grid[pos_y + 2][pos_x].getBlockState() == PLACED ||
				grid[pos_y + 2][pos_x - 1].getBlockState() == PLACED ||
				grid[pos_y + 2][pos_x + 1].getBlockState() == PLACED) {
				return 1;
			}
			break;

		}
		return 0;
	}

	bool check_X_Left() {
		switch (piece) {
		case I:
			if (grid[pos_y][pieceLeft - 1].getBlockState() == PLACED) {
				return 1;
			}
			break;

		case O:
			if (grid[pos_y][pieceLeft - 1].getBlockState() == PLACED ||
				grid[pos_y + 1][pieceLeft - 1].getBlockState() == PLACED)
				return 1;
			break;

		case S:
			if (grid[pos_y][pos_x - 1].getBlockState() == PLACED ||
				grid[pos_y + 1][pieceLeft - 1].getBlockState() == PLACED)
				return 1;
			break;

		case Z:
			if (grid[pos_y][pieceLeft - 1].getBlockState() == PLACED ||
				grid[pos_y + 1][pos_x - 1].getBlockState() == PLACED)
				return 1;
			break;

		case L:
			if (grid[pos_y][pieceLeft - 1].getBlockState() == PLACED ||
				grid[pos_y + 1][pieceLeft - 1].getBlockState() == PLACED ||
				grid[pos_y + 2][pieceLeft - 1].getBlockState() == PLACED)
				return 1;
			break;

		case J:
			if (grid[pos_y + 2][pieceLeft - 1].getBlockState() == PLACED ||
				grid[pos_y][pos_x - 1].getBlockState() == PLACED ||
				grid[pos_y + 1][pos_x - 1].getBlockState() == PLACED)
				return 1;
			break;

		case T:
			if (grid[pos_y][pos_x - 1].getBlockState() == PLACED ||
				grid[pos_y][pieceLeft - 1].getBlockState() == PLACED)
				return 1;
			break;
		}
		return 0;
	}

	bool check_X_Right() {
		switch (piece) {
		case I:
			if (grid[pos_y][pieceRight + 1].getBlockState() == PLACED) {
				return 1;
			}
			break;

		case O:
			if (grid[pos_y][pieceRight + 1].getBlockState() == PLACED ||
				grid[pos_y + 1][pieceRight + 1].getBlockState() == PLACED)
				return 1;
			break;

		case S:
			if (grid[pos_y][pieceRight + 1].getBlockState() == PLACED ||
				grid[pos_y + 1][pos_x + 1].getBlockState() == PLACED)
				return 1;
			break;

		case Z:
			if (grid[pos_y][pos_x + 1].getBlockState() == PLACED ||
				grid[pos_y + 1][pieceRight + 1].getBlockState() == PLACED)
				return 1;
			break;

		case L:
			if (grid[pos_y][pos_x + 1].getBlockState() == PLACED ||
				grid[pos_y + 1][pos_x + 1].getBlockState() == PLACED ||
				grid[pos_y + 2][pieceRight + 1].getBlockState() == PLACED)
				return 1;
			break;

		case J:
			if (grid[pos_y][pieceRight + 1].getBlockState() == PLACED ||
				grid[pos_y + 1][pieceRight + 1].getBlockState() == PLACED ||
				grid[pos_y + 2][pieceRight + 1].getBlockState() == PLACED)
				return 1;
			break;

		case T:
			if (grid[pos_y][pos_x + 1].getBlockState() == PLACED ||
				grid[pos_y][pieceRight + 1].getBlockState() == PLACED)
				return 1;
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
			for (int i = 0; i < 1; i++) {
				for (int j = 0; j < 4; j++) {
					grid[pos_y + i][pos_x + j].setBlockState(PLACED);
				}
			}
			break;

		case S:
			grid[pos_y][pos_x].setBlockState(PLACED);
			grid[pos_y][pos_x + 1].setBlockState(PLACED);
			grid[pos_y + 1][pos_x - 1].setBlockState(PLACED);
			grid[pos_y + 1][pos_x - 0].setBlockState(PLACED);
			break;

		case Z:
			grid[pos_y][pos_x].setBlockState(PLACED);
			grid[pos_y][pos_x - 1].setBlockState(PLACED);
			grid[pos_y + 1][pos_x].setBlockState(PLACED);
			grid[pos_y + 1][pos_x + 1].setBlockState(PLACED);
			break;

		case L:
			grid[pos_y][pos_x].setBlockState(PLACED);
			grid[pos_y + 1][pos_x].setBlockState(PLACED);
			grid[pos_y + 2][pos_x].setBlockState(PLACED);
			grid[pos_y + 2][pos_x + 1].setBlockState(PLACED);
			break;

		case J:
			grid[pos_y][pos_x].setBlockState(PLACED);
			grid[pos_y + 1][pos_x].setBlockState(PLACED);
			grid[pos_y + 2][pos_x].setBlockState(PLACED);
			grid[pos_y + 2][pos_x - 1].setBlockState(PLACED);
			break;

		case T:
			grid[pos_y][pos_x].setBlockState(PLACED);
			grid[pos_y + 1][pos_x].setBlockState(PLACED);
			grid[pos_y + 1][pos_x - 1].setBlockState(PLACED);
			grid[pos_y + 1][pos_x + 1].setBlockState(PLACED);
			break;
		}
	}

	void input() {
		if (Keyboard::isKeyPressed(Keyboard::D) && pieceRight < COLOUMNS - 1) {
			if (!check_X_Right()) {
				move_X(1);
			}
		}
		if (Keyboard::isKeyPressed(Keyboard::A) && pieceLeft > 0) {
			if (!check_X_Left()) {
				move_X(-1);
			}
		}
		if (Keyboard::isKeyPressed(Keyboard::S) && pieceBottom < ROWS - 1){
			if (!check_Y()) {
				move_Y(1);
			}
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

	void lineCheck() {
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
				return;
			}
			counter = 0;
		}
		return;
	}

	void update() {
		if (!gravity()) {
			placePiece();
			lineCheck();
			selectPiece();
		};
		input();
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
			for (int i = 0; i < 1; i++) {
				for (int j = 0; j < 4; j++) {
					grid[pos_y + i][pos_x + j].setPieceState(I);
				}
			}
			break;

		case S:
			grid[pos_y][pos_x].setPieceState(S);
			grid[pos_y][pos_x + 1].setPieceState(S);
			grid[pos_y + 1][pos_x - 1].setPieceState(S);
			grid[pos_y + 1][pos_x - 0].setPieceState(S);
			break;

		case Z:
			grid[pos_y][pos_x].setPieceState(Z);
			grid[pos_y][pos_x - 1].setPieceState(Z);
			grid[pos_y + 1][pos_x + 1].setPieceState(Z);
			grid[pos_y + 1][pos_x - 0].setPieceState(Z);
			break;

		case L:
			grid[pos_y][pos_x].setPieceState(L);
			grid[pos_y + 1][pos_x].setPieceState(L);
			grid[pos_y + 2][pos_x].setPieceState(L);
			grid[pos_y + 2][pos_x + 1].setPieceState(L);
			break;

		case J:
			grid[pos_y][pos_x].setPieceState(J);
			grid[pos_y + 1][pos_x].setPieceState(J);
			grid[pos_y + 2][pos_x].setPieceState(J);
			grid[pos_y + 2][pos_x - 1].setPieceState(J);
			break;

		case T:
			grid[pos_y][pos_x].setPieceState(T);
			grid[pos_y + 1][pos_x].setPieceState(T);
			grid[pos_y + 1][pos_x - 1].setPieceState(T);
			grid[pos_y + 1][pos_x + 1].setPieceState(T);
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