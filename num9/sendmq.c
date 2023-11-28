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
    struct msgq_data send_data;
    
    if ((qid = msgget(QKEY, IPC_CREAT | 0666)) == -1) {
        perror("msgget failed");
        exit(1);
    }

    while (1) {
        printf("Enter message to send (type 'exit' to quit): ");
        fgets(send_data.text, BUFSIZE, stdin);

        if (strncmp(send_data.text, "exit", 4) == 0) {
            break;
        }

        send_data.type = 1;

        if (msgsnd(qid, &send_data, strlen(send_data.text), 0) == -1) {
            perror("msgsnd failed");
            exit(1);
        }
    }

    return 0;
}

