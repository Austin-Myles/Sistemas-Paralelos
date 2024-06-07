#!/bin/bash
#SBATCH -N 2
#SBATCH --exclusive
#SBATCH --tasks-per-node=8
#SBATCH -o ./outputs/outputsmpi2.txt
#SBATCH -e ./outputs/errorsmpi2.txt

mpirun tp3MPI $1 $2