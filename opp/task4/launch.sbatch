#!/bin/bash
#SBATCH -J solomennikov
#SBATCH -p compclass
#SBATCH -o reports/job%j.out
#SBATCH -e reports/job%j.err
#SBATCH -N 4 # Total number of nodes requested
#SBATCH -n 8 # Total number of mpi tasks requested
#SBATCH -t 00:01:00
#Run executable files and commands

module load mpi/intelmpi
module load itac

#TCP-ports, opened for MPI-communications
export I_MPI_PORT_RANGE=23000:23100

which mpiexec
#mpiexec -ppn 1 -n 1 ./src/prog1.out
mpiexec -trace -ppn 4 -n 8 ./src/prog2.out
