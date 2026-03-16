#!/bin/bash
#SBATCH --job-name=runT2
#SBATCH --output=runT2.txt
#SBATCH --account=f202507959cpcaa0a
#SBATCH --partition=normal-arm
#SBATCH --time=4:00:00
#SBATCH --nodes=1
#SBATCH --ntasks=48
#SBATCH --cpus-per-task=1
#SBATCH --mem=24G


ml OpenMPI

# Teste 2 — fixar conjunto de instâncias de teste, e executar parametrizações globais do cadical , P9 (default, sat, unsat, plain).
# Objetivo: medir desempenho de cada configuração global para cada método de conversão
srun bin/MPI/NDamas 8:60 -F T2 -R Resultados/damasT2 -M 1 -P P2=1 P9=1 P3=1 \
      P4=60 P3=1:10 x P9=1:4 x P264=0:2 


 



