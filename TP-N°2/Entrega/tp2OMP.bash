#!/bin/bash
#SBATCH -N 1
#SBATCH --exclusive
#SBATCH -o ./outputs/outputsOMP.txt
#SBATCH -e ./outputs/errorsOMP.txt
export OMP_NUM_THREADS=2
./p3.exe 512 64
./p3.exe 1024 64
./p3.exe 2048 64
./p3.exe 4096 64
export OMP_NUM_THREADS=4
./p3.exe 512 64
./p3.exe 1024 64
./p3.exe 2048 64
./p3.exe 4096 64
export OMP_NUM_THREADS=8
./p3.exe 512 64
./p3.exe 1024 64
./p3.exe 2048 64
./p3.exe 4096 64
