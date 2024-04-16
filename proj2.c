/**
 * @file proj2.c
 * @brief IOS – projekt 2 (synchronizace)
 *
 * Zadání je inspirováno knihou Allen B. Downey: The Little Book of Semaphores (The Senate Bus
 * problem)
 *
 * @author Timotej Mikula, xmikult00
 * @date 16.4.2024
 */

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdbool.h>
#include <ctype.h>
#include <time.h>
#include <sys/wait.h>
#include <sys/mman.h>
#include <sys/types.h>
#include <unistd.h>
#include <semaphore.h>
#include "proj2.h"

sem_t *nameofsemaphore;
Service nameofservice;
FILE *file;
bool *post_open;
int *action_id;
int seed;

Arg ParseArgs(int argc, char *const argv[])
{
    Arg args;
    char *str;

    if (argc != 6)
    {
        exit_error("Incorrect count of args!\n", 1);
    }
    for (int i = 1; i < argc; i++)
    {
        if (!isInteger(argv[i]))
        {
            exit_error("Invalid input parameters. All parameters should be integers.\n", 1);
        }
    }

    args.L = atoi(argv[1]);
    args.Z = atoi(argv[2]);
    args.K = atoi(argv[3]);
    args.TL = atoi(argv[4]);
    args.TB = atoi(argv[5]);

    if (args.L >= 20000 || args.Z > 10 || args.K < 10 || args.K > 100 || args.TL > 10000 || args.TB > 1000)
    {
        exit_error("Invalid input parameters.\n", 1);
    }

    return args;
}

void exit_error(char *msg, int errcode)
{
    fprintf(stderr, "ERROR: %s Exit code: %d\n", msg, errcode);
    exit(errcode);
}

int isInteger(char *str)
{
    int length = strlen(str);
    for (int i = 0; i < length; i++)
    {
        if (!isdigit(str[i]))
        {
            return 0;
        }
    }
    return 1;
}

void skibus(Arg args) {}

void skier(Arg args, int id) {}

void wait_sem(sem_t **sem)
{
    if (sem_wait(*sem) == -1)
        exit_error("Wait sem failed.", 1);
}

void post_sem(sem_t **sem)
{
    if (sem_post(*sem) == -1)
        exit_error("Post sem failed.", 1);
}

void init_sem(sem_t **sem, int value)
{
    *sem = mmap(NULL, sizeof(sem_t), PROT_READ | PROT_WRITE, MAP_ANONYMOUS | MAP_SHARED, -1, 0);
    if (*sem == MAP_FAILED)
        exit_error("Mmap failed.", 1);

    if (sem_init(*sem, 1, value) == -1)
        exit_error("Init sem failed.", 1);
}

void init_semaphores() {}

void destroy_sem(sem_t **sem)
{
    if (sem_destroy(*sem) == -1)
        exit_error("Destroy sem failed.", 1);

    if (munmap((*sem), sizeof(sem_t)) == -1)
        exit_error("Munmap sem failed.", 1);
}

void cleanup_semaphores() {}

int random_int(int lower, int upper)
{
    seed += getpid() * 3696;

    srand(seed);
    int randn = (int)rand() % (upper - lower + 1) + lower;

    // generate random integer in the range <lower, upper>
    return randn;
}
void usleep_random_in_range(int lower, int upper)
{
    int rand_n = random_int(lower, upper);
    // usleep
    usleep(rand_n);
}

void clear_and_open_output_file()
{
    // Clear the output file.

    file = fopen("proj2.out", "w");
    if (file == NULL)
    {
        exit_error("Fopen failed.", 1);
    }

    if (fclose(file) == EOF)
    {
        exit_error("fclose close.", 1);
    }

    file = fopen("proj2.out", "a");
    if (file == NULL)
    {
        exit_error("Fopen 2 failed.", 1);
    }
}

void output(int action_type, int id, int service) {}

int main(int argc, char **argv)
{
    Arg args = ParseArgs(argc, argv);
    init_semaphores();
    clear_and_open_output_file();
    setbuf(file, NULL);
    setbuf(stdout, NULL);
    setbuf(stderr, NULL);

    skibus(args);

    for (int i = 1; i < args.L + 1; i++)
    {
        pid_t skier_id = fork();
        if (skier_id < 0)
            exit_error("Fork skier failed.", 1);

        if (skier_id == 0)
        {
            output(U_STARTED, i, NONE);
            skier(args, i);
        }
    }

    usleep_random_in_range((int)args.L / 2, args.L);
    change_post_status();

    while (wait(NULL) > 0)
        ;
    if (fclose(file) == EOF)
        exit_error("Final fclose close.", 1);

    cleanup_semaphores();
    return 0;
}
