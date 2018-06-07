//
// Created by m2 on 18/1/6.
//

#include "nstd/type.h"
#include "nstd/ipc/unix.h"
#include <stdio.h>
#include <sys/socket.h>
#include <string.h>
#include <sys/un.h>
#include <unistd.h>

int doLoopWrite(IpcUnix o)
{
    char msg[1<<8] = "x";

    for (;;) {
        int err = IpcUnix_Write(o, NULL, strlen(msg)+1, msg);
        if (err) {
            perror("IpcUnix_Write");
            goto ERROR_WRITE;
        }
        printf("IpcUnix_Write: %s\n", msg);


        err = IpcUnix_Read(o, NULL, &(size_t){sizeof msg}, msg);
        if (err) {
            perror("IpcUnix_Read");
            goto ERROR_READ;
        }
        printf("IpcUnix_Read: %s\n", msg);

    ERROR_READ:
    ERROR_WRITE:
        printf("\n");
        sleep(1);
    }
}




//int sayHello(IpcUnix o)
//{
//    char msg[256] = "hello";
//
//    size_t n = strlen(msg) + 1;
//    int err = IPcUnix_Write(o, n, msg);
//    if (err) {
//        return err;
//    }
//
//    n = sizeof msg;
//    err = IpcUnix_Read(o, &n, msg);
//    if (err) {
//        return err;
//    }
//
//    printf("%s\n", msg);
//    return 0;
//}


int main(void)
{
    int err = IpcUnix_With(
            "/tmp/unix-client.socket",
            "/tmp/unix.socket",
            doLoopWrite);
    if (err) {
        perror("IpcUnix_OpenWith");
        return 1;
    }

    return 0;
}