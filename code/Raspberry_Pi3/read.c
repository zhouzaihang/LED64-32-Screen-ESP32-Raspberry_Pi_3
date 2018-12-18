#include <sys/ipc.h>
#include <sys/msg.h>
#include <stdio.h>

#define KEY 75
#define SIZE 4096

struct msg_form{
    long msg_type;
    unsigned char msg_text[SIZE];
}msg;

int main(int argc, char const *argv[])
{
    do
    {
        int msgqid = msgget(KEY,0777|IPC_CREAT);
        msgrcv(msgqid, &msg,SIZE+32,0,0);
        printf("(server)receive: %ld\n", msg.msg_type);
        for (int i = 0; i < 64; i++) {
            printf("%d:", i);
            for (int j = 0; j < 8; j++) {
                printf("%x ", msg.msg_text[i * 8 + j]);
            }
            printf("\n");
        }
        // msgctl(msgqid, IPC_RMID,0);
    } while (1);
    return 0;
}
