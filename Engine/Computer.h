#pragma once

#include "Player.h"

class Ball;
class Computer : public Player
{
public:
	Computer( const Ball &B, const std::string &PlayerName, int ScoreX );
	void Update( float DeltaTime, Paddle &Pad )override;

private:
	const Ball &m_ball;
};