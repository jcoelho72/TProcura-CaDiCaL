#!/bin/bash
#SBATCH --job-name=runT1
#SBATCH --output=runT1.txt
#SBATCH --account=f202507959cpcaa0a
#SBATCH --partition=normal-arm
#SBATCH --time=4:00:00
#SBATCH --nodes=1
#SBATCH --ntasks=48
#SBATCH --cpus-per-task=1
#SBATCH --mem=24G


ml OpenMPI

# Teste 1 — executar testes com as configurações globais, diferentes tamanhos de instãncias permitidos
# Objetivo: analisar o tamanho máximo das instâncias que podem ser processadas
srun bin/MPI/NDamas 8:100:3,100:200:9 -F T1 -R Resultados/damasT1 -M 1 -P P2=1 P9=1 P3=1 \
      P4=10 P265=1000000 P266=10000000 P264=0:2 \
   -P P4=20 P265=2000000 P266=20000000 P264=0:2 \
   -P P4=50 P265=5000000 P266=50000000 P264=0:2 \
   -P P4=100 P265=10000000 P266=100000000 P264=0:2 

 



