#include "Board.h"
#include "Player.h"

Paddle MakePaddle( float YPos  )
{
	return Paddle( YPos );
}

Board::Board()
	:
	m_paddles{ MakePaddle( m_wallThickness ), MakePaddle(m_boundsRight - m_wallThickness - 10.f ) }
{
}

void Board::Update( float DeltaTime, std::vector<std::unique_ptr<Player>> &pPlayers )
{
	for( size_t i = 0; i < pPlayers.size(); ++i)
	{
		auto &paddle = m_paddles[ i ];

		pPlayers[ i ]->Update( DeltaTime, paddle );
		paddle.ClampToField( m_bounds );
	}

	m_ball.Update( DeltaTime );
}

void Board::HandleCollisions()
{
	const auto ballRect = m_ball.GetRect();
	for( auto &wall : m_walls )
	{
		if( BallIsCollidingWith( ballRect, wall.GetRect() ) )
		{
			const auto wallrect = wall.GetRect();
			const auto normal = CalculateNormalFrom( wallrect, m_ball.GetPosition() );

			DoRebound( wallrect, normal );
		}
	}

	for( auto &paddle : m_paddles )
	{
		if( BallIsCollidingWith( ballRect, paddle.GetRect() ) )
		{
			DoRebound( paddle.GetRect(), paddle.GetNormal() );
			m_ball.IncreaseSpeed();
		}
	}
}

bool Board::BallOutOfBounds( std::vector<std::unique_ptr<Player>> &pPlayers )
{
	const auto ballRect = m_ball.GetRect();
	bool outBounds = false;
	if( !BallIsInBounds( ballRect, m_bounds ) )
	{
		outBounds = true;
		if( ballRect.left > m_bounds.GetCenter().x )
		{
			++pPlayers[ 0 ]->GetScore();
		}
		else
		{
			++pPlayers[ 1 ]->GetScore();
		}
	}

	return outBounds;
}

bool Board::BallIsCollidingWith( const RectF &BallRect, const RectF & Rect ) const
{
	return Rect.Overlaps( BallRect );
}

void Board::DoRebound( const RectF & Surface, const Vec2f & Normal )
{
	auto ballPos = m_ball.GetPosition();
	auto ballVel = m_ball.GetVelocity();
	const auto ballRad = m_ball.GetRadius();
	ballVel = Reflect( ballVel, Normal );

	// Top collision, push ball up
	if( ballPos.x > Surface.left && ballPos.x < Surface.right )
	{
		if( ballPos.y < Surface.top )
		{
			ballPos.y += ( Surface.top - ( ballPos.y + ballRad ) );
		}
		else if( ballPos.y > Surface.bottom )
		{
			ballPos.y += ( Surface.bottom - ( ballPos.y - ballRad ) );
		}
	}
	if( ballPos.y > Surface.top && ballPos.y < Surface.bottom )
	{
		if( ballPos.x < Surface.left )
		{
			ballPos.x += ( Surface.left - ( ballPos.x + ballRad ) );
		}
		else if( ballPos.x > Surface.right )
		{
			ballPos.x += ( Surface.right - ( ballPos.x - ballRad ) );
		}
	}

	m_ball.SetPosition( ballPos );
	m_ball.SetVelocity( ballVel );
}

bool Board::BallIsInBounds( const RectF &BallRect, const RectF & Bounds ) const
{
	return Bounds.Contains( BallRect );
}

void Board::BuildLevel()
{
	// 0 = Top wall, 1 = bottom wall
	m_walls[ 0 ] = { RectF{ 0.f, 0.f, m_boundsRight, m_wallThickness } };
	m_walls[ 1 ] = {
	RectF{ m_wallThickness, m_boundsBottom - m_wallThickness, m_boundsRight, m_boundsBottom }
	};

	// Obstacles
	m_walls[ 2 ] = { RectF( 100.f, 100.f, 100.f + m_wallThickness, 200.f ) };
	m_walls[ 3 ] = { RectF(
	Vec2f{ ( m_boundsRight - 100.f ), ( m_boundsBottom - 200.f ) },
	Vec2f{ ( m_boundsRight - 100.f ) + m_wallThickness, ( m_boundsBottom - 200.f ) - 100.f } ) };
}

void Board::Reset()
{
	m_ball.Reset();
	for( auto &paddle : m_paddles )
	{
		paddle.Reset();
	}
}

const Ball & Board::GetBall() const
{
	return m_ball;
}

size_t Board::PaddleCount() const
{
	return m_paddles.size();
}

const Paddle & Board::GetPaddle( size_t index ) const
{
	return m_paddles[ index ];

}

size_t Board::WallCount() const
{
	return m_walls.size();
}

const Wall & Board::GetWall( size_t index ) const
{
	return m_walls[ index ];
}

Vec2f Board::CalculateNormalFrom( const RectF & Rect, const Vec2f & Point )
{
	auto IsAboveOrBelow = [ &Rect, &Point ]
	{
		return Point.y < Rect.top || Point.y > Rect.bottom;
	};
	auto IsLeftOrRight = [ &Rect, &Point ]
	{
		return Point.x < Rect.left || Point.x > Rect.right;
	};

	if( Point.x < Rect.left )
	{
		if( IsAboveOrBelow() )
		{
			return ( Point - Rect.GetCenter() ).Normalize();
		}

		return{ -1.f, 0.f };
	}
	else if( Point.x > Rect.right )
	{
		if( IsAboveOrBelow() )
		{
			return ( Point - Rect.GetCenter() ).Normalize();
		}

		return{ 1.f, 0.f };
	}
	else if( Point.y < Rect.top )
	{
		if( IsLeftOrRight() )
		{
			return ( Point - Rect.GetCenter() ).Normalize();
		}

		return{ 0.f, -1.f };
	}
	else if( Point.y > Rect.bottom )
	{
		if( IsLeftOrRight() )
		{
			return ( Point - Rect.GetCenter() ).Normalize();
		}

		return{ 0.f, 1.f };
	}
}

