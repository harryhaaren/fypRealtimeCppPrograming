#!/usr/bin/gnuplot

# This script plots two massif output files against eachother

set term png small size 800,600
set term png small size 800,600
set output 'output/massifCompare.png';
set xlabel "time CPU instructions";
set ylabel "mb dynamic memory";

plot "input1.txt" using 1:4 title '1 total' with linespoints pointtype 5, \
     "input2.txt" using 1:4 title '2 total' with linespoints pointtype 8;
