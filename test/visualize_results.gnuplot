set title "FIPA Bitefficient Message - Relative header overhead" 

# Message size
plot "results.txt" using ($1/1024):3 with lines notitle
#, "results.txt" using 1:3 with lines notitle

set xrange [0:100]
set ylabel "% of header overhead"
set xlabel "Message content size in kB"
set output "fipa_message-header_overhead.png"
set terminal png
replot


set title "FIPA Bitefficient Message - Encoding time"

plot "results.txt" using ($2/1024):4 with lines notitle

set xrange [0:10000]
set ylabel "Encoding time in ms"
set xlabel "Total message size in kB"
set output "fipa_message-encoding.png"
set terminal png
replot


set title "FIPA Bitefficient Message - Decoding time"
plot "results.txt" using ($2/1024):5 with lines notitle
set xrange [0:10000]
set ylabel "Encoding time in ms"
set xlabel "Total message size in kB"
set output "fipa_message-decoding.png"
set terminal png
replot

