//
// Created by m2 on 18/1/6.
//
#include <stdio.h>
#include "nstd/ipc/unix.h"
#include <unistd.h>
#include <sys/un.h>

int doLoopRead(IpcUnix o)
{
    size_t n = 1 << 8;
    char msg[1<<8] = {};

    for (;;) {
        struct sockaddr_un clientAddr;
        int err = IpcUnix_Read(o, &clientAddr, &n, msg);
        if (err) {
            perror("IpcUnix_Read");
            goto ERROR_READ;
        }
        printf("IpcUnix_Read: %s\n", msg);
        printf("clientAddr: %s\n", clientAddr.sun_path);

        err = IpcUnix_Write(o, &clientAddr, n, msg);
        if (err) {
            perror("IPcUnix_Write");
            goto ERROR_WRITE;
        }
        printf("IPcUnix_Write: %s\n", msg);

     ERROR_READ:
     ERROR_WRITE:
        printf("\n");
    }
}

//int echo(IpcUnix o, size_t n, char buffer[n])
//{
//    printf("%s\n", buffer);
//    int err = IPcUnix_Write(o, n, buffer);
//    if (err) {
//        perror("IpcUnix_Write");
//    }
//    return 0;
//}

int main(void)
{
    int err = IpcUnix_With(
            "/tmp/unix.socket",
            NULL,
            doLoopRead
    );
    if (err) {
        perror("IpcUnix_Serve");
    }

    return 0;
}