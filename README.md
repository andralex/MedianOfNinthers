This repository contains the implementation and benchmarks for the paper ["Fast Deterministic Selection"](https://arxiv.org/abs/1606.00484).

# Prerequisites

You need the GNU C++ compiler (or the clang drop-in replacement). To prepare the datasets, you need the D compiler dmd, downloadable from http://dlang.org/download.html.

# Running Benchmarks

To initiate bulding and running benchmarks, simply run `make` from the repository directory. The first run will take a long time because it downloads and preprocesses the Google Ngrams corpus, The default directory of all corpora is `$(HOME)/data/median`.

Binaries and intermediate files are produced by default in `/tmp/MedianOfNinthers`. The final (summarized) results are output in `./results`. You may change these locations by editing `Makefile`.
