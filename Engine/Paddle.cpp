#include "Paddle.h"
#include "ChiliMath.h"
#include "Graphics.h"


Paddle::Paddle( float X )
	:
	m_startPosition( X + ( m_width * .5f ), static_cast< float >( Graphics::ScreenHeight >> 1 ) ),
	m_position( m_startPosition ),
	m_normal( X < static_cast< float >( Graphics::ScreenWidth >> 1 ) ? Vec2f{ 1.f, 0.f } : Vec2f{ -1.f, 0.f } )
{
}

void Paddle::OnMove( Direction Dir, float DeltaTime )
{
	switch( Dir )
	{
		case Direction::IDLE:
			m_direction = 0.f;
			break;
		case Direction::UP:
			m_direction = -1.f;
			break;
		case Direction::DOWN:
			m_direction = 1.f;
			break;
	}

	m_velocity.y = ( m_direction * DeltaTime * m_speed );
	m_position += m_velocity;
}

void Paddle::Draw( Graphics & Gfx )const
{
	Gfx.DrawRectangle( GetRect(), Colors::Blue );
}

void Paddle::ClampToField( const RectF & Bounds )
{
	const auto halfSize = Vec2f( m_width, m_height ) * .5;
	m_position = {
		Clamp( m_position.x, Bounds.left + halfSize.x, Bounds.right - halfSize.x ),
		Clamp( m_position.y, Bounds.top + halfSize.y, Bounds.bottom - halfSize.y )
	};	
}

RectF Paddle::GetRect() const
{
	const Vec2f half = { m_width * .5f, m_height * .5f };
	return RectF( m_position - half, m_position + half );
}

const Vec2f & Paddle::GetNormal() const
{
	return m_normal;
}

void Paddle::Reset()
{
	m_position = m_startPosition;
}
