#!/bin/bash

# This script will run the implementations, and use gprof and freinds to generate
# a call graph image from the resulting gmon.out file.

# Set all CPU cores to 100% frequency.
echo "Setting up..."
cpupower frequency-set -g performance > /dev/null 2&>1

# Start RTIRQ to rtprio the indigoDJ 
/etc/rc.d/rtirq start > /dev/null 2&>1

# Set VM swapiness
/sbin/sysctl -q -w vm.swappiness=10

# disable screen blanking totally
xset -dpms; xset s off

# turn off WIFI
iwconfig wlan0 txpower off

# Clear existing data
rm -f output/showcase1.log output/showcase2.log

echo "Launching showcase1"
xterm -e "cd bin/selfProfiling;./showcase1" &

# Otherwise xterm won't have launched showcase yet
sleep 1

echo "Profiling now..."
while true; do
  PID=`ps -C showcase1 -o pid=`
  #echo $PID

  # Check for program exit, then interpret data to generate call graph image
  if [ "$PID" == "" ]; then
    sleep 1
    echo "Showcase1 done: generating call graph now..."
    mv bin/selfProfiling/gmon.out bin/selfProfiling/showcase1_gmon.out
    gprof bin/selfProfiling/showcase1 bin/selfProfiling/showcase1_gmon.out | gprof2dot.py -s | dot -Tsvg -o output/showcase1callgraph.svg
    #convert -background transparent output/showcase1callgraph.svg output/showcase1callgraph.png
    #rm output/showcase1callgraph.svg
    echo "Done!"
    echo ""
    echo ""
    break
  fi
  
  # collect stats on mem usage
  ps -C showcase1 -o pid=,rss=,vsz= >> output/showcase1.log
  
  sleep 1
done


echo "Launching showcase2"
xterm -e "cd bin/selfProfiling/;./showcase2" &

# Otherwise xterm won't have launched showcase yet
sleep 1

echo "Profiling now..."
while true; do
  PID=`ps -C showcase2 -o pid=`
  #echo $PID

  # Check for program exit, then interpret data to generate call graph image
  if [ "$PID" == "" ]; then
    sleep 1
    echo "showcase2 done: generating call graph now..."
    mv bin/selfProfiling/gmon.out bin/selfProfiling/showcase2_gmon.out
    gprof bin/selfProfiling/showcase2 bin/selfProfiling/showcase2_gmon.out | gprof2dot.py -s | dot -Tsvg -o output/showcase2callgraph.svg
    #convert -background transparent output/showcase2callgraph.svg output/showcase2callgraph.png
    #rm output/showcase2callgraph.svg
    echo "Done!"
    echo ""
    echo ""
    break
  fi
  
  # collect stats on mem usage
  ps -C showcase2 -o pid=,rss=,vsz= >> output/showcase2.log
  
  sleep 1
done


echo "Generating memory usage graphs..."
gnuplot memGraph.gnuplot;
echo "Done!";
exit 0
