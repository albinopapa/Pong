#include "Human.h"

Human::Human( Keyboard & Kbd, const std::string &PlayerName, int ScoreX, Paddle::PaddleInput PadInput )
	:
	Player( PlayerName, ScoreX, 5 ),
	m_keyboard( Kbd ),
	m_padInput( PadInput )
{
}

void Human::Update( float DeltaTime, Paddle &Pad )
{
	const auto input = m_padInput.GetPaddleDirection( m_keyboard );
	if( input == Paddle::Direction::UP)
	{
		Pad.OnMove( Paddle::Direction::UP, DeltaTime );
	}
	else if( input == Paddle::Direction::DOWN  )
	{
		Pad.OnMove( Paddle::Direction::DOWN, DeltaTime );
	}
	else
	{
		Pad.OnMove( Paddle::Direction::IDLE, DeltaTime );
	}

}
