#ifndef GAUSS_H
#define GAUSS_H

template <typename T, typename AT1, typename AT2, typename AT1_indx>
void Init(AT1 b, AT1 x, AT2 matrix, AT1_indx indx, int size)
{
	for(int i = 0; i < size; i++)
	{
		b[i] = locality::utils::RRand(i);
		x[i] = 0;
		indx[i] = 0;

		for(int j = 0; j < size; j++)
			matrix[i][j] = locality::utils::RRand(i, j);
	}
}

template <typename T, typename AT1>
T Check(AT1 x, int size)
{
	T sum = 0;

	for(int i = 0; i < size; i++)
		sum += x[i];

	return sum;
}

template <typename T, typename AT2, typename AT1_indx>
void Kernel1(AT2 matrix, AT1_indx indx, int size)
{
	int i, j, itmp;
	int k = 0;
	double c1, pi, pi1, pj;
	double c[size];

	/* Initialize the index */
	for (i = 0; i < size; ++i) {
			indx[i] = i; //size (write)
	}

	/* Find the rescaling factors, one from each row */
	for (i = 0; i < size; ++i) {
			c1 = 0;
			for (j = 0; j < size; ++j) {
				if (fabs(matrix[i][j]) > c1) {
				c1 = fabs(matrix[i][j]); //size^2 (read)
			}
			}
			c[i] = c1; //size (write)
	}

	/* Search the pivoting (largest) element from each column */
	for (j = 0; j < size-1; ++j) {
			pi1 = 0;
			for (i = j; i < size; ++i) {
				int indx_i = indx[i];   //size ^ 2  / 2 (read)
				pi = fabs(matrix[indx_i][j])/c[indx_i]; // 2 * size ^ 2  / 2 (read)
				if (pi > pi1) {
					pi1 = pi;
					k = i;
				}
			}
		/* Interchange the rows via indx[] to record pivoting order */
			itmp = indx[j]; //size (read)
			indx[j] = indx[k]; // 2 * size (rw)
			indx[k] = itmp; //size(w)

			int indx_j = indx[j]; //size(w)
			for (i = j+1; i < size; ++i) {
				int indx_i = indx[i];  //size ^ 2  / 2 (read)
				pj = matrix[indx_i][j]/matrix[indx_j][j];  // 2 * size ^ 2  / 2 (read)

			/* Record pivoting ratios below the diagonal */
				matrix[indx_i][j] = pj; //size ^ 2  / 2 (w)

			/* Modify other elements accordingly */
				for (k = j+1; k < size; ++k) {
					matrix[indx_i][k] = matrix[indx_i][k]-pj*matrix[indx_j][k];  // 3 * size ^ 3  / 4 (r + w)
				}
			}
	}
}

template <typename T, typename AT1, typename AT2, typename AT1_indx>
void Kernel2(AT1 b, AT1 x, AT2 matrix, AT1_indx indx, int size)
{
	for(int i = 0; i < size-1; ++i) {
			int indx_i = indx[i]; //size(read)
			for(int j = i+1; j < size; ++j) {
				b[indx[j]] = b[indx[j]]-matrix[indx[j]][i]*b[indx_i]; // 5 * size^2 / 2 (r+w)
			}
	}

	x[size-1] = b[indx[size-1]]/matrix[indx[size-1]][size-1]; //little number outside of cycle
	for (int i = size-2; i>=0; i--) {
			x[i] = b[indx[i]]; // 3 * size (r+w)

			int indx_i = indx[i]; //size(r)
			T x_i = x[i];      //size(r)
			for (int j = i+1; j < size; ++j) {
				x_i = x_i-matrix[indx_i][j]*x[j]; //2 * size^2/ 2 (read)
			}
			x[i] = x_i; //size(w)
		x[i] = x[i] / matrix[indx[i]][i]; //3 * size(w+ r)
	}
}

template <typename T, typename AT1, typename AT2, typename AT1_indx>
void Kernel(AT1 b, AT1 x, AT2 matrix, AT1_indx indx, int size)
{
	LOC_PAPI_BEGIN_BLOCK

	Kernel1<base_type, matrix_type, indx_type>(matrix, indx, size);
	Kernel2<base_type, vec_type, matrix_type, indx_type>(b, x, matrix, indx, size);

	LOC_PAPI_END_BLOCK
}

#endif
