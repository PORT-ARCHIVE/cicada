// © 2016 PORT INC.

#ifndef SEMI_CRF_SIGNAL__HPP
#define SEMI_CRF_SIGNAL__HPP

#include <csignal>

namespace Signal {

	std::sig_atomic_t getFlg();
	void handler(std::sig_atomic_t arg);
}


#endif // SEMI_CRF_SIGNAL__HPP
