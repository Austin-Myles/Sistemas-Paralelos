#!/bin/bash
#SBATCH -N 1
#SBATCH --exclusive
#SBATCH --partition=Blade
#SBATCH -o ./outputs/outputsOMP.txt
#SBATCH -e ./outputs/errorsOMP.txt
export OMP_NUM_THREADS=$1
./p3 $2 $3
