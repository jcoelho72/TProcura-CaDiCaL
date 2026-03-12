#!/bin/bash

# Run the TProcura-CaDiCaL executable
# for each set of instances, save a different CSV faile, and use appropriate number of repetitions to observe differences in CPU times

echo "Running TProcura-CaDiCaL on the 'add' instances with varying parameters..." 
./bin/Release/TProcura-CaDiCaL 4,8,16,32,64,128 -R Resultados/add -G 1 -F cadical/test/cnf/add -P P2=4 P3=1 P9=1 P3=1:10 x P9=1:4

echo "Running TProcura-CaDiCaL on the 'full' instances with varying parameters..." 
./bin/Release/TProcura-CaDiCaL 1:7 -R Resultados/full -G 1 -F cadical/test/cnf/full -P P2=4 P3=1 P9=1 P3=1:10 x P9=1:4

echo "Running TProcura-CaDiCaL on the 'ph' instances with varying parameters..." 
./bin/Release/TProcura-CaDiCaL 2:6 -R Resultados/ph -G 1 -F cadical/test/cnf/ph -P P2=4 P3=1 P9=1 P3=1:10 x P9=1:4

echo "Running TProcura-CaDiCaL on the 'prime' instances with varying parameters..." 
./bin/Release/TProcura-CaDiCaL 121,1369,1681,169,1849,2209,25,289,361,4,4294967297,49,529,65537,841,9,961 \
     -R Resultados/prime -G 1 -F cadical/test/cnf/prime -P P2=4 P3=1:10 x P9=1:4

echo "Running TProcura-CaDiCaL on the 'sat' instances with varying parameters..." 
./bin/Release/TProcura-CaDiCaL 0:13 -R Resultados/sat -G 1 -F cadical/test/cnf/sat -P P2=4 P3=1 P9=1 P3=1:10 x P9=1:4

echo "Running TProcura-CaDiCaL on the 'sqrt' instances with varying parameters..." 
./bin/Release/TProcura-CaDiCaL 10201,1042441,10609,11449,11881,12769,16129,259081,2809,3481,3721,4489,5041,5329,6241,63001,6889,7921 \
     -R Resultados/sqrt -G 1 -F cadical/test/cnf/sqrt -P P2=4 P3=1 P9=1 P3=1:10 x P9=1:4

echo "Running TProcura-CaDiCaL on the 'unit' instances with varying parameters..." 
./bin/Release/TProcura-CaDiCaL 0:7 -R Resultados/unit -G 1 -F cadical/test/cnf/unit -P P2=4 P3=1 P9=1 P3=1:10 x P9=1:4

