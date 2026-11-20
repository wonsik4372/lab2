/*
 * 7장 파일 처리
 * 파일 이름: file_append.c
 * 수정: string.h 추가, ssize_t 출력 형식 %zd
 */
#include <stdio.h>
#include <string.h>     // 추가 
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdlib.h>
 
int main(int argc, char *argv[])
{
    int fd;            /* file descriptor */
    char *buf = "This is a test.\n";
    ssize_t cnt;       /* write count */
    int flags = O_WRONLY | O_CREAT | O_APPEND;      // creat와 다른 점
    mode_t mode = S_IRUSR | S_IWUSR | S_IRGRP | S_IROTH;   
 
    if (argc < 2) {
        fprintf(stderr, "Usage: file_append filename\n");
        exit(1);
    }
    if ((fd = open(argv[1], flags, mode)) == -1) {
        perror("open");
        exit(1);
    }
    cnt = write(fd, buf, strlen(buf));
    printf("write count = %zd\n", cnt);     // ssize_t 출력 형식 %zd
    close(fd);
    return 0;
}
