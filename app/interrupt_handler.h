#pragma once
#include <csignal>

extern sig_atomic_t global_status_flag = 0;

#if defined(_WIN64) || defined(_WIN32)
#include <windows.h>
BOOL WINAPI interrupt_handler(DWORD fdwCtrlType) {
    switch (fdwCtrlType) {
    case CTRL_C_EVENT:
		if(global_status_flag++ > 0)
			exit(EXIT_FAILURE);
        return TRUE;

    default:
        return FALSE;
    }
}

inline void install_handler() { SetConsoleCtrlHandler(interrupt_handler, TRUE); }

#elif defined(__APPLE__) || defined(__linux) || defined(__unix) || defined(__posix)
#include <unistd.h>

void interrupt_handler(int signal_num) {
    if (signal_num == SIGINT) {
        global_status_flag = 1;
    }
}

inline void install_handler() {

    struct sigaction sigIntHandler;

    sigIntHandler.sa_handler = interrupt_handler;
    sigemptyset(&sigIntHandler.sa_mask);
    sigIntHandler.sa_flags = 0;

    sigaction(SIGINT, &sigIntHandler, NULL);
}
#endif