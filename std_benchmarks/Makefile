BENCHS=linpack fft matrix_mult triada rand gauss jacobi rec_fft d_matrix_vect_mult block_matrix_mult vectors vectors_seqpar holecky matrix_transp qubit_evolution givens

all:
	$(foreach bench, $(BENCHS), $(MAKE) -C $(bench) ;)

run:
	$(foreach bench, $(BENCHS), $(MAKE) -C $(bench) run;)
	$(MAKE) -C NPB_PAPI run
	$(MAKE) -C NPB_NP run
	$(MAKE) -C NPB_RECTS run

run_papi:
	$(foreach bench, $(BENCHS), $(MAKE) -C $(bench) run_papi ;)
	$(MAKE) -C NPB_PAPI run

run_files:
	$(foreach bench, $(BENCHS), $(MAKE) -C $(bench) run_files ;)

run_np:
	$(foreach bench, $(BENCHS), $(MAKE) -C $(bench) run_np ;)

run_rects:
	$(foreach bench, $(BENCHS), $(MAKE) -C $(bench) run_rects ;)

run_rects_2:
	$(foreach bench, $(BENCHS), $(MAKE) -i -C $(bench) run_rects_2 ;)

run_rects_3:
	$(foreach bench, $(BENCHS), $(MAKE) -i -C $(bench) run_rects_3 ;)

clean:
	$(foreach bench, $(BENCHS), $(MAKE) -C $(bench) clean ;)
	
	$(MAKE) -C NPB_RECTS veryclean
	$(MAKE) -C NPB_PAPI veryclean
	$(MAKE) -C NPB_FILES veryclean
	$(MAKE) -C NPB_NP veryclean
	rm -rf *.out-* *.rep-*
