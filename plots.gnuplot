set term svg mouse standalone size 800, 600
set boxwidth 0.9 relative
set style data histogram
set style histogram clustered
set style fill solid 1.0 border lt -1
set xlabel "Input size"
set ylabel "Speedup"
set yrange [0:]
set grid noxtics ytics
set key outside
set ytics 0.1

xlabel(n) = gprintf("%.2t · 10^{%T}", n)

do for [data in "m3killer organpipe random random01 rotated sorted"] {
    set title "Speedup relative to GNUIntroselect (" . data . " dataset)"
    input = "results/" . data
    set output "plots/" . data . ".svg"
    plot input using (column("nth_element")/column("nth_element")):xticlabels(xlabel($1)) title "GNUIntroselect", \
        input using (column("nth_element")/column("rnd3pivot")):xticlabels(xlabel($1)) title "RND3Pivot", \
        input using (column("nth_element")/column("ninther")):xticlabels(xlabel($1)) title "Ninther", \
        input using (column("nth_element")/column("median_of_ninthers")):xticlabels(xlabel($1)) title "QuickselectAdaptive"
}

set title "Speedup relative to GNUIntroselect (googlebooks dataset)"
input = "results/gbooks_freq"
set output "plots/gbooks_freq.svg"
plot input using (column("nth_element")/column("nth_element")):xticlabels(1) title "GNUIntroselect", \
    input using (column("nth_element")/column("rnd3pivot")):xticlabels(1) title "RND3Pivot", \
    input using (column("nth_element")/column("ninther")):xticlabels(1) title "Ninther", \
    input using (column("nth_element")/column("median_of_ninthers")):xticlabels(1) title "QuickselectAdaptive"
