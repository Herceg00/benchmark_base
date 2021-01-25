#ifndef VECTORS
#define VECTORS

const int BENCH_COUNT = 6;

const char* type_names[BENCH_COUNT] = {
	"op", // 0
	"op_d", // 1
	"scalar", // 2
	"scalar_d", // 3
	"horner", // 4
	"dqds" // 5
};

template <typename T, typename AT>
void Init(AT a, AT b, AT c, int size)
{
	for(int i = 0; i < size; i++)
	{
		a[i] = locality::utils::RRand(i);
		b[i] = locality::utils::RRand(i << 2);
		c[i] = 0;
	}
}

template <typename T, typename AT>
T Check(AT c, int)
{
	return c[0];
}

template <typename T, typename AT>
void KernelOp(AT a, AT, AT c, int size)
{
	T sum = 0;

	for(int i = 0; i < size; i++)
	{
		sum += a[i];
	}

	c[0] = sum;
}

template <typename T, typename AT>
void KernelOpD(AT a, AT, AT c, int size)
{

	int step = 1;

	while(step < size)
	{
		for (int i = 0; i < size; i += 2 * step)
		{
			a[i] = a[i] + a[i + step];
		}
		step *= 2;
	}

	c[0] = a[0];
}


template <typename T, typename AT>
void KernelScalar(AT a, AT b, AT c, int size)
{
	T sum = 0;

	for(int i = 0; i < size; i++)
	{
		sum += a[i] * b[i];
	}

	c[0] = sum;
}

template <typename T, typename AT>
void KernelScalarD(AT a, AT b, AT c, int size)
{
	int step = 1;

	while(step < size)
	{
		for (int i = 0; i < size; i += 2 * step)
		{
			if(step == 1)
			{
				a[i] = a[i]*b[i] + a[i + step] * b[i + step];
			}
			else
			{
				a[i] = a[i] + a[i + step];
			}
		}
		step *= 2;
	}

	c[0] = a[0];
}

template <typename T, typename AT>
void KernelHorner(AT a, AT, AT c, int size)
{
	T x = 0.1;

	for(int i = 1; i < size; i++)
	{
		c[i] = a[i] + c[i - 1] * x;
	}
}

template <typename T, typename AT>
void KernelDQDS(AT a, AT b, AT c, int size)
{
	T delta = 3.14; // some param

	T d = a[0] - delta;

	for(int j = 0; j < size - 2; j++)
	{
		a[j] = d + b[j];
		T t = a[j + 1] / a[j];
		b[j] = b[j] * t;
		d = d * t - delta;
	}

	a[size - 1] = d;

	c[0] = d; // checksum
}

template <typename T, typename AT>
void CallKernel(int core_type, AT a, AT b, AT c, int size)
{
	LOC_PAPI_BEGIN_BLOCK

	switch(core_type)
	{
		// op
		case 0: KernelOp<T, AT>(a, b, c, size); break;

		// op with double
		case 1: KernelOpD<T, AT>(a, b, c, size); break;

		// dot product
		case 2: KernelScalar<T, AT>(a, b, c, size); break;

		// dot product with double
		case 3: KernelScalarD<T, AT>(a, b, c, size); break;

		// horner
		case 4: KernelHorner<T, AT>(a, b, c, size); break;

		// dqds
		case 5: KernelDQDS<T, AT>(a, b, c, size); break;

		default: fprintf(stderr, "unexpected core type");
	}

	LOC_PAPI_END_BLOCK
}

#endif /*VECTORS*/
