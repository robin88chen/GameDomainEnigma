/*********************************************************************
 * \file   Matrix4.h
 * \brief  
 * 
 * \author Lancelot 'Robin' Chen
 * \date   April 2022
 *********************************************************************/
#ifndef _MATH_MATRIX4_H
#define _MATH_MATRIX4_H

#include "Vector3.h"
#include "Vector4.h"
#include <tuple>

namespace Enigma::MathLib
{
    class Quaternion;
    class Matrix3;
    /** Math Lib Matrix4
    @remarks
    Matrix operations are applied on the left.  For example, given a matrix M
    and a vector V, matrix-times-vector is M*V.  That is, V is treated as a
    column vector.  Similarly, to apply two matrix operations M0
    and M1, in that order, you compute M1*M0 so that the transform of a vector
    is (M1*M0)*V = M1*(M0*V).
    @par
    The generic form M * V which shows the layout of the matrix
    entries is shown below:
    <pre>
    [ m[0][0]  m[0][1]  m[0][2]  m[0][3] ]   {x}
    | m[1][0]  m[1][1]  m[1][2]  m[1][3] | * {y}
    | m[2][0]  m[2][1]  m[2][2]  m[2][3] |   {z}
    [ m[3][0]  m[3][1]  m[3][2]  m[3][3] ]   {1}
    </pre>
    @par
    旋轉與縮放的3x3矩陣在4x4矩陣的左上部份，translate是4x4矩陣的最右邊column.
    */
    class Matrix4
    {
    public:
        /// If bZero is true, create the zero matrix.  Otherwise, create the identity matrix.
        Matrix4(bool isZero = true);
        /// copy constructor
        Matrix4(const Matrix4& mx) = default;
        /// move constructor
        Matrix4(Matrix4&&) = default;
        /// input Mrc is in row r, column c.
        Matrix4(float m00, float m01, float m02, float m03,
            float m10, float m11, float m12, float m13,
            float m20, float m21, float m22, float m23,
            float m30, float m31, float m32, float m33);
        /** Create a matrix from an array of numbers.  The input array is \n
        entry[0..15]={m00,m01,m02,m03,m10,m11,m12,m13,
                                    m20,m21,m22,m23,m30,m31,m32,m33} */
        Matrix4(const float entry[16]);
        /** Create matrices based on vector (Vector3) input.  The Boolean is interpreted as \n
        true: vectors are columns of the matrix (use for pivot rotation) \n
        false: vectors are rows of the matrix (use for space transform)
        @remark rkPos不受bColumns影響，都會擺在最右邊column */
        Matrix4(const Vector3& u, const Vector3& v,
            const Vector3& w, const Vector3& pos, bool isColumns);
        /** Create matrices based on vector (Vector3) input.  The Boolean is interpreted as \n
        true: vectors are columns of the matrix (use for pivot rotation) \n
        false: vectors are rows of the matrix (use for space transform)
        @remark akV[3](position vector)不受bColumns影響，都會擺在最右邊column */
        Matrix4(const Vector3* vec, bool isColumns);
        /** Create from Matrix 3x3 */
        Matrix4(const Matrix3& mx);
        ~Matrix4() = default;

        /** @name create various matrices */
        //@{
        static Matrix4 MakeZero();
        static Matrix4 MakeIdentity();
        //@}

        /** @name member access */
        //@{
        operator const float* () const;
        operator float* ();
        const float* operator[] (int row) const;
        float* operator[] (int row);
        float operator() (int row, int col) const;
        float& operator() (int row, int col);
        void SetRow(int row, const Vector4& vec);
        Vector4 GetRow(int row) const;
        void SetColumn(int col, const Vector4& vec);
        Vector4 GetColumn(int col) const;
        void GetColumnMajor(float* colMajor) const;
        //@}

        /** @name assignment */
        //@{
        Matrix4& operator= (const Matrix4& mx) = default;
        Matrix4& operator= (Matrix4&& mx) = default;
        Matrix4& operator= (const Matrix3& mx);  ///< right column & bottom row will be 0, and m33 is 1
        //@}

        /** @name comparison */
        //@{
        bool operator== (const Matrix4& mx) const; ///< 浮點數值比較
        bool operator!= (const Matrix4& mx) const; ///< 浮點數值比較
        bool operator<  (const Matrix4& mx) const; ///< 比較陣列記憶體內容
        bool operator<= (const Matrix4& mx) const; ///< 比較陣列記憶體內容
        bool operator>  (const Matrix4& mx) const; ///< 比較陣列記憶體內容
        bool operator>= (const Matrix4& mx) const; ///< 比較陣列記憶體內容
        //@}

