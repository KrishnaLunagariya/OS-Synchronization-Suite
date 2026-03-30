#include <stdio.h>
#include <pthread.h>
#include <unistd.h>

#define N 5

pthread_mutex_t chopsticks[N];
pthread_mutex_t print_lock;

/* Colors */
#define RED "\033[1;31m"
#define GREEN "\033[1;32m"
#define YELLOW "\033[1;33m"
#define BLUE "\033[1;34m"
#define CYAN "\033[1;36m"
#define RESET "\033[0m"

void print_msg(const char *color, int id, const char *msg)
{
    pthread_mutex_lock(&print_lock);
    printf("%sPhilosopher %d %s%s\n", color, id, msg, RESET);
    pthread_mutex_unlock(&print_lock);
}

void* philosopher(void *arg)
{
    int id = *(int*)arg;

    while (1)
    {
        /* Thinking */
        print_msg(YELLOW, id, "is THINKING");
        sleep(3);

        /* Try to pick chopsticks */
        print_msg(BLUE, id, "is trying to pick chopsticks");

        if (id % 2 == 0)
        {
            pthread_mutex_lock(&chopsticks[id]);
            pthread_mutex_lock(&chopsticks[(id+1)%N]);
        }
        else
        {
            pthread_mutex_lock(&chopsticks[(id+1)%N]);
            pthread_mutex_lock(&chopsticks[id]);
        }

        /* Eating */
        print_msg(GREEN, id, "is EATING");
        sleep(3);

        /* Release chopsticks */
        pthread_mutex_unlock(&chopsticks[id]);
        pthread_mutex_unlock(&chopsticks[(id+1)%N]);

        print_msg(RED, id, "has released chopsticks");

        sleep(2);
    }

    return NULL;
}

int main()
{
    pthread_t threads[N];
    int id[N];

    pthread_mutex_init(&print_lock, NULL);

    for (int i = 0; i < N; i++)
        pthread_mutex_init(&chopsticks[i], NULL);

    printf(CYAN "\n===== Dining Philosophers (Deadlock-Free Simulation) =====\n\n" RESET);

    for (int i = 0; i < N; i++)
    {
        id[i] = i;
        pthread_create(&threads[i], NULL, philosopher, &id[i]);
    }

    for (int i = 0; i < N; i++)
        pthread_join(threads[i], NULL);

    return 0;
}
