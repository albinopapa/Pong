#pragma once

#include "Paddle.h"
#include "Scoreboard.h"

class Player
{
public:

	Player() = default;
	Player( const std::string &PlayerName, int ScoreX, int ScoreY );
	
	virtual ~Player() = default;

	virtual void Update( float DeltaTime, Paddle &Pad ) = 0;
	Scoreboard::Score &GetScore();
protected:
	Scoreboard::Score m_score;
};

