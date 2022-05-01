/*********************************************************************
 * \file   MathRandom.h
 * \brief  Math Random Utilities
 * 
 * \author Lancelot 'Robin' Chen
 * \date   April 2022
 *********************************************************************/
#ifndef _MATH_RANDOM_H
#define _MATH_RANDOM_H

namespace Enigma::MathLib
{
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
        static double IntervalRandom(double minA, double maxA);
        /** Generate a random integer (singed) in [min,max). */
        static int IntervalRandom(int minA, int maxA);
    };
}
#endif // !_MATH_RANDOM_H
