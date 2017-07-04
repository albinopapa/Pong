#pragma once

#include "Graphics.h"
#include <array>
#include <cassert>
#include <string>

class Scoreboard
{
public:
	static constexpr float m_scoreboxWidth = 160.f;
	static constexpr float m_scoreboxHeight = 48.f;
	static constexpr float m_scoreboxX = ( static_cast< float >( Graphics::ScreenWidth ) - m_scoreboxWidth ) * .5f;
	static constexpr float m_scoreboxY = 0.f;
	static constexpr RectF m_scorebox = {
		m_scoreboxX, m_scoreboxY,
		m_scoreboxX + m_scoreboxWidth,
		m_scoreboxY + m_scoreboxHeight };

public:
	class Score
	{
	public:
		Score( const std::string &Player, float X, float Y )
			:
			m_player( Player ),
			m_position( X, Y )
		{
		}
		
		Score &operator++()
		{
			++m_score;
			return *this;
		}
		Score operator++( int )const
		{
			return ( ++Score( *this ) );
		}
		bool operator==( int Val )
		{
			return m_score == Val;
		}
		const std::string &GetName()const
		{
			return m_player;
		}
		void Reset()
		{
			m_score = 0;
		}
		void Draw( Color C, Graphics &Gfx )
		{
			constexpr float m_scoreWidth =  m_scorebox.GetWidth() * .5f;
			constexpr float m_scoreHeight = m_scorebox.GetHeight();
			Gfx.DrawString(
				std::to_wstring( m_score ), RectF{ m_position, m_position + Vec2f(m_scoreWidth, m_scoreHeight) }, C );
		}
	private:
		std::string m_player;
		Vec2f m_position;
		int m_score = 0;
	};
public:
	void RegisterPlayer( Score *pScore )
	{
		assert( m_scoreCount < 2u );
		m_pScores[ m_scoreCount++ ] = pScore;
	}
	void Draw( Graphics &Gfx )
	{
		DrawScoreBox( Gfx );
		DrawScore( Gfx );
	}

private:
	void DrawScoreBox( Graphics &Gfx )
	{
		Gfx.DrawLine(
			m_scorebox.left, m_scorebox.top,
			m_scorebox.right, m_scorebox.top,
			Colors::White );
		Gfx.DrawLine(
			m_scorebox.left, m_scorebox.bottom,
			m_scorebox.right, m_scorebox.bottom,
			Colors::White );
		Gfx.DrawLine(
			m_scorebox.left, m_scorebox.top,
			m_scorebox.left, m_scorebox.bottom,
			Colors::White );
		Gfx.DrawLine(
			m_scorebox.right, m_scorebox.top,
			m_scorebox.right, m_scorebox.bottom,
			Colors::White );
	}
	void DrawScore( Graphics &Gfx )
	{
		float count = 0.f;
		for( auto pScore : m_pScores )
		{
			pScore->Draw( Colors::White, Gfx );
			count = count + 1.f;
		}
	}
private:
	size_t m_scoreCount = 0u;
	std::array<Score *, 2u> m_pScores;
};

