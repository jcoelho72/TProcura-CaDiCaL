#!/bin/bash
#SBATCH --job-name=runT3a
#SBATCH --output=runT3a.txt
#SBATCH --account=f202507959cpcaa0a
#SBATCH --partition=normal-arm
#SBATCH --time=4:00:00
#SBATCH --nodes=1
#SBATCH --ntasks=48
#SBATCH --cpus-per-task=1
#SBATCH --mem=24G


ml OpenMPI

# Teste 3a — desempenho do método quadrático
# Objetivo: medir desempenho do método quadrático (eficácia e eficiência), para diferentes valores de N
srun bin/MPI/NDamas 8:50,51:75:3  -F T3a -R Resultados/damasT3a -M 1 -P P2=1 P9=2 P3=1 \
      P4=60 P264=0 P3=1:100 



 



