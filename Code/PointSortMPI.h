
#ifndef PointSortMPI_h

#define PointSortMPI_h
#include <mpi.h>
#include "PointSortUtilities.h"
int QuickPointSortMPI(Point** array, int array_size, int nprocs, int rank, double* sort_time, int use_tree, int sort_by_x) {
    int i;
    Point *data_sub = NULL;
    int *send_counts = NULL, *send_displacements = NULL;
    double time_init, time_end;

    if (rank == 0) {
        send_counts = (int *)malloc(nprocs * sizeof(int));
        send_displacements = (int *)malloc(nprocs * sizeof(int));

        // Determine the send counts and displacements
        int remainder = array_size % nprocs;
        int sum = 0;
        for (i = 0; i < nprocs; i++) {
            send_counts[i] = (array_size / nprocs + (i < remainder ? 1 : 0))* sizeof(Point);
            send_displacements[i] = sum;
            sum += send_counts[i];
        }
        time_init = MPI_Wtime();
    }

    // Broadcast the size of each chunk to all processes
    int elements_per_proc;
    MPI_Scatter(send_counts, 1, MPI_INT, &elements_per_proc, 1, MPI_INT, 0, MPI_COMM_WORLD);
    elements_per_proc /= sizeof(Point);

    // Allocate space for each process's sub-array
    data_sub = (Point *)malloc(elements_per_proc * sizeof(Point));

    // Scatter the data using MPI_Scatterv
    MPI_Scatterv(*array, send_counts, send_displacements, MPI_BYTE, data_sub,
                 elements_per_proc* sizeof(Point), MPI_BYTE, 0, MPI_COMM_WORLD);

    // Quick sort in serial
    QuickPointSort(data_sub, elements_per_proc, sort_by_x);

    // Merge Algorithm Tree-based
    if (use_tree) {
        if (rank == 0) {
            free(send_counts);  send_counts = NULL;
            free(send_displacements); send_displacements = NULL;
            free(*array); *array = NULL;
        }
        int step = 1;
        MPI_Barrier(MPI_COMM_WORLD);
        while (step < nprocs) {
            if (rank % (2 * step) == 0) {
                if (rank + step < nprocs) {
                    int recv_count = -1;
                    MPI_Recv(&recv_count, 1, MPI_INT, rank + step, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
                    Point* buffer_recv = (Point*) malloc(recv_count * sizeof(Point));
                    MPI_Recv(buffer_recv, recv_count * sizeof(Point), MPI_BYTE, rank + step, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
                    Point* merger = (Point*) malloc((recv_count + elements_per_proc) * sizeof(Point));
                    MergeTwoSortedPointArrays(buffer_recv, recv_count, data_sub, elements_per_proc, merger, sort_by_x);
                    elements_per_proc += recv_count;
                    free(buffer_recv); buffer_recv = NULL;
                    free(data_sub);    data_sub = NULL;
                    data_sub = merger;
                }
            } else {
                int rank_recver = rank - step;
                MPI_Send(&elements_per_proc, 1, MPI_INT, rank_recver, 0, MPI_COMM_WORLD);
                MPI_Send(data_sub, elements_per_proc * sizeof(Point), MPI_BYTE, rank_recver, 0, MPI_COMM_WORLD);
                break;
            }
            step *= 2;
        }
        if (rank == 0) {
            int err_idx;
            time_end = MPI_Wtime() - time_init;
            if (!IsSortingPointsCorrect(data_sub, array_size, &err_idx, sort_by_x)) {
                return -1;
            }
            *array = data_sub; 
            *sort_time = time_end;
        }
    } else {
        MPI_Gatherv(data_sub, elements_per_proc * sizeof(Point), MPI_BYTE, *array, send_counts, send_displacements, MPI_BYTE, 0, MPI_COMM_WORLD);
        if (rank == 0) {
            int *current_indices = (int *)calloc(nprocs, sizeof(int));
            int sorted_index = 0;

            while (sorted_index < array_size) {
                Point min_value = {INFINITY, INFINITY};
                int min_index = -1;

                for (i = 0; i < nprocs; i++) {
                    int idx = send_displacements[i] + current_indices[i];
                    if (current_indices[i] < send_counts[i] && 
                        ((sort_by_x && (*array)[idx].x < min_value.x) || (!sort_by_x && (*array)[idx].y < min_value.y))) {
                        min_value = (*array)[idx];
                        min_index = i;
                    }
                }
                (*array)[sorted_index++] = min_value;
                current_indices[min_index]++;
            }
            free(current_indices);
            free(send_counts);
            free(send_displacements);
        }
        free(data_sub);

        if (rank == 0) {
            int err_idx;
            time_end = MPI_Wtime() - time_init;
            if (!IsSortingPointsCorrect(*array, array_size, &err_idx, sort_by_x)) {
                return -1;
            }
            *sort_time = time_end;
        }
    }

    MPI_Barrier(MPI_COMM_WORLD);
    return 0;
}

#endif