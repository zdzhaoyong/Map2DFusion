/*******************************************************************************

  Robot Toolkit ++ (RTK++)

  Copyright (c) 2007-2014 Shuhui Bu <bushuhui@nwpu.edu.cn>
    http://www.adv-ci.com

  ----------------------------------------------------------------------------

    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program. If not, see <http://www.gnu.org/licenses/>.

*******************************************************************************/

///
/// \file quaternions.h
///
/// \brief Quaternions class and utils functions
///
/// \note All functions are defined on right-hand coordinate
///
/// \see http://en.wikipedia.org/wiki/Euler_angles
///      http://en.wikipedia.org/wiki/Rotation_formalisms_in_three_dimensions
///      http://en.wikipedia.org/wiki/Conversion_between_quaternions_and_Euler_angles
///

#ifndef __QUATERNIONS_H__
#define __QUATERNIONS_H__


#include <math.h>


namespace pi {


////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

///
/// \brief Quaternions class
///
template<class T>
class Quaternions
{
public:
    Quaternions() {
        q0 = 1.0;
        q1 = 0.0;
        q2 = 0.0;
        q3 = 0.0;
    }

    Quaternions(T q0_, T q1_, T q2_, T q3_) {
        q0 = q0_;
        q1 = q1_;
        q2 = q2_;
        q3 = q3_;
    }

    ///
    /// \brief Quaternions class construction function
    ///
    /// \param yaw   - yaw (in radian)
    /// \param pitch - pitch (in radian)
    /// \param roll  - roll (in radian)
    ///
    Quaternions(T yaw, T pitch, T roll) {
        fromEuler(yaw, pitch, roll);
    }


    ///
    /// \brief Get each elements
    ///
    /// \param q0_
    /// \param q1_
    /// \param q2_
    /// \param q3_
    ///
    void get(T &q0_, T &q1_, T &q2_, T &q3_) {
        q0_ = q0;
        q1_ = q1;
        q2_ = q2;
        q3_ = q3;
    }

    ///
    /// \brief Set elements
    ///
    /// \param q0_
    /// \param q1_
    /// \param q2_
    /// \param q3_
    ///
    void set(T q0_, T q1_, T q2_, T q3_) {
        q0 = q0_;
        q1 = q1_;
        q2 = q2_;
        q3 = q3_;
    }

    ///
    /// \brief Normalize the Quaternions
    ///
    Quaternions<T>& norm(void) {
        T   recipNorm;

        recipNorm = q0*q0 + q1*q1 + q2*q2 + q3*q3;
        recipNorm = sqrt(recipNorm);

        q0 = q0 / recipNorm;
        q1 = q1 / recipNorm;
        q2 = q2 / recipNorm;
        q3 = q3 / recipNorm;

        return *this;
    }

    ///
    /// \brief Operator r = a*q
    ///
    /// \param q - Quaternions
    ///
    Quaternions<T>& operator *(Quaternions<T> &q) {
        Quaternions<T>  r;

        r.q0 = q0*q.q0 - q1*q.q1 - q2*q.q2 - q3*q.q3;
        r.q1 = q0*q.q1 + q1*q.q0 + q2*q.q3 - q3*q.q2;
        r.q2 = q0*q.q2 - q1*q.q3 + q2*q.q0 + q3*q.q1;
        r.q3 = q0*q.q3 + q1*q.q2 - q2*q.q1 + q3*q.q0;

        return r;
    }

    ///
    /// \brief operator =
    /// \param rhs - right hand obj
    /// \return new Quaternions
    ///
    Quaternions<T>& operator = (const Quaternions<T>& rhs) {
        q0 = rhs.q0;
        q1 = rhs.q1;
        q2 = rhs.q2;
        q3 = rhs.q3;

        return *this;
    }

    ///
    /// \brief Conjugate of a quaternion
    ///
    /// \return conjugate quaternion
    ///
    Quaternions<T>& conj(void) {
        Quaternions<T>  r;

        r.q0 =  q0;
        r.q1 = -q1;
        r.q2 = -q2;
        r.q3 = -q3;

        return r;
    }

