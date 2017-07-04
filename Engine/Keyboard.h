#pragma once
#include <queue>
#include <bitset>
#include "ChiliWin.h"
#undef DELETE

class Keyboard
{
	friend class MainWindow;
public:
	enum Key
	{
		NIL = 0,
		BACKSPACE = VK_BACK,
		TAB = VK_TAB,
		ENTER = VK_RETURN,
		SHIFT = VK_SHIFT,
		CONTROL = VK_CONTROL,
		ESCAPE = VK_ESCAPE,
		SPACE = VK_SPACE,
		PG_UP = VK_PRIOR, 
		PG_DOWN = VK_NEXT,
		END = VK_END,
		HOME = VK_HOME,
		LEFT = VK_LEFT, UP = VK_UP, RIGHT = VK_RIGHT, DOWN = VK_DOWN,
		DELETE = VK_DELETE,

		_0 = '0', _1 = '1', _2 = '2', _3 = '3', _4 = '4', 
		_5 = '5', _6 = '6', _7 = '7', _8 = '8', _9 = '9',
		A = 'A', B = 'B', C = 'C', D = 'D', E = 'E', F = 'F',
		G = 'G', H = 'H', I = 'I', J = 'J', K = 'K', L = 'L', 
		M = 'M', N = 'N', O = 'O', P = 'P', Q = 'Q', R = 'R',
		S = 'S', T = 'T', U = 'U', V = 'V', W = 'W', Y = 'Y', 
		Z = 'Z',
	};
	class Event
	{
	public:
		enum Type
		{
			Press,
			Release,
			Invalid
		};
	private:
		Type type;
		unsigned char code;
	public:
		Event()
			:
			type( Invalid ),
			code( 0u )
		{}
		Event( Type type,unsigned char code )
			:
			type( type ),
			code( code )
		{}
		bool IsPress() const
		{
			return type == Press;
		}
		bool IsRelease() const
		{
			return type == Release;
		}
		bool IsValid() const
		{
			return type != Invalid;
		}
		unsigned char GetCode() const
		{
			return code;
		}
	};
public:
	Keyboard() = default;
	Keyboard( const Keyboard& ) = delete;
	Keyboard& operator=( const Keyboard& ) = delete;
	bool KeyIsPressed( Key keycode ) const;
	Event ReadKey();
	bool KeyIsEmpty() const;
	Key ReadChar();
	bool CharIsEmpty() const;
	void FlushKey();
	void FlushChar();
	void Flush();
	void EnableAutorepeat();
	void DisableAutorepeat();
	bool AutorepeatIsEnabled() const;
private:
	void OnKeyPressed( Key keycode );
	void OnKeyReleased( Key keycode );
	void OnChar( Key character );
	template<typename T>
	void TrimBuffer( std::queue<T>& buffer );
private:
	static constexpr unsigned int nKeys = 256u;
	static constexpr unsigned int bufferSize = 4u;
	bool autorepeatEnabled = false;
	std::bitset<nKeys> keystates;
	std::queue<Event> keybuffer;
	std::queue<Keyboard::Key> charbuffer;
};