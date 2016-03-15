// Copyright:: Copyright 2016 PORT INC.
// Author:: Jiro Iwai

#include <iostream>
#include <DebugOut.hpp>

static Debug* debug = nullptr;

Debug& Debug::out()
{
	if( !debug ) {
		debug = new Debug();
	}

	return *debug;
}

void Debug::on()
{
	out().flg = true;
}

void Debug::off()
{
	out().flg = false;
}
