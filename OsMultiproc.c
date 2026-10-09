#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/wait.h>

#define MAX_LINE 1024
#define FILE_COUNT 20
#define NUM_PROC 6
#define MAX_RECORDS 1000000

typedef struct {
    char Region[100];
    char Country[100];
    char ItemType[100];
    char SalesChannel[100];
    double revenue;
    double profit;
} info;

int main() {
    char choice;
    char input[100];

    printf("Choose a category:\n");
    printf("a. Region\nb. Country\nc. Item Type\nd. Sales Channel\n");
    printf("Enter choice: ");
    scanf(" %c", &choice);

    printf("Enter your search value: ");
    scanf(" %[^\n]", input);
     struct timeval start, end;
    gettimeofday(&start, NULL);

    pid_t pids[NUM_PROC];
    int fd[NUM_PROC][2];
    int filesPerProc = FILE_COUNT / NUM_PROC;
    int extra = FILE_COUNT % NUM_PROC;

    const char *files[FILE_COUNT] = {
        "sales-data-1M/xaa.csv", "sales-data-1M/xab.csv","sales-data-1M/xac.csv", "sales-data-1M/xad.csv",
        "sales-data-1M/xae.csv", "sales-data-1M/xaf.csv","sales-data-1M/xag.csv", "sales-data-1M/xah.csv",
        "sales-data-1M/xai.csv", "sales-data-1M/xaj.csv","sales-data-1M/xak.csv", "sales-data-1M/xal.csv",
        "sales-data-1M/xam.csv", "sales-data-1M/xan.csv", "sales-data-1M/xao.csv", "sales-data-1M/xap.csv",
        "sales-data-1M/xaq.csv", "sales-data-1M/xar.csv", "sales-data-1M/xas.csv", "sales-data-1M/xat.csv"
    };

    for (int p = 0; p < NUM_PROC; p++) {
        pipe(fd[p]);
        if ((pids[p] = fork()) == 0) {   // child
            close(fd[p][0]);

            info *data = malloc(sizeof(info) * MAX_RECORDS);
            int recordCount = 0;

            int startFile = p * filesPerProc;
            int endFile = startFile + filesPerProc - 1;
            if (p == NUM_PROC - 1) endFile += extra;

            char line[MAX_LINE];

            for (int f = startFile; f <= endFile; f++) {
                FILE *fp = fopen(files[f], "r");
                if (!fp) continue;

                fgets(line, sizeof(line), fp); // skip header

                while (fgets(line, sizeof(line), fp)) {
                    info record;
                    char *token;
                    int col = 0;

                    token = strtok(line, ",");
                    while (token) {
                        switch (col) {
                            case 0: strcpy(record.Region, token); break;
                            case 1: strcpy(record.Country, token); break;
                            case 2: strcpy(record.ItemType, token); break;
                            case 3: strcpy(record.SalesChannel, token); break;
                            case 11: record.revenue = atof(token); break;
                            case 13: record.profit = atof(token); break;
                        }
                        token = strtok(NULL, ",");
                        col++;
                    }

                    data[recordCount++] = record;
                }

                fclose(fp);
            }

            long localOrders = 0;
            double localRevenue = 0.0, localProfit = 0.0;

            for (int i = 0; i < recordCount; i++) {
                int match = 0;

                if (choice == 'a' && strcmp(data[i].Region, input) == 0) match = 1;
                else if (choice == 'b' && strcmp(data[i].Country, input) == 0) match = 1;
                else if (choice == 'c' && strcmp(data[i].ItemType, input) == 0) match = 1;
                else if (choice == 'd' && strcmp(data[i].SalesChannel, input) == 0) match = 1;

                if (match) {
                    localOrders++;
                    localRevenue += data[i].revenue;
                    localProfit += data[i].profit;
                }
            }

            // send results only
            write(fd[p][1], &localOrders, sizeof(long));
            write(fd[p][1], &localRevenue, sizeof(double));
            write(fd[p][1], &localProfit, sizeof(double));

            free(data);
            close(fd[p][1]);
            exit(0);
        }

        close(fd[p][1]);
    }

    long totalOrders = 0;
    double totalRevenue = 0.0, totalProfit = 0.0;

    for (int p = 0; p < NUM_PROC; p++) {
        long o;
        double r, pf;

        read(fd[p][0], &o, sizeof(long));
        read(fd[p][0], &r, sizeof(double));
        read(fd[p][0], &pf, sizeof(double));

        close(fd[p][0]);

        totalOrders += o;
        totalRevenue += r;
        totalProfit += pf;

        wait(NULL);
    }

    printf("\nResults for '%s':\n", input);
    printf("Total Orders: %ld\n", totalOrders);
    printf("Total Revenue: %.2f\n", totalRevenue);
    printf("Total Profit: %.2f\n", totalProfit);
    gettimeofday(&end, NULL);
    double elapsed = (end.tv_sec - start.tv_sec) + (end.tv_usec - start.tv_usec)/1e6;
    printf("Time taken for search: %.4f seconds\n", elapsed );


    return 0;
}
