#ifndef SABAKURAI_SLEEP
#define SABAKURAI_SLEEP

#ifdef __cplusplus
	extern "C" void csleep(unsigned millis);
#else
	extern void csleep(unsigned millis);
#endif

#endif // SABAKURAI_SLEEP
