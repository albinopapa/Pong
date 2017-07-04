#pragma once

#include "Player.h"

class Keyboard;
class Human : public Player
{
public:
	Human(
		Keyboard &Kbd, 
		const std::string &PlayerName, 
		int ScoreX, 
		Paddle::PaddleInput PadInput );

	void Update( float DeltaTime, Paddle &Pad )override;
private:
	Keyboard &m_keyboard;
	Paddle::PaddleInput m_padInput;
};

