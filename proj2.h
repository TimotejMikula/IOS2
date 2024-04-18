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
#define BUS_STARTED 1
#define BUS_ARRIVED_TO 2
#define BUS_LEAVING 3
#define BUS_ARRIVED_TO_FINAL 4
#define BUS_LEAVING_FINAL 5
#define BUS_FINISH 6

#define L_STARTED 7
#define L_ARRIVED_TO 8
#define L_BOARDING 9
#define L_GOING_TO_SKI 10

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
} Bus_Stop;

Arg ParseArgs(int argc, char *const argv[]);
void exit_error(char *msg, int errcode);
int isInteger(char *str);
void skibus(Arg args);
void skier(Arg args, int id, int idz);
void skier_busstop(int id, int idz);
bool check_any_skier(int idz);
void skiers_boarding(int id, int idz);
void skier_going_to_ski(int id);

void wait_sem(sem_t **sem);
void post_sem(sem_t **sem);
void init_sem(sem_t **sem, int value);
void init_semaphores(void);
void destroy_sem(sem_t **sem);
void cleanup_semaphores(void);
void usleep_random_in_range(int lower, int upper);

void clear_and_open_output_file(void);
void output(int action_type, int id, int busstop);

#endif