    ///
    /// \brief to conjugate
    ///
    /// \return conjugate quaternions
    ///
    Quaternions<T>& toConj(void) {
        q1 = -q1;
        q2 = -q2;
        q3 = -q3;

        return *this;
    }

    ///
    /// \brief Convert quaternions to Euler angles (yaw, pitch, roll)
    ///
    /// \param angs - [yaw, pitch, roll] (in radian)
    ///
    void toEuler(T *angs) {
        T   yaw, pitch, roll;

        toEuler(&yaw, &pitch, &roll);
        angs[0] = yaw;
        angs[1] = pitch;
        angs[2] = roll;
    }

    ///
    /// \brief Convert quaternions to Euler angles (yaw, pitch, roll)
    ///
    /// \param yaw (radian)
    /// \param pitch (radian)
    /// \param roll (radian)
    ///
    void toEuler(T *yaw, T *pitch, T *roll) {

        /*
        T   R11, R21, R31, R32, R33;

        R11 = 2.0*q0*q0 - 1.0 + 2.0*q1*q1;
        R21 = 2.0*(q1*q2 - q0*q3);
        R31 = 2.0*(q1*q3 + q0*q2);
        R32 = 2.0*(q2*q3 - q0*q1);
        R33 = 2.0*q0*q0 - 1.0 + 2.0*q3*q3;

        *roll  = -atan2(R32, R33);
        *pitch = atan(R31) / sqrt(1.0 - R31*R31);
        *yaw   = -atan2(R21, R11);
        */

        /*
        #define CLAMP(x , min , max) \
            ((x) > (max) ? (max) : ((x) < (min) ? (min) : x))

        *pitch = atan2(2.0*(q0*q2 + q1*q3), 1.0 - 2.0*(q1*q1 + q2*q2));
        *roll  = asin(CLAMP(2.0*(q0*q1 - q2*q3), -1.0, 1.0));
        *yaw   = atan2(2.0*(q0*q3 + q1*q2), 1.0 - 2.0*(q1*q1 + q3*q3));
        */

        *yaw   = atan2(2.0*q1*q2 + 2.0*q0*q3,
                      -2.0*q2*q2 - 2.0*q3*q3 + 1.0);
        *pitch = asin(-2.0*q1*q3 + 2.0*q0*q2);
        *roll  = atan2(2.0*q2*q3 + 2.0*q0*q1,
                       -2.0*q1*q1 - 2.0*q2*q2 + 1.0);

        // This should fit for intinsic tait-bryan rotation of xyz-order.
        /*
        *yaw   = atan2(2.0*(q2*q3 + q0*q1), q0*q0 - q1*q1 - q2*q2 + q3*q3);
        *pitch = asin(-2.0*(q1*q3 - q0*q2));
        *roll  = atan2(2.0*(q1*q2 + q0*q3), q0*q0 + q1*q1 - q2*q2 - q3*q3);
        */
    }

    ///
    /// \brief Convert quaternions to Euler angles (Yaw, Pitch, Roll)
    ///
    /// \param yaw (in degree)
    /// \param pitch (in degree)
    /// \param roll (in degree)
    ///
    void toEulerDeg(T *yaw, T *pitch, T *roll) {
        *yaw   = atan2(2.0*q1*q2 + 2.0*q0*q3,
                      -2.0*q2*q2 - 2.0*q3*q3 + 1.0) * 180.0/M_PI;
        *pitch = asin(-2.0*q1*q3 + 2.0*q0*q2) * 180.0/M_PI;
        *roll  = atan2(2.0*q2*q3 + 2.0*q0*q1,
                       -2.0*q1*q1 - 2.0*q2*q2 + 1.0) * 180.0/M_PI;
    }

