#pragma once

#include "Rect.h"
#include "Colors.h"

class Wall
{
public:
	Wall() = default;
	Wall(const RectF &Rect);

	const RectF &GetRect()const;
	Vec2f GetNormal( const Vec2f &Point )const;
	void Draw( class Graphics &Gfx )const;
private:
	RectF m_surface;
	static constexpr Color m_color = Colors::White;

};

