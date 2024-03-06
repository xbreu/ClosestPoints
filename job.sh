#!/bin/bash

#SBATCH --job-name=mpi_test
#SBATCH -o mpi_out%j.out
#SBATCH -e mpi_err%j.err
#SBATCH -N 3
#SBATCH --ntasks-per-node=2

echo -e '\n submitted Open MPI job'
echo 'hostname'
hostname

# load Open MPI module
module load openmpi/gcc

# compile the C file
mpicc main.c -o closest_points

# run compiled main.c file
mpirun ./closest_points
