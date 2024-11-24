#!/bin/bash
#SBATCH -J d_zhdanov_mpiprog # Job name
#SBATCH -p compclass  # Queue name (or "compclass_unstable", or "gpuserv", or "a100serv")
#SBATCH -o 4_job.%j.out # Name of stdout output file (%j expands to %jobId)
#SBATCH -e 4_job.%j.err # Name of stderr output file (%j expands to %jobId)
#SBATCH -N 2 # Total number of nodes requested
#SBATCH -n 4 # Total number of mpi tasks requested
#SBATCH -t 00:01:00 # Run time (hh:mm:ss) - 1 minute
#Run executable files and commands

module load mpi/intelmpi

#TCP-ports, opened for MPI-communications
export I_MPI_PORT_RANGE=23000:23100

which mpirun
mpirun -ppn 2 -n 4 ./mpi 19200 0.00001 10
~
