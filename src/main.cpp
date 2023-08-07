#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <sys/stat.h>
#include <fstream>
#include <signal.h>
#include <string>

const char *get_pid_file_path() {
    // use /var/run/user/<uid>/daemon.pid
    static char path[1024];
#if defined(__linux__)
    uid_t uid = getuid();
    sprintf(path, "/var/run/user/%d/daemon.pid", uid);
    return path;
#elif defined(__APPLE__)
    int uid = getuid();
    return "/var/run/daemon.pid";
#elif defined(__unix__)
    return "/var/run/daemon.pid";
#elif defined(_WIN32) || defined(WIN32)
    return "C:\\daemon.pid";
#endif
}

void daemon() {
    pid_t pid = fork();
    if (pid < 0) {
        perror("fork failed\n");
        exit(EXIT_FAILURE);
    }
    if (pid > 0) {
        // this is the parent process
        exit(EXIT_SUCCESS);
    }

    // output pid
    printf("pid: %d\n", getpid());

    // this is the child process
    // do not use stdout, stderr, stdin
    // use syslog instead
    umask(0200);

    pid_t sid = setsid();
    if (sid < 0) {
        perror("setsid");
        exit(EXIT_FAILURE);
    }

    if ((chdir("/")) < 0) {
        perror("chdir\n");
        exit(EXIT_FAILURE);
    }
    pid = -1;

    std::ifstream file(get_pid_file_path());
    if (file >> pid) {
        // pid file exists
        // check if process is running
        printf("prev pid: %d\n", pid);

        if (kill(pid, 0) == 0) {
            // process is running
            perror("Process is already running\n");
            exit(EXIT_FAILURE);
        }
    }

    std::ofstream pid_file(get_pid_file_path());
    if (!pid_file) {
        perror("pid_file\n");
        exit(EXIT_FAILURE);
    }
    // log pid

    pid_file << getpid();
    pid_file.close();

    // close fd by sysconf(_SC_OPEN_MAX)
    for (int i = 0; i < sysconf(_SC_OPEN_MAX); i++) {
        close(i);
    }

    // do the work
    for (;;) {
        // do the work
        sleep(1);
    }
}

void client() {
}

int main(int argc, char *argv[]) {
    if (argc == 1) {
        perror("Please specify path\n");
    }
    bool daemon_flag = false;
    for (int i = 1; i < argc; i++) {
        if (!strcmp(argv[i], "--daemon") || !strcmp(argv[i], "-d")) {
            daemon_flag = true;
        }
    }

    if (daemon_flag) {
        daemon();
        // exit
    }
    client();

    return 0;
}