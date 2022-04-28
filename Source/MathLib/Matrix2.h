/*********************************************************************
 * \file   Matrix2.h
 * \brief  
 * 
 * \author Lancelot 'Robin' Chen
 * \date   April 2022
 *********************************************************************/
#ifndef _MATH_MATRIX2_H
#define _MATH_MATRIX2_H

#include "Vector2.h"
#include "AlgebraBasicTypes.h"

namespace Enigma::MathLib
{
    /** Math Lib Matrix2
          @remarks
              The layout of the matrix entries is shown below:
          <pre>
          [ m[0][0]  m[0][1] ]
          [ m[1][0]  m[1][1] ]
          </pre>
          @par
            Rotation matrices are of the form
              <pre>
             R = cos(t) -sin(t)
                 sin(t)  cos(t)
            </pre>
            where t > 0 indicates a clockwise (bcz. left-hand coordinate) rotation in the xy-plane.
    */
    class Matrix2
    {
    public:
    public:
        /// If bZero is true, create the zero matrix.  Otherwise, create the identity matrix.
        Matrix2(bool isZero = true);
        /// copy constructor
        Matrix2(const Matrix2&) = default;
        /// move constructor
        Matrix2(Matrix2&&) = default;
        /// input Mrc is in row r, column c.
        Matrix2(float m00, float m01, float m10, float m11);
        /** Create a matrix from an array of numbers.  The input array is \n
           entry[0..3] = {m00,m01,m10,m11} */
        Matrix2(const float entry[4]);
        /// create a diagonal matrix
        Matrix2(float m00, float m11);
        /// create a rotation matrix (positive angle - clockwise)
        Matrix2(float angle);

        /** @name create various matrices */
        //@{
        static Matrix2 MakeZero();
        static Matrix2 MakeIdentity();
        static Matrix2 MakeDiagonal(float m00, float m11);
        static Matrix2 FromAngle(float angle);
        //@}

        /** @name member access */
        //@{
        operator const float* () const;
        operator float* ();
        const float* operator[] (int row) const;
        float* operator[] (int row);
        float operator() (int row, int col) const;
        float& operator() (int row, int col);
        void SetRow(int row, const Vector2& v);
        Vector2 GetRow(int row) const;
        void SetColumn(int col, const Vector2& v);
        Vector2 GetColumn(int col) const;
        void GetColumnMajor(float* col_major) const;
        //@}

        /** @name assignment */
        //@{
        Matrix2& operator= (const Matrix2& mx) = default;
        Matrix2& operator= (Matrix2&& mx) = default;
        //@}

        /** @name comparison */
        //@{
        bool operator== (const Matrix2& mx) const;  ///< 浮點數值比較
        bool operator!= (const Matrix2& mx) const;  ///< 浮點數值比較
        bool operator<  (const Matrix2& mx) const;  ///< 比較陣列記憶體內容
        bool operator<= (const Matrix2& mx) const;   ///< 比較陣列記憶體內容
        bool operator>  (const Matrix2& mx) const;  ///< 比較陣列記憶體內容
        bool operator>= (const Matrix2& mx) const;  ///< 比較陣列記憶體內容
        //@}

        /** @name arithmetic operations */
        //@{
        Matrix2 operator+ (const Matrix2& mx) const;
        Matrix2 operator- (const Matrix2& mx) const;
        Matrix2 operator* (const Matrix2& mx) const;
        Matrix2 operator* (float scalar) const;
        Matrix2 operator/ (float scalar) const;
        Matrix2 operator- () const;
        //@}

        /** @name arithmetic updates */
        //@{
        Matrix2& operator+= (const Matrix2& mx);
        Matrix2& operator-= (const Matrix2& mx);
        Matrix2& operator*= (float scalar);
        Matrix2& operator/= (float scalar);
        //@}

        /** @name matrix times vector */
        //@{
        Vector2 operator* (const Vector2& v) const;  //< M * v
        //@}

        /** @name other operations */
        //@{
        Matrix2 Transpose() const;  // M^T
        Matrix2 Inverse() const;
        Matrix2 Adjoint() const;
        float Determinant() const;
        /** @remark The matrix must be a rotation for these functions to be valid. */
        float ToAngle() const;
        //@}

        /** Eigen Decomposition
        @remark  The matrix must be symmetric.  Factor M = R * D * R^T where \n
         R = [u0|u1] is a rotation matrix with columns u0 and u1 and \n
         D = diag(d0,d1) is a diagonal matrix whose diagonal entries are d0 and
         d1. \n The eigenvector u[i] corresponds to eigenvector d[i].  The
         eigenvalues are ordered as d0 <= d1. */
        EigenDecompose<Matrix2> EigenDecomposition() const;

        /** @name special matrix */
        //@{
        static const Matrix2 ZERO;
        static const Matrix2 IDENTITY;
        //@}

    private:
        // support for none-equal comparisons
        int CompareArrays(const Matrix2& mx) const;
        /** data members */
        union
        {
            struct
            {
                float        m_11, m_12;
                float        m_21, m_22;
            };
            float m_entry[2][2];
        };
    };

    /** scalar * Matrix2 */
    Matrix2 operator* (float scalar, const Matrix2& mx);
};

#endif // !_MATH_MATRIX2_H
