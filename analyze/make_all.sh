bash make2.sh input/block_matrix_mult_files_32.unified.log.kij "Block Matrix mult kij" &
bash make2.sh input/block_matrix_mult_files_32.unified.log.kji "Block Matrix mult kji" &

bash make2.sh input/matrix_mult_files_32.unified.log.kij "Matrix mult kij" &
bash make2.sh input/matrix_mult_files_32.unified.log.kji "Matrix mult kji" &

bash make2.sh input/rand_files_8192.unified.log "HPCC pseudo-random access"&
bash make2.sh input/holecky_files_64.unified.log "Holecky" &

bash make2.sh input/vectors_files_8192.unified.log.scalar "Dot product" &
