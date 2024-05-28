#!/bin/bash
#SBATCH -N 1
#SBATCH --exclusive
#SBATCH --partition=Blade
#SBATCH -o ./outputs/outputsPT.txt
#SBATCH -e ./outputs/errorsPT.txt
./p2 $1 $2 $3
