/* writefifo.c 
 * 수정: string.h, unistd.h 추가, 
 * 인자가 버퍼보다 길면 오류 처리 
 */
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <unistd.h>
 
#define MSGSIZE 64
 
char *testfifo = "fifo";
 
int main(int argc, char **argv)
{
    int fd, i, nwrite;
    char buf[MSGSIZE];
 
    if (argc < 2) {
        fprintf(stderr, "Usage: writefifo msg ...\n");
        exit(1);
    }
    /* open testfifo, setting O_WRONLY */
    if ((fd = open(testfifo, O_WRONLY)) < 0) {
        perror("fifo open failed");
        exit(1);
    }
    /* send message */
    for (i = 1; i < argc; i++) {
        if (strlen(argv[i]) >= MSGSIZE) {
            fprintf(stderr, "message too long: %s\n", argv[i]);
            exit(1);
        }
        memset(buf, 0, MSGSIZE);
        strcpy(buf, argv[i]);
        if ((nwrite = write(fd, buf, MSGSIZE)) < 0) {
            perror("fifo write failed");
            exit(1);
        }
    }
    return 0;
}
