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
			indx[i] = i;
	}

	/* Find the rescaling factors, one from each row */
	for (i = 0; i < size; ++i) {
			c1 = 0;
			for (j = 0; j < size; ++j) {
				if (fabs(matrix[i][j]) > c1) {
				c1 = fabs(matrix[i][j]);
			}
			}
			c[i] = c1;
	}

	/* Search the pivoting (largest) element from each column */
	for (j = 0; j < size-1; ++j) {
			pi1 = 0;
			for (i = j; i < size; ++i) {
				int indx_i = indx[i];
				pi = fabs(matrix[indx_i][j])/c[indx_i];
				if (pi > pi1) {
					pi1 = pi;
					k = i;
				}
			}
		/* Interchange the rows via indx[] to record pivoting order */
			itmp = indx[j];
			indx[j] = indx[k];
			indx[k] = itmp;

			int indx_j = indx[j];
			for (i = j+1; i < size; ++i) {
				int indx_i = indx[i];
				pj = matrix[indx_i][j]/matrix[indx_j][j];

			/* Record pivoting ratios below the diagonal */
				matrix[indx_i][j] = pj;

			/* Modify other elements accordingly */
				for (k = j+1; k < size; ++k) {
					matrix[indx_i][k] = matrix[indx_i][k]-pj*matrix[indx_j][k];
				}
			}
	}
}

template <typename T, typename AT1, typename AT2, typename AT1_indx>
void Kernel2(AT1 b, AT1 x, AT2 matrix, AT1_indx indx, int size)
{
	for(int i = 0; i < size-1; ++i) {
			int indx_i = indx[i];
			for(int j = i+1; j < size; ++j) {
				b[indx[j]] = b[indx[j]]-matrix[indx[j]][i]*b[indx_i];
			}
	}

	x[size-1] = b[indx[size-1]]/matrix[indx[size-1]][size-1];
	for (int i = size-2; i>=0; i--) {
			x[i] = b[indx[i]];

			int indx_i = indx[i];
			T x_i = x[i];
			for (int j = i+1; j < size; ++j) {
				x_i = x_i-matrix[indx_i][j]*x[j];
			}
			x[i] = x_i;
		x[i] = x[i] / matrix[indx[i]][i];
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
