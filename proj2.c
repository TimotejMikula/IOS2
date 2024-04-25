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

sem_t *mutex_output;
sem_t *boarding;
sem_t *leaving;
FILE *file;
int *action_id;
int seed;
int *count_in_bus;
int *allskiers;

BusStop busstop[11];

Arg ParseArgs(int argc, char *const argv[])
{
    Arg args;

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
    usleep(rand_n);
}

void clear_and_open_output_file(void)
{
    // Clear the output file.

    file = fopen("proj2.out", "w");
    if (file == NULL)
    {
        exit_error("Fopen failed.\n", 1);
    }

    if (fclose(file) == EOF)
    {
        exit_error("fclose close.\n", 1);
    }

    file = fopen("proj2.out", "a");
    if (file == NULL)
    {
        exit_error("Fopen 2 failed.\n", 1);
    }
}

void init_semaphores(Arg args)
{
    init_sem(&mutex_output, 1);
    init_sem(&boarding, 1);
    init_sem(&leaving, 1);

    for (int i = 1; i <= args.Z; i++)
    {
        init_sem(&busstop[i].semaphore, 0);
        busstop[i].count = mmap(NULL, sizeof(int), PROT_READ | PROT_WRITE, MAP_ANONYMOUS | MAP_SHARED, -1, 0);
        if (busstop[i].count == MAP_FAILED)
        {
            exit_error("Mmap failed.\n", 1);
        }
        *(busstop[i].count) = 0;
    }

    action_id = mmap(NULL, sizeof(int), PROT_READ | PROT_WRITE, MAP_ANONYMOUS | MAP_SHARED, -1, 0);
    if (action_id == MAP_FAILED)
    {
        exit_error("Mmap failed.\n", 1);
    }

    count_in_bus = mmap(NULL, sizeof(int), PROT_READ | PROT_WRITE, MAP_ANONYMOUS | MAP_SHARED, -1, 0);
    if (count_in_bus == MAP_FAILED)
    {
        exit_error("Mmap failed.\n", 1);
    }

    allskiers = mmap(NULL, sizeof(int), PROT_READ | PROT_WRITE, MAP_ANONYMOUS | MAP_SHARED, -1, 0);
    if (allskiers == MAP_FAILED)
    {
        exit_error("Mmap failed.\n", 1);
    }

    *action_id = 0;
    *count_in_bus = 0;
    *allskiers = 0;
}

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
    {
        exit_error("Mmap failed.", 1);
    }

    if (sem_init(*sem, 1, value) == -1)
    {
        exit_error("Init sem failed.", 1);
    }
}

void destroy_sem(sem_t **sem)
{
    if (sem_destroy(*sem) == -1)
        exit_error("Destroy sem failed.", 1);

    if (munmap((*sem), sizeof(sem_t)) == -1)
        exit_error("Munmap sem failed.", 1);
}

void cleanup_semaphores(Arg args)
{
    destroy_sem(&mutex_output);
    destroy_sem(&boarding);
    destroy_sem(&leaving);

    for (int i = 1; i <= args.Z; i++)
    {
        destroy_sem(&busstop[i].semaphore);
        if (munmap(busstop[i].count, sizeof(int)) == -1)
        {
            exit_error("Munmap failed.\n", 1);
        }
    }

    if (munmap(action_id, sizeof(int)) == -1)
    {
        exit_error("Munmap failed.\n", 1);
    }
    if (munmap(count_in_bus, sizeof(int)) == -1)
    {
        exit_error("Munmap failed.\n", 1);
    }
    if (munmap(allskiers, sizeof(int)) == -1)
    {
        exit_error("Munmap failed.\n", 1);
    }
}