        /** @name arithmetic operations */
        //@{
        Matrix4 operator+ (const Matrix4& mx) const;
        Matrix4 operator- (const Matrix4& mx) const;
        Matrix4 operator* (const Matrix4& mx) const;
        Matrix4 operator* (float scalar) const;
        Matrix4 operator/ (float scalar) const;
        Matrix4 operator- () const;
        //@}

        /** @name arithmetic updates */
        //@{
        Matrix4& operator+= (const Matrix4& mx);
        Matrix4& operator-= (const Matrix4& mx);
        Matrix4& operator*= (float scalar);
        Matrix4& operator/= (float scalar);
        //@}

        /** @name matrix times vector */
        //@{
        /** 沒有做homogenize, i.e. w未必等於1.0 */
        Vector4 operator* (const Vector4& vec) const;  //< M * v
        /** 傳回homogenize後的vector3 */
        Vector3 operator* (const Vector3& vec) const;
        //@}

        /** @name other operations */
        //@{
        Matrix4 Transpose() const;  // M^T
        Matrix4 Inverse() const;
        Matrix4 Adjoint() const;
        float Determinant() const;
        //@}

        /** @name special matrix */
        //@{
        static const Matrix4 ZERO;
        static const Matrix4 IDENTITY;
        //@}

        /** @name Transform Vectors */
        //@{
        /** transforms the vector, pV (x, y, z, 1), by the matrix, projecting the result back into w=1 */
        Vector3 TransformCoord(const Vector3& vec) const;
        /** transforms the vector, pV (x, y, z, 1), by the matrix */
        Vector3 Transform(const Vector3& vec) const;
        /** transforms the vector  (x, y, z, 0) of the vector, pV, by the matrix */
        Vector3 TransformVector(const Vector3& vec) const;
        /** transforms the vector  (x, y, z, 0) of the vector, pV, by the matrix, and normalize result, return length if needed */
        std::tuple<Vector3, float> TransformVectorNormalized(const Vector3& vec) const;
        //@}

        /** @name Make Transform Matrix */
        //@{
        static Matrix4 MakeTranslateTransform(const float tx, const float ty, const float tz);
        static Matrix4 MakeTranslateTransform(const Vector3& vec);
        static Matrix4 MakeScaleTransform(const float sx, const float sy, const float sz);
        static Matrix4 MakeScaleTransform(const Vector3& vec);
        /** radian > 0 indicates a clockwise rotation in the yz-plane */
        static Matrix4 MakeRotationXTransform(const float radian);
        /** radian > 0 indicates a clockwise rotation in the xz-plane */
        static Matrix4 MakeRotationYTransform(const float radian);
        /** radian > 0 indicates a clockwise rotation in the xy-plane */
        static Matrix4 MakeRotationZTransform(const float radian);
        /** 先對Z軸轉(roll),再對X軸轉(pitch),最後對Y軸轉(yaw) */
        static Matrix4 MakeRotationYawPitchRoll(const float yaw, const float pitch, const float roll);
        /** matrix from SRT
        @remark
        <pre>
        [ Sx*R[0][0]  Sy*R[0][1]  Sz*R[0][2]  Tx ]
        | Sx*R[1][0]  Sy*R[1][1]  Sz*R[1][2]  Ty |
        | Sx*R[2][0]  Sy*R[2][1]  Sz*R[2][2]  Tz |
        [      0           0           0       1 ]
        R is Rotate Matrix from Quaternion
        </pre>
        */
        static Matrix4 FromSRT(const Vector3& scale, const Matrix3& rot, const Vector3& trans);
        static Matrix4 FromSRT(const Vector3& scale, const Quaternion& rot, const Vector3& trans);
        //@}

        /** @name Matrix分解 */
        //@{
        /** 最右邊的column */
        Vector3 UnMatrixTranslate() const;
        /** 假設並沒有shear scale */
        Vector3 UnMatrixScale() const;
        /** 去掉translate & scale取出的rotation (no shear scale) */
        Matrix4 UnMatrixRotation() const;
        /** 分解SRT */
        std::tuple<Vector3, Quaternion, Vector3> UnMatrixSRT() const;
        std::tuple<Vector3, Matrix3, Vector3> UnMatrixSRT_WithRotateMatrix() const;
        //@}

        float GetMaxScale() const;

    public:
        /** public data members */
        union
        {
            struct
            {
                float        m_11, m_12, m_13, m_14;
                float        m_21, m_22, m_23, m_24;
                float               m_31, m_32, m_33, m_34;
                float               m_41, m_42, m_43, m_44;
            };
            float m_entry[4][4];
        };
    private:
        // support for none-equal comparisons
        int CompareArrays(const Matrix4& mx) const;
        // minor matrix determinant
        float MinorDet(const int r0, const int r1, const int r2, const int c0, const int c1, const int c2) const;

    };

    /** scalar * Matrix */
    Matrix4 operator* (float scalar, const Matrix4& mx);

}

#endif // !_MATH_MATRIX4_H
