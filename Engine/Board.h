#pragma once

#include "Ball.h"
#include "Graphics.h"
#include "Player.h"
#include "Scoreboard.h"
#include "Wall.h"
#include <array>

class Board
{
public:
	Board();

	void Update( float DeltaTime, std::vector<std::unique_ptr<Player>> &pPlayers );
	void HandleCollisions();
	bool BallOutOfBounds( std::vector<std::unique_ptr<Player>> &pPlayers );

	void BuildLevel();
	void Reset();
	const Ball&GetBall()const;
	
	size_t PaddleCount()const;
	const Paddle &GetPaddle( size_t index )const;

	size_t WallCount()const;
	const Wall &GetWall( size_t index )const;
private:
	// Ball vs Paddle/Wall Collision
	bool BallIsCollidingWith( const RectF &BallRect, const RectF &Rect )const;
	void DoRebound( const RectF &Surface, const Vec2f &Normal );

	// Check if ball is in bounds
	bool BallIsInBounds( const RectF &BallRect, const RectF &Bounds )const;

	Vec2f CalculateNormalFrom( const RectF &Rect, const Vec2f &Point );

private:
	Ball m_ball;
	std::array<Paddle, 2> m_paddles;

	std::array<Wall, 4> m_walls;
public:
	static constexpr float m_boundsLeft = 0.f;
	static constexpr float m_boundsTop = 0.f;
	static constexpr float m_boundsRight = static_cast< float >( Graphics::ScreenWidth );
	static constexpr float m_boundsBottom = static_cast< float >( Graphics::ScreenHeight );
	static constexpr float m_wallThickness = 5.f;

	static constexpr RectF m_bounds = { m_boundsLeft, m_boundsTop, m_boundsRight, m_boundsBottom };

};

