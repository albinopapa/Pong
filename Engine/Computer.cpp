#include "Computer.h"
#include "Ball.h"

Computer::Computer( const Ball & B, const std::string &PlayerName, int ScoreX )
	:
	Player( PlayerName, ScoreX, 5 ),
	m_ball(B)
{
}

void Computer::Update( float DeltaTime, Paddle &Pad )
{
	const auto padCenterY = Pad.GetRect().GetCenter().y;
	const auto ballY = m_ball.GetPosition().y;
	if( ballY > padCenterY )
	{
		Pad.OnMove( Paddle::Direction::DOWN, DeltaTime );
	}
	else if( ballY  < padCenterY )
	{
		Pad.OnMove( Paddle::Direction::UP, DeltaTime );
	}
	else
	{
		Pad.OnMove( Paddle::Direction::IDLE, DeltaTime );
	}
}
