#!/bin/bash
#SBATCH -N 2
#SBATCH --exclusive
#SBATCH --tasks-per-node=8
#SBATCH -o ./outputs/outputshibrido1.txt
#SBATCH -e ./outputs/errorshibrido1.txt

mpirun --bind-to none tp3H $1 $2