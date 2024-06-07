#!/bin/bash
#SBATCH -N 4
#SBATCH --exclusive
#SBATCH --tasks-per-node=8
#SBATCH -o ./outputs/outputsmpi4.txt
#SBATCH -e ./outputs/errorsmpi4.txt

mpirun tp3MPI $1 $2