void output(int action_type, int id, int busstop)
{
    wait_sem(&mutex_output);
    *action_id += 1;
    switch (action_type)
    {
        // bus
    case BUS_STARTED:
        fprintf(file, "%d: BUS: started\n", *action_id);
        break;
    case BUS_ARRIVED_TO:
        fprintf(file, "%d: BUS: arrived to %d\n", *action_id, busstop);
        break;
    case BUS_LEAVING:
        fprintf(file, "%d: BUS: leaving %d\n", *action_id, busstop);
        break;
    case BUS_ARRIVED_TO_FINAL:
        fprintf(file, "%d: BUS: arrived to final\n", *action_id);
        break;
    case BUS_LEAVING_FINAL:
        fprintf(file, "%d: BUS: leaving final\n", *action_id);
        break;
    case BUS_FINISH:
        fprintf(file, "%d: BUS: finish\n", *action_id);
        break;
        // skier
    case L_STARTED:
        fprintf(file, "%d: L %d: started\n", *action_id, id);
        break;
    case L_ARRIVED_TO:
        fprintf(file, "%d: L %d: arrived to %d\n", *action_id, id, busstop);
        break;
    case L_BOARDING:
        fprintf(file, "%d: L %d: boarding\n", *action_id, id);
        break;
    case L_GOING_TO_SKI:
        fprintf(file, "%d: L %d: going to ski\n", *action_id, id);
        break;
    default:
        exit_error("Internal error: Unknown output action_type.\n", 1);
    }

    post_sem(&mutex_output);
}

void skibus(Arg args)
{
    output(BUS_STARTED, NONE, NONE);
    for (int i = 1; i <= args.Z; i++)
    {
        usleep_random_in_range(0, args.TB);
        // wait_sem(&boarding);
        output(BUS_ARRIVED_TO, NONE, i);
        for (int s = 0; s < *busstop[i].count; s++)
        {
            post_sem(&busstop[i].semaphore);
        }
        output(BUS_LEAVING, NONE, i);
        // post_sem(&boarding);
        if (i == args.Z)
        {
            usleep_random_in_range(0, args.TB);
            output(BUS_ARRIVED_TO_FINAL, NONE, NONE);
            wait_sem(&leaving);
            output(BUS_LEAVING_FINAL, NONE, NONE);

            for (int j = 1; j <= args.Z; j++)
            {
                if (*busstop[j].count > 0)
                {
                    i = 1;
                    break;
                }
            }
        }
    }
    output(BUS_FINISH, NONE, NONE);
    exit(0);
}

void skier(Arg args, int id, int idz)
{
    output(L_STARTED, id, NONE);
    allskiers++;
    usleep_random_in_range(0, args.TL);

    output(L_ARRIVED_TO, id, idz);
    busstop[idz].count++;
    wait_sem(&busstop[idz].semaphore);

    output(L_BOARDING, id, NONE);
    if (*count_in_bus < args.K)
    {
        count_in_bus++;
        busstop[idz].count--;
    }
    if (busstop[idz].count == 0 || *count_in_bus == args.K)
    {
        post_sem(&busstop[idz].semaphore);
    }

    output(L_GOING_TO_SKI, id, NONE);
    count_in_bus--;
    if (count_in_bus == 0)
    {
        post_sem(&leaving);
    }

    exit(0);
}

int main(int argc, char **argv)
{
    Arg args = ParseArgs(argc, argv);
    init_semaphores(args);
    clear_and_open_output_file();
    setbuf(file, NULL);
    setbuf(stdout, NULL);
    setbuf(stderr, NULL);

    pid_t skibus_id = fork();
    if (skibus_id < 0)
    {
        exit_error("Skibus fork error\n", 1);
    }
    if (skibus_id == 0)
    {
        skibus(args);
    }

    for (int i = 1; i <= args.L; i++)
    {
        pid_t skier_id = fork();
        if (skier_id < 0)
        {
            exit_error("Fork skier failed.\n", 1);
        }
        if (skier_id == 0)
        {
            skier(args, i, random_int(1, args.Z));
        }
    }

    while (wait(NULL) > 0)
        ;
    if (fclose(file) == EOF)
    {
        exit_error("Final fclose close.\n", 1);
    }

    cleanup_semaphores(args);
    return 0;
}
