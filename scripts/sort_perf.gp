set title 'Sorting Time vs. Queue Size' font ',16'
set xlabel 'Size of Input (×1e6)' font ',14'
set ylabel 'Time (seconds)' font ',14'
set output 'sort_time.png'
set terminal pngcairo size 800,600 font ',12'
set key font ',12'
plot [-0.1:1.1][-0.1:]'data' every ::1::100 using ($1/1e6):3 with lines title 'Default Sort', \
     'data' every ::102::201 using ($1/1e6):3 with lines title 'Linux list\_sort'

reset

set title 'Comparisons vs. Queue Size' font ',16'
set xlabel 'Size of Input (×1e6)' font ',14'
set ylabel 'Comparisons (×1e7)' font ',14'
set output 'sort_cmp.png'
set terminal pngcairo size 800,600 font ',12'
set key font ',12'
plot [-0.1:1.1][-0.1:2.2]'data' every ::1::100 using ($1/1e6):($2/1e7) with lines title 'Default Sort', \
     'data' every ::102::201 using ($1/1e6):($2/1e7) with lines title 'Linux list\_sort'
