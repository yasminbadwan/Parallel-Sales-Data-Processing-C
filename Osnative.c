#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/time.h>

#define MAX_LINE 1024
#define FILE_COUNT 20
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

    const char *files[FILE_COUNT] = {
        "sales-data-1M/xaa.csv", "sales-data-1M/xab.csv", "sales-data-1M/xac.csv", "sales-data-1M/xad.csv",
        "sales-data-1M/xae.csv", "sales-data-1M/xaf.csv", "sales-data-1M/xag.csv", "sales-data-1M/xah.csv",
        "sales-data-1M/xai.csv", "sales-data-1M/xaj.csv", "sales-data-1M/xak.csv", "sales-data-1M/xal.csv",
        "sales-data-1M/xam.csv", "sales-data-1M/xan.csv", "sales-data-1M/xao.csv", "sales-data-1M/xap.csv",
        "sales-data-1M/xaq.csv", "sales-data-1M/xar.csv", "sales-data-1M/xas.csv", "sales-data-1M/xat.csv"};
    info *records = malloc(sizeof(info) * MAX_RECORDS);
    if (!records) {
        fprintf(stderr, "Memory allocation failed\n");
        return 1;}
    long recordCount = 0;
    char line[MAX_LINE];
     struct timeval Pstart, Pend;
    gettimeofday(&Pstart, NULL);
    for (int f = 0; f < FILE_COUNT; f++) {
        FILE *fp = fopen(files[f], "r");
        if (!fp) {
            fprintf(stderr, "Error opening %s\n", files[f]);continue;}
        fgets(line, sizeof(line), fp);
        while (fgets(line, sizeof(line), fp)) {
            if (recordCount >= MAX_RECORDS) break;
            info record;
            memset(&record, 0, sizeof(record));
            char *token = strtok(line, ",");
            int col = 0;
            while (token) {
                switch (col) {
                    case 0: strcpy(record.Region, token); break;
                    case 1: strcpy(record.Country, token); break;
                    case 2: strcpy(record.ItemType, token); break;
                    case 3: strcpy(record.SalesChannel, token); break;
                    case 11: record.revenue = atof(token); break;
                    case 13: record.profit = atof(token); break;}
                col++;
                token = strtok(NULL, ",");}
                records[recordCount++] = record;}
        fclose(fp);}


    long totalOrders = 0;
    double totalRevenue = 0.0, totalProfit = 0.0;

    for (long i = 0; i < recordCount; i++) {
        int match = 0;
        if (choice == 'a' && strcmp(records[i].Region, input) == 0) match = 1;
        else if (choice == 'b' && strcmp(records[i].Country, input) == 0) match = 1;
        else if (choice == 'c' && strcmp(records[i].ItemType, input) == 0) match = 1;
        else if (choice == 'd' && strcmp(records[i].SalesChannel, input) == 0) match = 1;

        if (match) {
            totalOrders++;
            totalRevenue += records[i].revenue;
            totalProfit += records[i].profit;}}
     free(records);
    gettimeofday(&Pend, NULL);
    double elapsedP = (Pend.tv_sec - Pstart.tv_sec) + (Pend.tv_usec - Pstart.tv_usec)/1e6;
    printf("\nResults for '%s':\n", input);
    printf("Total Orders: %ld\n", totalOrders);
    printf("Total Revenue: %.2f\n", totalRevenue);
    printf("Total Profit: %.2f\n", totalProfit);

    gettimeofday(&end, NULL);
    double elapsed = (end.tv_sec - start.tv_sec) + (end.tv_usec - start.tv_usec)/1e6;
    printf("Time taken for search: %.4f seconds\n", elapsed );
     printf("Time taken for serial: %.5f seconds\n", elapsed-elapsedP );

    return 0;
}
