/* readfifo.c   
 * 수정: stdllib.h 오타 수정, sys/stat.h, unistd.h 추가,
 *       이미 FIFO 가 있으면 그대로 사용 
 */
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <unistd.h>
#include <sys/stat.h>
 
#define MSGSIZE 64
 
char *testfifo = "fifo";
 
int main(int argc, char **argv)
{
    int fd;
    char buf[MSGSIZE];
 
    if (mkfifo(testfifo, 0666) == -1 && errno != EEXIST) {
        perror("mkfifo failed");
        exit(1);
    }
    /* open testfifo, setting O_RDWR */
    if ((fd = open(testfifo, O_RDWR)) < 0) {
        perror("fifo open failed");
        exit(1);
    }
    /* receive message */
    while (1) {
        if (read(fd, buf, MSGSIZE) < 0) {
            perror("fifo read failed");
            exit(1);
        }
        printf("received message: %s\n", buf);
    }
}
