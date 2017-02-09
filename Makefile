# Definitions pertanining to the project

# $D is a directory for data that is generated but should not be discarded
# naively. $T is a temporary directory for object files, generated binaries, and
# intermediate results. $R is the directory where the final summary results are
# kept.
D = $(HOME)/data/median
R = results
T = /tmp/MedianOfNinthers
$(shell mkdir -p $D $R $T)

# Data sizes present in the paper
SIZES = 10000 31620 100000 316220 1000000 3162280 10000000

# Compiler flags for instrumented and fast build
CFLAGS_INSTRUMENTED = -std=c++14 -O4 -DCOUNT_SWAPS -DCOUNT_WASTED_SWAPS -DCOUNT_COMPARISONS
CFLAGS = -std=c++14 -O4 -DNDEBUG -DMEASURE_TIME

# Utils
XPROD = $(foreach a,$1,$(foreach b,$3,$a$2$b))
XPROD3 = $(call XPROD,$1,$2,$(call XPROD,$3,$4,$5))

# Sources (without algos)
CXX_CODE = $(addprefix src/,main.cpp common.h timer.h)

# Algorithms
ALGOS = nth_element median_of_ninthers rnd3pivot ninther bfprt_baseline

# Data sets (synthetic)
SYNTHETIC_DATASETS = m3killer organpipe random random01 rotated sorted
# Benchmark files we're interested in
MK_OUTFILES = MEASUREMENTS_$1 = $(foreach n,$(SIZES),$(foreach a,$(ALGOS),$T/$1_$n_$a.time))
$(foreach d,$(SYNTHETIC_DATASETS),$(eval $(call MK_OUTFILES,$d)))

# Data sets (Google Books)
L = a b c d e f g h i j k l m n o p q r s t u v w x y z
GBOOKS_LANGS = eng fre ger ita rus spa
GBOOKS_CORPORA = $(foreach l,$(GBOOKS_LANGS),googlebooks-$l-all-1gram-20120701)

# All measurement output files
MEASUREMENT_OUTPUTS = $(foreach x,$(SYNTHETIC_DATASETS),$(MEASUREMENTS_$x)) \
  $(foreach x,$(call XPROD,$(ALGOS),_,$(GBOOKS_CORPORA)),$T/$x_freq.time)

# Results files will be included in the paper. Change this to affect what
# experiments are run.
RESULTS = $(addprefix $R/,$(SYNTHETIC_DATASETS) gbooks_freq)

###############################################################################

all: $(RESULTS)

clean:
	rm -rf $D/*.tmp $R*/* $T*/

