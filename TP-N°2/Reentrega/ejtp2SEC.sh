#!/bin/bash
#SBATCH -N 1
#SBATCH --exclusive
#SBATCH --partition=Blade
#SBATCH -o ./outputs/outputsSec.txt
#SBATCH -e ./outputs/errorsSec.txt
./p1 $1 $2