    ///
    /// \brief Convert Euler angles to quaternions
    ///
    /// \param yaw (in radian)
    /// \param pitch (in radian)
    /// \param roll (in radian)
    ///
    void fromEuler(T yaw, T pitch, T roll) {
        T fCosHRoll  = cos(roll * .5f);
        T fSinHRoll  = sin(roll * .5f);
        T fCosHPitch = cos(pitch * .5f);
        T fSinHPitch = sin(pitch * .5f);
        T fCosHYaw   = cos(yaw * .5f);
        T fSinHYaw   = sin(yaw * .5f);

        // Cartesian coordinate System
        q0 = fCosHRoll * fCosHPitch * fCosHYaw + fSinHRoll * fSinHPitch * fSinHYaw;
        q1 = fSinHRoll * fCosHPitch * fCosHYaw - fCosHRoll * fSinHPitch * fSinHYaw;
        q2 = fCosHRoll * fSinHPitch * fCosHYaw + fSinHRoll * fCosHPitch * fSinHYaw;
        q3 = fCosHRoll * fCosHPitch * fSinHYaw - fSinHRoll * fSinHPitch * fCosHYaw;

        norm();
    }

    ///
    /// \brief Convert Euler angles to quaternions
    ///
    /// \param yaw (in degree)
    /// \param pitch (in degree)
    /// \param roll (in degree)
    ///
    void fromEulerDeg(T yaw, T pitch, T roll) {
        fromEuler(yaw*M_PI/180.0, pitch*M_PI/180.0, roll*M_PI/180.0);
    }

    ///
    /// \brief toRotMat
    ///
    /// \param R - output roatation matrix (column first, C style)
    ///
    /// \see http://en.wikipedia.org/wiki/Rotation_matrix
    ///      http://en.wikipedia.org/wiki/Euler_angles
    ///
    /// \note
    ///     The x, y, and z axis can be obtained by:
    ///         [ux, vx, wx] = [R0, R3, R6] is the x-axis
    ///         [uy, vy, wy] = [R1, R4, R7] is the y-axis
    ///         [uz, vz, wz] = [R2, R5, R8] is the z-axis
    ///
    void toRotMat(T *R) {
        /*
        // this is frome xIMU,
        //  [ux, vx, wx] = [R0, R3, R6] is the x-axis
        //  [uy, vy, wy] = [R1, R4, R7] is the y-axis
        //  [uz, vz, wz] = [R2, R5, R8] is the z-axis

        R[0] = 2.0*q0*q0 - 1.0 + 2.0*q1*q1;
        R[1] = 2.0*(q1*q2 + q0*q3);
        R[2] = 2.0*(q1*q3 - q0*q2);

        R[3] = 2.0*(q1*q2 - q0*q3);
        R[4] = 2.0*q0*q0 - 1.0 + 2.0*q2*q2;
        R[5] = 2.0*(q2*q3 + q0*q1);

        R[6] = 2.0*(q1*q3 + q0*q2);
        R[7] = 2.0*(q2*q3 - q0*q1);
        R[8] = 2.0*q0*q0 - 1.0 + 2.0*q3*q3;
        */

        //to use with normalised quaternion (this from ArdroneLib)
        //  work fine for airplane model rotation!
        R[0] = 1.0 - 2.0*(q2*q2 + q3*q3);
        R[1] = 2.0*(q1*q2 - q0*q3);
        R[2] = 2.0*(q3*q1 + q2*q0);

        R[3] = 2.0*(q1*q2 + q3*q0);
        R[4] = 1.0 - 2.0*(q1*q1 + q3*q3);
        R[5] = 2.0*(q3*q2 - q1*q0);

        R[6] = 2.0*(q3*q1 - q2*q0);
        R[7] = 2.0*(q3*q2 + q1*q0);
        R[8] = 1.0 - 2.0*(q1*q1 + q2*q2);
    }

