#define POLY 0x0000000000000007UL
#define PERIOD 1317624576693539401L

/* Utility routine to start random number generator at Nth step */
uint64_t HPCC_starts(int64_t n)
{
	int i;
	uint64_t m2[64];
	uint64_t temp, ran;

	while (n < 0) n += PERIOD;
	while (n > PERIOD) n -= PERIOD;

	if (n == 0) return 0x1;

	temp = 0x1;
		for(i = 0; i < 64; i++) {
		m2[i] = temp;
			temp = (temp << 1) ^ ((int64_t) temp < 0 ? POLY : 0);
		temp = (temp << 1) ^ ((int64_t) temp < 0 ? POLY : 0);
	}

	for(i = 62; i>=0; i--)
		if ((n >> i) & 1)
			break;

	ran = 0x2;
	while (i > 0) {
		temp = 0;
		for(int j = 0; j < 64; j++)
			if ((ran >> j) & 1)
				temp ^= m2[j];
			ran = temp;
		i -= 1;
		if ((n >> i) & 1)
		ran = (ran << 1) ^ ((int64_t) ran < 0 ? POLY : 0);
	}

	return ran;
}

template<typename T, typename AT1, typename AT2>
void Init(AT1 table, AT2 ran, int size, int helper_size)
{
	for(int i = 0; i < size; i++)
		table[i] = i; // initial values the same as in HPCC

	for(int j = 0; j < helper_size; j++)
		ran[j] = HPCC_starts ((4*size) / helper_size * j);
}

template<typename T, typename AT1, typename AT2>
T Check(AT1 table, AT2 ran, int size, int helper_size)
{
	T sum = 0;
	for(int i = 0; i < size; i++)
		sum += table[i] % 256; // 256? 256!

	sum = sum / size;

	for(int j = 0; j < helper_size; j++)
		sum += ran[j] % 256;

	return sum;
}

template<typename T, typename AT1, typename AT2>
void Kernel(AT1 table, AT2 ran, int size, int helper_size)
{
	/* // original
	for(int i = 0; i < (4*size)) / helper_size; i++) {
		for(int j = 0; j < helper_size; j++) {
			ran[j] = (ran[j] << 1) ^ ((int64_t) ran[j] < 0 ? POLY : 0);
			table[ran[j] & (size-1)] = table[ran[j] & (size-1)] ^ ran[j];
		}
	}
	*/

	LOC_PAPI_BEGIN_BLOCK

	for(int i = 0; i < (4*size) / helper_size; i++) {
		for(int j = 0; j < helper_size; j++) {
			T tmp = ran[j];
			T new_val = (tmp << 1) ^ ((int64_t) tmp < 0 ? POLY : 0);
			ran[j] = new_val;
			table[new_val & (size-1)] = table[new_val & (size-1)] ^ new_val;
		}
	}

	LOC_PAPI_END_BLOCK
}
