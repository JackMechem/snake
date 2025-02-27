//             / . .\
//             \  ---<
//              \  /
//    __________/ /
// -=:___________/

#include <atomic>
#include <chrono>
#include <cstdio>
#include <curses.h>
#include <string>
#include <thread>
#include <unordered_map>

class Game {
  protected:
	std::atomic<bool> gameEnded;

	int rows, cols;

	virtual void GameStart() = 0;
	virtual void TimedGameLoop() = 0;
	virtual void InputLoop() = 0;

	WINDOW *NewWindow(std::string name, int _rows, int _cols, int yOff,
					  int xOff) {
		windows[name] = newwin(_rows, _cols, yOff, xOff);
		return windows[name];
	}

	WINDOW *GetWindow(std::string name) { return windows[name]; }

	void DelWindow(std::string name) {
		wclear(windows[name]);
		werase(windows[name]);
		wrefresh(windows[name]);
		delwin(windows[name]);
		windows.erase(name);
	}

	bool WindowExists(std::string name) {
		return windows.count(name) > 0 ? true : false;
	}

  private:
	std::unordered_map<std::string, WINDOW *> windows;

	int frameInterval = 10;
    int mainFrameInterval = 10;
	std::thread gameLoopThread, inputThread;
	void _timedGameLoop() {
		while (!gameEnded) {
			TimedGameLoop();
			for (const auto &[key, value] : windows) {
				wrefresh(value);
			}
			refresh();
			std::this_thread::sleep_for(
				std::chrono::milliseconds(frameInterval));
		}
	}
	void _inputLoop() {
		while (!gameEnded) {
			InputLoop();
			// std::this_thread::sleep_for(std::chrono::milliseconds(10));
		}
	}

  public:
	Game() : gameEnded(false) {
		initscr();
		cbreak();
		noecho();
		keypad(stdscr, TRUE);
		nodelay(stdscr, TRUE);
		clear();
		curs_set(0);
		getmaxyx(stdscr, rows, cols);
		refresh();
	}

	void SetFrameInterval(int frameInterval) {
		this->frameInterval = frameInterval;
	}
    
    int GetFrameInterval() {
        return this->frameInterval;
    }

    int GetMainFrameInterval() {
        return this->mainFrameInterval;
    }

    void SetMainFrameInterval(int frameInterval) {
        this->mainFrameInterval = frameInterval;
    }

	void Start() {
		GameStart();
		inputThread = std::thread(&Game::_inputLoop, this);
		gameLoopThread = std::thread(&Game::_timedGameLoop, this);

		while (!gameEnded) {
			std::this_thread::sleep_for(std::chrono::milliseconds(10));
		}
		Exit();
	}

	void Exit() {
		gameEnded = true;
		if (gameLoopThread.joinable())
			gameLoopThread.join();
		if (inputThread.joinable())
			inputThread.join();
		endwin();
	}

	virtual ~Game() { Exit(); }
};
