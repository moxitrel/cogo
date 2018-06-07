//
// Created by m2 on 18/1/10.
//

#include <sys/ipc.h>
#include <sys/shm.h>
#include <stdio.h>
#include <stdint.h>
#include "nstd/ipc_memory.h"
#include <unistd.h>

void test_SharedMemory(void);

int main(void)
{
    test_SharedMemory();
    return 0;
}

void test_SharedMemory(void)
{
    static const char *const PATH = "/tmp/shm";
    static const size_t      SIZE = 9;

    int err = 0;
    SharedMemory x = SharedMemory_New(PATH, SIZE);
    if (!x) {
        perror("New");
        return;
    }

    size_t memorySize;
    err = SharedMemory_Size(x, &memorySize);
    if (err) {
        perror("Size");
        return;
    }
    printf("memory.size: %zd\n", memorySize);

    int *sharedX = (int *)SharedMemory_Address(x);
    printf("sharedX: %d\n", *sharedX);
    *sharedX = 1;
    printf("sharedX: %d\n", *sharedX);

    sleep(10);
    printf("sharedX: %d\n", *sharedX);
    err = SharedMemory_Destory(&x);
    if (err) {
        perror("Delete");
        return;
    }
}


void shm_stat_get(int res)
{
    struct shmid_ds state;

    int err = shmctl(res, IPC_STAT, &state);
    if (err) {
        perror("shmctl");
    } else {
        printf("size: %zd\n", state.shm_segsz);
        printf("creator: %d\n", state.shm_cpid);
        printf("accessor: %d\n", state.shm_lpid);
        printf("count: %d\n", state.shm_nattch);
        printf("\n");
    }
}

