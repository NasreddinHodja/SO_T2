#include <stdio.h>
#include <wait.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

#define MAX_LINE 512
#define MAX_ARGC 256
#define MAX_WORD 256

void print_lambda(int ret_code, bool colored) {
    /* prints prompt, can be colored */
    if(colored) {
        printf(" ");
        switch(ret_code) {
            case 0: // bold green
                printf("\033[1;32m");
                break;
            default: // bold red
                printf("\033[1;31m");
        }
        printf("λ");
        printf("\033[0m "); // reset
    } else printf(" λ ");
}

int get_args(char* line, char** arg_v) {
    char* p;
    int arg_c = 0;
    for(p = strtok(line, " \n\t"); p; p = strtok(NULL, " \n\t"), arg_c++)
        arg_v[arg_c] = p;
    return arg_c;
}

int get_cmd_path(char* cmd, char* file_path) {
    /** find path of any cmd in $PATH */
    char* full_path = getenv("PATH");
    int exists;

    char *token;
    for(token = strtok(full_path, ":"); token; token = strtok(NULL, ":")) {
        sprintf(file_path, "%s/%s", token, cmd);
        if(access(file_path, F_OK) == 0)
            return 0;
    }

    file_path = "";
    return 1;
}

int main(int argc, char** argv) {
    int id, status;
    char* line = NULL;
    size_t len = 0;
    ssize_t line_size = 0;

    // [-s] stylizes the prompt
    bool colored = (getopt(argc, argv, "-s") != -1);

    print_lambda(0, colored);
    while((line_size = getline(&line, &len, stdin)) != -1) {

        char* arg_v[MAX_WORD];
        int arg_c = get_args(line, arg_v);

        if(arg_c == 0) {print_lambda(0, colored); continue;}

        if((id = fork()) < 0) {
            exit(1);
        } else if(id == 0) {
            char path[MAX_WORD];
            arg_v[arg_c] = NULL;
            if(get_cmd_path(arg_v[0], path) == 1)
                exit(execvp("", arg_v));
            else
                exit(execvp(path, arg_v));
            exit(1);
        }
        // parent
        wait(&status);

        if(status == 0)
            printf("[Executado com sucesso.]\n");
        else
            printf("[Código de retorno = %d]\n", status);

        print_lambda(status, colored);
    }

    free(line);
    exit(0);
}
