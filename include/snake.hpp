//             / . .\
//             \  ---<
//              \  /
//    __________/ /
// -=:___________/

#include "game.hpp"
#include <curses.h>
#include <vector>

class Snake : public Game {
  public:
	enum class Direction { LEFT, RIGHT, UP, DOWN, STILL };
	enum class GameState { MENU, LOSE, WIN, PLAY, PAUSE };
	Snake() {
		SetFrameInterval(200);
		SetMainFrameInterval(200);
	}

  private:
	int helpWindowSize = 2;
	bool helpShown = false;
	WINDOW *helpWin;
	WINDOW *loseWin;
	WINDOW *gameWin = NewWindow("board", rows, cols, 0, 0);

	GameState gameState;

	char snakeChar = '+';
	Direction snakeDirection;
	std::vector<std::vector<int>> snakeCords;
	std::vector<int> fruitCord;

	void MainMenu();
	void ShowHelp();
	void HideHelp();
	void ShowLose();
	void DrawBoard();

	void NewFruit();

	void Wait();

  protected:
	void GameStart() override;
	void InputLoop() override;
	void TimedGameLoop() override;
};
