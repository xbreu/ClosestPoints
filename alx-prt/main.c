#include <mpi.h>
#include <stdio.h>
#include "point.h"

#define NO_TAG (0)

int main(int argc, char **argv) {
    // int world_size, world_rank;
    struct point_array *a;
    a = read_file("input.dat");

    // Initialize MPI environment
    MPI_Init(&argc, &argv);

    // Send and receive the second point from the input
    struct point b;
    MPI_Send(&a->points[1], POINT_SIZE, MPI_BYTE, 0, NO_TAG, MPI_COMM_WORLD);
    MPI_Recv(&b, POINT_SIZE, MPI_BYTE, 0, NO_TAG, MPI_COMM_WORLD, 0);
    printf("Received point: (%lf, %lf)\n", b.x, b.y);

    // Finish MPI environment and free the memory used
    MPI_Finalize();
    free_point_array(a);
}
