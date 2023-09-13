#!/bin/bash

#SBATCH --error=myJob.err            # standard error file
#SBATCH --output=myJob.out           # standard output file
#SBATCH --account=IscrC_HOPE 
#SBATCH --partition=g100_usr_prod 
#SBATCH --time=00:20:00
#SBATCH --exclusive


# example for running writer and reader on different nodes creating 10 files of 2KB:
# sbatch --account=your_account --nodes=2 --time=00:02:00 --partition=g100_usr_prod sb_prod_cons_fs_new.sh 2048 10

# input checking

module load spack 
spack load adios2@2.9.1

mkdir -p results/sb_prod_cons_$SLURM_NNODES

OUTDIR="results/sb_prod_cons_$SLURM_NNODES"

if [ $# -ne 2 ]; then
	echo "input error: file size (in bytes) and number of files required"
	exit 1
fi

echo Number of nodes $SLURM_NNODES

# getting the list of nodes and storing it in the variable "list"
read -d ' ' -a list <<< "$(scontrol show hostnames $SLURM_NODELIST)"


file_size=$1
n_files=$2


# run one writer process one the first node
echo "executing writer in node ${list[0]}"
srun -n 1 -w ${list[0]} ./writer $file_size $n_files 0 >>$OUTDIR/writer_time_spsc.txt 2> $OUTDIR/writer_spsc.log
echo writer terminated

if [ $SLURM_NNODES -eq 1 ]; then
	reader_node=${list[0]}
	echo "reader in local node $reader_node"
else
	reader_node=${list[1]}
	echo "reader in remote node $reader_node"
fi

# run one reader process reader one node
srun -n 1 -w $reader_node  ./reader $n_files 0 $file_size >> $OUTDIR/reader_time_spsc.txt 2> $OUTDIR/reader_spsc.log

echo reader terminated

rm -rf *.sb

