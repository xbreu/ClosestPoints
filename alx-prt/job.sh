#!/bin/bash

#SBATCH --job-name=mpi_test
#SBATCH -o mpi_out%j.out
#SBATCH -e mpi_err%j.err
#SBATCH -N 10
#SBATCH --ntasks-per-node=2

echo -e '\n submitted Open MPI job'
echo 'hostname'
hostname

# load Open MPI module
# module load openmpi/gcc

# compile the C file
mpicc main.c point.c -o closest_points -Wall -lm

# run compiled main.c file
mpirun -n 1 --use-hwthread-cpus ./closest_points
