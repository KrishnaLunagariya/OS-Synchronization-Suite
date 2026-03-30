#include <stdio.h>
#include <pthread.h>
#include <unistd.h>
#include <string.h>

#define N 5
#define CYCLES 5

pthread_mutex_t chopstick[N];
pthread_mutex_t screen;

/* Color codes */
#define RED "\033[1;31m"
#define GREEN "\033[1;32m"
#define YELLOW "\033[1;33m"
#define CYAN "\033[1;36m"
#define RESET "\033[0m"

char *state[N]={"THINK","THINK","THINK","THINK","THINK"};

int think[N]={0};
int wait_t[N]={0};
int eat[N]={0};

void circular_table()
{
    printf("\n          (%s P0)\n",state[0]);
    printf("     (%s P4)       (%s P1)\n",state[4],state[1]);

    printf("\n        [ DINING TABLE ]\n\n");

    printf("     (%s P3)       (%s P2)\n",state[3],state[2]);
}

void dashboard()
{
    printf("\033[2J\033[H");

    printf(CYAN "\n========== AI DINING DASHBOARD ==========\n" RESET);

    circular_table();

    printf("\nID   STATE   THINK   WAIT   EAT\n");

    for(int i=0;i<N;i++)
    {
        printf("%d    %s     %d      %d      %d\n",
        i,state[i],think[i],wait_t[i],eat[i]);
    }

    printf("\n------------------------------------------\n");

    int active=0;

    for(int i=0;i<N;i++)
        if(strcmp(state[i],"EAT")==0)
            active++;

    int cpu=active*10;

    printf("CPU UTILIZATION : [");

    for(int i=0;i<cpu/10;i++)
        printf("#");

    for(int i=cpu/10;i<10;i++)
        printf("-");

    printf("] %d%%\n",cpu);

    printf("\nAI ANALYSIS:\n");

    if(active>0)
        printf(GREEN "System stable (no starvation)\n" RESET);
    else
        printf(YELLOW "All philosophers thinking\n" RESET);
}

void* philosopher(void* num)
{
    int id=*(int*)num;

    for(int i=0;i<CYCLES;i++)
    {
        pthread_mutex_lock(&screen);

        state[id]="THINK";
        think[id]++;
        dashboard();

        pthread_mutex_unlock(&screen);

        sleep(1);

        pthread_mutex_lock(&screen);

        state[id]="WAIT";
        wait_t[id]++;
        dashboard();

        pthread_mutex_unlock(&screen);

        if(id%2==0)
        {
            pthread_mutex_lock(&chopstick[id]);
            pthread_mutex_lock(&chopstick[(id+1)%N]);
        }
        else
        {
            pthread_mutex_lock(&chopstick[(id+1)%N]);
            pthread_mutex_lock(&chopstick[id]);
        }

        pthread_mutex_lock(&screen);

        state[id]="EAT";
        eat[id]++;
        dashboard();

        pthread_mutex_unlock(&screen);

        sleep(1);

        pthread_mutex_unlock(&chopstick[id]);
        pthread_mutex_unlock(&chopstick[(id+1)%N]);

        usleep(200000);
    }

    return NULL;
}

int main()
{
    pthread_t thread[N];
    int id[N];

    pthread_mutex_init(&screen,NULL);

    for(int i=0;i<N;i++)
        pthread_mutex_init(&chopstick[i],NULL);

    for(int i=0;i<N;i++)
    {
        id[i]=i;
        pthread_create(&thread[i],NULL,philosopher,&id[i]);
    }

    for(int i=0;i<N;i++)
        pthread_join(thread[i],NULL);

    printf("\nSimulation Finished\n");

    return 0;
}
