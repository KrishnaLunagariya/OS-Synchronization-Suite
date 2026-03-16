#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <semaphore.h>
#include <unistd.h>
#include <time.h>

#define READERS 3
#define WRITERS 2
#define MAX_ITER 3

sem_t mutex, wrt;
int readcount = 0;

// ---------- Reader Function ----------
void *reader(void *arg)
{
    int id = *(int *)arg;

    for(int i = 0; i < MAX_ITER; i++)
    {
        printf("\033[1;34mReader %d wants to read...\033[0m\n", id);

        sem_wait(&mutex);
        readcount++;

        if(readcount == 1)
            sem_wait(&wrt);  // first reader blocks writers

        sem_post(&mutex);

        // ---- Reading Section ----
        printf("\033[1;32mReader %d is reading...\033[0m\n", id);

        FILE *fp = fopen("shared.txt", "r");
        if(fp != NULL)
        {
            char ch;
            while((ch = fgetc(fp)) != EOF)
                putchar(ch);
            fclose(fp);
        }

        printf("\n");

        // ---- Exit Section ----
        sem_wait(&mutex);
        readcount--;

        if(readcount == 0)
            sem_post(&wrt);  // last reader allows writer

        sem_post(&mutex);

        sleep(1);
    }

    pthread_exit(NULL);
}

// ---------- Writer Function ----------
void *writer(void *arg)
{
    int id = *(int *)arg;

    for(int i = 0; i < MAX_ITER; i++)
    {
        printf("\033[1;31mWriter %d wants to write...\033[0m\n", id);

        sem_wait(&wrt);

        printf("\033[1;35mWriter %d is writing...\033[0m\n", id);

        FILE *fp = fopen("shared.txt", "a");
        if(fp != NULL)
        {
            time_t now = time(NULL);
            fprintf(fp, "Written by Writer %d at %s", id, ctime(&now));
            fclose(fp);
        }

        sem_post(&wrt);

        sleep(2);
    }

    pthread_exit(NULL);
}

// ---------- Show File Content ----------
void showFile()
{
    FILE *fp = fopen("shared.txt", "r");
    if(fp == NULL)
    {
        printf("File not found!\n");
        return;
    }

    printf("\n----- FILE CONTENT -----\n");
    char ch;
    while((ch = fgetc(fp)) != EOF)
        putchar(ch);
    printf("\n------------------------\n");

    fclose(fp);
}

// ---------- Main Function ----------
int main()
{
    int choice;

    sem_init(&mutex, 0, 1);
    sem_init(&wrt, 0, 1);

    while(1)
    {
        printf("\n===== Reader-Writer Project =====\n");
        printf("1. Run Synchronization\n");
        printf("2. Show File Content\n");
        printf("3. Exit\n");
        printf("Enter choice: ");
        scanf("%d", &choice);

        if(choice == 1)
        {
            pthread_t r[READERS], w[WRITERS];
            int rid[READERS], wid[WRITERS];

            for(int i = 0; i < READERS; i++)
            {
                rid[i] = i + 1;
                pthread_create(&r[i], NULL, reader, &rid[i]);
            }

            for(int i = 0; i < WRITERS; i++)
            {
                wid[i] = i + 1;
                pthread_create(&w[i], NULL, writer, &wid[i]);
            }

            for(int i = 0; i < READERS; i++)
                pthread_join(r[i], NULL);

            for(int i = 0; i < WRITERS; i++)
                pthread_join(w[i], NULL);

            printf("\nSynchronization Completed Successfully!\n");
        }
        else if(choice == 2)
        {
            showFile();
        }
        else if(choice == 3)
        {
            printf("Exiting Program...\n");
            break;
        }
        else
        {
            printf("Invalid Choice!\n");
        }
    }

    sem_destroy(&mutex);
    sem_destroy(&wrt);

    return 0;
}
