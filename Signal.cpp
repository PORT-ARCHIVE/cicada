// © 2016 PORT INC.

#include <csignal>

namespace Signal {

	volatile sig_atomic_t flg {0};
	sig_atomic_t getFlg() { return flg; }
	void handler(sig_atomic_t arg) { flg = 0x1; }
}
