#!/bin/bash
#SBATCH -N 1
#SBATCH --exclusive
#SBATCH --tasks-per-node=8
#SBATCH -o ./outputs/outputsmpi1.txt
#SBATCH -e ./outputs/errorsmpi1.txt

mpirun tp3MPI $1 $2

