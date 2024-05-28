#!/bin/bash

#headers = "Dim - bs, 512 - 64, 1024 - 64, 2048 - 64, 4096 - 64"
#echo $headers > ./outputs/time_sec.csv
echo
row="Time,"
dim=256
for i in {1..4}; do
    dim=$((dim * 2))
    sbatch --wait ./ejtp2SEC.sh $dim 64
    exTime=$(cat ./outputs/outputsSec.txt)
    row="$row,$exTime"
done
echo $row >> ./outputs/time_sec.csv

headers="Threads, 512 - 64, 1024 - 64, 2048 - 64, 4096 - 64"
echo $headers > ./outputs/time_pt.csv
echo
for i in {1..3}; do
    threads=$((2 ** i))
    dim=256
    row="$threads,"
    for j in {1..4}; do
        dim=$((dim * 2))
        sbatch --wait ./ejtp2PT.sh $threads $dim 64
        exTime=$(cat ./outputs/outputsPT.txt)
        row="$row,$exTime"
    done
    echo $row >> ./outputs/time_pt.csv
    echo
done

headers="Threads, 512 - 64, 1024 - 64, 2048 - 64, 4096 - 64"
echo $headers > ./outputs/time_omp.csv
echo
for i in {1..3}; do
    threads=$((2 ** i))
    dim=256
    row="$threads,"
    for j in {1..4}; do
        dim=$((dim * 2))
        sbatch --wait ./ejtp2OMP.sh $threads $dim 64
        exTime=$(cat ./outputs/outputsOMP.txt)
        row="$row,$exTime"
    done
    # Now we have to concat the row in ./outputs/time_pt.csv and begin a new line
    echo $row >> ./outputs/time_omp.csv
    echo
done