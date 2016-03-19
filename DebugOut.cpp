// © 2016 PORT INC.

#include <iostream>
#include <DebugOut.hpp>

static Debug* debug = nullptr;

Debug& Debug::out(int level)
{
	if( !debug ) {
		debug = new Debug();
	}

	debug->setLLevel(level);

	return *debug;
}

void Debug::setLevel(int level)
{
	if( !debug ) {
		debug = new Debug();
	}

	debug->setGLevel(level);
}

// void Debug::on()
// {
// 	out().flg = true;
// }

// void Debug::off()
// {
// 	out().flg = false;
// }
