/****************************************************************************************** 
 *	Chili DirectX Framework Version 16.10.01											  *	
 *	Keyboard.cpp																		  *
 *	Copyright 2016 PlanetChili.net <http://www.planetchili.net>							  *
 *																						  *
 *	This file is part of The Chili DirectX Framework.									  *
 *																						  *
 *	The Chili DirectX Framework is free software: you can redistribute it and/or modify	  *
 *	it under the terms of the GNU General Public License as published by				  *
 *	the Free Software Foundation, either version 3 of the License, or					  *
 *	(at your option) any later version.													  *
 *																						  *
 *	The Chili DirectX Framework is distributed in the hope that it will be useful,		  *
 *	but WITHOUT ANY WARRANTY; without even the implied warranty of						  *
 *	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the						  *
 *	GNU General Public License for more details.										  *
 *																						  *
 *	You should have received a copy of the GNU General Public License					  *
 *	along with The Chili DirectX Framework.  If not, see <http://www.gnu.org/licenses/>.  *
 ******************************************************************************************/
#include "Keyboard.h"

bool Keyboard::KeyIsPressed( Key keycode ) const
{
	return keystates[keycode];
}

Keyboard::Event Keyboard::ReadKey()
{
	if( keybuffer.size() > 0u )
	{
		Keyboard::Event e = keybuffer.front();
		keybuffer.pop();
		return e;
	}
	else
	{
		return Keyboard::Event();
	}
}

bool Keyboard::KeyIsEmpty() const
{
	return keybuffer.empty();
}

Keyboard::Key Keyboard::ReadChar()
{
	if( charbuffer.size() > 0u )
	{
		const auto charcode = charbuffer.front();
		charbuffer.pop();
		return charcode;
	}
	else
	{
		return NIL;
	}
}

bool Keyboard::CharIsEmpty() const
{
	return charbuffer.empty();
}

void Keyboard::FlushKey()
{
	std::swap( keybuffer,std::queue<Event>() );
}

void Keyboard::FlushChar()
{
	std::swap( charbuffer,std::queue<Key>() );
}

void Keyboard::Flush()
{
	FlushKey();
	FlushChar();
}

void Keyboard::EnableAutorepeat()
{
	autorepeatEnabled = true;
}

void Keyboard::DisableAutorepeat()
{
	autorepeatEnabled = false;
}

bool Keyboard::AutorepeatIsEnabled() const
{
	return autorepeatEnabled;
}

void Keyboard::OnKeyPressed( Key keycode )
{
	keystates[ keycode ] = true;	
	keybuffer.push( Keyboard::Event( Keyboard::Event::Press,keycode ) );
	TrimBuffer( keybuffer );
}

void Keyboard::OnKeyReleased( Key keycode )
{
	keystates[ keycode ] = false;
	keybuffer.push( Keyboard::Event( Keyboard::Event::Release,keycode ) );
	TrimBuffer( keybuffer );
}

void Keyboard::OnChar( Key character )
{
	charbuffer.push( character );
	TrimBuffer( charbuffer );
}

template<typename Ty>
void Keyboard::TrimBuffer( std::queue<Ty>& buffer )
{
	while( buffer.size() > bufferSize )
	{
		buffer.pop();
	}
}

