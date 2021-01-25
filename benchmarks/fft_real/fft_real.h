#ifndef FFT_REAL_H
#define FFT_REAL_H

template <typename T, typename AT>
void Init(AT dIn, AT dOut, AT data, int nn)
{
 	for(int i = 0; i < nn; i++)
 		dIn[i] = locality::utils::RRand(i);

 	for(int i = 0; i < nn/2; i++)
 		dOut[i] = 0;

	for(int i = 0; i <= nn; i++)
	{
		data[ i * 2 ] = 0;
		data[ i * 2 + 1 ] = 0;
	}

/*	for( type x = 0.; x <= con*10. * M_PI; x += M_PI / 180.0 )
	{
		dIn[i] =  sin( x ) * 1000. + cos( 0.5 * x ) * 1000.;
	}*/
}

template <typename T, typename AT>
T Check(AT dOut, int nn)
{
	T sum = 0;

	for (int i = 0; i < nn; i++)
		sum += dOut[i];

	return sum / nn;
}

template <typename T, typename AT>
void Kernel(AT dIn, AT dOut, AT data, int nn)
{
	LOC_PAPI_BEGIN_BLOCK

	int i, j, n, m, mmax, istep;
	T tempr, tempi, wtemp, theta, wpr, wpi, wr, wi;

	for( i = 0; i < nn; i++ )
	{
		data[ i * 2 ] = 0;
		data[ i * 2 + 1 ] = dIn[ i ];
	}

	int isign = -1;

	n = nn << 1;
	j = 1;
	i = 1;

	while( i < n )
	{
		if( j > i )
		{
			tempr = data[ i ]; data[ i ] = data[ j ]; data[ j ] = tempr;

			tempr = data[ i + 1 ]; data[ i + 1 ] = data[ j + 1 ]; data[ j + 1 ] = tempr;
		}
		m = n >> 1;
		while( ( m >= 2 ) && ( j > m ) )
		{
			j = j - m;
			m = m >> 1;
		}
		j = j + m;
		i = i + 2;
	}

	mmax = 2;
	while( n > mmax )
	{
		istep = 2 * mmax;
		theta = 2.0 * M_PI / ( isign * mmax );
		wtemp = sin( 0.5 * theta );
		wpr = -2.0 * wtemp * wtemp;
		wpi = sin( theta );
		wr = 1.0;
		wi = 0.0;
		m = 1;
		while( m < mmax )
		{
			i = m;
			while( i < n )
			{
				j = i + mmax;
				tempr = wr * data[ j ] - wi * data[ j + 1 ];

				tempi = wr * data[ j + 1 ] + wi * data[ j ];

				data[ j ] = data[ i ] - tempr;

				data[ j + 1 ] = data[ i + 1 ] - tempi;

				data[ i ] = data[ i ] + tempr;

				data[ i + 1 ] = data[ i + 1 ] + tempi;
				i = i + istep;

			}
			wtemp = wr;
			wr = wtemp * wpr - wi * wpi + wr;
			wi = wi * wpr + wtemp * wpi + wi;
			m = m + 2;
		}
		mmax = istep;
	}

	for( i = 0; i < ( nn / 2 ); i++ )
	{
		dOut[ i ] = sqrt( data[ i * 2 ] * data[ i * 2 ] + data[ i * 2 + 1 ] * data[ i * 2 + 1 ] );
	}

	LOC_PAPI_END_BLOCK
}

#endif /*FFT_REAL_H*/
