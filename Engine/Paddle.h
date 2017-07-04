#pragma once

#include "Keyboard.h"
#include "Rect.h"

class Paddle
{
public:
	enum class Direction
	{
		IDLE, UP, DOWN
	};
	class PaddleInput
	{
	public:
		PaddleInput() = default;
		PaddleInput( Keyboard::Key UpKey, Keyboard::Key DownKey )
			:
			m_upKey( UpKey ), m_downKey( DownKey )
		{
		}
		Direction GetPaddleDirection( Keyboard &Kbd )const
		{
			if( Kbd.KeyIsPressed( m_upKey ) )
			{
				return Direction::UP;
			}
			else if( Kbd.KeyIsPressed( m_downKey ) )
			{
				return Direction::DOWN;
			}
			else
			{
				return Direction::IDLE;
			}
		}
	private:
		Keyboard::Key m_upKey, m_downKey;
	};
public:
	Paddle() = default;
	Paddle( float X );

	void OnMove( Direction Dir, float DeltaTime );
	
	void Draw( class Graphics &Gfx )const;
	void ClampToField( const RectF &Bounds );
	RectF GetRect()const;
	const Vec2f &GetNormal()const;

	void Reset();
private:
	static constexpr float m_width = 10.f;
	static constexpr float m_height = 80.f;
	static constexpr float m_speed = 5.f * 60.f;

	Vec2f m_startPosition;

	Vec2f m_position, m_velocity, m_normal;
	float m_direction = 0.f;
	PaddleInput m_paddleInput;
};

