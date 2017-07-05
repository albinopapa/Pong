#include "MainWindow.h"
#include "Game.h"
#include "Human.h"
#include "Computer.h"

Game::Game( MainWindow& wnd )
	:
	wnd( wnd ),
	gfx( wnd ),
	m_pPlayers( 2 ),
	m_boardview( m_scoreboard, m_board )
{
	m_board.BuildLevel();
}

void Game::Go()
{
	gfx.BeginFrame();
	UpdateModel();
	ComposeFrame();
	gfx.EndFrame();
}

void Game::UpdateModel()
{
	const float dt = ft.Mark();
	CheckForQuit();
	switch( m_state )
	{
		case State::Title:
			if( wnd.kbd.KeyIsPressed( Keyboard::ENTER ) )
			{
				m_state = State::Setup;
			}
			break;
		case State::Setup:
			SetupGame();
			break;
		case State::PlayGame:
		{
			m_board.Update( dt, m_pPlayers );
			m_board.HandleCollisions();
			if( m_board.BallOutOfBounds( m_pPlayers ) )
			{
				m_board.Reset();
			}
			for( auto &pPlayer : m_pPlayers )
			{
				if( pPlayer->GetScore() == 10 )
				{
					m_pWinner = &pPlayer->GetScore();
					m_state = State::GameOverWin;
				}
			}
			break;
		}
		case State::GameOverWin:
			if( wnd.kbd.KeyIsPressed( Keyboard::ENTER ) )
			{
				m_state = State::GameOver;
			}
			break;
		case State::GameOver:
			m_board.Reset();
			for( auto &pPlayer : m_pPlayers )
			{
				pPlayer->GetScore().Reset();
			}

			m_state = State::Title;
			break;
	}

}

void Game::CheckForQuit()
{
	while( !wnd.kbd.KeyIsEmpty() )
	{
		const auto e = wnd.kbd.ReadKey();
		if( e.GetCode() == Keyboard::ESCAPE && e.IsPress() )
		{
			wnd.Kill();
		}
	}
}

void Game::SetupGame()
{
	while( !wnd.kbd.CharIsEmpty() )
	{
		const auto key = static_cast< char >( wnd.kbd.ReadChar() );
		if( key == '1' || key == '2' )
		{
			m_numPlayers = static_cast< int >( key - '0' );

			switch( m_numPlayers )
			{
				case 1:
				{
					m_pPlayers[ 0 ] =
						std::make_unique<Human>(
							wnd.kbd,
							"Player1",
							Scoreboard::m_scoreboxX + 5.f + 64.f,
							Paddle::PaddleInput( Keyboard::W, Keyboard::S )
							);
					m_pPlayers[ 1 ] =
						std::make_unique<Computer>(
							m_board.GetBall(),
							"Computer",
							Scoreboard::m_scoreboxX + 5.f + ( 64.f * 2.f )
							);
					break;
				}
				case 2:
				{
					m_pPlayers[ 0 ] =
						std::make_unique<Human>(
							wnd.kbd,
							"Player1",
							Scoreboard::m_scoreboxX + 5.f + 64.f,
							Paddle::PaddleInput( Keyboard::W, Keyboard::S )
							);
					m_pPlayers[ 1 ] =
						std::make_unique<Human>(
							wnd.kbd,
							"Player2",
							Scoreboard::m_scoreboxX + 5.f + ( 64.f * 2.f ),
							Paddle::PaddleInput( Keyboard::UP, Keyboard::DOWN )
							);
					break;
				}
			}

			for( auto &pPlayer : m_pPlayers )
			{
				m_scoreboard.RegisterPlayer( &pPlayer->GetScore() );
			}

			response.push_back( key );
			m_state = State::PlayGame;
		}
	}	
}

void Game::ComposeFrame()
{
	switch( m_state )
	{
		case State::PlayGame:
			m_boardview.Draw( gfx );
			break;
		case State::Title:
		{
			// TODO: Create intro screen
			constexpr auto left = static_cast< float >( Graphics::ScreenWidth >> 2 );
			constexpr auto top = static_cast< float >( ( Graphics::ScreenHeight >> 2 ) + 64 );
			constexpr auto right = left + ( left * 2.f);
			constexpr auto bottom = top + top;
			constexpr RectF textRect{ left, top, right, bottom };

			gfx.DrawString( L"Press ENTER to start.", textRect, Colors::White );
			break;
		}
		case State::Setup:
		{
			const std::wstring wResponse =
				std::wstring( L"How many players? ( 1 or 2 ) " ) +
				std::wstring( response.begin(), response.end() );

			constexpr auto left = static_cast< float >( Graphics::ScreenWidth >> 2 );
			constexpr auto top = static_cast< float >( ( Graphics::ScreenHeight >> 2 ) + 64 );
			constexpr auto right = left + ( left * 2.f );
			constexpr auto bottom = top + top;
			constexpr RectF textRect{ left, top, right, bottom };

			gfx.DrawString( wResponse, textRect, Colors::White );

			break;
		}
		case State::GameOverWin:
		{
			// TODO: Create intro screen
			constexpr auto left = static_cast< float >( Graphics::ScreenWidth >> 2 );
			constexpr auto top = static_cast< float >( ( ( Graphics::ScreenHeight >> 2 ) ) + 64 );
			constexpr auto right = left + ( left * 2.f );
			constexpr auto bottom = top + top;
			constexpr RectF textRect{ left, top, right, bottom };

			const auto wplayername = 
				std::wstring( m_pWinner->GetName().begin(), m_pWinner->GetName().end() ) + 
				std::wstring( L" WINS!!!\n\nPress ENTER to play again, ESC to exit." );
			gfx.DrawString( wplayername, textRect, Colors::White );

			break;
		}
		case State::GameOver:
			// TODO: Create reset message
			break;
	}
}