#pragma once

#include "Rect.h"
#include "Graphics.h"
#include <random>

class Ball
{
public:
	Ball();

	void Update( float DeltaTime );
	void Draw( Graphics &Gfx )const;
	void IncreaseSpeed();
	void Reset();

	RectF GetRect()const;
	const Vec2f &GetPosition()const;
	const Vec2f &GetVelocity()const;
	void SetPosition( const Vec2f &NewPos );
	void SetVelocity( const Vec2f &NewVel );
	const float GetRadius()const;
		
private:
	static constexpr Vec2f m_startPosition = { 
		static_cast< float >( Graphics::ScreenWidth >> 1 ), 
		static_cast< float >( Graphics::ScreenHeight >> 1 ) };
	static constexpr Vec2f m_maxVelocity = { 5.f * 60.f, 5.f * 60.f };
	static constexpr float m_radius = 10.f;
	
	Vec2f m_position, m_velocity;
};

