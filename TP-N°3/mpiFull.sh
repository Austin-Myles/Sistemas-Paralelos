#!/bin/bash

headers="Nucleos, 512 - 16, 1024 - 16, 2048 - 16, 4096 - 16"
echo $headers > ./outputs/time_mpi.csv
echo $headers > ./outputs/comm_mpi.csv
echo
for i in {0..2}; do
    nucleos=$((2 ** $i))
    dim=256
    rowt="$nucleos"
    rowc="$nucleos"
    for j in {1..4}; do
        dim=$((dim * 2))
        sbatch --wait "./mpi${nucleos}.sh" $dim 16
        str=$(cat "./outputs/outputsmpi${nucleos}.txt")
        time=$(echo $str | cut -d ',' -f 1)
        comm=$(echo $str | cut -d ',' -f 2)
        rowt="$rowt,$time"
        rowc="$rowc,$comm"
    done
    echo $rowt >> ./outputs/time_mpi.csv
    echo $rowc >> ./outputs/comm_mpi.csv
    echo
done

headers="Nucleos, 512 - 16, 1024 - 16, 2048 - 16, 4096 - 16"
echo $headers > ./outputs/time_hibrido.csv
echo $headers > ./outputs/comm_hibrido.csv
echo
for i in {0..1}; do
    nucleos=$((2 ** $i))
    dim=256
    rowt="$nucleos"
    rowc="$nucleos"
    for j in {1..4}; do
        dim=$((dim * 2))
        sbatch --wait "./hibrido${nucleos}.sh" $dim 16
        str=$(cat "./outputs/outputshibrido${nucleos}.txt")
        time=$(echo $str | cut -d ',' -f 1)
        comm=$(echo $str | cut -d ',' -f 2)
        rowt="$rowt,$time"
        rowc="$rowc,$comm"
    done
    echo $rowt >> ./outputs/time_hibrido.csv
    echo $rowc >> ./outputs/comm_hibrido.csv
    echo
done

headers="Dim - bs, 512 - 16, 1024 - 16, 2048 - 16, 4096 - 16"
echo $headers > ./outputs/time_sec.csv
echo
row="Time"
dim=256
for i in {1..4}; do
    dim=$((dim * 2))
    sbatch --wait ./ejtp2SEC.sh $dim 16
    exTime=$(cat ./outputs/outputsSec.txt)
    row="$row,$exTime"
done
echo $row >> ./outputs/time_sec.csv
