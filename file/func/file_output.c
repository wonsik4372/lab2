/*
 * 7장 파일 처리
 * 파일 이름: file_output.c   
 * 수정: string.h 추가
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>     // 추가 
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
 
int main(int argc, char *argv[])
{
    int fd;            /* file descriptor */
    char *buf = "This is a test output file.\n";
    int flags = O_WRONLY | O_CREAT | O_TRUNC;
    mode_t mode = S_IRUSR | S_IWUSR | S_IRGRP | S_IROTH;   /* == 0644 */
 
    if (argc < 2) {
        fprintf(stderr, "Usage: file_output filename\n");
        exit(1);
    }
    if ((fd = open(argv[1], flags, mode)) == -1) {
        perror("open");
        exit(1);
    }
    if (dup2(fd, 1) == -1) {       /* 표준 출력(1)이 파일을 가리키게 함 */
        perror("dup2");
        exit(1);
    }
    if (close(fd) == -1) {
        perror("close");
        exit(1);
    }
    write(1, buf, strlen(buf));
    exit(0);
}
