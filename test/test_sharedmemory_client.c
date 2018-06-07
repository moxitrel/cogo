//
// Created by m2 on 18/1/10.
//

#include <sys/ipc.h>
#include <sys/shm.h>
#include <stdio.h>
#include <stdint.h>
#include "nstd/ipc_memory.h"
#include <unistd.h>

int main(void)
{
    static const char *const PATH = "/tmp/shm";

    SharedMemory client = SharedMemory_Open(PATH);
    if (!client) {
        perror("Open");
        return 1;
    }

    size_t memorySize;
    int err = SharedMemory_Size(client, &memorySize);
    if (err) {
        perror("Size");
        return 1;
    }
    printf("memory.size: %zd\n", memorySize);

    int *sharedX = (int *)SharedMemory_Address(client);
    printf("sharedX: %d\n", *sharedX);
    *sharedX = 2;
    printf("sharedX: %d\n", *sharedX);

    SharedMemory_Close(&client);
    return 0;
}