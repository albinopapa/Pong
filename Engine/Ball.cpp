#include "Ball.h"

Ball::Ball(  )
{
	Reset();
}

void Ball::Update( float DeltaTime )
{
	m_position += ( m_velocity * DeltaTime );
}

void Ball::Draw( Graphics & Gfx )const
{
	Gfx.DrawDisc( m_position, m_radius, Colors::Red );
}

void Ball::IncreaseSpeed()
{
	m_velocity *= ( m_velocity.LenSq() < m_maxVelocity.LenSq() ) ? 1.25f : 1.f;
}

const Vec2f & Ball::GetPosition() const
{
	return m_position;
}

const Vec2f & Ball::GetVelocity() const
{
	return m_velocity;
}

void Ball::SetPosition( const Vec2f &NewPos )
{
	m_position = NewPos;
}

void Ball::SetVelocity( const Vec2f &NewVel )
{
	m_velocity = NewVel;
}

const float Ball::GetRadius() const
{
	return m_radius;
}

void Ball::Reset()
{
	m_position = m_startPosition;

	std::random_device rd;
	std::mt19937 rng( rd() );
	std::uniform_real_distribution<float> xVel( -1.f, 1.f );
	std::uniform_real_distribution<float> yVel( -1.f, 1.f );

	bool recheck = true;
	Vec2f randVec;
	while( recheck )
	{		
		randVec = Vec2f{ xVel( rng ), yVel( rng ) }.Normalize();
		recheck =
			fabs( Vec2f{ 1.f, 0.f } *randVec ) >= .8f ||
			fabs( Vec2f{ -1.f, 0.f } *randVec ) >= .8f ||
			fabs( Vec2f{ 0.f, 1.f } *randVec ) >= .8f ||
			fabs( Vec2f{ 0.f, -1.f } *randVec ) >= .8f;
	};
	m_velocity = randVec * 100.f;
}

RectF Ball::GetRect() const
{
	const Vec2f radVec = { m_radius, m_radius };
	return RectF( m_position - radVec, m_position + radVec );
}
