#!/bin/bash

#SBATCH --exclusive
#SBATCH --error=myJob.err            # standard error file
#SBATCH --output=myJob.out           # standard output file

#
# ./sb_gather_fs.sh file_size n_files conf_file
#
# example: sbatch --account=IscrC_HIWM --nodes=6 --time=00:05:00 --partition=g100_usr_prod sb_gather_fs.sh 1000000000 100
#
# file_size = files size in bytes
# n_files = number of files produced by the writer
# conf_file = configuration file for CAPIO
# 
# This script runs the gather test using the filesystem. 
# The streaming semantic must be specified in the configuration file that
# it is passed at this script.
#
# One node is used for the reader why the other for the writers.
# Each writer is executed in a different node. For example, if sbatch
# is launched with --nodes=6, one reader will run in the first node
# while the 5 writers will run in the other nodes, one per node.
#

module load openmpi

echo Number of nodes $SLURM_NNODES
read -d ' ' -a list <<< "$(scontrol show hostnames $SLURM_NODELIST)"

file_size=$1
n_files=$2

echo writer terminated
nwriters=$((SLURM_NNODES - 1))
partition_length=$((n_files/nwriters))
partition=0
i=0
while [ $i -lt $nwriters ]
do
	echo iteration $i on $nwriters
	echo $partition_length $partition
	ssh ${list[$i]} "cd /g100_scratch/userexternal/icolonne/capio_tests/motexample && ./writer $file_size $partition_length $partition > writer_time$i.txt 2>> writer$i.log" &
	partition=$((partition + partition_length))
	((++i))
done
wait
ssh ${list[$i]} "cd /g100_scratch/userexternal/icolonne/capio_tests/motexample && ./reader $n_files 0 $file_size >> reader_time.txt 2>> reader.log"
rm file*.dat
