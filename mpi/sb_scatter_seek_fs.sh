#!/bin/bash

#SBATCH --exclusive
#SBATCH --error=myJob.err            # standard error file
#SBATCH --output=myJob.out           # standard output file


#
# sb_scatter_fs file_size n_files [round_robin]
#
# example: sbatch --nodes=6 --time=00:05:00 sb_scatter_fs.sh 1000000 100
# example2: sbatch --account=IscrC_HIWM --nodes=6 --time=00:05:00 --partition=g100_usr_prod sb_scatter_fs.sh 1000000000 100 rr
#
# file_size = files size in bytes
# n_files = number of files produced by the writer
# round_robin = use the round robin mode for the reader
# 
# This script runs the scatter test using the filesystem. 
#
# One node is used for the writer why the other for the readers.
# Each reader is executed in a different node. For example, if sbatch
# is launched with --nodes=6, one writer will run in the first node
# while the 5 readers will run in the other nodes, one per node.

module load openmpi

echo Number of nodes $SLURM_NNODES
read -d ' ' -a list <<< "$(scontrol show hostnames $SLURM_NODELIST)"

file_size=$1


ssh ${list[0]} "cd /g100_scratch/userexternal/icolonne/capio_tests/motexample && ./writer $file_size 1 0 > writer_time.txt 2>> writer.log"
echo writer terminated
nreaders=$((SLURM_NNODES - 1))
partition_length=$((file_size/nreaders))
partition=0
i=1
while [ $i -le $nreaders ]
do
	echo iteration $i on $nreaders
	echo $partition_length $partition
	ssh  ${list[$i]} "cd /g100_scratch/userexternal/icolonne/capio_tests/motexample && ./reader_seek $partition $partition_length > reader_time$i.txt 2>> reader$i.log" &
	partition=$((partition + partition_length))
	((++i))
done
echo last reader terminated
wait
