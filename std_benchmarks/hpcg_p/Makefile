# -*- Makefile -*-

# by default, "arch" is unknown, should be specified in the command line
arch = UNKNOWN

W_SIZE ?= 512

setup_file = setup/Make.Linux_Serial
include $(setup_file)

HPCG_DEPS = src/CG.o src/CG_ref.o src/TestCG.o src/ComputeResidual.o \
         src/ExchangeHalo.o src/GenerateGeometry.o src/GenerateProblem.o \
	 src/OptimizeProblem.o src/ReadHpcgDat.o src/ReportResults.o \
	 src/SetupHalo.o src/TestSymmetry.o src/TestNorms.o src/WriteProblem.o \
         src/YAML_Doc.o src/YAML_Element.o src/ComputeDotProduct.o \
         src/ComputeDotProduct_ref.o src/finalize.o src/init.o src/mytimer.o src/ComputeSPMV.o \
         src/ComputeSPMV_ref.o src/ComputeSYMGS.o src/ComputeSYMGS_ref.o src/ComputeWAXPBY.o src/ComputeWAXPBY_ref.o \
         src/ComputeMG_ref.o src/ComputeMG.o src/ComputeProlongation_ref.o src/ComputeRestriction_ref.o src/GenerateCoarseProblem.o

DIR = /home/localityman/locality/locutils_new/

all: bin/xhpcg_files_$(W_SIZE) bin/xhpcg_rects_$(W_SIZE)

bin/xhpcg_files_$(W_SIZE): testing/main_files.o $(HPCG_DEPS)
	$(LINKER) $(LINKFLAGS) testing/main_files.o $(HPCG_DEPS) $(DIR)/loc_profile.cpp -o $@ $(HPCG_LIBS)

bin/xhpcg_rects_$(W_SIZE): testing/main_rects.o $(HPCG_DEPS)
	$(LINKER) $(LINKFLAGS) testing/main_rects.o $(HPCG_DEPS) $(DIR)/loc_profile.cpp  -o $@ $(HPCG_LIBS)

run_rects: all
	mpirun -np 1 -stderr err -as single bin/xhpcg_rects_$(W_SIZE) --nx=32 --ny=32 -nz=32

run_files: all
	mpirun -np 1 -stderr err -as single bin/xhpcg_files_$(W_SIZE) --nx=16 --ny=16 -nz=16

run: run_rects run_files

clean:
	rm -f $(HPCG_DEPS) bin/xhpcg testing/main.o

.PHONY: clean

