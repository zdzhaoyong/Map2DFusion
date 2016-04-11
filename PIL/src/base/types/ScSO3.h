#ifndef SCSO3_H
#define SCSO3_H

#include "SO3.h"

namespace pi {
/**
 Class ScSO3 :Scaled SO3

 */
template <class Precision=double>
class ScSO3
{
public:
    ScSO3():x(0),y(0),z(0),w(0){}

    ScSO3(const Precision& X,const Precision& Y,const Precision& Z,const Precision& W)
        :x(X),y(Y),z(Z),w(W) { }

    /// Construct from SO3 and scale paraments
    ScSO3(const SO3<Precision>& rotation,const Precision& scale=1.0)
        :x(rotation.x*scale),y(rotation.y*scale),z(rotation.z*scale),
          w(rotation.w*scale){}

    /// Construct from a rotation matrix.
    ScSO3(const Precision* M)
    {
        fromMatrix(M);
    }

    /// Coversion from different precision
    template<typename Scalar>
    operator ScSO3<Scalar>()const
    {
        return ScSO3<Scalar>(x,y,z,w);
    }

    inline Precision get_scale()const
    {
        return sqrt(x*x+y*y+z*z+w*w);
    }

    operator SO3<Precision>()
    {
        Precision scaleInv=1.0/get_scale();
        return SO3<Precision>(x*scaleInv,y*scaleInv,z*scaleInv,w*scaleInv);
    }

    Array_<Precision,4> ln()const
    {
        const Precision scale=get_scale();
        const Precision Sinv=1./scale;
        const SO3<Precision> so3(x*Sinv,y*Sinv,z*Sinv,w*Sinv);

        Array_<Precision,4> result;
        *(Point3_<Precision>*)&result=(so3.ln());
        result.data[3]=log(scale);

        return result;
    }

    template<typename Scalar>
    static ScSO3<Precision> exp(const Array_<Scalar,4>& r)
    {
        return ScSO3<Precision>(SO3<Precision>::exp(*((Point3_<Precision>*)&r)),
                                ::exp(r.data[3]));
    }

    inline bool fromMatrix(const Precision* m)
    {
        //this is unsafe but fast,please ensure that the m is absolutely right
        Precision scale=sqrt(m[0]*m[0]+m[1]*m[1]+m[2]*m[2]);
        Precision Sinv=1./scale;

        w=0.5*sqrt((m[0]+m[4]+m[8]+scale)*scale);
        Precision oneover4ws=0.25*scale/w;
        x=(m[7]-m[5])*oneover4ws;
        y=(m[2]-m[6])*oneover4ws;
        z=(m[3]-m[1])*oneover4ws;

        if(w<0){x*=-1;y*=-1;z*=-1;w*=-1;}
        return true;
    }

    template <typename T>
    inline bool fromMatrixUnsafe(T& matrix)
    {
        if(sizeof(matrix)==9*sizeof(Precision))
            return fromMatrix((Precision*)&matrix);
        else
        {
            std::cerr<<"SO3::fromMatrixUnsafe: Wrong precision detected!";
            return false;
        }
    }

    /// return the matrix M
    inline bool getMatrix(Precision* m)const
    {
        const Precision scale=get_scale();
        const Precision Sinv=1./scale;
        const SO3<Precision> so3(x*Sinv,y*Sinv,z*Sinv,w*Sinv);
        so3.getMatrix(m);
        for(int i=0;i<9;i++)
            (m[i])*=scale;
        return true;
    }

    template <typename T>
    inline bool getMatrixUnsafe(T& matrix)
    {
        if(sizeof(matrix)==9*sizeof(Precision))
            return getMatrix((Precision*)&matrix);
        else
        {
            std::cerr<<"GetMatrixUnsafe detected wrong demission!\n";
            return false;
        }
    }

    ScSO3 operator* (const ScSO3& rq) const
    {
        // the constructor takes its arguments as (x, y, z, w)
        return ScSO3( w * rq.x + x * rq.w + y * rq.z - z * rq.y,
                      w * rq.y + y * rq.w + z * rq.x - x * rq.z,
                      w * rq.z + z * rq.w + x * rq.y - y * rq.x,
                      w * rq.w - x * rq.x - y * rq.y - z * rq.z);
    }

    // Multiplying a quaternion q with a vector v applies the q-rotation to v
    Point3_<Precision> operator* (const Point3_<Precision>& p) const
    {
        const Precision scale=get_scale();
        const Precision Sinv=1./scale;
        const SO3<Precision> so3(x*Sinv,y*Sinv,z*Sinv,w*Sinv);
        return scale*(so3*p);
    }

    // We need to get the inverse of a quaternion to properly apply a quaternion-rotation to a vector
    // The conjugate of a quaternion is the same as the inverse, as long as the quaternion is unit-length
    ScSO3 inv() const
    {
        return ScSO3(-x, -y, -z, w);
    }

    void getValue(Precision& X,Precision& Y,Precision& Z,Precision& W) const
    {
        X=x;Y=y;Z=z;W=w;
    }

    operator std::string()const
    {
        std::string os=to_str(x)+" "+to_str(y)+" "+to_str(z)+" "+to_str(w);
        return os;
    }

    operator std::ostream()const
    {
        std::ostream os;
        os<<x<<" "<<y<<" "<<z<<" "<<w;
        return os;
    }

protected:
    Precision x,y,z,w;
};

typedef ScSO3<double> ScSO3d;
typedef ScSO3<float> ScSO3f;

/// Write an ScSO3 to a stream
/// @relates ScSO3
template <typename Precision>
inline std::ostream& operator << (std::ostream& os,const ScSO3<Precision>& so3)
{
#if 0//HAS_TOON
    os<<so3.getMatrix();
    return os;
#else
    Array_<Precision,4> my_ln=so3.ln();
    os<<so3.get_scale()<<" * "<<*(Point3_<Precision>*)&my_ln;
    return os;
#endif
}

} //end of namespace


#endif // SCSO3_H
