#include "PointSortUtilities.h"
#include "PointSortMPI.h"

int main(int argc, char *argv[]) {
    MPI_Init(&argc, &argv);
    int nprocs, rank;
    MPI_Comm_size(MPI_COMM_WORLD, &nprocs);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);

    int n = 569;
    Point *array = NULL;
    double sort_time;
    int i;
    if (rank == 0) {
        array = (Point *)malloc(n * sizeof(Point));
        for (i = 0; i < n; i++) {
            array[i].x = 0.0 + (double)rand() / RAND_MAX * (1000.0 - 0.0);
            array[i].y = 0.0 + (double)rand() / RAND_MAX * (1000.0 - 0.0);
        }
    }

    QuickPointSortMPI(&array, n, nprocs, rank, &sort_time, 1, 1);

    if (rank == 0) {
        printf("\nElapsed time: %10.6lf s\n", sort_time);
        PrintPointArray(array, n); // Uncomment to print the array
        free(array);
    }

    MPI_Finalize();
    return 0;
}
