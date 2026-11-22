/* forkexecls.c   
 * fork 와 execl 을 함께 사용
 * 수정: stdio.h, stdlib.h, sys/wait.h 추가, exec 실패 시 자식 종료 
 */
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>
 
int main()
{
    pid_t pid;
 
    pid = fork();
    if (pid == 0) {
        /* 자식 프로세스가 execl 호출 */
        execl("/bin/ls", "ls", "-l", (char *) 0);
        perror("execl failed");
        exit(1);                 /* exec 실패 시, 자식이 부모 코드 실행 x */
    } else if (pid > 0) {
        /* 자식이 끝날 때까지 수행을 일시 중단 */
        wait((int *) 0);
        printf("ls completed\n");
        exit(0);
    } else
        perror("fork failed");
    return 0;
}
