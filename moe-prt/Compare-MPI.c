// #include "mpi.h"
// #include "ClosestPointUtilities.h"

// int main(int argc, char* argv[]) 
// {
//     MPI_Init(&argc, &argv);

//     int rank, size;
//     MPI_Comm_rank(MPI_COMM_WORLD, &rank);
//     MPI_Comm_size(MPI_COMM_WORLD, &size);

//     if (argc != 3) {
//         if (rank == 0) {
//             printf("Usage: %s sampleFilePath resultFilePath\n", argv[0]);
//         }
//         MPI_Finalize();
//         return 0;
//     }

//     const char* sampleFilePath = argv[1];
//     const char* resultFilePath = argv[2];
//     Point* points = NULL;
//     size_t numPoints;  // Number of points
//     double minDistance;
//     double *all_minDistances = NULL; // Gather all minimum distances to rank 0
//     clock_t start, end;
//     double cpu_time_used;
//     // Only rank 0 reads points from file
//     if (rank == 0) {
//         printf("Reading the points...\n");
//         double minX, maxX; // X domain limits
//         double minY, maxY; // Y domain limits
//         int dimension;
//         int errcode = readPointsFromFile(sampleFilePath, &points, &numPoints, &minX, &maxX, &minY, &maxY, &dimension);
//         if (errcode) {
//             printf("Read Points From File Failed with Error Code %d!\n", errcode);
//             MPI_Finalize();
//             return -1;
//         }
//         printf("File read successfully!\n");
//     }
    
//     if (rank==0)
//     {
//         start = clock();
//         all_minDistances = (double*)malloc(size * sizeof(double));
//     }
    
//     // Calculate the number of points per process, the starting index and number of points for this process
//     MPI_Bcast(&numPoints, 1, MPI_UNSIGNED_LONG, 0, MPI_COMM_WORLD);

//     size_t points_per_process = numPoints / size;
//     size_t remaining_points   = numPoints % size;
    
//     size_t local_numPoints = points_per_process + (rank == size - 1 ? remaining_points : 0);
//     Point *local_points = (Point*)malloc(local_numPoints * sizeof(Point));
    
//     // Scatter the points to all processes
//     MPI_Scatter(points, local_numPoints, MPI_BYTE, local_points, local_numPoints, MPI_BYTE, 0, MPI_COMM_WORLD);
//     printf("Process %d received %ld points\n", rank, local_numPoints);
//     // Free Memory
//     if (rank == 0) {
//         free(points);
//     }

//     // Solve Closest Point Problem [Brute-Force]
//     if (closestPairDAC(local_points, local_numPoints, &minDistance)){
//         fprintf(stderr, "Solution Failed!\n");
//         free(local_points);
//         MPI_Finalize();
//     }
//     free(local_points);

//     MPI_Gather(&minDistance, 1, MPI_DOUBLE, all_minDistances, 1, MPI_DOUBLE, 0, MPI_COMM_WORLD);

//     if (rank==0)
//     {
//         // Find the global minimum distance
//         minDistance = DBL_MAX;
//         for (i = 0; i < size; i++) {
//             if (all_minDistances[i] < minDistance) {
//                 minDistance = all_minDistances[i];
//             }
//         }
//         free(all_minDistances);
//         end = clock();

//         cpu_time_used = ((double)(end - start)) / CLOCKS_PER_SEC;
//         printf("Process %d completed in %10.6lf seconds!\n", rank, cpu_time_used);

//         // Open file to write the results
//         printf("Writing results...\n");
//         FILE *fp = fopen(resultFilePath, "w");
//         if (fp == NULL) {
//             fprintf(stderr, "Failed to open result file.\n");
//             MPI_Finalize();
//             return -1;
//         }

//         fprintf(fp, "The closest pair distance is %15.10lf\n", minDistance);
//         fprintf(fp, "Elapsed Time: %15.10lf seconds\n", cpu_time_used);

//         fclose(fp);
//         printf("Results written to %s\n", resultFilePath);
//     }

//     MPI_Finalize();
//     return 0;
// }
