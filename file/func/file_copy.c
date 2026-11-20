/*
 * 7장 파일 처리
 * 파일 이름: file_copy.c  
 * 수정: ssize_t 출력 형식 %zd, 
 * 버퍼 크기를 인자로 바꿀 수 있게 하여
 * 버퍼 크기에 따른 효율성 측정
 */
#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdlib.h>
 
#define MAX_READ 8192
 
int main(int argc, char *argv[])
{
    int src_fd;              /* source file descriptor */
    int dst_fd;              /* destination file descriptor */
    char buf[MAX_READ];
    ssize_t rcnt;            /* read count */
    ssize_t tot_cnt = 0;     /* total write count */
    mode_t mode = S_IRUSR | S_IWUSR | S_IRGRP | S_IROTH;   /* == 0644 */
    int bufsize = 10;        /* 기본값 MAX_READ 10 */
 
    if (argc < 3) {
        fprintf(stderr, "Usage: file_copy src_file dest_file [bufsize]\n");
        exit(1);
    }
    if (argc > 3)
        bufsize = atoi(argv[3]);
    if (bufsize < 1 || bufsize > MAX_READ)
        bufsize = 10;
    if ((src_fd = open(argv[1], O_RDONLY)) == -1) {
        perror("src open");
        exit(1);
    }
    if ((dst_fd = creat(argv[2], mode)) == -1) {
        perror("dst open");
        exit(1);
    }
    while ((rcnt = read(src_fd, buf, bufsize)) > 0) {
        tot_cnt += write(dst_fd, buf, rcnt);
    }
    if (rcnt < 0) {
        perror("read");
        exit(1);
    }
    printf("total write count = %zd\n", tot_cnt);
    close(src_fd);
    close(dst_fd);
    return 0;
}
