#ifndef __VECTOR3_DEFAULT_INL__
#define __VECTOR3_DEFAULT_INL__
#else
#error "Already included"
#endif // !__VECTOR3_DEFAULT_INL__


namespace FFTL
{


//
//	vec3 specialization
//

template<>
inline void vecT<3>::Set(const vecT<3>& xy, f32 z)
{
	m_v[0] = xy.GetX();
	m_v[1] = xy.GetY();
	m_v[2] = z;
	m_v[3] = 0;
}


//
//	Functions unique to vec3
//

inline vec3 Cross(const vec3& a, const vec3& b)
{
return vec3(
	a.GetY()*b.GetZ() - a.GetZ()*b.GetY(),
	a.GetZ()*b.GetX() - a.GetX()*b.GetZ(),
	a.GetX()*b.GetY() - a.GetY()*b.GetX()
	);
}

inline void Transpose3x3(vec3& a, vec3& b, vec3& c)
{
	f32 t;

	t = a.Ptr()[1];  a.Ptr()[1] = b.Ptr()[0];  b.Ptr()[0] = t;
	t = a.Ptr()[2];  a.Ptr()[2] = c.Ptr()[0];  c.Ptr()[0] = t;
	t = b.Ptr()[2];  b.Ptr()[2] = c.Ptr()[1];  c.Ptr()[1] = t;
}

FFTL_FORCEINLINE void Transpose3x3SaveW(vec3& a, vec3& b, vec3& c)
{
	Transpose3x3(a, b, c);
}




} // namespace FFTL

