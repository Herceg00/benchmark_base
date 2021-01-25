#ifndef S_MATRIX_VECT_MULT_ROW
#define S_MATRIX_VECT_MULT_ROW

struct SparseRowMatrixSpec
{
	size_t length;

	size_t a;
	size_t b;
	size_t x;
	size_t ip;
	size_t ia;

	size_t* lengths;
};

template<typename DAT, typename IAT>
struct SparseRowMatrix
{
	DAT a;
	DAT b;
	DAT x;
	IAT ip;
	IAT ia;
};

SparseRowMatrixSpec GenerateSpec(int length, int sparsity)
{
	SparseRowMatrixSpec spec;

	spec.length = length;
	spec.lengths = new size_t[length];

	int sum = 0;

	// generate number of elements in each string of matrix A, 1<= number of elements <= length/SPARSE_COEF
	for(int i = 0; i < length; i++) {
		spec.lengths[i] = (uint)((locality::utils::RRand(i) * (length/sparsity)) + 1); // value between 1 and length/SPARSE_COEF
		sum += spec.lengths[i];
		//cout << lengths[i] << ' ';
	}
	//cout << endl << sum << endl;

	spec.a = sum;
	spec.b = length;
	spec.x = length;
	spec.ia = sum;
	spec.ip = length + 1;

	return spec;
}

// this function creates random sparse matrix and returns length of a array
template<typename DAT, typename IAT>
void GenerateMatrix(SparseRowMatrix<DAT, IAT>& matrix, SparseRowMatrixSpec spec)
{
	int user_max = spec.length;

	for(size_t i = 0; i < spec.x; i++)
		matrix.x[i] = ((float)(locality::utils::RRand(i)-0.01)*user_max+0.01);

	for(size_t i = 0; i < spec.b; i++)
		matrix.b[i] = 0;

	int count = 0;
	int col = 0;

	matrix.ip[0] = 0;
	for (size_t i = 0; i < spec.length; i++) {
		col = 0;
		// for each string generate number of elements in it
		for (size_t  j = 0; j < spec.lengths[i]; j++) {
			// for a
			matrix.a[count] = (locality::utils::RRand(i, j)*user_max+0.01); // if type=float or double, then random type value between 0.01 and spec.length^2

			col = (locality::utils::RRand(j, i) * (spec.length/spec.lengths[i]))+j*spec.length/spec.lengths[i];
			matrix.ia[count] = col;

			count++;
		}
		// for ip
		matrix.ip[i+1] = count;
	}
}

template <typename DT, typename IT, typename DAT, typename IAT>
double Check(SparseRowMatrix<DAT, IAT> matrix, SparseRowMatrixSpec spec)
{
	DT sum = 0;

	for(size_t i = 0; i < spec.x; i++)
		sum += matrix.x[i];

	return sum;
}

template <typename DT, typename IT, typename DAT, typename IAT>
void Kernel(SparseRowMatrix<DAT, IAT> matrix, SparseRowMatrixSpec spec)
{
	LOC_PAPI_BEGIN_BLOCK

	for(size_t i = 0; i < spec.length; i++)
	{
		for(IT  j = matrix.ip[i]; j <= matrix.ip[i+1]-1; j++)
		{
			matrix.b[i] = matrix.b[i] + matrix.a[j] * matrix.x[matrix.ia[j]];
		}
	}

	LOC_PAPI_END_BLOCK
}

#endif /*S_MATRIX_VECT_MULT_ROW*/
