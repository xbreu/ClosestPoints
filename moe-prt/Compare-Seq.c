#include <stdlib.h>
#include <time.h>
#include "ClosestPointUtilities.h"

int main(int argc, char* argv[]) {
    // Argument Management
    if (argc != 3) {
        printf("Definition:\n\tThis Function Compares the Closest Point Problem Sequential Solution.\n");
        printf("Usage:\n\tCompare-Seq sampleFilePath resultFilePath\n");
        printf("Arguments:\n");
        printf("\t- sampleFilePath: Path to the file containing sample points\n");
        printf("\t- resultFilePath: Path to the file containing results\n");
        return 0;
    }

    const char* sampleFilePath = argv[1];
    const char* resultFilePath = argv[2];
    size_t numPoints; // Number of points
    double minX, maxX; // X domain limits
    double minY, maxY; // Y domain limits
    int dimension;

    double minDistance_BF, minDistance_DAC;
    double cpu_time_used_BF, cpu_time_used_DAC;
    clock_t start, end;

    // Read the points from file
    Point *points = NULL;
    printf("Reading the points...\n");
    int errcode = readPointsFromFile(sampleFilePath, &points, &numPoints, &minX, &maxX, &minY, &maxY, &dimension);
    if (errcode) {
        printf("Read Points From File Failed with Error Code %d!\n", errcode);
        return -1;
    }
    printf("File read successfully!\n");

    printf("Solving Closest Point Problem [Brute-Force]...\n");
    start = clock();
    if (closestPairBruteForce(points, numPoints, &minDistance_BF) != 0) {
        fprintf(stderr, "Failed to find the closest pair.\n");
        free(points);
        return -1;
    }
    end = clock();
    cpu_time_used_BF = ((double) (end - start)) / CLOCKS_PER_SEC;
    printf("Solution Completed in %10.6lf seconds!\n", cpu_time_used_BF);

    printf("Solving Closest Point Problem [Divide and Conquere]...\n");
    start = clock();
    if (closestPairDAC(points, numPoints, &minDistance_DAC) != 0) {
        fprintf(stderr, "Failed to find the closest pair.\n");
        free(points);
        return -1;
    }
    end = clock();
    cpu_time_used_DAC = ((double) (end - start)) / CLOCKS_PER_SEC;
    printf("Solution Completed in %f seconds!\n", cpu_time_used_DAC);

    // Open file to write the results
    printf("Writing results...\n");
    FILE *fp = fopen(resultFilePath, "w");
    if (fp == NULL) {
        fprintf(stderr, "Failed to open result file.\n");
        free(points);
        return -1;
    }

    // fprintf(fp, "The closest pair is between points at indices %ld and %ld:\n", index1, index2);
    // fprintf(fp, "Point-A %ld (X:%15.10f, Y:%15.10f)\n", index1, points[index1].x, points[index1].y);
    // fprintf(fp, "Point-B %ld (X:%15.10f, Y:%15.10f)\n", index2, points[index2].x, points[index2].y);
    // fprintf(fp, "Distance: %.10f\n", minDistance);
    
    fprintf(fp, "Brute-Force-------------------\n");
    fprintf(fp, "The closest pair distance is %15.10lf\n", minDistance_BF);
    fprintf(fp, "Elapsed Time: %15.10f seconds\n", cpu_time_used_BF);

    fprintf(fp, "Divide And Conquere-------------------\n");
    fprintf(fp, "The closest pair distance is %15.10lf\n", minDistance_DAC);
    fprintf(fp, "Elapsed Time: %15.10lf seconds\n", cpu_time_used_DAC);

    fclose(fp);
    printf("Results written to %s\n", resultFilePath);

    free(points); // Clean up allocated memory
    printf("Done!\n");  
    return 0;
}
