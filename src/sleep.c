#include <unistd.h>

#include "sleep.h"

#ifdef _WIN32
	void csleep(unsigned millis) {
		Sleep(millis);
	}
#else
	void csleep(unsigned millis) {
		usleep(millis * 1000);
	}
#endif
