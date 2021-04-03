##### input #####

# window size
# W_SIZE=...
W_SIZE ?= 512

# task size
# SIZE=...

#####

CXX=g++
# VECTOR_FLAGS=-O3 -funroll-loops -ftree-vectorize -ffast-math -march=armv8.1-a #-march=skylake-avx512 -mavx512f -mavx512cd  #-fopt-info-vec-all
VECTOR_FLAGS=-O3 -fno-tree-vectorize -funroll-loops -ffast-math #-mno-sse -mno-avx


ifdef DEBUG
	CXXFLAGS := $(VECTOR_FLAGS) -std=c++11 -w -Wall -Wextra -Wno-error=unused-parameter -Wno-error=unknown-pragmas -Wno-unknown-pragmas -Wno-misleading-indentation -I ../../external_libraries/VectorGraphLibrary/
else
	CXXFLAGS := $(VECTOR_FLAGS) -std=c++11 -w -Wall -Wextra -Wno-error=unused-parameter -Wno-error=unknown-pragmas -Wno-unknown-pragmas -Wno-misleading-indentation -I ../../external_libraries/VectorGraphLibrary/
endif

ifdef SIZE
	CPPFLAGS := -DRECTS_PREDEF=$(W_SIZE) -DSIZE_PREDEF=$(SIZE)
else
	CPPFLAGS := -DRECTS_PREDEF=$(W_SIZE)
	SIZE := STD
endif

INC_DIR = ../../locutils_new
OUT_DIR = bin
METR = $(METRIC_FLAG)

LOC_NP = $(BENCH)_np
LOC_PAPI = $(BENCH)_papi
LOC_FILES = $(BENCH)_files
LOC_RECTS = $(BENCH)_rects

PAPI ?= 1
REPEAT ?= 8

TGT_NP = $(OUT_DIR)/$(LOC_NP)_$(SIZE)
TGT_PAPI = $(OUT_DIR)/$(LOC_PAPI)_$(SIZE)_Set$(PAPI)
TGT_FILES = $(OUT_DIR)/$(LOC_FILES)_$(SIZE)
TGT_RECTS = $(OUT_DIR)/$(LOC_RECTS)_$(SIZE)_$(W_SIZE)

TGT_OMP_NP = $(OUT_DIR)/omp_$(LOC_NP)_$(SIZE)
TGT_OMP_PAPI = $(OUT_DIR)/omp_$(LOC_PAPI)_$(SIZE)_Set$(PAPI)
TGT_OMP_FILES = $(OUT_DIR)/omp_$(LOC_FILES)_$(SIZE)
TGT_OMP_RECTS = $(OUT_DIR)/omp_$(LOC_RECTS)_$(SIZE)_$(W_SIZE)

#TGT := $(TGT_OMP_NP) $(TGT_NP)
TGT := $(TGT_OMP_NP)
#TGT := $(TGT_NP)

CMD := $(CXX) $(CPPFLAGS) $(CXXFLAGS) $(ADD_FLAGS) -I $(INC_DIR)

all : $(TGT)

$(TGT_NP): $(LOC_NP).cpp size.h ../size.h $(BENCH).h $(INC_DIR)/*
	$(CMD) -DLOC_THREADS=1 -DLOC_REPEAT=$(REPEAT) -DRADIUS=$(ELEMS) -DLENGTH=$(LENGTH) -DMODE=$(MODE) $< -o $@ $(ADD_SRC)

$(TGT_PAPI): $(LOC_PAPI).cpp size.h ../size.h $(BENCH).h $(INC_DIR)/*
	$(CMD) -DLOC_THREADS=1 -DLOC_REPEAT=1 -DLOC_PAPI=$(PAPI) $< -lpapi $(ADD_SRC) -o $@

$(TGT_FILES): $(LOC_FILES).cpp size.h ../size.h $(BENCH)_p.h $(BENCH).h $(INC_DIR)/*
	$(CMD) -DLOC_THREADS=1 $< -o $@  $(ADD_SRC)

$(TGT_RECTS): $(LOC_RECTS).cpp size.h ../size.h $(BENCH)_p.h $(BENCH).h $(INC_DIR)/*
	$(CMD) -DLOC_THREADS=1 $< -o $@   $(ADD_SRC)

$(TGT_OMP_NP): $(LOC_NP).cpp size.h ../size.h $(BENCH).h $(INC_DIR)/*
	mkdir bin
	$(CMD) -DLOC_THREADS=$(THREADS) -DLOC_REPEAT=$(REPEAT) -D$(METR)=0 -DRADIUS=$(ELEMS) -DLENGTH=$(LENGTH) -DMODE=$(MODE) $< -fopenmp -pthread -o $@  $(ADD_SRC)

$(TGT_OMP_PAPI): $(LOC_PAPI).cpp size.h ../size.h $(BENCH).h $(INC_DIR)/*
	$(CMD) -DLOC_THREADS=$(THREADS) -DLOC_REPEAT=1 -DLOC_PAPI=$(PAPI) $< -fopenmp -pthread -lpapi $(ADD_SRC) -o $@

$(TGT_OMP_FILES): $(LOC_FILES).cpp size.h ../size.h $(BENCH)_p.h $(BENCH).h $(INC_DIR)/*
	$(CMD) -DLOC_THREADS=$(THREADS) $< -fopenmp -pthread -o $@  $(ADD_SRC)

$(TGT_OMP_RECTS): $(LOC_RECTS).cpp size.h ../size.h $(BENCH)_p.h $(BENCH).h $(INC_DIR)/*
	$(CMD) -DLOC_THREADS=$(THREADS) $< -fopenmp -pthread -o $@   $(ADD_SRC)


COMMAND := cleo-submit -np 1 -stderr err -as single

run_np: $(TGT_NP)
	$(COMMAND) $<

run_papi: $(TGT_PAPI)
	$(COMMAND) $<

run_files: $(TGT_FILES)
	$(COMMAND) $<

run_rects: $(TGT_RECTS)
	$(COMMAND) $<

run_omp_np: $(TGT_OMP_NP)
	$(COMMAND) $<

run_omp_papi: $(TGT_OMP_PAPI)
	$(COMMAND) $<

run_omp_files: $(TGT_OMP_FILES)
	$(COMMAND) $<

run_omp_rects: $(TGT_OMP_RECTS)
	$(COMMAND) $<

run: run_np run_papi run_rects run_omp_np run_omp_papi run_omp_rects

clean:
	rm -rf $(OUT_DIR)/* err *.rep-* *.out-* arrays/*/*
