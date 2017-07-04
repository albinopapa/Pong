#include "Player.h"

Player::Player( const std::string & PlayerName, int ScoreX, int ScoreY )
	:
	m_score( PlayerName, ScoreX, ScoreY )
{

}

Scoreboard::Score & Player::GetScore()
{
	return m_score;
}
