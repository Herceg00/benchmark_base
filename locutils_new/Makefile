##### input #####

# window size
# W_SIZE=...
W_SIZE ?= 512

# task size
# SIZE=...

#####
#CC_FLAGS=-O3 -funroll-loops -ftree-vectorize -ffast-math -march=armv8.1-a #-march=skylake-avx512 -mavx512f -mavx512cd  #-fopt-info-vec-all
#CC_FLAGS= -O3 -ftree-vectorize -funroll-loops -ffast-math -fopenmp #-mno-sse -mno-avx

CXX=$(COMPILER)

ifeq ($(COMPILER),clang++)
    CC_FLAGS=-O3 -fopenmp -ffast-math -ffreestanding -fnt-store -march=znver3 #-Rpass=loop-vectorize -Rpass-missed=loop-vectorize -Rpass-analysis=loop-vectorize   #clang AMD
endif

ifeq ($(COMPILER),g++)
    CC_FLAGS=-D __USE_KUNPENG__ -O3 -fno-trapping-math -fopenmp-simd -fopenmp  -ffreestanding -fopt-info-vec-all=report.lst -ffast-math #gcc kunpeng
endif

ifeq ($(COMPILER),icpc)
    CC_FLAGS=-O3 -D NOFUNCCALL -qopt-report=1 -qopt-report-phase=vec -qopenmp -ffreestanding -qopt-streaming-stores always -xCOMMON-AVX512 # icc
endif

ifdef DEBUG
	CXXFLAGS := $(CC_FLAGS) -std=c++11 -w -Wall -Wextra -Wno-error=unused-parameter -Wno-error=unknown-pragmas -Wno-unknown-pragmas -Wno-misleading-indentation -I ../../external_libraries/VectorGraphLibrary/
else
	CXXFLAGS := $(CC_FLAGS) -std=c++11 -w -Wall -Wextra -Wno-error=unused-parameter -Wno-error=unknown-pragmas -Wno-unknown-pragmas -Wno-misleading-indentation -I ../../external_libraries/VectorGraphLibrary/
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

REPEAT ?= 8

TGT_OMP_NP = $(OUT_DIR)/omp_$(LOC_NP)_$(SIZE)

TGT := $(TGT_OMP_NP)

CMD := $(CXX) $(CPPFLAGS) $(CXXFLAGS) -I $(INC_DIR)

all : $(TGT)

$(TGT_OMP_NP): $(LOC_NP).cpp size.h ../size.h $(BENCH).h $(INC_DIR)/*
	mkdir bin
	$(CMD) -DLOC_REPEAT=$(REPEAT) -D$(METR)=0 -DRADIUS=$(ELEMS) -DLENGTH=$(LENGTH) -DMODE=$(MODE) $< -fopenmp -o $@  $(ADD_SRC)

COMMAND := cleo-submit -np 1 -stderr err -as single

run_omp_np: $(TGT_OMP_NP)
	$(COMMAND) $<

clean:
	rm -rf $(OUT_DIR)/* err *.rep-* *.out-* arrays/*/*
