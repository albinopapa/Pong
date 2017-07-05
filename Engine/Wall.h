#pragma once

#include "Colors.h"
#include "Rect.h"
#include <vector>

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

class WallGenerator
{
public:
	WallGenerator( int Left, int Top, int Width, int Height );
	std::vector<Wall> operator()(const Size_t<int> &MinValues, const Size_t<int> &MaxValues, int Count );

private:
	RectI m_bounds;
};

