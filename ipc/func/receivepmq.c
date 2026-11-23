/* receivepmq.c   
 * 수정: fcntl.h 추가, prio 를 unsigned 로, 
 * 받은 길이로 문자열 끝 처리
 */
#include <sys/types.h>
#include <fcntl.h>
#include <mqueue.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
 
#define BUFSIZE 16
#define QNAME "/my_queue"
#define PRIORITY 1
 
char recv_data[BUFSIZE + 1];
 
int main()
{
    mqd_t qd;
    struct mq_attr q_attr, old_q_attr;
    unsigned int prio;
    ssize_t len;
 
    q_attr.mq_maxmsg = 10;          /* max message number in queue */
    q_attr.mq_msgsize = BUFSIZE;    /* max message size */
    if ((qd = mq_open(QNAME, O_RDWR | O_NONBLOCK, 0600, NULL)) == -1) {
        perror("mq_open failed");
        exit(1);
    }
    q_attr.mq_flags = 0;            /* release O_NONBLOCK */
    if (mq_setattr(qd, &q_attr, NULL)) {
        perror("mq_setattr failed");
        exit(1);
    }
    if (mq_getattr(qd, &old_q_attr)) {
        perror("mq_getattr failed");
        exit(1);
    }
    if (!(old_q_attr.mq_flags & O_NONBLOCK))
        printf("O_NONBLOCK not set\n");
    if ((len = mq_receive(qd, recv_data, BUFSIZE, &prio)) == -1) {
        perror("mq_receive failed");
        exit(1);
    }
    recv_data[len] = '\0';
    printf("received from message queue : %s, prio : %u\n", recv_data, prio);
    if (mq_close(qd) == -1) {
        perror("mq_close failed");
        exit(1);
    }
    if (mq_unlink(QNAME) == -1) {
        perror("mq_unlink failed");
        exit(1);
    }
    return 0;
}
