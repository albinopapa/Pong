#pragma once

#include "Player.h"
#include "Observer.h"

class Ball;
class Board;
class Computer : public Player
{
public:
	class BounceObserver :public Observer
	{
	public:
		BounceObserver( Computer &Parent );
		void OnNotify( Board &Brd )override;
	private:
		Computer &m_parent;
	};

public:
	Computer( const Ball &B, const std::string &PlayerName, int ScoreX );
	void Update( float DeltaTime, Paddle &Pad )override;
	BounceObserver *GetObserver();

private:
	void CalculateTarget( const Paddle &Pad );
private:
	Vec2f m_target;
	const Ball &m_ball;
	BounceObserver m_observer;
	static constexpr float m_responseDelay = .1f;
	Paddle::Direction m_currentDirection = Paddle::Direction::IDLE;
	float m_responseDelayCounter = 0.f;
	bool m_targetSet = false;
};