    ///
    /// \brief quaternions to OpenGL matrix
    /// \param SE3 - OpenGL matrix (row-first order)
    ///
    void toGLMatrix(T *SE3) {
        int     i;

        for(i=0; i<16; i++) SE3[i] = 0;

        SE3[0] = 1.0 - 2.0*(q2*q2 + q3*q3);
        SE3[4] = 2.0*(q1*q2 - q0*q3);
        SE3[8] = 2.0*(q3*q1 + q2*q0);

        SE3[1] = 2.0*(q1*q2 + q3*q0);
        SE3[5] = 1.0 - 2.0*(q1*q1 + q3*q3);
        SE3[9] = 2.0*(q3*q2 - q1*q0);

        SE3[2] = 2.0*(q3*q1 - q2*q0);
        SE3[6] = 2.0*(q3*q2 + q1*q0);
        SE3[10] = 1.0 - 2.0*(q1*q1 + q2*q2);

        SE3[15] = 1;
    }

public:
    T   q0, q1, q2, q3;
};

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

///
/// \brief Yaw, pitch, roll to rotation matrix (column-first order)
///
/// \param R - Rotation matrix (column-first order, C-style)
/// \param m_yaw   - Yaw angle (in degree)
/// \param m_pitch - Pitch angle (in degree)
/// \param m_roll  - Roll angle (in degree)
///
/// \note right hand axis z=cross(x,y)  order: ZYX
///
/// \note Work fine for the following yaw, pitch, roll calculation equations
///         where H = [q0, q1, q2, q3], H = q0 + i*q1 + j*q2 + k*q3
///
///         *yaw   = atan2(2.0*q1*q2 + 2.0*q0*q3,
///                       -2.0*q2*q2 - 2.0*q3*q3 + 1.0);
///         *pitch = asin(-2.0*q1*q3 + 2.0*q0*q2);
///         *roll  = atan2(2.0*q2*q3 + 2.0*q0*q1,
///                       -2.0*q1*q1 - 2.0*q2*q2 + 1.0);
///
template <class T>
void EulerToRotation(T *R, T m_yaw, T m_pitch, T m_roll)
{
    const double	cy = cos(m_yaw);
    const double	sy = sin(m_yaw);
    const double	cp = cos(m_pitch);
    const double	sp = sin(m_pitch);
    const double	cr = cos(m_roll);
    const double	sr = sin(m_roll);

    R[0] = cy*cp;   R[1] = cy*sp*sr-sy*cr;      R[2] = cy*sp*cr+sy*sr;
    R[3] = sy*cp;   R[4] = sy*sp*sr+cy*cr;      R[5] = sy*sp*cr-cy*sr;
    R[6] = -sp;     R[7] = cp*sr;               R[8] = cp*cr;
}

///
/// \brief Yaw, pitch, roll to rotation matrix (column-first order)
///
/// \param R - Rotation matrix (column-first order, C-style)
/// \param m_yaw   - Yaw angle (in radian)
/// \param m_pitch - Pitch angle (in radian)
/// \param m_roll  - Roll angle (in radian)
///
/// \note right hand axis z=cross(x,y)  order: ZYX
///
/// \note Work fine for the following yaw, pitch, roll calculation equations
///         where H = [q0, q1, q2, q3], H = q0 + i*q1 + j*q2 + k*q3
///
///         *yaw   = atan2(2.0*q1*q2 + 2.0*q0*q3,
///                       -2.0*q2*q2 - 2.0*q3*q3 + 1.0);
///         *pitch = asin(-2.0*q1*q3 + 2.0*q0*q2);
///         *roll  = atan2(2.0*q2*q3 + 2.0*q0*q1,
///                       -2.0*q1*q1 - 2.0*q2*q2 + 1.0);
///
template <class T>
void EulerDegToRotation(T *R, T m_yaw, T m_pitch, T m_roll)
{
    const double    deg2rad = M_PI/180.0;

    const double	cy = cos(m_yaw*deg2rad);
    const double	sy = sin(m_yaw*deg2rad);
    const double	cp = cos(m_pitch*deg2rad);
    const double	sp = sin(m_pitch*deg2rad);
    const double	cr = cos(m_roll*deg2rad);
    const double	sr = sin(m_roll*deg2rad);

    R[0] = cy*cp;   R[1] = cy*sp*sr-sy*cr;      R[2] = cy*sp*cr+sy*sr;
    R[3] = sy*cp;   R[4] = sy*sp*sr+cy*cr;      R[5] = sy*sp*cr-cy*sr;
    R[6] = -sp;     R[7] = cp*sr;               R[8] = cp*cr;
}

///
/// \brief Yaw, pitch, roll to OpenGL rotation matrix
///
/// \param R - OpenGL matrix (row-first order)
/// \param m_yaw   - Yaw angle (in radian)
/// \param m_pitch - Pitch angle (in radian)
/// \param m_roll  - Roll angle (in radian)
///
/// \note right hand axis z=cross(x,y)  order: ZYX
///
/// \note Work fine for the following yaw, pitch, roll calculation equations
///         where H = [q0, q1, q2, q3], H = q0 + i*q1 + j*q2 + k*q3
///
///         *yaw   = atan2(2.0*q1*q2 + 2.0*q0*q3,
///                       -2.0*q2*q2 - 2.0*q3*q3 + 1.0);
///         *pitch = asin(-2.0*q1*q3 + 2.0*q0*q2);
///         *roll  = atan2(2.0*q2*q3 + 2.0*q0*q1,
///                       -2.0*q1*q1 - 2.0*q2*q2 + 1.0);
///
template <class T>
void EulerToGLMatrix(T *R, T m_yaw, T m_pitch, T m_roll)
{
    const double	cy = cos(m_yaw);
    const double	sy = sin(m_yaw);
    const double	cp = cos(m_pitch);
    const double	sp = sin(m_pitch);
    const double	cr = cos(m_roll);
    const double	sr = sin(m_roll);

    for(int i=0; i<16; i++) R[i] = 0.0;

    R[0] = cy*cp;   R[4] = cy*sp*sr-sy*cr;      R[8] = cy*sp*cr+sy*sr;
    R[1] = sy*cp;   R[5] = sy*sp*sr+cy*cr;      R[9] = sy*sp*cr-cy*sr;
    R[2] = -sp;     R[6] = cp*sr;               R[10] = cp*cr;

    R[15] = 1.0;
}

///
/// \brief Yaw, pitch, roll to OpenGL rotation matrix
///
/// \param R - OpenGL matrix (row-first order)
/// \param m_yaw   - Yaw angle (in degree)
/// \param m_pitch - Pitch angle (in degree)
/// \param m_roll  - Roll angle (in degree)
///
/// \note right hand axis z=cross(x,y)  order: ZYX
///
/// \note Work fine for the following yaw, pitch, roll calculation equations
///         where H = [q0, q1, q2, q3], H = q0 + i*q1 + j*q2 + k*q3
///
///         *yaw   = atan2(2.0*q1*q2 + 2.0*q0*q3,
///                       -2.0*q2*q2 - 2.0*q3*q3 + 1.0);
///         *pitch = asin(-2.0*q1*q3 + 2.0*q0*q2);
///         *roll  = atan2(2.0*q2*q3 + 2.0*q0*q1,
///                       -2.0*q1*q1 - 2.0*q2*q2 + 1.0);
///
template <class T>
void EulerDegToGLMatrix(T *R, T m_yaw, T m_pitch, T m_roll)
{
    const double    deg2rad = M_PI/180.0;

    const double	cy = cos(m_yaw*deg2rad);
    const double	sy = sin(m_yaw*deg2rad);
    const double	cp = cos(m_pitch*deg2rad);
    const double	sp = sin(m_pitch*deg2rad);
    const double	cr = cos(m_roll*deg2rad);
    const double	sr = sin(m_roll*deg2rad);

    for(int i=0; i<16; i++) R[i] = 0.0;

    R[0] = cy*cp;   R[4] = cy*sp*sr-sy*cr;      R[8] = cy*sp*cr+sy*sr;
    R[1] = sy*cp;   R[5] = sy*sp*sr+cy*cr;      R[9] = sy*sp*cr-cy*sr;
    R[2] = -sp;     R[6] = cp*sr;               R[10] = cp*cr;

    R[15] = 1.0;
}

} // end of namespace pi

#endif // end of __QUATERNIONS_H__
