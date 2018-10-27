#ifndef WIN32_THREADING
#define WIN32_THREADING

#include "threading.h"

DWORD WINAPI ThreadProc(LPVOID lp_parameter)
{
	printf("Thread started!\n");
	return 0;
}

static void create_thread(ThreadState &thread_state, void *parameters)
{
	DWORD thread_id;
	HANDLE thread_handle = CreateThread(0, 0, ThreadProc, parameters, 0, &thread_id);
}

#endif