pristine:
	rm -rf $D/ $R/* $T/

# Don't delete intermediary files
.SECONDARY:

################################################################################
# Data
################################################################################

.PHONY: data
data: $(foreach x,$(call XPROD,$(SYNTHETIC_DATASETS),_,$(SIZES)),$D/$x.dat) $(foreach c,$(GBOOKS_CORPORA),$D/$c_freq.dat)

$D/googlebooks-%_freq.dat: $D/googlebooks-%.txt
	cut -f 2 <$^ | rdmd -O -inline support/binarize.d >$@.tmp
	mv $@.tmp $@
$D/googlebooks-%.txt: $(foreach l,$L,$D/googlebooks-%-$l.gz)
	gunzip --stdout $^ | rdmd -O -inline support/aggregate_ngrams.d >$@.tmp
	mv $@.tmp $@
$D/googlebooks-%.gz:
	curl --fail http://storage.googleapis.com/books/ngrams/books/googlebooks-$*.gz >$@.tmp
	mv $@.tmp $@

define GENERATE_DATA
$D/$1_%.dat: support/generate.d
	rdmd -O -inline support/generate.d --kind=$1 --n=$$* >$$@.tmp
	mv $$@.tmp $$@
endef

$(foreach d,$(SYNTHETIC_DATASETS),$(eval $(call GENERATE_DATA,$d)))

################################################################################
# Measurements
################################################################################

.PHONY: measurements $(SYNTHETIC_DATASETS) $(GBOOKS_CORPORA)
measurements: $(SYNTHETIC_DATASETS) $(GBOOKS_CORPORA)

gbooks: $(foreach x,$(call XPROD,$(GBOOKS_CORPORA),_freq_,$(ALGOS)),$T/$x.time)

$(foreach d,$(SYNTHETIC_DATASETS),$(eval \
$d: $(MEASUREMENTS_$d);\
))

define MAKE_MEASUREMENT
$T/%_$1.time: $T/$1 $T/$1_instrumented $D/%.dat
	$T/$1 $D/$$*.dat >$T/$$*_$1.tmp
	$T/$1_instrumented $D/$$*.dat >>$T/$$*_$1.tmp
	mv $T/$$*_$1.tmp $T/$$*_$1.stats
	sed -n '/^milliseconds: /s/milliseconds: //p' $T/$$*_$1.stats >$T/$$*_$1.tmp
	mv $T/$$*_$1.tmp $$@
	sed -n '/^rsd: /s/rsd: //p' $T/$$*_$1.stats >$T/$$*_$1.tmp
	mv $T/$$*_$1.tmp $T/$$*_$1.rsd
	sed -n '/^comparisons: /s/comparisons: //p' $T/$$*_$1.stats >$T/$$*_$1.tmp
	mv $T/$$*_$1.tmp $T/$$*_$1.comps
	sed -n '/^swaps: /s/swaps: //p' $T/$$*_$1.stats >$T/$$*_$1.tmp
	mv $T/$$*_$1.tmp $T/$$*_$1.swaps
$T/$1: src/$1.cpp $(CXX_CODE)
	$(CXX) $(CFLAGS) -o $$@ $$(patsubst %.h,,$$^)
$T/$1_instrumented: src/$1.cpp $(CXX_CODE)
	$(CXX) $(CFLAGS_INSTRUMENTED) -o $$@ $$(patsubst %.h,,$$^)
endef

$(foreach a,$(ALGOS),$(eval $(call MAKE_MEASUREMENT,$a)))

################################################################################
# Assemble measurement results
################################################################################

$R/gbooks_freq: gbooks
	echo "Corpus" $(foreach a,$(ALGOS), "  $a") >$@.tmp
	$(foreach l,$(GBOOKS_LANGS),echo -n "$l " >>$@.tmp && paste $(foreach a,$(ALGOS),$T/googlebooks-$l-all-1gram-20120701_freq_$a.time) >>$@.tmp &&) true
	mv $@.tmp $@
	echo "Corpus" $(foreach a,$(ALGOS), "  $a") >$@.tmp
	$(foreach l,$(GBOOKS_LANGS),echo -n "$l " >>$@.tmp && paste $(foreach a,$(ALGOS),$T/googlebooks-$l-all-1gram-20120701_freq_$a.rsd) >>$@.tmp &&) true
	mv $@.tmp $@.rsd
	echo "Corpus" $(foreach a,$(ALGOS), "  $a") >$@.tmp
	$(foreach l,$(GBOOKS_LANGS),echo -n "$l " >>$@.tmp && paste $(foreach a,$(ALGOS),$T/googlebooks-$l-all-1gram-20120701_freq_$a.comps) >>$@.tmp &&) true
	mv $@.tmp $@.comps
	echo "Corpus" $(foreach a,$(ALGOS), "  $a") >$@.tmp
	$(foreach l,$(GBOOKS_LANGS),echo -n "$l " >>$@.tmp && paste $(foreach a,$(ALGOS),$T/googlebooks-$l-all-1gram-20120701_freq_$a.swaps) >>$@.tmp &&) true
	mv $@.tmp $@.swaps

define MAKE_RESULT_FILE
$R/$1: $$(MEASUREMENTS_$1)
	echo "Size" $$(foreach a,$$(ALGOS), "  $$a") >$$@.tmp
	$$(foreach n,$$(SIZES),echo -n "$$n\t" >>$$@.tmp && paste $$(foreach a,$$(ALGOS),$$T/$1_$$n_$$a.time) >>$$@.tmp &&) true
	mv $$@.tmp $$@
# Relative Standard Deviation
	echo "Size" $$(foreach a,$$(ALGOS), "  $$a") >$$@.tmp
	$$(foreach n,$$(SIZES),echo -n "$$n\t" >>$$@.tmp && paste $$(foreach a,$$(ALGOS),$$T/$1_$$n_$$a.rsd) >>$$@.tmp &&) true
	mv $$@.tmp $$@.rsd
# Comparisons
	echo "Size" $$(foreach a,$$(ALGOS), "  $$a") >$$@.tmp
	$$(foreach n,$$(SIZES),echo -n "$$n\t" >>$$@.tmp && paste $$(foreach a,$$(ALGOS),$$T/$1_$$n_$$a.comps) >>$$@.tmp &&) true
	mv $$@.tmp $$@.comps
# Swaps
	echo "Size" $$(foreach a,$$(ALGOS), "  $$a") >$$@.tmp
	$$(foreach n,$$(SIZES),echo -n "$$n\t" >>$$@.tmp && paste $$(foreach a,$$(ALGOS),$$T/$1_$$n_$$a.swaps) >>$$@.tmp &&) true
	mv $$@.tmp $$@.swaps
endef

$(foreach a,$(SYNTHETIC_DATASETS),$(eval $(call MAKE_RESULT_FILE,$a)))
