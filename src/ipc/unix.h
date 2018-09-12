//
// Created by m2 on 18/1/8.
//

#ifndef NSTD_IPC_UNIX_H
#define NSTD_IPC_UNIX_H

#include "../memory.h"
#include "../type.h"
#include <sys/un.h>

typedef struct UnixIpc *    UnixIpc;
typedef struct sockaddr_un *UnixIpcAddr;
typedef int (*UnixIpcCallback)(UnixIpc);

UnixIpcAddr UnixIpcAddr_New(const char *);
void        UnixIpcAddr_Gc(UnixIpcAddr *);

// UnixIpc_New: create a new socket at localPath, and connected to the server at
// remotePath.
//
// localPath: socket file created by self.
//   NULL: without creating the socket at localPath. (act as a client can only
//   sending)
// remotePath: socket file path will connect to.
//   NULL: without connecting to the server at remotePath. (act as a server)
// return:
//   NULL: error
//  !NULL: ok
//
// if localPath  = remotePath = NULL: error
UnixIpc UnixIpc_New(const char *localPath, const char *remotePath);

// UnixIpc_Gc: Close all sockets and free memory.
//
// return:
//   0: ok
//  !0: error
int UnixIpc_Gc(UnixIpc *);

// UnixIpc_Read: receive data into buffer
//
// UnixIpcAddr:
//   NULL: don't save remote address
//  !NULL: save the remote address here
// size_t *: when call  , the capacity of buffer
//           when return, the number of bytes received
// return:
//   0: ok
//  !0: error
int UnixIpc_Read(UnixIpc, UnixIpcAddr, size_t *, char[]);

// UnixIpc_Write: Send data to remote
// remote:
//   NULL: use the socket at remotePath when UnixIpc_New().
//         If remotePath is NULL when init, UnixIpc_Read() error
//  !NULL: the destination address will be sent to
// return:
//   0: ok
//  !0: error
int UnixIpc_Write(UnixIpc, UnixIpcAddr, size_t, const char[]);

// UnixIpc_With: Create an unix-socket , destory when finished the callback.
//  UnixIpc_New() -> callback() -> UnixIpc_Gc()
// return  0: ok
//        !0: error = errno
int UnixIpc_With(const char *localPath, const char *remotePath, UnixIpcCallback);

#define SOCKET_FILE_PATH_MAX (sizeof(ZERO_OF(UnixIpcAddr)->sun_path) - 1) // NULL terminated

struct UnixIpc {
    int         socket;
    UnixIpcAddr local;
    UnixIpcAddr remote;
};

#endif // NSTD_IPC_UNIX_H
