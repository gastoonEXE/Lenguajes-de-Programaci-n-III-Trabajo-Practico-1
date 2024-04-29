#include <stdio.h>
#include <signal.h>
#include <unistd.h>

void signal_handler(int signal) {
    switch (signal) {
        case SIGINT:
            printf("Recibida señal SIGINT\n");
            break;
        case SIGALRM:
            printf("Recibida señal SIGALRM\n");
            break;
        case SIGUSR1:
            printf("Recibida señal SIGUSR1\n");
            break;
        case SIGUSR2:
            printf("Recibida señal SIGUSR2\n");
            break;
        case SIGSEGV:
            printf("Recibida señal SIGSEGV\n");
            break;
        case SIGPIPE:
            printf("Recibida señal SIGPIPE\n");
            break;
        case SIGCHLD:
            printf("Recibida señal SIGCHLD\n");
            break;
        case SIGFPE:
            printf("Recibida señal SIGFPE\n");
            break;
        default:
            printf("Recibida señal desconocida\n");
            break;
    }
}

int main() {
    signal(SIGINT, signal_handler);
    signal(SIGALRM, signal_handler);
    signal(SIGUSR1, signal_handler);
    signal(SIGUSR2, signal_handler);
    signal(SIGSEGV, signal_handler);
    signal(SIGPIPE, signal_handler);
    signal(SIGCHLD, signal_handler);
    signal(SIGFPE, signal_handler);

    // El proceso se mantiene en ejecución para recibir señales
    while (1) {
        pause();  // Pausa el proceso hasta que se reciba una señal
    }

    return 0;
}

