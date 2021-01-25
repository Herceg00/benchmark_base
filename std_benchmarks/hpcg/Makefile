# -*- Makefile -*-

# by default, "arch" is unknown, should be specified in the command line
arch = Linux_Serial

setup_file = setup/Make.$(arch)
include $(setup_file)

HPCG_DEPS = src/CG.o src/CG_ref.o src/TestCG.o src/ComputeResidual.o \
         src/ExchangeHalo.o src/GenerateGeometry.o src/GenerateProblem.o \
	 src/OptimizeProblem.o src/ReadHpcgDat.o src/ReportResults.o \
	 src/SetupHalo.o src/TestSymmetry.o src/TestNorms.o src/WriteProblem.o \
         src/YAML_Doc.o src/YAML_Element.o src/ComputeDotProduct.o \
         src/ComputeDotProduct_ref.o src/finalize.o src/init.o src/mytimer.o src/ComputeSPMV.o \
         src/ComputeSPMV_ref.o src/ComputeSYMGS.o src/ComputeSYMGS_ref.o src/ComputeWAXPBY.o src/ComputeWAXPBY_ref.o \
         src/ComputeMG_ref.o src/ComputeMG.o src/ComputeProlongation_ref.o src/ComputeRestriction_ref.o src/GenerateCoarseProblem.o

all : bin/xhpcg_np bin/xhpcg_papi

DIR = /home/localityman/locality/locutils_new/

run: all
	bin/xhpcg_np --nx=32 --ny=32 --nz=32
	bin/xhpcg_papi --nx=32 --ny=32 --nz=32

bin/xhpcg_np: testing/main_np.o $(HPCG_DEPS)
	$(LINKER) $(LINKFLAGS) testing/main_np.o $(HPCG_DEPS) $(DIR)/loc_profile.cpp -o $@ $(HPCG_LIBS)

bin/xhpcg_papi: testing/main_papi.o $(HPCG_DEPS)
	$(LINKER) $(LINKFLAGS) testing/main_papi.o $(HPCG_DEPS) -lpapi $(DIR)/loc_profile.cpp $(DIR)/papi_utils.cpp -o $@ $(HPCG_LIBS)

clean:
	rm -f $(HPCG_DEPS) bin/xhpcg testing/main.o

.PHONY: clean

