cd matrix_mult
for size in 16 32 64 128 256; do make run_files SIZE=$size; done 
cd ..

cd block_matrix_mult
for size in 16 32 64 128 256; do make run_files SIZE=$size; done 
cd ..

cd holecky
for size in 64 128 256 512 1024; do make run_files SIZE=$size; done 
cd ..

cd vectors
for size in 8192 16384 32768 65536 131072 262144 524288 1048576; do make run_files SIZE=$size; done 
cd ..

cd rand
for size in 8192 16384 32768 65536 131072 262144 524288 1048576 2097152 4194304; do make run_files SIZE=$size; done 
cd ..
