/* waitprocess.c   
 * wait / waitpid example
 * 수정: stdio.h 추가, fatal() 대신 perror+exit 
 */
#include <stdio.h>
#include <string.h>
#include <sys/wait.h>
#include <unistd.h>
#include <stdlib.h>
 
int main(int argc, char *argv[])
{
    pid_t pid;
    int status, exit_status;
 
    if ((pid = fork()) < 0) {
        perror("fork failed");
        exit(1);
    }
    if (pid == 0) {              /* 자식 */
        sleep(4);                /* 4초 중단 */
        exit(5);
    }
 
    if (argc > 1 && strcmp(argv[1], "nohang") == 0) {
        /* 부모 코드: 자식 탈출 확인, 퇴장하지 않았으면 1초 잠든 후 다시 검사 */
        while (waitpid(pid, &status, WNOHANG) == 0) {
            printf("still waiting...\n");
            sleep(1);
        }
    } else {
        /* 부모 코드: 자식이 탈출 할 때까지 대기 */
        if ((pid = wait(&status)) == -1) {
            perror("wait failed");
            exit(2);
        }
    }
    if (WIFEXITED(status)) {
        exit_status = WEXITSTATUS(status);
        printf("Exit status from %d was %d\n", pid, exit_status);
    }
    exit(0);
}
