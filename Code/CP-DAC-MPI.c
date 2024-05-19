#include "ClosestPairUtilities.h"
#include "PointSortMPI.h"

int main(int argc, char* argv[]) 
{
    MPI_Init(&argc, &argv);

    int rank, size;
    int i, j; // For Loops
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &size);

    if (argc != 3) {
        if (rank == 0) {
            printf("Usage: %s sampleFilePath resultFilePath\n", argv[0]);
        }
        MPI_Finalize();
        return 0;
    }

    const char* sampleFilePath = argv[1];
    const char* resultFilePath = argv[2];
    Point* points = NULL;
    size_t numPoints;  // Number of points
    double minDistance=DBL_MAX;
    double *zonal_min_dist = NULL; // Gather all minimum distances to rank 0
    clock_t start, end;
    double cpu_time_used;
    double* midpointsX = (double*) malloc((size-1) * sizeof(double));
    // Only rank 0 reads points from file
    if (rank == 0) {
        printf("Reading the points...\n");
        double minX, maxX; // X domain limits
        double minY, maxY; // Y domain limits
        int dimension;
        int errcode = readPointsFromFile(sampleFilePath, &points, &numPoints, &minX, &maxX, &minY, &maxY, &dimension);
        if (errcode) {
            printf("Read Points From File Failed with Error Code %d!\n", errcode);
            MPI_Finalize();
            return -1;
        }
        printf("File read %lu Points successfully!\n", numPoints);
    }

    // Update Other Processes
    MPI_Bcast(&numPoints, 1, MPI_UNSIGNED_LONG, 0, MPI_COMM_WORLD);
    
    if (rank==0)
    {
        start = clock();
        zonal_min_dist = (double*) malloc(size * sizeof(double));
    }
    
    // Sort All points According to X coordinate (Sequenctial APPROACH)
    // if (rank == 0)
    //     qsort(points, numPoints, sizeof(Point), compareX);
        
    double sorting_time;
    QuickPointSortMPI(&points, numPoints, size, rank, &sorting_time, 1, 1);
    MPI_Barrier(MPI_COMM_WORLD);
    
    int points_per_process = numPoints / size;
    int remaining_points   = numPoints % size;    
    int local_numPoints = (rank < remaining_points) ? (points_per_process + 1) : points_per_process;
    Point *local_points = (Point*)malloc(local_numPoints * sizeof(Point));
    int* sendcounts = (int*) malloc(size * sizeof(int));
    int* displs     = (int*) malloc(size * sizeof(int));
    
    int accumulated_displs = 0;
    for (i = 0; i < size; i++) {
        sendcounts[i] = (i < remaining_points) ? (points_per_process + 1) * sizeof(Point) : points_per_process * sizeof(Point);
        displs[i] = accumulated_displs;
        accumulated_displs += sendcounts[i];
    }

    if (rank==0)
    {
        printf("Mid Points Are:\n");
        for (i = 0; i < size-1; i++){
            midpointsX[i] = 0.5 * (points[(displs[i+1]/sizeof(Point))-1].x + points[displs[i+1]/sizeof(Point)].x);
            // printf(" - Left: %-15.10lf  Right: %-15.10lf, Mid: %-15.10lf\n", 
            //         points[(displs[i+1]/sizeof(Point))-1].x, points[(displs[i+1]/sizeof(Point))].x, midpointsX[i]);
        }
    }
    
    MPI_Bcast(midpointsX, (size-1), MPI_DOUBLE, 0, MPI_COMM_WORLD);

    // Scatter the points to all processes
    MPI_Scatterv(points, sendcounts, displs, MPI_BYTE, local_points, sendcounts[rank], MPI_BYTE, 0, MPI_COMM_WORLD);
    // printf("Process %d received %d points\n", rank, local_numPoints);

    // Free Unnecessary Memory
    if (rank == 0) free(points); points = NULL;
    free(sendcounts); sendcounts = NULL;
    free(displs); displs = NULL;

    // Solve Closest Point Problem [Brute-Force]
    if (closestPairDACMPI(local_points, local_numPoints, &minDistance)){
        fprintf(stderr, "Solution Failed!\n");
        free(local_points); local_points = NULL;
        MPI_Finalize();
    }
    // else{
    //     printf("process %d with %d points is %-15.10lf [Zonal]\n", rank, local_numPoints, minDistance);
    // }

    // Calculate The Least in Each Domain
    MPI_Gather(&minDistance, 1, MPI_DOUBLE, zonal_min_dist, 1, MPI_DOUBLE, 0, MPI_COMM_WORLD);
    if (rank==0)
    {
        minDistance = DBL_MAX;
        for (i = 0; i < size; i++) {
            if (zonal_min_dist[i] < minDistance) {
                minDistance = zonal_min_dist[i];
            }
        }
        // free(zonal_min_dist);
    }
    
    // Send The New Min and Go for Strips
    MPI_Barrier(MPI_COMM_WORLD);
    MPI_Bcast(&minDistance, 1, MPI_DOUBLE, 0, MPI_COMM_WORLD);
    MPI_Barrier(MPI_COMM_WORLD);
    if (size > 1){
        // Assume each process computes a 'strip' array and a count 'SR SL' of how many points are in it
        int count_SR=0, count_SL=0; 
        Point *stripR = NULL, *stripL = NULL;  
        if (rank == 0) { // Boundary is between 0 and 1 ONLY!
            count_SR = 0; count_SL = 0;
            stripR = (Point*) malloc(local_numPoints* sizeof(Point));
            stripL = NULL;
            for (i = 0; i < local_numPoints; i++) {
                if (fabs(local_points[i].x - midpointsX[rank]) < minDistance) {
                    stripR[count_SR].x = local_points[i].x; 
                    stripR[count_SR].y = local_points[i].y; 
                    count_SR++;
                    // printf("Right-of-Rank %d Element (%d): %lf   %lf\n", rank, count_SR-1, stripR[count_SR-1].x, stripR[count_SR-1].y);
                }
            }
        }
        else if (rank == size-1){ // Boundary is between size-2 and size-1 ONLY!
            count_SR = 0; count_SL = 0;
            stripR = NULL;
            stripL = (Point*) malloc(local_numPoints* sizeof(Point));  
            for (i = 0; i < local_numPoints; i++) {
                if ((fabs(local_points[i].x - midpointsX[rank-1]) < minDistance)) {
                    stripL[count_SL].x = local_points[i].x; 
                    stripL[count_SL].y = local_points[i].y; 
                    count_SL++;
                    // printf("Left-of-Rank %d Element (%d): %lf   %lf\n", rank, count_SL-1, stripL[count_SL-1].x, stripL[count_SL-1].y);
                }
            }
        }
        else{
            count_SR = 0; count_SL = 0;
            stripR = (Point*) malloc(local_numPoints* sizeof(Point));
            stripL = (Point*) malloc(local_numPoints* sizeof(Point));
            for (i = 0; i < local_numPoints; i++) {
                if (fabs(local_points[i].x - midpointsX[rank]) < minDistance) {
                    stripR[count_SR].x = local_points[i].x; 
                    stripR[count_SR].y = local_points[i].y; 
                    count_SR++;
                    // printf("Right-of-Rank %d Element (%d): %lf   %lf\n", rank, count_SR-1, stripR[count_SR-1].x, stripR[count_SR-1].y);
                }
                if (fabs(local_points[i].x - midpointsX[rank-1]) < minDistance) {
                    stripL[count_SL].x = local_points[i].x; 
                    stripL[count_SL].y = local_points[i].y; 
                    count_SL++;
                    // printf("Left-of-Rank %d Element (%d): %lf   %lf\n", rank, count_SL-1, stripL[count_SL-1].x, stripL[count_SL-1].y);
                }
            }
        }

        // printf("Rank %d: Left-Strip has %d Node and Right-Strip has %d Nodes.\n", rank, count_SL, count_SR);
        // Free midPointsX and Local Points since we dont need them
        if (rank==0) {
            free(midpointsX);
            midpointsX = NULL;
        }
        free(local_points); local_points = NULL;

        // Exchange Points at Strips and calculate the boundary Cases!
        double mid_min = DBL_MAX;
        if (rank == 0){
            // Send To the next process : Send Info
            int count_sent = count_SR;
            MPI_Send(&count_sent, 1, MPI_INT, (rank+1), 0, MPI_COMM_WORLD);
            // Preparing Send Buffer (For each node we send to next Neighbor)
            Point* buff_send = (Point*) malloc((count_sent*sizeof(Point))); 
            // memcpy(buff_send, stripR, (count_sent*sizeof(Point)));
            for (i = 0; i < count_sent; i++){
                buff_send[i].x = stripR[i].x;
                buff_send[i].y = stripR[i].y;
            }
            // Sending the buffer
            MPI_Send(buff_send, (count_sent*sizeof(Point)), MPI_BYTE, (rank+1), 1, MPI_COMM_WORLD);
            
            // For Debug!
            // printf("Rank %d Sent %d Points to Rank %d\n", rank, count_sent, rank+1);
            // for (i = 0; i < count_sent; i++)
            // {
            //     printf("Point-%d Sent by Rank %d to Rank %d [Send-Buffer]: X: %15.10lf  Y:%15.10lf\n", i, rank, rank+1, buff_send[i].x, buff_send[i].y);
            // }
            
            // Free Buffer Memory
            if (buff_send != NULL)
            {
                free(buff_send); buff_send = NULL;
            }
        }
        else if(rank == size-1){
            // Recieve From The Previous Process: Recv Info
            MPI_Status status;
            int count_recv;
            MPI_Recv(&count_recv, 1, MPI_INT, (rank-1), 0, MPI_COMM_WORLD, &status);
            // Preparing Recv Buffer (For each node we recv from prev Neighbor)
            Point* buff_recv = (Point*) malloc((count_recv*sizeof(Point))); 
            MPI_Recv(buff_recv, (count_recv*sizeof(Point)), MPI_BYTE, (rank-1), 1, MPI_COMM_WORLD, &status);

            // For Debug!
            // printf("Rank %d Recv %d Points From Rank %d\n", rank, count_recv, rank-1);
            // for (i = 0; i < count_recv; i++)
            // {
            //     printf("Point-%d Recv by Rank %d From Rank %d [Recv-Buffer]: X %15.10lf  Y %15.10lf\n", i, rank, rank-1, buff_recv[i].x, buff_recv[i].y);
            // }

            // Merge Operation
            Point* merged_strips = (Point*) malloc(((count_SL+count_recv)*sizeof(Point)));
            for (i = 0; i < count_recv; i++){
                merged_strips[i].x = buff_recv[i].x;
                merged_strips[i].y = buff_recv[i].y;
            }
            for (i = count_recv; i < count_SL+count_recv; i++){
                merged_strips[i].x = stripL[i-count_recv].x;
                merged_strips[i].y = stripL[i-count_recv].y;
            }
            // memcpy(merged_strips, buff_recv, (count_recv*sizeof(Point)));
            // memcpy((merged_strips+(count_recv*sizeof(Point))), stripL, (count_SL*sizeof(Point)));
            mid_min = stripClosest(merged_strips, (count_SL + count_recv), minDistance);
            if (buff_recv != NULL)
            {
                free(buff_recv); buff_recv = NULL;
            }

            // For Debug!
            // printf("Results on Rank %d Min is: %15.10lf [Boundary]\n", rank, mid_min);
            // for (i = 0; i < count_recv + count_SL; i++)
            // {
            //     printf("Point-%d in Rank %d [Merge-Buffer]: X %15.10lf  Y %15.10lf\n", i, rank, merged_strips[i].x, merged_strips[i].y);
            // }

            // Free Buffer Memory
            if (merged_strips != NULL)
            {
                free(merged_strips); merged_strips = NULL;
            }
        }
        else{
            // Recieve From The Previous Process: Recv Info
            MPI_Status status;
            int count_recv; // Size of received data in bytes
            MPI_Recv(&count_recv, 1, MPI_INT, (rank-1), 0, MPI_COMM_WORLD, &status);
            // Preparing Recv Buffer (For each node we recv from prev Neighbor)
            Point* buff_recv = (Point*) malloc((count_recv*sizeof(Point))); 
            MPI_Recv(buff_recv, (count_recv*sizeof(Point)), MPI_BYTE, (rank-1), 1, MPI_COMM_WORLD, &status);
            // For Debug!
            // printf("Rank %d recieved %d Points From Rank %d\n", rank, count_recv, rank-1);
            // for (i = 0; i < count_recv; i++)
            // {
            //     printf("Point-%d Recv by Rank %d From Rank %d [Recv-Buffer]: X %15.10lf  Y %15.10lf\n", i, rank, rank-1, buff_recv[i].x, buff_recv[i].y);
            // }
            // Merge Operation
            Point* merged_strips = (Point*) malloc(((count_SL+count_recv)*sizeof(Point)));
            
            for (i = 0; i < count_recv; i++){
                merged_strips[i].x = buff_recv[i].x;
                merged_strips[i].y = buff_recv[i].y;
            }
            for (i = count_recv; i < count_SL+count_recv; i++){
                merged_strips[i].x = stripL[i-count_recv].x;
                merged_strips[i].y = stripL[i-count_recv].y;
            }
            
            // memcpy(merged_strips, buff_recv, (count_recv*sizeof(Point)));
            // memcpy((merged_strips+(count_recv*sizeof(Point))), stripL, (count_SL*sizeof(Point)));
            mid_min = stripClosest(merged_strips, (count_SL + count_recv), minDistance);
            if (buff_recv != NULL)
            {
                free(buff_recv); buff_recv = NULL;
            }
            // For Debug!
            // printf("Results on Rank %d Min is: %15.10lf [Boundary]\n", rank, mid_min);
            // for (i = 0; i < count_recv + count_SL; i++)
            // {
            //     printf("Point-%d in Rank %d [Merge-Buffer]: X %15.10lf  Y %15.10lf\n", i, rank, merged_strips[i].x, merged_strips[i].y);
            // }

            if (merged_strips != NULL)
            {
                free(merged_strips); merged_strips = NULL;
            }
            
            // Send To the next process : Send Info
            int count_sent = count_SR;
            MPI_Send(&count_sent, 1, MPI_INT, (rank+1), 0, MPI_COMM_WORLD);
            // Preparing Recv Buffer (For each node we recv from prev Neighbor)
            Point* buff_send = (Point*) malloc((count_sent*sizeof(Point))); // Allocate buffer for received data
            // memcpy(buff_send, stripR, (count_sent*sizeof(Point)));
            for (i = 0; i < count_sent; i++){
                buff_send[i].x = stripR[i].x;
                buff_send[i].y = stripR[i].y;
            }
            MPI_Send(buff_send, (count_sent*sizeof(Point)), MPI_BYTE, (rank+1), 1, MPI_COMM_WORLD);
            // For Debug!
            // printf("Rank %d Sent %d Points to Rank %d\n", rank, count_sent, rank+1);
            // for (i = 0; i < count_sent; i++)
            // {
            //     printf("Point-%d Sent by Rank %d to Rank %d [Send-Buffer]: X: %15.10lf  Y:%15.10lf\n", i, rank, rank+1, buff_send[i].x, buff_send[i].y);
            // }
            if (buff_send != NULL)
            {
                free(buff_send); buff_send = NULL;
            }
        }

        MPI_Barrier(MPI_COMM_WORLD);
        // Free Memory
        if (stripL != NULL) free(stripL);
        if (stripR != NULL) free(stripR);
        MPI_Gather(&mid_min, 1, MPI_DOUBLE, zonal_min_dist, 1, MPI_DOUBLE, 0, MPI_COMM_WORLD);

        if (rank == 0)
        {
            for (i = 1; i < size; i++) {
                // printf("%d: %15.10lf\n", i, zonal_min_dist[i]);
                if (zonal_min_dist[i] < minDistance) {
                    minDistance = zonal_min_dist[i];
                }
            }
        }
    }

    if (rank==0)
    {
        free(zonal_min_dist);
        end = clock();
        cpu_time_used = ((double)(end - start)) / CLOCKS_PER_SEC;
        printf("The closest pair distance is %-15.10lf\n", minDistance);
        printf("Solution Completed in %-10.6lf seconds!\n", cpu_time_used);

        // Open file to write the results
        printf("Writing results...\n");
        FILE *fp = fopen(resultFilePath, "w");
        if (fp == NULL) {
            fprintf(stderr, "Failed to open result file.\n");
            MPI_Finalize();
            return -1;
        }

        fprintf(fp, "The closest pair distance is %-15.10lf\n", minDistance);
        fprintf(fp, "Elapsed Time: %-15.10lf seconds\n", cpu_time_used);

        fclose(fp);
        printf("Results written to %s\n", resultFilePath);
    }

    MPI_Finalize();
    return 0;
}
