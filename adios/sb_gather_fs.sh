#!/bin/bash

#SBATCH --error=myJob.err            # standard error file
#SBATCH --output=myJob.out           # standard output file
#SBATCH --account=IscrC_HOPE 
#SBATCH --partition=g100_usr_prod 
#SBATCH --time=00:20:00

#
# ./sb_gather_fs.sh file_size n_files conf_file
#
# example: sbatch --nodes=6 sb_gather_fs.sh 1000000000 100
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

module load spack
spack load adios2@2.9.1

mkdir -p results/sb_gather_$SLURM_NNODES
OUTDIR="results/sb_gather_$SLURM_NNODES"


if [[ $# -ne 2 ]]; then
	echo "input error: file size (in bytes) and number of files required."
	exit 1
fi

echo Number of nodes $SLURM_NNODES

# getting the list of nodes and storing it in the variable "list"
read -d ' ' -a list <<< "$(scontrol show hostnames $SLURM_NODELIST)"

file_size=$1
n_files=$2

nwriters=$((SLURM_NNODES - 1))
partition_length=$((n_files/nwriters))
partition=0
i=0

# running the writers
while [ $i -lt $nwriters ]
do
	echo assigning to writer $i \(on node ${list[$i]} \) files from $partition to $((partition+partition_length)) \(partition len: $partition_length \)
	srun -n1 -w ${list[$i]} ./writer $file_size $partition_length $partition > $OUTDIR/writer_time${i}_gather.txt 2>> $OUTDIR/writer${i}_gather.log &
	partition=$((partition + partition_length))
	((++i))
done

# waiting for the writers termination
wait

# running the reader
echo "running  ./reader $n_files 0 $file_size"
srun -n 1 -w ${list[$i]} ./reader $n_files 0 $file_size >> $OUTDIR/reader_time_gather.txt 2>> $OUTDIR/reader_gather.log



