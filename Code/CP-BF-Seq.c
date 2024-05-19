#include "ClosestPairUtilities.h"

int main(int argc, char* argv[]) {
    // Argument Management
    if (argc != 3) {
        printf("Definition:\n\tThis Function Solves the Closest Point Problem (Brute-Force)\n");
        printf("Usage:\n\tCP-BF-Seq sampleFilePath resultFilePath\n");
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

    size_t index1, index2;
    double minDistance;

    clock_t start, end;
    double cpu_time_used;

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
    if (closestPairBruteForce(points, numPoints, &minDistance) != 0) {
        fprintf(stderr, "Failed to find the closest pair.\n");
        free(points);
        return -1;
    }
    end = clock();
    cpu_time_used = ((double) (end - start)) / CLOCKS_PER_SEC;
    printf("The closest pair distance is %15.10lf\n", minDistance);
    printf("Solution Completed in %15.10lf seconds!\n", cpu_time_used);

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
    
    fprintf(fp, "The closest pair distance is %15.10lf\n", minDistance);
    fprintf(fp, "Elapsed Time: %15.10lf seconds\n", cpu_time_used);

    fclose(fp);
    printf("Results written to %s\n", resultFilePath);

    free(points); // Clean up allocated memory
    printf("Done!\n");  
    return 0;
}
