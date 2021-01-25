#ifndef S_MATRIX_VECT_MULT_DIAG
#define S_MATRIX_VECT_MULT_DIAG

struct SparseDiagMatrixSpec
{
	size_t length;

	size_t a;
	size_t b;
	size_t x;
	size_t ip;
	size_t ia;
	size_t ic;

	size_t* lengths;
};

template<typename DAT, typename IAT>
struct SparseDiagMatrix
{
	DAT a;
	DAT b;
	DAT x;
	IAT ip;
	IAT ia;
	IAT ic;

	size_t num_diag;
};

SparseDiagMatrixSpec GenerateSpec(int length, int sparsity)
{
	SparseDiagMatrixSpec spec;

	spec.length = length;
	spec.lengths = new size_t[length];

	int sum = 0;

	size_t len_max = 0;

	// generate number of elements in each string of matrix A, 1<= number of elements <= length/SPARSE_COEF
	for(int i = 0; i < length; i++) {
		spec.lengths[i] = (uint)((locality::utils::RRand(i) * (length/sparsity)) + 1); // value between 1 and length/SPARSE_COEF
		sum += spec.lengths[i];

		if (spec.lengths[i] > len_max)
			len_max = spec.lengths[i];
		//cout << lengths[i] << ' ';
	}
	//cout << endl << sum << endl;

	spec.a = sum;
	spec.b = length;
	spec.x = length;
	spec.ia = sum;
	spec.ip = len_max + 1;
	spec.ic = sum;

	return spec;
}

// this function creates random sparse matrix and returns length of a array
template<typename DAT, typename IAT>
void GenerateMatrix(SparseDiagMatrix<DAT, IAT>& matrix, SparseDiagMatrixSpec spec)
{
	matrix.num_diag = 0;

	size_t *cols = new size_t[spec.length]; // this array will contain current column number for each string
	size_t *fix_lengths = new size_t[spec.length];

	for(size_t i = 0; i < spec.length; i++)
	{
		cols[i] = 0;
		fix_lengths[i] = spec.lengths[i];
	}

	matrix.ip[0] = 0;

	size_t str = 0;
	size_t count = 0;
	int user_max = spec.length;
	int notnull_strings = spec.length;

	while(true)
	{
		if (fix_lengths[str] != 0)
		{ // this means that string str has used all its not null numbers
			matrix.ia[count] = str;
			// choose column number for this string element
						cols[str] = (locality::utils::RRand(count) * (spec.length/spec.lengths[str]))+(spec.lengths[str]-spec.lengths[str])*spec.length/spec.lengths[str];
			matrix.ic[count] = cols[str];
			// create element value

			matrix.a[count] = (locality::utils::RRand(count)*user_max+0.01); // if type=float or double, then random type value between 0.01 and length^2

			count++;
			fix_lengths[str]--;
			if (fix_lengths[str] == 0)
				notnull_strings--;
		}

		if (notnull_strings == 0) break; // end of loop

		if (str == spec.length-1)
		{
			str = 0;
			matrix.num_diag++;
			matrix.ip[matrix.num_diag] = count;
		}
		else str++;
	}

	matrix.ip[++matrix.num_diag] = count;
}

template <typename DT, typename IT, typename DAT, typename IAT>
double Check(SparseDiagMatrix<DAT, IAT> matrix, SparseDiagMatrixSpec spec)
{
	DT sum = 0;

	for(size_t i = 0; i < spec.x; i++)
		sum += matrix.x[i];

	return sum;
}

template <typename DT, typename IT, typename DAT, typename IAT>
void Kernel(SparseDiagMatrix<DAT, IAT> matrix, SparseDiagMatrixSpec)
{
	LOC_PAPI_BEGIN_BLOCK

	for(size_t j = 0; j <= matrix.num_diag-1; j++)
	{
		for(IT i = matrix.ip[j]; i <= matrix.ip[j+1]-1; i++)
		{
			IT ia_i = matrix.ia[i];

			matrix.b[ia_i] = matrix.b[ia_i] + matrix.a[i] * matrix.x[matrix.ic[i]];
		}
	}

	LOC_PAPI_END_BLOCK
}

#endif /*S_MATRIX_VECT_MULT_DIAG*/
