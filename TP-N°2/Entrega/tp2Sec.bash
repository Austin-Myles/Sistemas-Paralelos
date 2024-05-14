#!/bin/bash
#SBATCH -N 1
#SBATCH --exclusive
#SBATCH -o ./outputs/outputsSec.txt
#SBATCH -e ./outputs/errorsSec.txt
./p1.exe 512 64
./p1.exe 1024 64
./p1.exe 2048 64
./p1.exe 4096 64