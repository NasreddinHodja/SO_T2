#include <stdio.h>
#include <wait.h>
#include <unistd.h>
#include <stdlib.h>

#define MAX_MSG 128

int main(void) {
    int id, j;
    int ffd[2], cfd[2], n_bytes;
    char inbuf[MAX_MSG];

    printf("PID: %lu\n", getpid());

    if(pipe(ffd) < 0)
        exit(1);
    if(pipe(cfd) < 0)
        exit(1);

    if((id = fork()) < 0) {
        exit(1);
    } else if (id > 0) {
        close(cfd[1]);
        printf("PID pai: %lu | PID filho: %lu\n", getpid(), id);

        char msg_to_child[MAX_MSG] = "hello, child";
        write(ffd[1], msg_to_child, MAX_MSG);
        printf("mensagem para o filho: %s\n", msg_to_child);

        while((n_bytes = read(cfd[0], inbuf, MAX_MSG)) < 0)
            sleep(0.5);
        printf("mensagem recebida do filho: %s\n", inbuf);

        while((n_bytes = read(cfd[0], inbuf, MAX_MSG)) < 0)
            sleep(0.5);
        printf("mensagem recebida do filho: %s\n", inbuf);

        wait(NULL);

        printf("filho terminou de executar, pai irá encerrar\n");
        close(ffd[1]);
    } else {
        // todo print do filho sera prefixado com "***"
        // para diferencia-lo de um print do pai
        close(ffd[1]);
        printf("*** PID: %lu | PPID: %lu\n", getpid(), getppid());

        while((n_bytes = read(ffd[0], inbuf, MAX_MSG)) < 0)
            sleep(0.5);
        printf("*** mensagem recebida do pai: %s\n", inbuf);

        char msg_to_parent[MAX_MSG] = "hello, parent";
        write(cfd[1], msg_to_parent, MAX_MSG);

        for(j = 0; j <= 10000; j++);
        sprintf(msg_to_parent, "j = %d", j);
        write(cfd[1], msg_to_parent, MAX_MSG);

        close(cfd[1]);

        execl("/Bin/ls", "ls", NULL);
    }

    exit(0);
}
