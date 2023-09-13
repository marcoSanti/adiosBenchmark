#!/bin/bash

#SBATCH --exclusive
#SBATCH --error=myJob.err            # standard error file
#SBATCH --output=myJob.out           # standard output file
#SBATCH --account=IscrC_HOPE 
#SBATCH --partition=g100_usr_prod 
#SBATCH --time=00:20:00


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

module load spack
spack load adios2@2.9.1

mkdir -p results/sb_scatter_$SLURM_NNODES
OUTDIR="results/sb_scatter_$SLURM_NNODES"


if [[ $# -lt 2 && $# -gt 3 ]]; then
	echo "input error: file size (in bytes) and number of files required. The rr argument is optional and enables the round robin version"
	exit 1
fi

if [ $# -eq 3 ]; then
	echo scatter round robin
fi

echo Number of nodes $SLURM_NNODES

# getting the list of nodes and storing it in the variable "list"
read -d ' ' -a list <<< "$(scontrol show hostnames $SLURM_NODELIST)"

file_size=$1
n_files=$2


srun -n 1 -w ${list[0]} ./writer $file_size $n_files 0 >$OUTDIR/writer_time_scatter.txt 2>> $OUTDIR/writer_scatter.log
echo writer terminated
nreaders=$((SLURM_NNODES - 1))
partition_length=$((n_files/nreaders))
partition=0
i=1
while [ $i -le $nreaders ]
do
	echo iteration $i on $nreaders
	echo $partition_length $partition
	# launch srun in background (&) in order to run the readers in parallel

	srun -n 1 -w ${list[$i]} ./reader $partition_length $partition $file_size >> $OUTDIR/reader_time${i}_scatter.txt 2>> $OUTDIR/reader${i}_scatter.log &

	partition=$((partition + partition_length))
	((++i))
done
echo last reader terminated

# this wait is necessary to wait for the termination of all readers
wait

rm -rf *.sb

