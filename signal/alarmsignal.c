/* alarmsignal.c   
 * alarm example
 * 수정: unistd.h 추가, sa_mask/sa_flags 초기화
 *      플래그를 volatile sig_atomic_t 로 
 */
#define _POSIX_C_SOURCE 200809L

#include <stdio.h>
#include <signal.h>
#include <unistd.h>
 
void alarm_handler(int);
volatile sig_atomic_t alarm_flag = 0;
 
int main()
{
    struct sigaction act;
 
    act.sa_handler = alarm_handler;
    sigemptyset(&act.sa_mask);
    act.sa_flags = 0;
    sigaction(SIGALRM, &act, NULL);
 
    alarm(5);     /* Turn alarm on. */
    pause();      /* pause */
    if (alarm_flag)
        printf("Passed a 5 secs.\n");
    return 0;
}
 
void alarm_handler(int sig)
{
    printf("Received a alarm signal.\n");
    alarm_flag = 1;
}
