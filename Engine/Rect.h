#pragma once

#include "Vec2.h"
#include <algorithm>
#undef min
#undef max

template < typename T >
class _Rect
{
public:
	constexpr _Rect() :left( 0 ), top( 0 ), right( 0 ), bottom( 0 ) {}
	constexpr _Rect( T Left,T Top,T Right,T Bottom)
		:
	top( Top ),
	bottom( Bottom ),
	left( Left ),
	right( Right )
	{}
	constexpr _Rect( const _Rect& rect )
		:
	top( rect.top ),
	bottom( rect.bottom ),
	left( rect.left ),
	right( rect.right )
	{}
	constexpr _Rect( const _Vec2<T> &p0, const _Vec2<T> &p1 )
		:
		_Rect( std::min( p0.x, p1.x ), std::min( p0.y, p1.y ),
			   std::max( p0.x, p1.x ), std::max( p0.y, p1.y ) )
	{}
	void Translate( const _Vec2<T> &d )
	{
		*this = { _Vec2<T>( left, top ) + d, _Vec2<T>( right, bottom ) + d };
	}
	void Translate( T dx,T dy )
	{
		Translate( { dx, dy } );
	}
	template <typename T2>
	operator _Rect<T2>() const
	{
		return { (T2)top,(T2)bottom,(T2)left,(T2)right };
	}
	void ClipTo( const _Rect& rect )
	{
		top = std::max( top,rect.top );
		bottom = std::min( bottom,rect.bottom );
		left = std::max( left,rect.left );
		right = std::min( right,rect.right );
	}
	constexpr T GetWidth() const
	{
		return right - left;
	}
	constexpr T GetHeight() const
	{
		return bottom - top;
	}
	constexpr _Vec2<T> GetCenter()const
	{
		return _Vec2<T>( left + ( GetWidth() * .5f ), top + ( GetHeight() * .5f ) );
	}
	constexpr bool Overlaps( const _Rect& rect ) const
	{
		return top < rect.bottom && bottom > rect.top && 
			left < rect.right && right > rect.left;
	}
	template <typename T2>
	constexpr bool Contains( const _Vec2<T2> &p ) const
	{
		return 
			( ( p.y >= top ) && ( p.y <= bottom ) ) &&
			( ( p.x >= left ) && ( p.x <= right ) );
	}
	template <typename T2>
	constexpr bool Contains( const _Rect<T2> &p ) const
	{
		return 
			( ( p.top >= top ) && ( p.bottom <= bottom ) ) &&
			( ( p.left >= left ) && ( p.right <= right ) );
	}
public:
	T left, top, right, bottom;
};

typedef _Rect< int > RectI;
typedef _Rect< float > RectF;