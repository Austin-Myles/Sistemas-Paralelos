#!/bin/bash
#SBATCH -N 1
#SBATCH --exclusive
#SBATCH -o ./outputs/outputsPT.txt
#SBATCH -e ./outputs/errorsPT.txt
./p2.exe 2 512 64
./p2.exe 4 512 64
./p2.exe 8 512 64
./p2.exe 2 1024 64
./p2.exe 4 1024 64
./p2.exe 8 1024 64
./p2.exe 2 2048 64
./p2.exe 4 2048 64
./p2.exe 8 2048 64
./p2.exe 2 4096 64
./p2.exe 4 4096 64
./p2.exe 8 4096 64
