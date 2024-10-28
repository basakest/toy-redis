#include <sys/socket.h>
#include <arpa/inet.h>
#include <cstdio>
#include <cstdlib>
#include <unistd.h>
#include <cstring>
#include <cerrno>

static void do_something(int connfd);
static void msg(const char *msg);
static void die(const char *msg);

int main()
{
    int fd;
    fd = socket(AF_INET, SOCK_STREAM, 0);
    if (fd == -1) {
        die("socket() error");
    }
    int val = 1;
    setsockopt(fd, SOL_SOCKET, SO_REUSEADDR, &val, sizeof val);

    struct sockaddr_in addr = {};
    addr.sin_family = AF_INET;
    addr.sin_port = ntohs(1234);
    addr.sin_addr.s_addr = ntohl(INADDR_ANY);
    int rv = bind(fd, (struct sockaddr *)&addr, sizeof addr);
    if (rv == -1) {
        die("bind() error");
    }
    // listen
    rv = listen(fd, SOMAXCONN);
    if (rv == -1) {
        die("listen() error");
    }

    while (true) {
        // accept
        struct sockaddr_in client_addr = {};
        socklen_t socklen = sizeof client_addr;
        int connfd = accept(fd, (struct sockaddr *)&client_addr, &socklen);
        if (connfd == -1) {
            continue; // error
        }

        do_something(connfd);
        close(connfd);
    }

    return 0;
}

static void msg(const char *msg)
{
    fprintf(stderr, "%s\n", msg);
}

static void die(const char *msg)
{
    int err = errno;
    fprintf(stderr, "[%d] %s\n", err, msg);
    abort();
}

static void do_something(int connfd)
{
    char rbuf[64] = {};
    ssize_t n = read(connfd, rbuf, sizeof rbuf - 1);
    if (n < 0) {
        msg("read() error");
        return;
    }
    printf("client says: %s\n", rbuf);

    char wbuf[] = "world";
    write(connfd, wbuf, strlen(wbuf));
}