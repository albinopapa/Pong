#pragma once

#include "Ball.h"
#include "FrameTimer.h"
#include "Graphics.h"
#include "Board.h"
#include "Wall.h"
#include "Player.h"
#include "BoardView.h"
#include <memory>
#include <vector>

class Game
{
public:
	enum class State
	{
		Title, Setup, PlayGame, GameOverWin, GameOver
	};
public:
	Game( class MainWindow& wnd );
	Game( const Game& ) = delete;
	Game& operator=( const Game& ) = delete;
	void Go();
private:
	void ComposeFrame();
	void UpdateModel();
	/********************************/
	/*  User Functions              */
	void CheckForQuit();
	void SetupGame();
	/********************************/
private:
	MainWindow& wnd;
	Graphics gfx;
	/********************************/
	/*  User Variables              */
	FrameTimer ft;
	State m_state = State::Title;
	std::string response;
	int m_numPlayers = 0;

	Board m_board;
	BoardView m_boardview;

	std::vector<std::unique_ptr<Player>> m_pPlayers;

	Scoreboard m_scoreboard;
	Scoreboard::Score *m_pWinner = nullptr;
	/********************************/
};