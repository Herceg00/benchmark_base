#ifndef S_MATRIX_VECT_MULT_SDIAG
#define S_MATRIX_VECT_MULT_SDIAG

struct SparseSDiagMatrixSpec
{
	size_t length;

	size_t a;
	size_t b;
	size_t x;
	size_t ibeg;
	size_t ip;
	size_t ic;

	size_t* lengths;
};

template<typename DAT, typename IAT>
struct SparseSDiagMatrix
{
	DAT a;
	DAT b;
	DAT x;
	IAT ibeg;
	IAT ip;
	IAT ic;

	size_t num_diag;
};

SparseSDiagMatrixSpec GenerateSpec(size_t length, int sparsity)
{
	SparseSDiagMatrixSpec spec;

	spec.length = length;
	spec.lengths = new size_t[length];

	int sum = 0;

	// generate number of elements in each string of matrix A, 1<= number of elements <= length/SPARSE_COEF
	for(size_t i = 0; i < length; i++) {
		spec.lengths[i] = (uint)((locality::utils::RRand(i) * (length/sparsity)) + 1); // value between 1 and length/SPARSE_COEF
		sum += spec.lengths[i];
		//cout << lengths[i] << ' ';
	}
	//cout << endl << sum << endl;

	// count number of diagonals (it wiil be the size of ip array)
	size_t tmp = 0, tmp_size = 0, tmp_base = 0;
	tmp = spec.lengths[0];
	size_t i = 0;
	while (true) {
		// phase 1 - find local max
		while (i < length) {
			if (tmp <= spec.lengths[i]) tmp = spec.lengths[i];
			else break;
			i++;
		}
		tmp_size += tmp-tmp_base;
		//cout << "tmp_size: " << tmp_size << ", tmp: " << tmp << ", tmp_base: " << tmp_base << endl;
		if (i >= length) break; // finita la loop
		tmp = spec.lengths[i];
		// phase 2 - find local min
		while (i < length) {
			if (tmp >= spec.lengths[i]) tmp = spec.lengths[i];
			else break;
			i++;
		}
				if (i >= length) break; // finita la loop
		tmp_base = tmp;
		tmp = spec.lengths[i];
	}

	spec.a = sum;
	spec.b = length;
	spec.x = length;
	spec.ibeg = tmp_size;
	spec.ip = tmp_size + 1;
	spec.ic = sum;

	return spec;
}

// this function creates random sparse matrix and returns length of a array
template<typename DAT, typename IAT>
void GenerateMatrix(SparseSDiagMatrix<DAT, IAT>& matrix, SparseSDiagMatrixSpec spec)
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
	bool str_was_empty = false;

	while(true) {
		if (fix_lengths[str] != 0) { // this means that string str has used all its not null numbers
			// create ip
			if (str == 0) {
				matrix.ip[matrix.num_diag] = count;
				matrix.ibeg[matrix.num_diag] = str;
				//cout << "1.matrix.num_diag: " << matrix.num_diag << ", count: " << count << endl;
				matrix.num_diag++;
			}
			else {
				if ((fix_lengths[str-1] == 0) && (fix_lengths[str] != 0) && str_was_empty)
				{
					matrix.ip[matrix.num_diag] = count;
					matrix.ibeg[matrix.num_diag] = str;
					str_was_empty = false;
									//cout << "2.matrix.num_diag: " << matrix.num_diag << ", count: " << count << endl;
					matrix.num_diag++;
				}
			}
			// choose column number for this string element
			cols[str] = (locality::utils::RRand(count)*(spec.length/spec.lengths[str]))+(spec.lengths[str]-fix_lengths[str])*spec.length/spec.lengths[str];
			matrix.ic[count] = cols[str];
			// create element value
			matrix.a[count] = (locality::utils::RRand(count)*user_max+0.01); // if type=float or double, then random type value between 0.01 and length^2
			count++;
			fix_lengths[str]--;
			str_was_empty = false;

			if (fix_lengths[str] == 0)
				notnull_strings--;
		}
		else str_was_empty = true;

		if (notnull_strings == 0) break; // end of loop

		if (str == spec.length-1) str = 0;
		else str++;
	}
	matrix.ip[matrix.num_diag] = count;
}

template <typename DT, typename IT, typename DAT, typename IAT>
double Check(SparseSDiagMatrix<DAT, IAT> matrix, SparseSDiagMatrixSpec spec)
{
	DT sum = 0;

	for(size_t i = 0; i < spec.x; i++)
		sum += matrix.x[i];

	return sum;
}

template <typename DT, typename IT, typename DAT, typename IAT>
void Kernel(SparseSDiagMatrix<DAT, IAT> matrix, SparseSDiagMatrixSpec)
{
	LOC_PAPI_BEGIN_BLOCK

	for (size_t j = 0; j <= matrix.num_diag-1; j++)
	{
		IT k = matrix.ibeg[j];

		for (IT i = matrix.ip[j]; i <= matrix.ip[j+1]-1; i++)
		{
			matrix.b[k] = matrix.b[k] + matrix.a[i] * matrix.x[matrix.ic[i]];
			k++;
		}
	}

	LOC_PAPI_END_BLOCK
}

#endif /*S_MATRIX_VECT_MULT_SDIAG*/
