#!/bin/bash
echo "Usage: ./trudpcat.sh <number_of_clients>"
for i in `seq 1 $1`;
do
  #exec ./teoroom_cli.sh $1 $i &
  exec dist/Debug/GNU-Linux/trudpcat -S 127.0.0.1 8000 &
done
