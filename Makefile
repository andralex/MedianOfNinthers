# Definitions pertanining to the project

# Temporary directory for e.g. object files and generated binaries
T = /tmp/MedianOfNinthers
$(shell mkdir -p $T)
# Directory for data that is generated but should not be discarded naively
D = $(HOME)/data/median
$(shell mkdir -p $D)
# Directory where results are kept
ifeq ($(DRAFT),1)
R = results_draft
else
R = results
endif
$(shell mkdir -p $R)

# Data sizes present in the paper
ifeq ($(DRAFT),1)
SIZES = 10000 31620 100000 316220 1000000 3162280
CFLAGS = -O4 -pg
else
SIZES = 10000 31620 100000 316220 1000000 3162280 10000000 31622800 100000000
CFLAGS = -O4 -DNDEBUG
endif

# Utils
XPROD = $(foreach a,$1,$(foreach b,$3,$a$2$b))
XPROD3 = $(call XPROD,$1,$2,$(call XPROD,$3,$4,$5))

# Sources (without algos)
CXX_CODE = $(addprefix src/,main.cpp common.h timer.h)

# Algorithms
ALGOS = nth_element rnd3pivot ninther median_of_ninthers bfprt_baseline

# Data sets (synthetic)
SYNTHETIC_DATASETS = m3killer organpipe random random01 rotated sorted
# Benchmark files we're interested in
MK_OUTFILES = MEASUREMENTS_$1 = $(foreach n,$(SIZES),$(foreach a,$(ALGOS),$D/$1_$n_$a.out))
$(foreach d,$(SYNTHETIC_DATASETS),$(eval $(call MK_OUTFILES,$d)))

# Data sets (Google Books)
L = a b c d e f g h i j k l m n o p q r s t u v w x y z
GBOOKS_LANGS = eng fre ger ita rus spa
GBOOKS_CORPORA = $(foreach l,$(GBOOKS_LANGS),googlebooks-$l-all-1gram-20120701)

# All data sets ever
DATASETS = $(SYNTHETIC_DATASETS) $(GBOOKS_CORPORA)

# All measurement output files
MEASUREMENT_OUTPUTS = $(foreach x,$(SYNTHETIC_DATASETS),$(MEASUREMENTS_$x)) \
  $(foreach x,$(call XPROD,$(ALGOS),_,$(GBOOKS_CORPORA)),$D/$x_freq.out)

# Results files will be included in the paper
RESULTS = $(addprefix $R/,$(SYNTHETIC_DATASETS) gbooks_freq)

###############################################################################

all: $(RESULTS)

clean:
	rm -rf $T/

clean-measurements:
	rm -rf $T/ $D/*.out $D/*.check $R $D/*.tmp

pristine:
	rm -rf $T/ $D/

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

.PHONY: measurements $(DATASETS)
measurements: $(DATASETS)

gbooks: $(foreach x,$(call XPROD,$(GBOOKS_CORPORA),_freq_,$(ALGOS)),$D/$x.out)

$(foreach d,$(SYNTHETIC_DATASETS),$(eval \
$d: $(MEASUREMENTS_$d);\
))

define MAKE_MEASUREMENT
$D/%_$1.out: $T/$1 $D/%.dat
	$$^ >$$@.tmp 2>$$@.check
	mv $$@.tmp $$@
$T/$1: src/$1.cpp $(CXX_CODE)
	$(CXX) $(CFLAGS) -std=c++14 -o $$@ $$(patsubst %.h,,$$^)
endef

$(foreach a,$(ALGOS),$(eval $(call MAKE_MEASUREMENT,$a)))

################################################################################
# Assemble measurement results
################################################################################

$R/gbooks_freq: gbooks
	echo "Corpus" $(foreach a,$(ALGOS), "  $a") >$@.tmp
	$(foreach l,$(GBOOKS_LANGS),echo -n "$l " >>$@.tmp && paste $(foreach a,$(ALGOS),$D/googlebooks-$l-all-1gram-20120701_freq_$a.out) >>$@.tmp &&) true
	mv $@.tmp $@

define MAKE_RESULT_FILE
$R/$1: $$(MEASUREMENTS_$1)
	echo $$^
	echo "Size" $$(foreach a,$$(ALGOS), "  $$a") >$$@.tmp
	$$(foreach n,$$(SIZES),echo -n "$$n\t" >>$$@.tmp && paste $$(foreach a,$$(ALGOS),$$D/$1_$$n_$$a.out) >>$$@.tmp &&) true
	mv $$@.tmp $$@
endef

$(foreach a,$(SYNTHETIC_DATASETS),$(eval $(call MAKE_RESULT_FILE,$a)))
