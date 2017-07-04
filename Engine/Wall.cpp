#include "Wall.h"
#include "Graphics.h"


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
