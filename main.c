#include <mpi.h>
#include <stdio.h>

struct point {
    int x;
    int y;
};

int main(int argc, char **argv) {
    int world_size, world_rank;

    // Initialize MPI environment
    MPI_Init(&argc, &argv);

    // number of processes
    MPI_Comm_size(MPI_COMM_WORLD, &world_size);

    // the rank of the current process
    MPI_Comm_rank(MPI_COMM_WORLD, &world_rank);

    char processor_name[MPI_MAX_PROCESSOR_NAME]; // gets the name of the processor
    int name_len;
    MPI_Get_processor_name(processor_name, &name_len);

    printf("Hello world from processor %s, rank %d out of %d processors\n",
           processor_name, world_rank, world_size);

    MPI_Finalize(); // finish MPI environment
}
