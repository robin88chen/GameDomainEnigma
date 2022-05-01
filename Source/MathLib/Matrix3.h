#ifndef _MATH_MATRIX3_H
#define _MATH_MATRIX3_H

#include "Vector2.h"
#include "Vector3.h"
#include "AlgebraBasicTypes.h"
#include <tuple>

namespace Enigma::MathLib
{
    class Matrix4;
    /** Math Lib Matrix3
    @remarks
     Matrix operations are applied on the left.  For example, given a matrix M
     and a vector V, matrix-times-vector is M*V.  That is, V is treated as a
     column vector.  Similarly, to apply two matrix operations M0
     and M1, in that order, you compute M1*M0 so that the transform of a vector
     is (M1*M0)*V = M1*(M0*V).
    @par
    The layout of the matrix entries is shown below:
    <pre>
    [ m[0][0]  m[0][1] m[0][2] ]
    [ m[1][0]  m[1][1] m[1][2] ]
    [ m[2][0]  m[2][1] m[2][2] ]
    </pre>
    @par
    Rotation matrices are of the form
    <pre>
       RX =    1       0       0
               0     cos(t) -sin(t)
               0     sin(t)  cos(t)
     where t > 0 indicates a clockwise rotation in the yz-plane
       RY =  cos(t)    0     sin(t)
               0       1       0
            -sin(t)    0     cos(t)
     where t > 0 indicates a clockwise rotation in the zx-plane
       RZ =  cos(t) -sin(t)    0
             sin(t)  cos(t)    0
               0       0       1
     where t > 0 indicates a clockwise rotation in the xy-plane.
    </pre>
    */
    class Matrix3
    {
    public:
        /// If bZero is true, create the zero matrix.  Otherwise, create the identity matrix.
        Matrix3(bool isZero = true);
        /// copy constructor
        Matrix3(const Matrix3& m) = default;
        /// move constructor
        Matrix3(Matrix3&&) = default;
        /// input Mrc is in row r, column c.
        Matrix3(float m00, float m01, float m02,
            float m10, float m11, float m12,
            float m20, float m21, float m22);
        /** Create a matrix from an array of numbers.  The input array is \n
        entry[0..8]={m00,m01,m02,m10,m11,m12,m20,m21,m22} */
        Matrix3(const float entry[9]);
        /// create a diagonal matrix
        Matrix3(float m00, float m11, float m22);
        /// create a rotation matrix (positive angle - clockwise)
        Matrix3(const Vector3& axis, float angle);
        /** Create matrices based on vector input.  The Boolean is interpreted as \n
           true: vectors are columns of the matrix (use for pivot rotation) \n
           false: vectors are rows of the matrix (use for space transform) */
        Matrix3(const Vector3& u, const Vector3& v,
            const Vector3& w, bool isColumns);
        /** Create matrices based on vector input.  The Boolean is interpreted as \n
             true: vectors are columns of the matrix (use for pivot rotation) \n
             false: vectors are rows of the matrix (use for space transform) */
        Matrix3(const Vector3* v, bool isColumns);
        /** Create from Matrix 4x4 */
        Matrix3(const Matrix4& m);
        ~Matrix3() = default;

        /** @name create various matrices */
        //@{
        static Matrix3 MakeZero();
        static Matrix3 MakeIdentity();
        static Matrix3 MakeDiagonal(float m00, float m11, float m22);
        static Matrix3 FromAxisAngle(const Vector3& axis, float angle);
        static Matrix3 FromColumnVectors(Vector3 col[3]);
        //@}

        /** @name member access */
        //@{
        operator const float* () const;
        operator float* ();
        const float* operator[] (int row) const;
        float* operator[] (int row);
        float operator() (int row, int col) const;
        float& operator() (int row, int col);
        void SetRow(int row, const Vector3& v);
        Vector3 GetRow(int row) const;
        void SetColumn(int col, const Vector3& v);
        Vector3 GetColumn(int col) const;
        void GetColumnMajor(float* col_major) const;
        //@}

        /** @name assignment */
        //@{
        Matrix3& operator= (const Matrix3& mx) = default;
        Matrix3& operator= (Matrix3&& mx) = default;
        Matrix3& operator= (const Matrix4& mx);  ///< 左上角的 3x3 matrix
        //@}

        /** @name comparison */
        //@{
        bool operator== (const Matrix3& mx) const;  ///< 浮點數值比較
        bool operator!= (const Matrix3& mx) const;  ///< 浮點數值比較
        bool operator<  (const Matrix3& mx) const;  ///< 比較陣列記憶體內容
        bool operator<= (const Matrix3& mx) const;   ///< 比較陣列記憶體內容
        bool operator>  (const Matrix3& mx) const;  ///< 比較陣列記憶體內容
        bool operator>= (const Matrix3& mx) const;  ///< 比較陣列記憶體內容
        //@}

        /** @name arithmetic operations */
        //@{
        Matrix3 operator+ (const Matrix3& mx) const;
        Matrix3 operator- (const Matrix3& mx) const;
        Matrix3 operator* (const Matrix3& mx) const;
        Matrix3 operator* (float scalar) const;
        Matrix3 operator/ (float scalar) const;
        Matrix3 operator- () const;
        //@}

        /** @name arithmetic updates */
        //@{
        Matrix3& operator+= (const Matrix3& mx);
        Matrix3& operator-= (const Matrix3& mx);
        Matrix3& operator*= (float scalar);
        Matrix3& operator/= (float scalar);
        //@}

        /** @name matrix times vector */
        //@{
        Vector3 operator* (const Vector3& v) const;  //< M * v
        //@}

