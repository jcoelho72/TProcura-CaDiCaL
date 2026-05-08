


prefixSet=( "1kAD" "1kLA" "1kRC" "1kRU" "1kSP" "1kTF" "1kVAR"
            "CV" "DC1" "DC2" "J120" "J30" "J60" "J90" "RG30"
            "sD" "RG300" "VNR" "LPP" "LPSP" )
lastInstance=( 480000 720000 540000 270000 540000 720000 540000
               623 1800 720 600 480 480 480 1800
               390 480 1750 80 81 )
versions=( "original" "lowRU" "highRD" "keepOPT" )

# {0..3}
for idversion in {0..3}
do

# {0..19}   0 1 2 3 4 5 6 11 14 17 7 8 9 10 12 13 15 16 18 19
for idx in 8
do

echo ""
echo "###################################################################################################"
echo "Executing the set ${prefixSet[$idx]} with ${lastInstance[$idx]} instances, Version ${versions[$idversion]} ."
date

if [ $idx -lt 7 ];  then stepInc=1000; else stepInc=1; fi

# extract instances

if [$idversion -gt 0]; 
then
mkdir -p ${versions[$idversion]}
tar xzf Instancias/rcp.tar.gz --wildcards "original/${prefixSet[$idx]}_*.rcp"
cp original/*.rcp ${versions[$idversion]}
rm original/*.rcp
rmdir original
fi

tar xzf Instancias/rcp.tar.gz --wildcards "${versions[$idversion]}/${prefixSet[$idx]}_*.rcp"

bin/Release/RCPSP 1:${lastInstance[$idx]} -R Resultados/${prefixSet[$idx]}_${versions[$idversion]}_LB \
  -F ${versions[$idversion]}/${prefixSet[$idx]}_ \
  -I 16 -P P1=0 P2=3 P268=1:12 x P272=0:2 x P277=0:3 \
        -P P1=0 P2=3 P268=7 P269=1:3 x P272=0:2 x P277=0:3 \
        -P P1=0 P2=3 P268=8 P269=1:2 x P272=0:2 x P277=0:3

#  -I 16 -P P1=0 P2=4 P272=0 P268=1

rm ${versions[$idversion]}/*.rcp
rmdir ${versions[$idversion]}

done
done

