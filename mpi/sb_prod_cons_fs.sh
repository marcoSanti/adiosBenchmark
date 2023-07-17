#!/bin/bash

#SBATCH --exclusive
#SBATCH --error=myJob.err            # standard error file
#SBATCH --output=myJob.out           # standard output file

module load openmpi

echo Number of nodes $SLURM_NNODES
read -d ' ' -a list <<< "$(scontrol show hostnames $SLURM_NODELIST)"


file_size=$1
n_files=$2



ssh ${list[0]} "cd /g100_scratch/userexternal/icolonne/capio_tests/motexample && ./writer $file_size $n_files 0 >> writer_time.txt 2> writer.log"
echo writer terminated

if [ $SLURM_NNODES -eq 1 ]; then
	echo "reader in local node"
	ssh  ${list[0]} "cd /g100_scratch/userexternal/icolonne/capio_tests/motexample && ./reader $n_files 0 $file_size >> reader_time$i.txt 2> reader$i.log" 
else
	echo "reader in remote node"
	ssh  ${list[1]} "cd /g100_scratch/userexternal/icolonne/capio_tests/motexample && ./reader $n_files 0 $file_size >> reader_time$i.txt 2> reader$i.log" 
fi
echo reader terminated

