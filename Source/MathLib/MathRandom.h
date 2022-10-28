/*********************************************************************
 * \file   MathRandom.h
 * \brief  Math Random Utilities
 * 
 * \author Lancelot 'Robin' Chen
 * \date   April 2022
 *********************************************************************/
#ifndef _MATH_RANDOM_H
#define _MATH_RANDOM_H

#include <random>

namespace Enigma::MathLib
{
    class UniformDoubleDistribution : public std::uniform_real_distribution<double>
    {
    public:
        UniformDoubleDistribution(double minA, double maxA, std::default_random_engine generator)
            : std::uniform_real_distribution<double>(minA, maxA), m_generator(generator) {}

        double operator()()
        {
            return std::uniform_real_distribution<double>::operator()(m_generator);
        }
    protected:
        std::default_random_engine m_generator;
    };

    class UniformFloatDistribution : public std::uniform_real_distribution<float>
    {
    public:
        UniformFloatDistribution(float minA, float maxA, std::default_random_engine generator)
            : std::uniform_real_distribution<float>(minA, maxA), m_generator(generator) {}

        float operator()()
        {
            return std::uniform_real_distribution<float>::operator()(m_generator);
        }
    protected:
        std::default_random_engine m_generator;
    };

    class UniformIntDistribution : public std::uniform_int_distribution<int>
    {
    public:
        UniformIntDistribution(int minA, int maxA, std::default_random_engine generator)
            : std::uniform_int_distribution<int>(minA, maxA), m_generator(generator) {}

        int operator()()
        {
            return std::uniform_int_distribution<int>::operator()(m_generator);
        }
    protected:
        std::default_random_engine m_generator;
    };

    class MathRandom
    {
    public:
        /** has initialize seeded */
        static bool HasSeeded();
        /** Random Seed */
        static void RandomSeed();
        /** Generate a random number in [0,1).  */
        static double UnitRandom();

        /** Generate a random number in [-1,1). */
        static double SymmetricRandom();

        /** Generate a random number in [min,max).  */
        static UniformDoubleDistribution IntervalDistribution(double minA, double maxA);
        /** Generate a random number in [min,max).  */
        static UniformFloatDistribution IntervalDistribution(float minA, float maxA);
        /** Generate a random integer (singed) in [min,max). */
        static UniformIntDistribution IntervalDistribution(int minA, int maxA);
    };
}
#endif // !_MATH_RANDOM_H
