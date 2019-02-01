#!/bin/bash

# run script for levy flight simulation
# version 1.0
# 20160213

alpha=2.0
ratio=0.1
# change sim.par
#sed -i '' "4 s/.*alpha.*/alpha  $alpha/" sim.par
sed -i '' "18 s/.*focusRatio.*/focusRatio  $ratio/" sim.par

# run simulation 5 times

for i in `seq 1 15`;
do
    echo $i 
    time app_cloud2 > log_r${ratio}_$i 
done
