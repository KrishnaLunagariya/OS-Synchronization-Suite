#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <semaphore.h>
#include <unistd.h>
#include <time.h>

// ===== COLOR CODES =====
#define RESET "\033[0m"
#define RED "\033[31m"
#define GREEN "\033[32m"
#define YELLOW "\033[33m"
#define CYAN "\033[36m"
#define BLUE "\033[34m"

// ===== GLOBAL VARIABLES =====
int *buffer;
int BUFFER_SIZE;
int PRODUCERS;
int CONSUMERS;
int TOTAL_ITEMS;

int PRODUCER_DELAY;
int CONSUMER_DELAY;

int in = 0;
int out = 0;

int produced_count = 0;
int consumed_count = 0;

sem_t empty, full;
pthread_mutex_t mutex;

FILE *logfile;

// ===== PRINT TIME =====
void print_time() {
    time_t now = time(NULL);
    printf(YELLOW "[%ld] " RESET, now);
}

// ===== DISPLAY BUFFER =====
void display_buffer() {

    printf(CYAN "\nBUFFER: [ ");

    for(int i = 0; i < BUFFER_SIZE; i++)
        printf("%d ", buffer[i]);

    printf("]\n" RESET);
}

// ===== AI SEMAPHORE PREDICTOR =====
void ai_predictor() {

    int buffer_size;
    int produced;
    int consumed;

    int mutex_val = 1;
    int full_val;
    int empty_val;

    printf("\n=========== AI SEMAPHORE PREDICTION ===========\n");

    printf("Enter Buffer Size: ");
    scanf("%d",&buffer_size);

    printf("Enter Produced Items: ");
    scanf("%d",&produced);

    printf("Enter Consumed Items: ");
    scanf("%d",&consumed);

    full_val = produced - consumed;

    if(full_val < 0)
        full_val = 0;

    if(full_val > buffer_size)
        full_val = buffer_size;

    empty_val = buffer_size - full_val;

    printf("\n----- SEMAPHORE VALUES -----\n");

    printf("Mutex = %d\n",mutex_val);
    printf("Full  = %d\n",full_val);
    printf("Empty = %d\n",empty_val);

    printf("\n----- AI ANALYSIS -----\n");

    if(full_val == buffer_size)
    {
        printf("Buffer FULL\n");
        printf("Producer will BLOCK (sem_wait(empty))\n");
    }
    else if(full_val == 0)
    {
        printf("Buffer EMPTY\n");
        printf("Consumer will BLOCK (sem_wait(full))\n");
    }
    else
    {
        printf("System operating normally\n");
    }

    printf("-----------------------------------------------\n");
}

// ===== PRODUCER THREAD =====
void* producer(void *arg) {

    int id = *(int*)arg;

    while(1) {

        pthread_mutex_lock(&mutex);

        if(produced_count >= TOTAL_ITEMS) {
            pthread_mutex_unlock(&mutex);
            break;
        }

        produced_count++;

        pthread_mutex_unlock(&mutex);

        int item = rand() % 100;

        sem_wait(&empty);
        pthread_mutex_lock(&mutex);

        buffer[in] = item;

        print_time();
        printf(GREEN "Producer %d produced %d at slot %d\n" RESET,id,item,in);

        fprintf(logfile,"Producer %d produced %d\n",id,item);
        fflush(logfile);

        in = (in + 1) % BUFFER_SIZE;

        display_buffer();

        pthread_mutex_unlock(&mutex);
        sem_post(&full);

        sleep(PRODUCER_DELAY);
    }

    return NULL;
}

// ===== CONSUMER THREAD =====
void* consumer(void *arg) {

    int id = *(int*)arg;

    while(1) {

        pthread_mutex_lock(&mutex);

        if(consumed_count >= TOTAL_ITEMS) {
            pthread_mutex_unlock(&mutex);
            break;
        }

        pthread_mutex_unlock(&mutex);

        sem_wait(&full);
        pthread_mutex_lock(&mutex);

        if(consumed_count >= TOTAL_ITEMS) {
            pthread_mutex_unlock(&mutex);
            sem_post(&full);
            break;
        }

        int item = buffer[out];
        buffer[out] = 0;

        consumed_count++;

        print_time();
        printf(RED "Consumer %d consumed %d from slot %d\n" RESET,id,item,out);

        fprintf(logfile,"Consumer %d consumed %d\n",id,item);
        fflush(logfile);

        out = (out + 1) % BUFFER_SIZE;

        display_buffer();

        pthread_mutex_unlock(&mutex);
        sem_post(&empty);

        sleep(CONSUMER_DELAY);
    }

    return NULL;
}

// ===== MAIN FUNCTION =====
int main() {

    int choice;

    printf(BLUE "\n===== PRODUCER CONSUMER AI SYSTEM =====\n" RESET);

    printf("\n1. Run Producer Consumer Simulation\n");
    printf("2. Run AI Semaphore Predictor\n");

    printf("Enter Choice: ");
    scanf("%d",&choice);

    if(choice == 2) {
        ai_predictor();
        return 0;
    }

    srand(time(NULL));

    printf("\nEnter Buffer Size: ");
    scanf("%d",&BUFFER_SIZE);

    printf("Enter Number of Producers: ");
    scanf("%d",&PRODUCERS);

    printf("Enter Number of Consumers: ");
    scanf("%d",&CONSUMERS);

    printf("Enter Total Items to Produce: ");
    scanf("%d",&TOTAL_ITEMS);

    printf("Producer Delay (seconds): ");
    scanf("%d",&PRODUCER_DELAY);

    printf("Consumer Delay (seconds): ");
    scanf("%d",&CONSUMER_DELAY);

    buffer = (int*)malloc(sizeof(int)*BUFFER_SIZE);

    for(int i=0;i<BUFFER_SIZE;i++)
        buffer[i] = 0;

    logfile = fopen("log.txt","w");

    pthread_t *p = malloc(sizeof(pthread_t)*PRODUCERS);
    pthread_t *c = malloc(sizeof(pthread_t)*CONSUMERS);

    int *pid = malloc(sizeof(int)*PRODUCERS);
    int *cid = malloc(sizeof(int)*CONSUMERS);

    sem_init(&empty,0,BUFFER_SIZE);
    sem_init(&full,0,0);

    pthread_mutex_init(&mutex,NULL);

    for(int i=0;i<PRODUCERS;i++) {
        pid[i]=i+1;
        pthread_create(&p[i],NULL,producer,&pid[i]);
    }

    for(int i=0;i<CONSUMERS;i++) {
        cid[i]=i+1;
        pthread_create(&c[i],NULL,consumer,&cid[i]);
    }

    for(int i=0;i<PRODUCERS;i++)
        pthread_join(p[i],NULL);

    for(int i=0;i<CONSUMERS;i++)
        pthread_join(c[i],NULL);

    printf(BLUE "\n=========== EXECUTION COMPLETED ===========\n" RESET);

    printf("Total Produced: %d\n",produced_count);
    printf("Total Consumed: %d\n",consumed_count);

    fclose(logfile);

    sem_destroy(&empty);
    sem_destroy(&full);
    pthread_mutex_destroy(&mutex);

    free(buffer);
    free(p);
    free(c);
    free(pid);
    free(cid);

    return 0;
}
