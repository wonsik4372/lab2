/*
 * 7장 파일 처리
 * 파일 이름: file_creat.c
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
    char *buf = "This is a test.";
    ssize_t cnt;       /* write count */
    int flags = O_WRONLY | O_CREAT | O_TRUNC;
    mode_t mode = S_IRUSR | S_IWUSR | S_IRGRP | S_IROTH;   /* == 0644 */
 
    if (argc < 2) {
        fprintf(stderr, "Usage: file_creat filename\n");
        exit(1);
    }
    if ((fd = open(argv[1], flags, mode)) == -1) {
        perror("open");    /* errno에 대응하는 메시지 출력됨 */
        exit(1);
    }
    cnt = write(fd, buf, strlen(buf));
    printf("write count = %zd\n", cnt);     // 출력 형식 %zd 
    close(fd);
    return 0;
}
