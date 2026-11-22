/* sendsignal.c    
 * 부모와 자식이 SIGUSR1 을 번갈아 주고받는 프로그램
 * 수정: sa_mask, sa_flags 초기화, 헤더 추가, 5회 후 종료 
 */
#define _POSIX_C_SOURCE 200809L

#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <signal.h>
#include <unistd.h>
 
int i = 0;
void p_handler(int), c_handler(int);
 
int main()
{
    pid_t pid, ppid;
    struct sigaction act;
 
    sigemptyset(&act.sa_mask);
    act.sa_flags = 0;
    pid = fork();
    if (pid == 0) {
        act.sa_handler = c_handler;
        sigaction(SIGUSR1, &act, NULL);
        ppid = getppid();          /* get parent's process id. */
        while (i < 5) {
            sleep(1);
            kill(ppid, SIGUSR1);
            pause();
        }
        exit(0);
    } else if (pid > 0) {
        act.sa_handler = p_handler;
        sigaction(SIGUSR1, &act, NULL);
        while (i < 5) {
            pause();
            sleep(1);
            kill(pid, SIGUSR1);
        }
        wait(NULL);
    } else
        perror("Error");
    return 0;
}
 
void p_handler(int signo)
{
    printf("Parent handler: call %d times.\n", ++i);
}
 
void c_handler(int signo)
{
    printf("Child handler: call %d times.\n", ++i);
}
