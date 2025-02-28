//             / . .\
//             \  ---<
//              \  /
//    __________/ /
// -=:___________/

#include "../include/snake.hpp"
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <curses.h>
#include <random>
#include <string>
#include <vector>

void Snake::Wait() {
	std::this_thread::sleep_for(std::chrono::milliseconds(10));
}

void Snake::ShowHelp() {
	clear();
	Wait();
	if (!WindowExists("help")) {
		helpWin = NewWindow("help", rows, cols, 0, 0);
	}
	mvwprintw(helpWin, 0, 1, " --- Snake --- ");
	mvwprintw(helpWin, 2, 1, " -- Help -- ");
	mvwprintw(helpWin, 3, 1, " q - quit");
	mvwprintw(helpWin, 4, 1, " h - help");
	mvwprintw(helpWin, 5, 1, " [ - lower frame interval");
	mvwprintw(helpWin, 6, 1, " [ - higher frame interval");
}

void Snake::ShowLose() {
	clear();
	refresh();
	Wait();
	if (!WindowExists("lose")) {
		loseWin = NewWindow("lose", rows, cols, 0, 0);
	}
	mvwprintw(loseWin, 0, 1, " --- GAME OVER --- ");
	mvwprintw(loseWin, 2, 1, "             / . .\\");
	mvwprintw(loseWin, 3, 1, "             \\  ---<");
	mvwprintw(loseWin, 4, 1, "              \\  /");
	mvwprintw(loseWin, 5, 1, "    __________/ /");
	mvwprintw(loseWin, 6, 1, " -=:___________/");
	int snakeLength = snakeCords.size();
	mvwprintw(loseWin, 8, 1, "Score: %d", snakeLength);
	mvwprintw(loseWin, 10, 1, "Press any key to return to the main menu");
	wrefresh(loseWin);
	getchar();
	DelWindow("lose");
	clear();
	refresh();
	GameStart();
}

void Snake::HideHelp() {
	clear();
	Wait();
	DrawBoard();
}

void Snake::MainMenu() {
	clear();
	refresh();

	WINDOW *mainMenu = NewWindow("mainMenu", rows, cols, 0, 0);
	// box(mainMenu, 0, 0);
	wrefresh(mainMenu);
	mvwprintw(mainMenu, 0, 1, " --- Snake --- ");
	mvwprintw(mainMenu, 2, 1, "Press '?' to pause and show help");
	mvwprintw(mainMenu, 4, 1, "             / . .\\");
	mvwprintw(mainMenu, 5, 1, "             \\  ---<");
	mvwprintw(mainMenu, 6, 1, "              \\  /");
	mvwprintw(mainMenu, 7, 1, "    __________/ /");
	mvwprintw(mainMenu, 8, 1, " -=:___________/");
	mvwprintw(mainMenu, 10, 1, "Press any key to start");

	wrefresh(mainMenu);
	refresh();
	getchar();
	DelWindow("mainMenu");
	gameState = GameState::PLAY;
}

void Snake::NewFruit() {
	std::srand(std::time(0));

	std::random_device rd;
	std::mt19937 genCol(rd());
	std::mt19937 genRow(rd());
	std::uniform_int_distribution<int> distCol(2, cols - 2);
	std::uniform_int_distribution<int> distRow(2, rows - 2);

	int randomCol = distCol(genCol);
	int randomRow = distRow(genRow);

	fruitCord = {randomCol, randomRow};
}

void Snake::GameStart() {
	MainMenu();
	DrawBoard();
	snakeCords = {{1, 1}};
	snakeDirection = Direction::RIGHT;
	NewFruit();
}

void Snake::DrawBoard() {
	clear();
	refresh();
	wclear(gameWin);
	box(gameWin, 0, 0);
	int frameInterval = GetFrameInterval();
	mvwprintw(gameWin, 0, 1, " | --- Snake --- | Frame Interval - %d ms | ",
			  frameInterval);
}

void Snake::InputLoop() {
	char ch = getch();
	switch (ch) {
	case 'q':
		gameEnded = true;
		break;
	case 'Q':
		gameEnded = true;
		break;
	case '?':
		helpShown = !helpShown;
		if (helpShown) {
			gameState = GameState::PAUSE;
			ShowHelp();
		} else {
			gameState = GameState::PLAY;
			DrawBoard();
		}
		break;
	case '[':
		SetMainFrameInterval(GetMainFrameInterval() - 10);
		SetFrameInterval(GetFrameInterval() - 10);
		break;
	case ']':
		SetMainFrameInterval(GetMainFrameInterval() + 10);
		SetFrameInterval(GetFrameInterval() + 10);
		break;
	case 'w':
		snakeDirection = Direction::UP;
		break;
	case 'k':
		snakeDirection = Direction::UP;
		break;

	case 'h':
		snakeDirection = Direction::LEFT;
		break;
	case 'a':
		snakeDirection = Direction::LEFT;
		break;

	case 's':
		snakeDirection = Direction::DOWN;
		break;
	case 'j':
		snakeDirection = Direction::DOWN;
		break;

	case 'd':
		snakeDirection = Direction::RIGHT;
		break;
	case 'l':
		snakeDirection = Direction::RIGHT;
		break;
	}
}

