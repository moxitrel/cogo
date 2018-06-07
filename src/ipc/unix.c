#include "unix.h"
#include <string.h>
#include <stdio.h>
#include <unistd.h>
#include <sys/socket.h>
#include "../memory.h"


// path: truncated silently if too long
UnixIpcAddr UnixIpcAddr_New(const char *path)
{
    if (path == NULL) {
        return NULL;
    }
    UnixIpcAddr v = NEW(v);
    v->sun_family = AF_UNIX;
    strncpy(v->sun_path, path, SOCKET_FILE_PATH_MAX);
    v->sun_path[SOCKET_FILE_PATH_MAX] = '\0';
    return v;
}

void UnixIpcAddr_Gc(UnixIpcAddr *addr)
{
    GC(*addr);
}

// localPath, remotePath: truncated silently if too long
UnixIpc UnixIpc_New(const char *localPath, const char *remotePath)
{
    assert(localPath || remotePath);

    UnixIpc v = NULL;

    int v_socket = socket(AF_UNIX, SOCK_DGRAM, 0);
    if (v_socket < 0) {
        goto ERROR_SOCKET;
    }

    UnixIpcAddr v_local = UnixIpcAddr_New(localPath);
    if (v_local) {
        (void) remove(v_local->sun_path);    //delete if exists
        int err = bind(v_socket, (struct sockaddr *)v_local, sizeof *v_local);
        if (err) {
            goto ERROR_BIND;
        }
    }

    UnixIpcAddr v_remote = UnixIpcAddr_New(remotePath);

    v = NEW(v);
    v->socket = v_socket;
    v->local = v_local;
    v->remote = v_remote;

FINALLY:
    return v;
ERROR_BIND:
    UnixIpcAddr_Gc(&v_local);
    (void) close(v_socket);
ERROR_SOCKET:
    goto FINALLY;
}

int UnixIpc_Gc(UnixIpc *o)
{
    assert(o);
    if (*o == NULL) {
        return 0;
    }

    int err = close((*o)->socket);
    if ((*o)->local != NULL) {
        (void) remove((*o)->local->sun_path);
    }
    UnixIpcAddr_Gc(&(*o)->local);
    UnixIpcAddr_Gc(&(*o)->remote);
    GC(*o);

    return err;
}

int UnixIpc_Read(UnixIpc o, UnixIpcAddr remote, size_t *bufferSize, char buffer[*bufferSize])
{
    assert(o);
    assert(o->local);
    assert(bufferSize);
    assert(buffer);

    socklen_t *remoteSize = remote ? &(socklen_t){sizeof *remote} : NULL;

    ssize_t n = recvfrom(o->socket, buffer, *bufferSize, 0, (struct sockaddr *)remote, remoteSize);
    if (n < 0) {
        return errno;
    }

    *bufferSize = (size_t)n;
    return 0;
}

int UnixIpc_Write(UnixIpc o, UnixIpcAddr remote, size_t bufferSize, const char buffer[bufferSize])
{
    assert(o);
    assert(remote || o->remote);
    assert(buffer);

    remote = remote ? remote :
             o->remote;

    for (;;) {
        ssize_t n = sendto(o->socket, buffer, bufferSize, 0, (struct sockaddr *)remote, sizeof *remote);
        assert(n < bufferSize);
        if (n < 0) {
            return errno;
        }
        if (n == bufferSize) {
            return 0;
        }
        buffer += n;
        bufferSize -= n;
    }
}

int UnixIpc_With(const char *localPath, const char *remotePath, int callback(UnixIpc))
{
    assert(callback);

    UnixIpc o = UnixIpc_New(localPath, remotePath);
    if (!o) {
        return errno;
    }

    int err_callback = callback(o);
    int err_gc   = UnixIpc_Gc(&o);

    return err_callback ? err_callback :
           err_gc;
}
