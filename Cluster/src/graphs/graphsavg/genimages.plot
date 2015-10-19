set terminal png

set output 'readyavg.png'
set title 'Number of Tasks in Ready Queue (exponential smoothing)'
set xlabel "Time(H)"
set yrange [0:50000]
set xrange [5:40]
plot "d2" u (($1-600)/3600):2 with lines title "#Tasks in Ready Queue (Real)" , \
     "d1" u ($1/3600):($2) with lines title "#Tasks in Ready Queue (Simulation)" 

set output 'evictedavg.png'
set title 'Number of Tasks Evicted (15 min window, exponential smoothing)'
set xlabel "Time(H)"
set yrange [0:25000]
set xrange [5:40]
plot "d2" u (($1-600)/3600):3 with lines title "#Tasks Evicted (Real)", \
     "d1" u ($1/3600):($3) with lines title "#Tasks Evicted (Simulation)"


set output 'completedavg.png'
set title 'Number of Tasks Completed (15 min window, exponential smoothing)'
set xlabel "Time(H)"
set yrange [0:16000]
set xrange [5:40]
plot "d2" u (($1-600)/3600):4 with lines title "#Tasks Successfully completed (Real)", \
     "d1" u ($1/3600):4 with lines title "#Tasks Successfully completed (Simulation)"

set output 'runningavg.png'
set title 'Number of Tasks Running (exponential smoothing)'
set xlabel "Time(H)"
set yrange [0:300000]
set xrange [5:40]
plot "d2" u (($1-600)/3600):5 with lines title "#Tasks Running (Real)", \
     "d1" u ($1/3600):5 with lines title "#Tasks Running (Simulation)"



set output 'ready.png'
set title 'Number of Tasks in Ready Queue'
set xlabel "Time(H)"
set yrange [0:50000]
set xrange [5:40]
plot "reallog.txt" u (($1-600)/3600):2 with lines title "#Tasks in Ready Queue (Real)" , \
     "avglog.txt" u ($1/3600):($2) with lines title "#Tasks in Ready Queue (Simulation)"

set output 'evicted.png'
set title 'Number of Tasks Evicted (15 min window)'
set xlabel "Time(H)"
set yrange [0:25000]
set xrange [5:40]
plot "reallog.txt" u (($1-600)/3600):3 with lines title "#Tasks Evicted (Real)", \
     "avglog.txt" u ($1/3600):($3) with lines title "#Tasks Evicted (Simulation)"


set output 'completed.png'
set title 'Number of Tasks Completed (15 min window)'
set xlabel "Time(H)"
set yrange [0:16000]
set xrange [5:40]
plot "reallog.txt" u (($1-600)/3600):4 with lines title "#Tasks Successfully completed (Real)", \
     "avglog.txt" u ($1/3600):4 with lines title "#Tasks Successfully completed (Simulation)"


set output 'running.png'
set title 'Number of Tasks Running'
set xlabel "Time(H)"
set yrange [0:300000]
set xrange [5:40]
plot "reallog.txt" u (($1-600)/3600):5 with lines title "#Tasks Running (Real)", \
     "avglog.txt" u ($1/3600):5 with lines title "#Tasks Running (Simulation)"


reset

set output 'histready.png'
set title 'Average Tasks In Ready Queue'
set style fill solid border -1
set style histogram errorbars gap 2 lw 1
set style data histogram
set yrange [14000:18000]
plot 'db1' using 1:2:3  , 'db1' using 4:5:6

set output 'histevicted.png'
set title 'Average Tasks Evicted'
set style fill solid border -1
set style histogram errorbars gap 2 lw 1
set style data histogram
set yrange [2000:4000]
plot 'db2' using 1:2:3  , 'db2' using 4:5:6


set output 'histcompleted.png'
set title 'Average Tasks Completed'
set style fill solid border -1
set style histogram errorbars gap 2 lw 1
set style data histogram
set yrange [3600:3700]
plot 'db3' using 1:2:3  , 'db3' using 4:5:6


set output 'histrunning.png'
set title 'Average Tasks Running'
set style fill solid border -1
set style histogram errorbars gap 2 lw 1
set style data histogram
set yrange [134000:138000]
plot 'db4' using 1:2:3  , 'db4' using 4:5:6



reset

set output 'waits.png'
set title 'Average Wait Time'
set yrange [0:530]
set style fill solid border -1
set style histogram errorbars gap 2 lw 1
set style data histogram
set ylabel 'Wait Time (s)'
unset xtics
plot 'wncwc.txt' using 1:2:3 title "Tasks without constraints", '' using 4:5:6 title "Tasks with constraints"

reset

set output 'waitrealsim.png'
set title 'Average Wait Time'
set style fill solid border -1
set style histogram errorbars gap 2 lw 1
set style data histogram
plot 'waitrealsim.txt' using 1:2:3, '' using 4:5:6



