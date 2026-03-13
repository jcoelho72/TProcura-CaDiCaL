#!/bin/bash

# Run the TProcura-CaDiCaL executable
# In this script, for each set of instances, is saved a different CSV file in Resultados folder 
# Is used some number of repetitions (P3 is the seed value) and 
# global parameter (P9 specifying default, plain, sat, unsat setting), to swap and 
# observe differences in CPU times
# Results in the folder Resultados, that can be processed directly by Excel file with
# power query, in this case resultados.xlsx

# compile the program
make

# to run in MPI compile:
# make mpi
# run the program with the specified number of processes:
# srun ./bin/MPI/TProcura-CaDiCaL <appropriate arguments>
# Note: should be specified more runs to justify mpi, like increasing number of seed values
#       with for example P3=1:100 or make a critical parameter range

# to see full help in arguments use: ./bin/Release/TProcura-CaDiCaL -h

printf "\n\nRunning TProcura-CaDiCaL on the 'add' instances with varying global parameters." 
./bin/Release/TProcura-CaDiCaL 4,8,16,32,64,128 -R Resultados/add -G 1 -F cadical/test/cnf/add -P P2=3 P3=1 P9=1 P3=1:4 x P9=1:4

printf "\n\nRunning TProcura-CaDiCaL on the 'full' instances with varying global parameters." 
./bin/Release/TProcura-CaDiCaL 1:7 -R Resultados/full -G 1 -F cadical/test/cnf/full -P P2=3 P3=1 P9=1 P3=1:4 x P9=1:4

printf "\n\nRunning TProcura-CaDiCaL on the 'ph' instances with varying global parameters." 
./bin/Release/TProcura-CaDiCaL 2:6 -R Resultados/ph -G 1 -F cadical/test/cnf/ph -P P2=3 P3=1 P9=1 P3=1:4 x P9=1:4

printf "\n\nRunning TProcura-CaDiCaL on the 'prime' instances with varying global parameters." 
./bin/Release/TProcura-CaDiCaL 121,1369,1681,169,1849,2209,25,289,361,4,49,529,65537,841,9,961 \
     -R Resultados/prime -G 1 -F cadical/test/cnf/prime -P P2=3 P3=1 P9=1 P3=1:4 x P9=1:4

printf "\n\nRunning TProcura-CaDiCaL on the 'sat' instances with varying global parameters." 
./bin/Release/TProcura-CaDiCaL 0:13 -R Resultados/sat -G 1 -F cadical/test/cnf/sat -P P2=3 P3=1 P9=1 P3=1:4 x P9=1:4

printf "\n\nRunning TProcura-CaDiCaL on the 'sqrt' instances with varying global parameters." 
./bin/Release/TProcura-CaDiCaL 10201,1042441,10609,11449,11881,12769,16129,259081,2809,3481,3721,4489,5041,5329,6241,63001,6889,7921 \
     -R Resultados/sqrt -G 1 -F cadical/test/cnf/sqrt -P P2=3 P3=1 P9=1 P3=1:4 x P9=1:4

printf "\n\nRunning TProcura-CaDiCaL on the 'unit' instances with varying global parameters." 
./bin/Release/TProcura-CaDiCaL 0:7 -R Resultados/unit -G 1 -F cadical/test/cnf/unit -P P2=3 P3=1 P9=1 P3=1:4 x P9=1:4

