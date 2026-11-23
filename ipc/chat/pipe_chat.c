/*
 * 파이프를 이용한 텍스트 채팅 프로그램
 * 파일 이름: pipe_chat.c
 */
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <unistd.h>
#include <sys/stat.h>
#include <sys/select.h>
 
#define MSGSIZE 64
 
int main(int argc, char **argv)
{
    int rfd, wfd, n;
    char buf[MSGSIZE];
    char rfifo[64], wfifo[64];
    fd_set set, master;
 
    if (argc < 3) {
        fprintf(stderr, "Usage: pipe_chat my_name peer_name\n");
        exit(1);
    }
    sprintf(wfifo, "fifo_%s_%s", argv[1], argv[2]);   /* 내가 쓰는 FIFO */
    sprintf(rfifo, "fifo_%s_%s", argv[2], argv[1]);   /* 내가 읽는 FIFO */
 
    /* readfifo.c: FIFO 생성 (이미 있으면 그대로 사용) */
    if (mkfifo(rfifo, 0666) == -1 && errno != EEXIST) {
        perror("mkfifo failed");
        exit(1);
    }
    if (mkfifo(wfifo, 0666) == -1 && errno != EEXIST) {
        perror("mkfifo failed");
        exit(1);
    }
    /* readfifo.c 처럼 O_RDWR 로 열면 상대가 아직 없어도 open 에서 멈추지 않음
     * (O_RDONLY/O_WRONLY 로 열면 반대편이 열릴 때까지 블록되어 순서를 맞춰야 함) */
    if ((rfd = open(rfifo, O_RDWR)) < 0 || (wfd = open(wfifo, O_RDWR)) < 0) {
        perror("fifo open failed");
        exit(1);
    }
    printf("[%s] chat with %s (quit to exit)\n", argv[1], argv[2]);
 
    /* selectpipe.c: 표준 입력과 수신 FIFO 를 함께 감시 */
    FD_ZERO(&master);
    FD_SET(0, &master);
    FD_SET(rfd, &master);
    while (set = master, select(rfd + 1, &set, NULL, NULL, NULL) > 0) {
        if (FD_ISSET(0, &set)) {                 /* 키보드 입력 -> 상대에게 전송 */
            memset(buf, 0, MSGSIZE);
            if (fgets(buf, MSGSIZE, stdin) == NULL)
                strcpy(buf, "quit\n");            /* Ctrl-D 도 종료로 처리 */
            write(wfd, buf, MSGSIZE);            /* writefifo.c: 고정 크기로 전송 */
            if (strcmp(buf, "quit\n") == 0)
                break;
        }
        if (FD_ISSET(rfd, &set)) {               /* 상대 메시지 수신 */
            if ((n = read(rfd, buf, MSGSIZE)) <= 0) {
                perror("fifo read failed");
                break;
            }
            if (strcmp(buf, "quit\n") == 0) {
                printf("[%s] %s left the chat\n", argv[1], argv[2]);
                break;
            }
            printf("%s> %s", argv[2], buf);
        }
    }
    close(rfd);
    close(wfd);
    unlink(rfifo);                               /* 내가 읽던 FIFO 삭제 */
    return 0;
}
