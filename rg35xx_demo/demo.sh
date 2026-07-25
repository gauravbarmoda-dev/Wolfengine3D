#!/bin/sh
  
cd $(dirname "$0")
  
export OMP_WAIT_POLICY=PASSIVE
export OMP_NUM_THREADS=4
  
./game
