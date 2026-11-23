/*
 * POSIX 메시지 큐를 이용한 텍스트 채팅 프로그램
 * 파일 이름: mq_chat.c
 */
#include <sys/types.h>
#include <sys/wait.h>
#include <fcntl.h>
#include <mqueue.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
 
#define BUFSIZE 64
#define NORMAL  1
#define URGENT  10
 
int peer_quit = 0;
 
void quit_handler(int signo)
{
    peer_quit = 1;
}
 
int main(int argc, char *argv[])
{
    mqd_t rqd, wqd;
    struct mq_attr q_attr;
    struct sigaction act;
    char rname[32], wname[32];
    char buf[BUFSIZE + 1];
    unsigned int prio;
    ssize_t len;
    pid_t pid;
    int sent_quit = 0;
 
    if (argc < 3) {
        fprintf(stderr, "Usage: mq_chat my_name peer_name\n");
        exit(1);
    }
    sprintf(rname, "/chat_%s", argv[1]);   /* 내 받은편지함 */
    sprintf(wname, "/chat_%s", argv[2]);   /* 상대 받은편지함 */
 
    /* sendpmq.c: 큐 속성 지정 후 생성 */
    q_attr.mq_maxmsg = 10;                 /* max message number in queue */
    q_attr.mq_msgsize = BUFSIZE;           /* max message size */
    if ((rqd = mq_open(rname, O_CREAT | O_RDONLY, 0600, &q_attr)) == -1 ||
        (wqd = mq_open(wname, O_CREAT | O_WRONLY, 0600, &q_attr)) == -1) {
        perror("mq_open failed");
        exit(1);
    }
    mq_getattr(rqd, &q_attr);
    printf("[%s] chat with %s (maxmsg=%ld, msgsize=%ld, !msg = urgent, quit to exit)\n",
           argv[1], argv[2], q_attr.mq_maxmsg, q_attr.mq_msgsize);
 
    /* blocksigusr.c: SIGUSR1 핸들러 등록 (SA_RESTART 를 쓰지 않으므로 fgets 가 중단) */
    sigemptyset(&act.sa_mask);
    act.sa_flags = 0;
    act.sa_handler = quit_handler;
    sigaction(SIGUSR1, &act, NULL);
 
    fflush(stdout);                        /* fork 전에 버퍼를 비워 중복 출력 방지 */
    pid = fork();
    if (pid < 0) {
        perror("fork failed");
        exit(1);
    }
 
    if (pid == 0) {
        /* ----- 자식: 수신 담당 (receivepmq.c) ----- */
        for (;;) {
            if ((len = mq_receive(rqd, buf, BUFSIZE, &prio)) == -1) {
                perror("mq_receive failed");
                break;
            }
            buf[len] = '\0';
            if (strcmp(buf, "quit") == 0) {
                printf("[%s] %s left the chat\n", argv[1], argv[2]);
                kill(getppid(), SIGUSR1);   /* sendsignal.c: 부모에게 종료 알림 */
                break;
            }
            if (prio == URGENT)
                printf("[URGENT] %s> %s\n", argv[2], buf + 1);
            else
                printf("%s> %s\n", argv[2], buf);
            fflush(stdout);
        }
        exit(0);
    }
 
    /* ----- 부모: 송신 담당 (sendpmq.c) ----- */
    while (!peer_quit && fgets(buf, BUFSIZE, stdin) != NULL) {
        buf[strcspn(buf, "\n")] = '\0';
        prio = (buf[0] == '!') ? URGENT : NORMAL;
        if (mq_send(wqd, buf, strlen(buf), prio) == -1) {
            perror("mq_send failed");
            break;
        }
        if (strcmp(buf, "quit") == 0) {
            sent_quit = 1;
            break;
        }
    }
    if (!peer_quit) {
        if (!sent_quit)                    /* Ctrl-D 로 끝낸 경우에도 상대에게 알림 */
            mq_send(wqd, "quit", 4, NORMAL);
        kill(pid, SIGTERM);                /* 수신 담당 자식 종료 */
    }
    wait((int *) 0);
 
    /* receivepmq.c: 닫고 내 큐 삭제 */
    if (mq_close(rqd) == -1 || mq_close(wqd) == -1)
        perror("mq_close failed");
    if (mq_unlink(rname) == -1)
        perror("mq_unlink failed");
    return 0;
}
