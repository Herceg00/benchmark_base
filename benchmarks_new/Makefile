BENCHS= bfs stencil_3D stencil_2D stencil_1D random_access bellman_ford block_matrix_mult dijkstra d_matrix_vect_mult fft_complex fft_real gauss gauss_back gauss_forward gauss_simple_forward givens holecky householder jacobi k_means matrix_mult matrix_transp prime poisson qubit_evolution rand rec_fft s_matrix_vect_mult_col s_matrix_vect_mult_diag s_matrix_vect_mult_row s_matrix_vect_mult_sdiag stencil_1D strassen tsp triada tridiagonal tridiagonal_other vectors vectors_seqpar monosweep_repeated countersweep_repeated cyclic_reduction boruvka holderhouse

all:
	$(foreach bench, $(BENCHS), $(MAKE) -C $(bench) ;)

run:
	$(foreach bench, $(BENCHS), $(MAKE) -C $(bench) run;)

run_papi:
	$(foreach bench, $(BENCHS), $(MAKE) -C $(bench) run_papi ;)

run_files:
	$(foreach bench, $(BENCHS), $(MAKE) -C $(bench) run_files ;)

run_np:
	$(foreach bench, $(BENCHS), $(MAKE) -C $(bench) run_np ;)

run_rects:
	$(foreach bench, $(BENCHS), $(MAKE) -C $(bench) run_rects ;)

run_omp_papi:
	$(foreach bench, $(BENCHS), $(MAKE) -C $(bench) run_omp_papi ;)

run_omp_files:
	$(foreach bench, $(BENCHS), $(MAKE) -C $(bench) run_omp_files ;)

run_omp_np:
	$(foreach bench, $(BENCHS), $(MAKE) -C $(bench) run_omp_np ;)

run_omp_rects:
	$(foreach bench, $(BENCHS), $(MAKE) -C $(bench) run_omp_rects ;)

clean:
	$(foreach bench, $(BENCHS), $(MAKE) -C $(bench) clean ;)
	rm -rf *.out-* *.rep-*
