#ifndef S_MATRIX_VECT_MULT_COL
#define S_MATRIX_VECT_MULT_COL

struct SparseColMatrixSpec
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
struct SparseColMatrix
{
	DAT a;
	DAT b;
	DAT x;
	IAT ip;
	IAT ia;
};

SparseColMatrixSpec GenerateSpec(int length, int sparsity)
{
	SparseColMatrixSpec spec;

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
void GenerateMatrix(SparseColMatrix<DAT, IAT>& matrix, SparseColMatrixSpec spec)
{
	int user_max = spec.length;

	for(size_t i = 0; i < spec.x; i++)
		matrix.x[i] = ((float)(locality::utils::RRand(i)-0.01)*user_max+0.01);

	for(size_t i = 0; i < spec.b; i++)
		matrix.b[i] = 0;

	// main cycle for generating arrays: a,ip,ia
	int count = 0;
	int col = 0;

	matrix.ip[0] = 0;
	for(size_t i = 0; i < spec.length; i++) {
		col = 0;
		// for each string generate number of elements in it
		for(size_t j = 0; j < spec.lengths[i]; j++) {
			// for a
			matrix.a[count] = ((float)(locality::utils::RRand(i)-0.01)*user_max+0.01); // if type=float or double, then random type value between 0.01 and length^2

			// for ia
//                      col = (rand()%(length-col-1-(lengths[i]-j-1)))+(col+not_first);
//                      not_first = 1;
			col = (locality::utils::RRand(j, i)*(spec.length/spec.lengths[i]))+j*spec.length/spec.lengths[i];
			matrix.ia[count] = col;

			count++;
		}
		// for ip
		matrix.ip[i+1] = count;
	}
}

template <typename DT, typename IT, typename DAT, typename IAT>
double Check(SparseColMatrix<DAT, IAT> matrix, SparseColMatrixSpec spec)
{
	DT sum = 0;

	for(size_t i = 0; i < spec.x; i++)
		sum += matrix.x[i];

	return sum;
}

template <typename DT, typename IT, typename DAT, typename IAT>
void Kernel(SparseColMatrix<DAT, IAT> matrix, SparseColMatrixSpec spec)
{
	LOC_PAPI_BEGIN_BLOCK

	for(size_t j = 0; j < spec.length; j++)
	{
		for(IT i = matrix.ip[j]; i <= matrix.ip[j+1]-1; i++)
		{
			IT ia_i = matrix.ia[i];
			matrix.b[ia_i] = matrix.b[ia_i] + matrix.a[i]*matrix.x[j];
		}
	}

	LOC_PAPI_END_BLOCK
}

#endif /*S_MATRIX_VECT_MULT_COL*/
