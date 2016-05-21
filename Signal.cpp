// © 2016 PORT INC.

#include <csignal>

namespace Signal {

	volatile std::sig_atomic_t flg {0};
	std::sig_atomic_t getFlg() { return flg; }
	void handler(std::sig_atomic_t arg) { flg = 0x1; }
}
