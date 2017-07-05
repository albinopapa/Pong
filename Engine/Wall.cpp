#include "Wall.h"
#include "Graphics.h"
#include <random>

Wall::Wall( const RectF &Rect )
	:
	m_surface( Rect )
{
}

const RectF & Wall::GetRect() const
{
	return m_surface;
}

Vec2f Wall::GetNormal( const Vec2f & Point ) const
{
	auto IsAboveOrBelow = [ this, &Point ]
	{
		return Point.y < m_surface.top || Point.y > m_surface.bottom;
	};
	auto IsLeftOrRight = [ this, &Point ]
	{
		return Point.x < m_surface.left || Point.x > m_surface.right;
	};

	if( Point.x < m_surface.left )
	{
		if( IsAboveOrBelow() )
		{
			return ( Point - m_surface.GetCenter() ).Normalize();
		}

		return{ -1.f, 0.f };
	}
	else if( Point.x > m_surface.right )
	{
		if( IsAboveOrBelow() )
		{
			return ( Point - m_surface.GetCenter() ).Normalize();
		}

		return{ 1.f, 0.f };
	}	
	else if( Point.y < m_surface.top )
	{
		if( IsLeftOrRight() )
		{
			return ( Point - m_surface.GetCenter() ).Normalize();
		}

		return{ 0.f, -1.f };
	}
	else if( Point.y > m_surface.bottom )
	{
		if( IsLeftOrRight() )
		{
			return ( Point - m_surface.GetCenter() ).Normalize();
		}

		return{ 0.f, 1.f };
	}
}

void Wall::Draw( Graphics & Gfx ) const
{
	Gfx.DrawRectangle( m_surface, m_color );
}

WallGenerator::WallGenerator( int Left, int Top, int Width, int Height )
	:
	m_bounds( Left, Top, Left + Width, Top + Height )
{
}

std::vector<Wall> WallGenerator::operator()( const Size_t<int>& MinValues, const Size_t<int>& MaxValues, int Count )
{
	std::random_device rd;
	std::mt19937 rng( rd() );
	std::uniform_int_distribution<int> wDist( MinValues.width, MaxValues.width );
	std::uniform_int_distribution<int> hDist( MinValues.height, MaxValues.height );
	std::uniform_real_distribution<float> xDist( m_bounds.left, m_bounds.right );
	std::uniform_real_distribution<float> yDist( m_bounds.top, m_bounds.bottom );

	auto MakeWall = [&wDist, &hDist, &xDist, &yDist, &rng, this]()
	{
		bool recheck = true;
		const auto randPos = Vec2f{ xDist( rng ), yDist( rng ) };
		Size_t<int> randSize;
		while( recheck )
		{
			randSize = Size_t<int>{ wDist( rng ), hDist( rng ) };
			recheck =
				randPos.x + static_cast< float >( randSize.width )  > m_bounds.right ||
				randPos.y + static_cast< float >( randSize.height ) > m_bounds.bottom ||
				randSize.width == randSize.height;
		}
		return Wall( { randPos.x, randPos.y, static_cast< Size_t<float> >( randSize ) } );
	};

	std::vector<Wall> walls( Count );
	for( size_t i = 0; i < Count; ++i )
	{
		walls[i] = MakeWall();
	}

	return walls;
}
