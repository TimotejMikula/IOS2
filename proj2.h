/**
 * @file proj2.h
 * @brief IOS – projekt 2 (synchronizace)
 *
 * Zadání je inspirováno knihou Allen B. Downey: The Little Book of Semaphores (The Senate Bus
 * problem)
 *
 * @author Timotej Mikula, xmikult00
 * @date 16.4.2024
 */

#ifndef PROJ2_H
#define PROJ2_H

#define ARGCOUNT 6

#define ANY -1
#define NONE 0
#define Z_STARTED 1
#define Z_GOING_HOME 2
#define Z_ENTERING_OFFICE 3
#define Z_CALLED_BY_WORKER 4

#define U_STARTED 5
#define U_SERVING_SERVICE 6
#define U_SERVICE_FINISHED 7
#define U_BREAK 8
#define U_BREAK_FINISHED 9
#define U_GOING_HOME 10
#define CLOSING 11
#define DEBUG 12

typedef struct
{
    int L;
    int Z;
    int K;
    int TL;
    int TB;
} Arg;

typedef struct
{
    int *count;
    sem_t *queue;
} Service;

Arg ParseArgs(int argc, char *const argv[]);
void exit_error(char *msg, int errcode);
int isInteger(char *str);
void skibus(Arg args);
void skier(Arg args, int id);

void wait_sem(sem_t **sem);
void post_sem(sem_t **sem);
void init_sem(sem_t **sem, int value);
void init_semaphores();
void destroy_sem(sem_t **sem);
void cleanup_semaphores();

void clear_and_open_output_file();
void output(int action_type, int id, int service);

#endif
