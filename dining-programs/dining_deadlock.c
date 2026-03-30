#include <stdio.h>
#include <pthread.h>

#define N 5

pthread_mutex_t chopsticks[N];

/* Colors */
#define RED "\033[1;31m"
#define GREEN "\033[1;32m"
#define YELLOW "\033[1;33m"
#define BLUE "\033[1;34m"
#define CYAN "\033[1;36m"
#define RESET "\033[0m"

/* Heavy CPU work */
void heavy_cpu()
{
    volatile long i;
    for(i=0;i<800000000;i++);
}

void* philosopher(void *num)
{
    int id = *(int*)num;

    printf(YELLOW "Philosopher %d is THINKING\n" RESET,id);

    while(1)
    {
        heavy_cpu();   // keep CPU busy

        /* Pick LEFT chopstick */
        pthread_mutex_lock(&chopsticks[id]);
        printf(BLUE "Philosopher %d picked LEFT chopstick\n" RESET,id);

        printf(RED "Philosopher %d waiting for RIGHT chopstick\n" RESET,id);

        /* Busy wait for right chopstick */
        while(pthread_mutex_trylock(&chopsticks[(id+1)%N]) != 0)
        {
            heavy_cpu();   // spin → high CPU usage
        }

        /* This part will never be reached due to deadlock */
        printf(GREEN "Philosopher %d is EATING\n" RESET,id);
    }

    return NULL;
}

int main()
{
    pthread_t thread[N];
    int phil[N];

    printf(CYAN "\n===== Dining Philosophers Deadlock + High CPU Demo =====\n\n" RESET);

    for(int i=0;i<N;i++)
        pthread_mutex_init(&chopsticks[i],NULL);

    for(int i=0;i<N;i++)
    {
        phil[i]=i;
        pthread_create(&thread[i],NULL,philosopher,&phil[i]);
    }

    for(int i=0;i<N;i++)
        pthread_join(thread[i],NULL);

    return 0;
}
