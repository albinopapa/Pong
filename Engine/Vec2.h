/******************************************************************************************
*	Chili DirectX Framework Version 16.10.01											  *
*	Vec2.h																				  *
*	Copyright 2016 PlanetChili <http://www.planetchili.net>								  *
*																						  *
*	This file is part of The Chili DirectX Framework.									  *
*																						  *
*	The Chili DirectX Framework is free software: you can redistribute it and/or modify	  *
*	it under the terms of the GNU General Public License as published by				  *
*	the Free Software Foundation, either version 3 of the License, or					  *
*	(at your option) any later version.													  *
*																						  *
*	The Chili DirectX Framework is distributed in the hope that it will be useful,		  *
*	but WITHOUT ANY WARRANTY; without even the implied warranty of						  *
*	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the						  *
*	GNU General Public License for more details.										  *
*																						  *
*	You should have received a copy of the GNU General Public License					  *
*	along with The Chili DirectX Framework.  If not, see <http://www.gnu.org/licenses/>.  *
******************************************************************************************/
#pragma once

#include "ChiliMath.h"

template <typename T>
class _Vec2
{
public:
	constexpr _Vec2()
		:
		x( 0 ), y( 0 )
	{}
	constexpr _Vec2( T x,T y )
		:
		x( x ),
		y( y )
	{}
	constexpr _Vec2( const _Vec2& vect )
		:
		_Vec2( vect.x,vect.y )
	{}
	template <typename T2>
	constexpr explicit operator _Vec2<T2>() const
	{
		return{ (T2)x,(T2)y };
	}
	constexpr T		LenSq() const
	{
		return sq( *this );
	}
	constexpr T		Len() const
	{
		return sqrt( LenSq() );
	}
	_Vec2&	Normalize()
	{
		const auto len = Len();
		*this *= ( len != static_cast< T >( 0 ) ? 1.f / len : len );
		return *this;
	}
	constexpr _Vec2	GetNormalized() const
	{
		return _Vec2( *this ).Normalize();
	}
	constexpr _Vec2	operator-() const
	{
		return _Vec2( -x,-y );
	}
	_Vec2&	operator=( const _Vec2 &rhs )
	{
		x = rhs.x;
		y = rhs.y;
		return *this;
	}
	_Vec2&	operator+=( const _Vec2 &rhs )
	{
		x += rhs.x;
		y += rhs.y;
		return *this;
	}
	_Vec2&	operator-=( const _Vec2 &rhs )
	{
		x -= rhs.x;
		y -= rhs.y;
		return *this;
	}
	constexpr T		operator*( const _Vec2 &rhs ) const
	{
		return x * rhs.x + y * rhs.y;
	}
	constexpr _Vec2	operator+( const _Vec2 &rhs ) const
	{
		return _Vec2( *this ) += rhs;
	}
	constexpr _Vec2	operator-( const _Vec2 &rhs ) const
	{
		return _Vec2( *this ) -= rhs;
	}
	_Vec2&	operator*=( const T &rhs )
	{
		x *= rhs;
		y *= rhs;
		return *this;
	}
	constexpr _Vec2	operator*( const T &rhs ) const
	{
		return _Vec2( *this ) *= rhs;
	}
	_Vec2&	operator/=( const T &rhs )
	{
		x /= rhs;
		y /= rhs;
		return *this;
	}
	constexpr _Vec2	operator/( const T &rhs ) const
	{
		return _Vec2( *this ) /= rhs;
	}
	constexpr bool	operator==( const _Vec2 &rhs ) const
	{
		return x == rhs.x && y == rhs.y;
	}
	constexpr bool	operator!=( const _Vec2 &rhs ) const
	{
		return !(*this == rhs);
	}
	constexpr _Vec2	InterpolateTo( const _Vec2& dest,T alpha ) const
	{
		return *this + (dest - *this) * alpha;
	}
public:
	T x;
	T y;
};

template<class T>
_Vec2<T> Reflect( const _Vec2<T> &Direction, const _Vec2<T> &Normal )
{
	return Direction - ( Normal * ( 2.f * ( Direction * Normal ) ) );
}

typedef _Vec2<float> Vec2;
typedef _Vec2<double> Ved2;
typedef _Vec2<int> Vei2;

using Vec2f = Vec2;
using Vec2i = Vei2;
using Vec2d = Ved2;