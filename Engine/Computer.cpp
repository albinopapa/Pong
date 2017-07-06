#include "Computer.h"
#include "Ball.h"
#include "Board.h"

Computer::Computer( const Ball & B, const std::string &PlayerName, int ScoreX )
	:
	Player( PlayerName, ScoreX, 5 ),
	m_ball( B ),
	m_observer( *this )
{
	
}

void Computer::Update( float DeltaTime, Paddle &Pad )
{
	if( !m_targetSet )
	{
		CalculateTarget( Pad );
		m_targetSet = true;
	}
	const auto padTop = Pad.GetRect().top;
	const auto padBottom = Pad.GetRect().bottom;
	
	if( m_target.y > padBottom )
	{
		Pad.OnMove( Paddle::Direction::DOWN, DeltaTime );
	}
	else if( m_target.y < padTop )
	{
		Pad.OnMove( Paddle::Direction::UP, DeltaTime );
	}
	else
	{
		Pad.OnMove( Paddle::Direction::IDLE, DeltaTime );
		m_targetSet = false;
	}
}

Computer::BounceObserver *Computer::GetObserver()
{
	return &m_observer;
}

void Computer::CalculateTarget( const Paddle &Pad )
{
	const auto ballVel = m_ball.GetVelocity();
	const auto ballPos = m_ball.GetPosition();
	const auto padPos = Pad.GetRect().GetCenter();

	if( ballVel.x > 0.f )
	{
		const auto xDist = padPos.x - ballPos.x;
		const auto yDist = padPos.y - ballPos.y;

		const auto slope = ballVel.y / ballVel.x;
		const auto target = Vec2f{ padPos.x, ballPos.y + ( slope * xDist ) };
		m_target = target;
	}
	else if( ballVel.x < 0.f )
	{
		m_target = { padPos.x, static_cast< float >( Graphics::ScreenHeight >> 1 ) };
	}
}

Computer::BounceObserver::BounceObserver( Computer & Parent )
	:
	m_parent( Parent )
{
}

void Computer::BounceObserver::OnNotify( Board &Brd )
{
	m_parent.CalculateTarget( Brd.GetPaddle( 1 ) );
	m_parent.m_targetSet = true;
}
