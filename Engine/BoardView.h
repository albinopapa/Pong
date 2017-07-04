#pragma once


#include "BallView.h"

class Board;
class Graphics;

class BoardView
{
public:
	BoardView( class Scoreboard &ScBoard, class Board &Brd );

	void Draw( Graphics &Gfx );
private:
	const BallView m_ballview;
	class Scoreboard &m_scoreboard;
	class Board &m_board;
};

