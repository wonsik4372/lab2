/* handlesignal.c   
 * 인자가 없으면 SIGINT 를 핸들러로 처리 
 * 인자가 ignore 이면 SIGINT 를 무시 
 * 수정: sa_flags 초기화
 */
#define _POSIX_C_SOURCE 200809L

#include <stdio.h>
#include <string.h>
#include <signal.h>
#include <unistd.h>
 
void handler(int signo);
 
int main(int argc, char *argv[])
{
    struct sigaction act;
    int i = 0;
    int ignore = (argc > 1 && strcmp(argv[1], "ignore") == 0);
 
    act.sa_handler = ignore ? SIG_IGN : handler;
    sigfillset(&(act.sa_mask));
    act.sa_flags = 0;
    sigaction(SIGINT, &act, NULL);
 
    printf(ignore ? "SIGINT off\n" : "SIGINT on\n");
    while (1) {
        sleep(1);
        printf("sleep for %d sec(s).\n", ++i);
    }
}
 
void handler(int signo)
{
    printf("handler: signo=%d\n", signo);
}