void Snake::TimedGameLoop() {
	if (gameState == GameState::LOSE) {
		ShowLose();
	} else if (gameState == GameState::PLAY) {
		if (!helpShown) {
			DrawBoard();
		}

		// Print fruit
		mvwaddch(gameWin, fruitCord[1], fruitCord[0], 'x');

		// Print snake
		for (int i = 0; i < snakeCords.size(); i++) {
			if (snakeCords[i][0] == cols - 1) {
				gameState = GameState::LOSE;
			}
			if (snakeCords[i][0] == 0) {
				gameState = GameState::LOSE;
			}
			if (snakeCords[i][1] == rows - 1) {
				gameState = GameState::LOSE;
			}
			if (snakeCords[i][1] == 0) {
				gameState = GameState::LOSE;
			}
			mvwaddch(gameWin, snakeCords[i][1], snakeCords[i][0], snakeChar);
		}

		if (snakeCords[0] == fruitCord) {
			NewFruit();
			if (snakeCords.size() > 1) {
				// Adds one to the right
				if (snakeCords[snakeCords.size() - 1][0] >
					snakeCords[snakeCords.size() - 2][0]) {
					snakeCords.push_back(
						{snakeCords[snakeCords.size() - 1][0] + 1,
						 snakeCords[snakeCords.size() - 1][1]});
				} else if (snakeCords[snakeCords.size() - 1][0] <
						   snakeCords[snakeCords.size() - 2][0]) {
					snakeCords.push_back(
						{snakeCords[snakeCords.size() - 1][0] - 1,
						 snakeCords[snakeCords.size() - 1][1]});
				} else if (snakeCords[snakeCords.size() - 1][1] >
						   snakeCords[snakeCords.size() - 2][1]) {
					snakeCords.push_back(
						{snakeCords[snakeCords.size() - 1][0],
						 snakeCords[snakeCords.size() - 1][1] + 1});
				} else if (snakeCords[snakeCords.size() - 1][1] <
						   snakeCords[snakeCords.size() - 2][1]) {
					snakeCords.push_back(
						{snakeCords[snakeCords.size() - 1][0],
						 snakeCords[snakeCords.size() - 1][1] - 1});
				}
			} else {
				switch (snakeDirection) {
				case Direction::UP:
					snakeCords.push_back(
						{snakeCords[snakeCords.size() - 1][0],
						 snakeCords[snakeCords.size() - 1][1] - 1});
					break;
				case Direction::LEFT:
					snakeCords.push_back(
						{snakeCords[snakeCords.size() - 1][0] + 1,
						 snakeCords[snakeCords.size() - 1][1]});
					break;
				case Direction::DOWN:
					snakeCords.push_back(
						{snakeCords[snakeCords.size() - 1][0],
						 snakeCords[snakeCords.size() - 1][1] + 1});
					break;
				case Direction::RIGHT:
					snakeCords.push_back(
						{snakeCords[snakeCords.size() - 1][0] - 1,
						 snakeCords[snakeCords.size() - 1][1]});
					break;
				case Direction::STILL:
					break;
				}
			}
		}

		// Update cords
		std::vector<int> _prev = {0, 0};
		std::vector<int> prev = {0, 0};
		for (int i = 0; i < snakeCords.size(); i++) {
			switch (snakeDirection) {
			case Direction::UP:
				if (i == 0) {
					prev = snakeCords[i];
					snakeCords[i] = {snakeCords[i][0], snakeCords[i][1] - 1};
				} else {

					_prev = snakeCords[i];
					snakeCords[i] = prev;
					prev = _prev;
				}
				SetFrameInterval(GetMainFrameInterval() * 2);
				break;
			case Direction::LEFT:
				if (i == 0) {
					prev = snakeCords[i];
					snakeCords[i] = {snakeCords[i][0] - 1, snakeCords[i][1]};
				} else {

					_prev = snakeCords[i];
					snakeCords[i] = prev;
					prev = _prev;
				}
				SetFrameInterval(GetMainFrameInterval());
				break;
			case Direction::DOWN:
				if (i == 0) {
					prev = snakeCords[i];
					snakeCords[i] = {snakeCords[i][0], snakeCords[i][1] + 1};
				} else {
					_prev = snakeCords[i];
					snakeCords[i] = prev;
					prev = _prev;
				}
				SetFrameInterval(GetMainFrameInterval() * 2);
				break;
			case Direction::RIGHT:
				if (i == 0) {
					prev = snakeCords[i];
					snakeCords[i] = {snakeCords[i][0] + 1, snakeCords[i][1]};
				} else {
					_prev = snakeCords[i];
					snakeCords[i] = prev;
					prev = _prev;
				}
				SetFrameInterval(GetMainFrameInterval());
				break;
			case Direction::STILL:
				break;
			}

			if (snakeCords[i] == snakeCords[0] && i > 0) {
				gameState = GameState::LOSE;
				ShowLose();
			}
		}
	}
}
