#include <stdio.h>
#include <stdlib.h>
#include <semaphore.h>
#include <unistd.h>
#include <sys/mman.h>
#include <sys/stat.h>        /* For mode constants */
#include <fcntl.h>           /* For O_* constants */

#define TRUE 1

sem_t* mutex;
sem_t* a;
sem_t* b;

void init() {
    // Alocando memoria compartilhada para semaforos
    int fd;
    fd = shm_open("/semmutex", O_CREAT | O_RDWR, 0666);
    ftruncate(fd, sizeof(sem_t));
    mutex = mmap(NULL, sizeof(sem_t), PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);

    fd = shm_open("/sema", O_CREAT | O_RDWR, 0666);
    ftruncate(fd, sizeof(sem_t));
    a = mmap(NULL, sizeof(sem_t), PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);

    fd = shm_open("/semb", O_CREAT | O_RDWR, 0666);
    ftruncate(fd, sizeof(sem_t));
    b = mmap(NULL, sizeof(sem_t), PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);

    // Iniciando valores dos semaforos
    sem_init(mutex, TRUE, 1);
    sem_init(a, TRUE, 2);
    sem_init(b, TRUE, 0);
}

void processo_a() {
    while (TRUE) {
        sem_wait(a);
        sem_wait(mutex);
        printf("A\n");
        printf("B\n");
        sem_post(mutex);
        sem_post(b);
    }
}

void processo_b() {
    while (TRUE) {
        sem_wait(b);
        sem_wait(mutex);
        printf("C\n");
        printf("D\n");
        sem_post(mutex);
        sem_post(a);
    }

}

int main() {

    init();

    pid_t pid = fork();

    // Problema ao executar fork(). Abortando o programa.
    if(pid < 0) {
        exit(1);
    }

    // Processo filho executa PROCESSO A
    if(pid == 0) {
        processo_a();
    }
    // Processo pai executa PROCESSO B
    else {
        processo_b();
    }

    return 0;
}
