#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>
#include <time.h>
#define MAX_LINE 1024
#define FILE_COUNT 20
#define NUM_THREADS 6

typedef struct {
    char Region[100];
    char Country[100];
    char ItemType[100];
    char SalesChannel[100];
    double revenue;
    double profit;
} info;

const char *files[FILE_COUNT] = {
    "sales-data-1M/xaa.csv", "sales-data-1M/xab.csv", "sales-data-1M/xac.csv", "sales-data-1M/xad.csv",
    "sales-data-1M/xae.csv", "sales-data-1M/xaf.csv", "sales-data-1M/xag.csv", "sales-data-1M/xah.csv",
    "sales-data-1M/xai.csv", "sales-data-1M/xaj.csv", "sales-data-1M/xak.csv", "sales-data-1M/xal.csv",
    "sales-data-1M/xam.csv", "sales-data-1M/xan.csv", "sales-data-1M/xao.csv", "sales-data-1M/xap.csv",
    "sales-data-1M/xaq.csv", "sales-data-1M/xar.csv", "sales-data-1M/xas.csv", "sales-data-1M/xat.csv"
};

char choice;
char input[100];
long threadOrders[NUM_THREADS];
double threadRevenue[NUM_THREADS];
double threadProfit[NUM_THREADS];

typedef struct {
    int tid;
    int startFile;
    int endFile;
} ThreadArg;

void* threadFunc(void* arg) {
    ThreadArg *targ = (ThreadArg*)arg;
    int tid = targ->tid;
    int start = targ->startFile;
    int end = targ->endFile;
    free(targ);
    long localOrders = 0;
    double localRevenue = 0.0, localProfit = 0.0;
    char line[MAX_LINE];

    for (int f = start; f < end; f++) {
        FILE *fp = fopen(files[f], "r");
        if (!fp) {
            fprintf(stderr, "Error opening %s\n", files[f]);
            continue;
        }
        fgets(line, sizeof(line), fp);
        while (fgets(line, sizeof(line), fp)) {
            info record;
            char *token, *saveptr;
            int i = 0;
            token = strtok_r(line, ",", &saveptr);
            while (token) {
                switch (i) {
                    case 0: strcpy(record.Region, token); break;
                    case 1: strcpy(record.Country, token); break;
                    case 2: strcpy(record.ItemType, token); break;
                    case 3: strcpy(record.SalesChannel, token); break;
                    case 11: record.revenue = atof(token); break;
                    case 13: record.profit = atof(token); break;
                }
                token = strtok_r(NULL, ",", &saveptr);
                i++;
            }

            int match = 0;
            if (choice == 'a' && strcmp(record.Region, input) == 0) match = 1;
            else if (choice == 'b' && strcmp(record.Country, input) == 0) match = 1;
            else if (choice == 'c' && strcmp(record.ItemType, input) == 0) match = 1;
            else if (choice == 'd' && strcmp(record.SalesChannel, input) == 0) match = 1;

            if (match) {
                localOrders++;
                localRevenue += record.revenue;
                localProfit += record.profit;
            }
        }
        fclose(fp);
    }

    threadOrders[tid] = localOrders;
    threadRevenue[tid] = localRevenue;
    threadProfit[tid] = localProfit;
    pthread_exit(NULL);
}

int main() {
    printf("Choose a category:\n");
    printf("a. Region\nb. Country\nc. Item Type\nd. Sales Channel\n");
    printf("Enter choice: ");
    scanf("%c", &choice);
    printf("Enter your search value: ");
    scanf("%s", input);

    struct timeval start, end;
    gettimeofday(&start, NULL);

    pthread_t threads[NUM_THREADS];
    int filesPerThread = FILE_COUNT / NUM_THREADS;
    int extra = FILE_COUNT % NUM_THREADS;

    for (int t = 0; t < NUM_THREADS; t++) {
        ThreadArg *arg = malloc(sizeof(ThreadArg));
        arg->tid = t;
        arg->startFile = t * filesPerThread;
        arg->endFile = arg->startFile + filesPerThread;
        if (t == NUM_THREADS - 1) arg->endFile += extra;
        pthread_create(&threads[t], NULL, threadFunc, arg);
    }

    long totalOrders = 0;
    double totalRevenue = 0.0, totalProfit = 0.0;

    for (int t = 0; t < NUM_THREADS; t++) {
        pthread_join(threads[t], NULL);
        totalOrders += threadOrders[t];
        totalRevenue += threadRevenue[t];
        totalProfit += threadProfit[t];
    }

    gettimeofday(&end, NULL);
    double elapsed = (end.tv_sec - start.tv_sec) + (end.tv_usec - start.tv_usec)/1e6;

    printf("\nResults for '%s':\n", input);
    printf("Total Orders: %ld\n", totalOrders);
    printf("Total Revenue: %.2f\n", totalRevenue);
    printf("Total Profit: %.2f\n", totalProfit);
    printf("Time taken: %.4f seconds\n", elapsed);

    return 0;
}
