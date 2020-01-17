/*==================================================================================================================================
                                                     random.cpp
====================================================================================================================================



=================================================================================================================================*/

#include "random.h"

namespace rnd
{
	boost::mt19937 rng;
	Random_Small_Integral_Type<int> random_small_int;
	Random_Integral_Type<int> random_int;
	const double BINOMIALCOST = 7.0;	//time cost of drawing a binomial() deviate relative to drawing a uniform() deviate

	void set_seed()
	{
		rng.seed(static_cast<unsigned int>(time(0)));
	}

	void set_seed(const unsigned int &seed)
	{
		rng.seed(seed);
	}

	int bernoulli(const double &p)
	{
		return static_cast<int>(boost::random::bernoulli_distribution<double>(p)(rng));
	}

	int binomial(const int &n, const double &p)
	{
		return boost::random::binomial_distribution<int>(n, p)(rng);
	}

	long binomial(const long &n, const double &p)
	{
		return boost::random::binomial_distribution<long>(n, p)(rng);
	}

	int poisson(const double &lambda) 
	{
		return boost::random::poisson_distribution<int, double>(lambda)(rng);
	}

	double uniform(const double &max)
	{
		return boost::random::uniform_real_distribution<double>(0.0, max)(rng);
	}
    
    double normal(const double &mean, const double &stdev)
	{
		return boost::random::normal_distribution<double>(mean, stdev)(rng);
	}
    
    double exponential(const double &lambda)
	{
		return boost::random::exponential_distribution<double>(lambda)(rng);
	}

	int sample_1(const double cdf[], const int &n)
	//samples from a discrete cdf using bisection
    {
		int jmin = -1, jmax = n - 1;
        const double f = uniform() * cdf[jmax];
		while(jmax - jmin > 1)
        {
            const int jmid = (jmax + jmin) / 2;
            if(f > cdf[jmid]) jmin = jmid;
            else jmax = jmid;
        }
        return jmax;
    }
    
	void sample_n(int ksum, int k[], double pdf[], const int &n)
	//draws nsum samples from a discrete pdf (multinomial sampling)
	{
		double rsum = 0.0;
		for(int j = 0; j < n; ++j) rsum += pdf[j];
		
		//binomial sampling part
		double fsum = 0.0;
		for(int j = 0; ksum > 0 && j < n; ++j) {
			const double pj = pdf[j] / rsum;
			if(pj * ksum > BINOMIALCOST) {
				ksum -= k[j] = binomial(ksum, pj);
				rsum -= pdf[j];
				pdf[j] = fsum;
			}
			else {
				k[j] = 0;
				pdf[j] = fsum += pdf[j];
			}
		}

		//direct sampling part
		while(ksum) {
			++k[sample_1(pdf, n)];
			--ksum;
		}
	}
}
