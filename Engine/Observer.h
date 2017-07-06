#pragma once

class Observer
{
public:
	virtual void OnNotify( class Board &Brd ) = 0;
};


// Computer needs to know when ball is redirected
// Computer needs to know balls velocity
// Computer needs to know balls position

// Audio needs to know when ball is redirected

