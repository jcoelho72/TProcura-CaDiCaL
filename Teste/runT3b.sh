#!/bin/bash
#SBATCH --job-name=runT3b
#SBATCH --output=runT3b.txt
#SBATCH --account=f202507959cpcaa0a
#SBATCH --partition=normal-arm
#SBATCH --time=4:00:00
#SBATCH --nodes=1
#SBATCH --ntasks=48
#SBATCH --cpus-per-task=1
#SBATCH --mem=24G


ml OpenMPI

# Teste 3b — desempenho do método sequential counter
# Objetivo: medir desempenho do método sequential counter (eficácia e eficiência), para diferentes valores de N
srun bin/MPI/NDamas 8:50,51:100:3,100:150:5  -F T3b -R Resultados/damasT3b -M 1 -P P2=1 P9=4 P3=1 \
      P4=60 P264=1 P3=1:100 





 



