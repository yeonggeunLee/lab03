#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#define BUFSIZE 256
#define QKEY (key_t)0111

struct msgq_data {
    long type;
    char text[BUFSIZE];
};

int main() {
    int qid;
    struct msgq_data recv_data;

    if ((qid = msgget(QKEY, IPC_CREAT | 0666)) == -1) {
        perror("msgget failed");
        exit(1);
    }

    while (1) {
        if (msgrcv(qid, &recv_data, BUFSIZE, 0, 0) == -1) {
            perror("msgrcv failed");
            exit(1);
        }

        printf("Received from message queue: %s", recv_data.text);
    }

    return 0;
}

