/*==================================================================================================================================
                                                     random.h
====================================================================================================================================



=================================================================================================================================*/

#ifndef random_h
#define random_h

#include <ctime>
#include <boost/random/mersenne_twister.hpp>
#include <boost/random/binomial_distribution.hpp>
#include <boost/random/bernoulli_distribution.hpp>
#include <boost/random/poisson_distribution.hpp>
#include <boost/random/uniform_real_distribution.hpp>
#include <boost/random/uniform_smallint.hpp>
#include <boost/random/uniform_int.hpp>
#include <boost/random/normal_distribution.hpp>
#include <boost/random/exponential_distribution.hpp>

namespace rnd
{
	extern boost::mt19937 rng;
	void set_seed();
	void set_seed(const unsigned int&);

	template <class T> 
	class Random_Small_Integral_Type 
	{
		public:
			T operator() (const T &n) const
			{
				return boost::random::uniform_smallint<T>(0, n - 1)(rng);
			}
	};

	template <class T> 
	class Random_Integral_Type 
	{
		public:
			T operator() (const T &n) const
			{
				return boost::random::uniform_int_distribution<T>(0, n - 1)(rng);
			}
	};
    
    template <class T>
    void shuffle(T k[], const int &n)
    //shuffles the array k[]
    {
        for(int i = 0; i < n - 1; ++i)
        {
            const int j = i + boost::random::uniform_int_distribution<int>(0, n - 1 - i)(rng);
            T tmp(k[i]);
            k[i] = k[j];
            k[j] = tmp;
        }
    }

	extern Random_Integral_Type<int> random_int;
	extern Random_Small_Integral_Type<int> random_small_int;
	int bernoulli(const double& = 0.5);
	int binomial(const int&, const double& = 0.5);
	long binomial(const long&, const double& = 0.5);
	int poisson(const double& = 1.0);
	double uniform(const double& = 1.0);
    double normal(const double& = 0.0, const double& = 1.0);
    double exponential(const double& = 1.0);
    int sample_1(const double[], const int &);
	void sample_n(int, int[], double[], const int &);
}

#endif
