#include "Board.h"
#include "Player.h"
#include <algorithm>

Paddle MakePaddle( float YPos  )
{
	return Paddle( YPos );
}

Board::Board()
	:
	m_paddles{ MakePaddle( m_wallThickness ), MakePaddle(m_boundsRight - m_wallThickness - 10.f ) }
{
}

void Board::AddObserver( Observer * pObserver )
{
	m_pObservers.push_back( pObserver );
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
	bool colliding = false;
	const auto ballRect = m_ball.GetRect();
	const auto ballPos = m_ball.GetPosition();

	for( auto &wall : m_walls )
	{
		if( BallIsCollidingWith( ballRect, wall.GetRect() ) )
		{
			const auto wallrect = wall.GetRect();
			const auto normal = CalculateNormalFrom( wallrect, ballPos );

			DoRebound( wallrect, normal );
			colliding = true;
		}
	}

	if( !colliding )
	{
		for( auto &paddle : m_paddles )
		{
			const auto padRect = paddle.GetRect();
			if( BallIsCollidingWith( ballRect, padRect ) )
			{
				DoRebound( padRect, CalculateNormalFrom( padRect, ballPos ) );
				m_ball.IncreaseSpeed();
				colliding = true;
			}
		}
	}

	if( colliding )
	{
		for( auto &pObserver : m_pObservers )
		{
			pObserver->OnNotify( *this );
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
	// Obstacles
	constexpr auto left = 100;
	constexpr auto top = 100;
	constexpr auto right = Graphics::ScreenWidth - 100;
	constexpr auto bottom = Graphics::ScreenHeight - 100;

	WallGenerator gen( left, top, right - left, bottom - top );
	const auto walls = gen( Size_t<int>{ 10, 10 }, Size_t<int>{ 20, 20 }, 4 );
	m_walls.resize( 2 );

	// 0 = Top wall, 1 = bottom wall
	m_walls[ 0 ] = { RectF{ 0.f, 0.f, m_boundsRight, m_wallThickness } };
	m_walls[ 1 ] = {
		RectF{ m_wallThickness, m_boundsBottom - m_wallThickness, m_boundsRight, m_boundsBottom }
	};
	m_walls.insert( m_walls.begin() + 2, walls.begin(), walls.end() );
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

