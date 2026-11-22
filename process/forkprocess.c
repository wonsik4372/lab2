/* forkprocess.c   
 * fork example
 * 수정: 헤더 추가, PID 출력 추가 */
#include <stdio.h>
#include <sys/types.h>
#include <unistd.h>
 
int main()
{
    pid_t pid;   /* 부모에서 프로세스 식별번호 저장 */
 
    printf("Calling fork \n");
    fflush(stdout);              /* 버퍼를 비우지 않으면 파이프 출력 시 두 번 찍힘 */
    pid = fork();                /* 새로운 프로세스 생성 */
    if (pid == 0)
        printf("I'm the child process  (pid=%d, ppid=%d)\n", getpid(), getppid());
    else if (pid > 0)
        printf("I'm the parent process (pid=%d, child=%d)\n", getpid(), pid);
    else
        printf("fork failed\n");
    return 0;
}
