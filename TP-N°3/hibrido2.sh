#!/bin/bash
#SBATCH -N 4
#SBATCH --exclusive
#SBATCH --tasks-per-node=8
#SBATCH -o ./outputs/outputshibrido2.txt
#SBATCH -e ./outputs/errorshibrido2.txt

mpirun --bind-to none tp3H $1 $2