        /** @name other operations */
        //@{
        Matrix3 Transpose() const;  // M^T
        Matrix3 Inverse() const;
        Matrix3 Adjoint() const;
        float Determinant() const;
        /** @remark The matrix must be a rotation for these functions to be valid. */
        std::tuple<Vector3, float> ToAxisAngle() const;
        //@}

        /** @name Euler Angle Rotation
         @remark
         The matrix must be orthonormal.  The decomposition is yaw*pitch*roll
         where yaw is rotation about the Up vector, pitch is rotation about the
         Right axis, and roll is rotation about the Direction axis.
         @par
          the rotation is roll first, then pitch, and finally yaw. for example,
            FromEulerAnglesXYZ is roll against Z-axis first, then pitch against Y-axis,
            and finally yaw against X-axis.
        */
        //@{
        static Matrix3 FromEulerAnglesXYZ(const EulerAngles& angles);
        static Matrix3 FromEulerAnglesXZY(const EulerAngles& angles);
        static Matrix3 FromEulerAnglesYXZ(const EulerAngles& angles);
        static Matrix3 FromEulerAnglesYZX(const EulerAngles& angles);
        static Matrix3 FromEulerAnglesZXY(const EulerAngles& angles);
        static Matrix3 FromEulerAnglesZYX(const EulerAngles& angles);
        /// @return eular angles and (is unique or not)
        std::tuple<EulerAngles, UniqueEulerAngle> ToEulerAnglesXYZ() const;
        /// @return eular angles and (is unique or not)
        std::tuple<EulerAngles, UniqueEulerAngle> ToEulerAnglesXZY() const;
        /// @return eular angles and (is unique or not)
        std::tuple<EulerAngles, UniqueEulerAngle> ToEulerAnglesYXZ() const;
        /// @return eular angles and (is unique or not)
        std::tuple<EulerAngles, UniqueEulerAngle> ToEulerAnglesYZX() const;
        /// @return eular angles and (is unique or not)
        std::tuple<EulerAngles, UniqueEulerAngle> ToEulerAnglesZXY() const;
        /// @return eular angles and (is unique or not)
        std::tuple<EulerAngles, UniqueEulerAngle> ToEulerAnglesZYX() const;
        //@}

        /** @name 單一角度旋轉矩陣 */
        //@{
        static Matrix3 RotationX(float radian);
        static Matrix3 RotationY(float radian);
        static Matrix3 RotationZ(float radian);
        //@}
        /** @name default yaw, pitch, roll */
        //@{
        /** 先對Z軸轉(roll),再對X軸轉(pitch),最後對Y軸轉(yaw) */
        static Matrix3 RotationYawPitchRoll(float yaw, float pitch, float roll);
        //@}

        /** SLERP (spherical linear interpolation) without quaternions.
            @remark Computes R(t) = R0*(Transpose(R0)*R1)^t.  If Q is a rotation matrix with
         unit-length axis U and angle A, then Q^t is a rotation matrix with
         unit-length axis U and rotation angle t*A. */
        static Matrix3 Slerp(float t, const Matrix3& rot0, const Matrix3& rot1);

        /** TransposeTimes M^T * mx */
        Matrix3 TransposeTimes(const Matrix3& mx) const;
        /** TimesTranspose M * mx^T */
        Matrix3 TimesTranspose(const Matrix3& mx) const;

        /** Eigen Decomposition
        @remark  The matrix must be symmetric.  Factor M = R * D * R^T where \n
        R = [u0|u1|u2] is a rotation matrix with columns u0, u1 and u2 and \n
        D = diag(d0,d1,d2) is a diagonal matrix whose diagonal entries are d0, d1 and
        d2. \n The eigenvector u[i] corresponds to eigenvector d[i].  The
        eigenvalues are ordered as d0 <= d1 <= d2. */
        EigenDecompose<Matrix3> EigenDecomposition() const;

        /** @name Transform Vectors */
        //@{
        /** transforms the vector, pV (x, y, 1), by the matrix, projecting the result back into z=1 */
        Vector2 TransformCoord(const Vector2& v) const;
        /** transforms the vector, pV (x, y, 1), by the matrix */
        Vector2 Transform(const Vector2& v) const;
        /** transforms the vector  (x, y, 0) of the vector, pV, by the matrix */
        Vector2 TransformVector(const Vector2& v) const;
        /** transforms the vector  (x, y, 0) of the vector, pV, by the matrix, and normalize result, return length if needed */
        std::tuple<Vector2, float> TransformVectorNormalized(const Vector2& v) const;
        //@}

        float GetMaxScale() const;

        /** @name special matrix */
        //@{
        static const Matrix3 ZERO;
        static const Matrix3 IDENTITY;
        //@}

    private:
        // Support for eigendecomposition.  The Tridiagonalize function applies
        // a Householder transformation to the matrix.  If that transformation
        // is the identity (the matrix is already tridiagonal), then the return
        // value is 'false'.  Otherwise, the transformation is a reflection and
        // the return value is 'true'.  The QLAlgorithm returns 'true' iff the
        // QL iteration scheme converged.
        bool Tridiagonalize(float diag[3], float subd[2]);
        bool QLAlgorithm(float diag[3], float subd[2]);

        // support for none-equal comparisons
        int CompareArrays(const Matrix3& mx) const;
        /** data members */
        union
        {
            struct
            {
                float        m_11, m_12, m_13;
                float        m_21, m_22, m_23;
                float               m_31, m_32, m_33;
            };
            float m_entry[3][3];
        };
    };

    /** scalar * Matrix */
    Matrix3 operator* (float scalar, const Matrix3& mx);
};

#endif // !_MATH_MATRIX3_H
