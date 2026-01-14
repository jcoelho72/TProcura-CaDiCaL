 #!/bin/bash
 
 # Run the TProcura-CaDiCaL executable
 

./bin/Release/TProcura-CaDiCaL 4:13 -R Resultados/resSAT -G 1 -F cadical/test/cnf/sat -P P2=4 P6=20:30 x P7=1:2
./bin/Release/TProcura-CaDiCaL 0:7 -R Resultados/resUNIT -G 1 -F cadical/test/cnf/unit -P P2=4
