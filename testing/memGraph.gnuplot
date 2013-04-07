#!/usr/bin/gnuplot

# This script plots the PS output data files

set term png small size 800,600
set output "output/memGraphVSZ.png"
set ylabel "VSZ"
set ytics nomirror
set yrange [*:*]
#set grid


plot "output/showcase1.log" using 3 with linespoints pointtype 5 axes x1y1  title "1 VSZ", \
     "output/showcase2.log" using 3 with linespoints pointtype 8 axes x1y1 title "2 VSZ"

set y2range [*:*]
set ylabel "RSS"
set output "output/memGraphRSS.png"
plot "output/showcase1.log" using 2 with linespoints pointtype 5 axes x1y2 title "1 RSS", \
     "output/showcase2.log" using 2 with linespoints pointtype 8 axes x1y2 title "2 RSS"
