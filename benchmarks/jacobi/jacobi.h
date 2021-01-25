
#define MAX_INNER_ITERS 10
const double eps = 0.001;

template <typename T, typename AT1, typename AT2>
void Init(AT2 matrix, AT1 f, AT1 x, AT1 temp_x, int size)
{
	for (int i = 0; i < size; i++) {
		T sum = 0.0;
		for (int j = 0; j < size; j++) {
			if (j%2 == 0) {matrix[i][j] = 1.0/(size+i+j); sum += matrix[i][j];}
			else {matrix[i][j] = 1.0/(size+i+(size-j)); sum += matrix[i][j];}
			//if (j%2 == 0) matrix[i][j] = 1.0/(size+j);
						//else matrix[i][j] = 1.0/(size+(size-j));
		}
		matrix[i][i] = sum;
		x[i] = 0;
		f[i] = 1.0/(size-i+1);
		temp_x[i] = 0;
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

template <typename T, typename AT1, typename AT2>
void Kernel(AT2 matrix, AT1 f, AT1 x, AT1 temp_x, int size)
{
	LOC_PAPI_BEGIN_BLOCK

	double norm = 0;
	int counter = 0;

	do {
#pragma omp parallel for num_threads(LOC_THREADS)
		for(int i = 0; i < size; i++)
			{
				T temp_x_i = -f[i];

				for (int g = 0; g < size; g++)
				{
					if (i != g)
						temp_x_i = temp_x_i + matrix[i][g] * x[g];
				}

				temp_x[i] = -temp_x_i/matrix[i][i];
			}

		norm = fabs(x[0] - temp_x[0]);

		for(int h = 0; h < size; h++)
		{
			if (fabs(x[h] - temp_x[h]) > norm)
				norm = fabs(x[h] - temp_x[h]);

			x[h] = temp_x[h];
		}
		counter++;
	} while (norm > eps && counter < MAX_INNER_ITERS);

	LOC_PAPI_END_BLOCK
}
