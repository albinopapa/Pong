#include "BoardView.h"
#include "Ball.h"
#include "Board.h"
#include "Graphics.h"
#include "Scoreboard.h"


BoardView::BoardView( Scoreboard &ScBoard, Board &Brd )
	:
	m_scoreboard( ScBoard ),
	m_board( Brd )
{
}

void BoardView::Draw( Graphics & Gfx )
{
	m_board.GetBall().Draw( Gfx );

	for( size_t i = 0; i < m_board.PaddleCount(); ++i )
	{
		m_board.GetPaddle( i ).Draw( Gfx );
	}
	for( size_t i = 0; i < m_board.WallCount(); ++i )
	{
		m_board.GetWall( i ).Draw( Gfx );
	}

	m_scoreboard.Draw( Gfx );
}
