/*
 * Lab2-5: 타자 연습 프로그램
 * 파일 이름: typingtest.c
 */
#define _POSIX_C_SOURCE 200809L
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <signal.h>
#include <termios.h>
#include <sys/types.h>
#include <fcntl.h>
#include <unistd.h>

char *texts[] = {
    "The magic thing is that you can change it.",
    "Linux is a free and open source operating system.",
    "A process is a program in execution.",
    "Pipes connect the output of one process to another.",
};
#define NTEXT (sizeof(texts) / sizeof(texts[0]))

int fd;
struct termios init_attr;

void restore(int signo)
{
    tcsetattr(fd, TCSANOW, &init_attr);   /* 터미널 속성 복구 */
    write(fd, "\n중단합니다.\n", strlen("\n중단합니다.\n"));
    exit(1);
}

int main(void)
{
    int nread, cnt, errcnt;
    int tot_err = 0;
    char ch, mark[256];
    struct termios new_attr;
    struct sigaction act;
    size_t i;

    fd = open(ttyname(fileno(stdin)), O_RDWR);
    if (fd < 0) {
        perror("open tty");
        exit(1);
    }
    tcgetattr(fd, &init_attr);
    new_attr = init_attr;
    new_attr.c_lflag &= ~ICANON;          /* 비정규 모드: 줄 단위가 아닌 글자 단위 */
    new_attr.c_lflag &= ~ECHO;            /* 입력 글자를 자동으로 화면에 표시하지 않음 */
    new_attr.c_cc[VMIN] = 1;              /* 1글자가 들어오면 read 반환 */
    new_attr.c_cc[VTIME] = 0;

    act.sa_handler = restore;             /* Ctrl-C 시 터미널 복구 */
    sigfillset(&act.sa_mask);
    act.sa_flags = 0;
    sigaction(SIGINT, &act, NULL);

    if (tcsetattr(fd, TCSANOW, &new_attr) != 0) {
        fprintf(stderr, "터미널 속성을 설정할 수 없음.\n");
    }

    printf("=== 타자 연습 (%zu 문장) ===\n", NTEXT);
    for (i = 0; i < NTEXT; i++) {
        char *text = texts[i];
        int len = strlen(text);

        printf("\n[%zu] 다음 문장을 그대로 입력하세요.\n%s\n", i + 1, text);
        fflush(stdout);
        cnt = errcnt = 0;
        while ((nread = read(fd, &ch, 1)) > 0 && ch != '\n') {
            if (ch == 127 || ch == '\b') {    /* 백스페이스: 한 글자 지움 */
                if (cnt > 0) {
                    cnt--;
                    if (mark[cnt] == '*')
                        errcnt--;
                    write(fd, "\b \b", 3);
                }
                continue;
            }
            if (cnt < len && ch == text[cnt]) {
                mark[cnt++] = ch;
                write(fd, &ch, 1);
            } else {
                if (cnt < (int)sizeof(mark))
                    mark[cnt++] = '*';
                write(fd, "*", 1);
                errcnt++;
            }
        }
        if (cnt < len)                    /* 다 치지 않고 Enter: 남은 글자도 오타 */
            errcnt += len - cnt;
        printf("\n  오타 %d개\n", errcnt);
        tot_err += errcnt;
    }
    printf("\n=== 결과 ===\n타이핑 오류의 횟수는 %d\n", tot_err);

    tcsetattr(fd, TCSANOW, &init_attr);   /* 터미널 속성 복구 */
    close(fd);
    return 0;
}