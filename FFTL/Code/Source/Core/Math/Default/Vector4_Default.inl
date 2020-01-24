#ifndef __VECTOR4_DEFAULT_INL__
#define __VECTOR4_DEFAULT_INL__
#else
#error "Already included"
#endif // !__VECTOR4_DEFAULT_INL__

namespace FFTL
{

//
//	vec4 specialization
//




//
//	Functions unique to vec4
//

inline vec4 Cross(const vec4& a, const vec4& b, const vec4& c)
{
	return vec4(
		a.GetY()*(b.GetZ()*c.GetW() - c.GetZ()*b.GetW()) - a.GetZ()*(b.GetY()*c.GetW() - c.GetY()*b.GetW()) + a.GetW()*(b.GetY()*c.GetZ() - c.GetY()*b.GetZ()),
		-a.GetX()*(b.GetZ()*c.GetW() - c.GetZ()*b.GetW()) + a.GetZ()*(b.GetX()*c.GetW() - c.GetX()*b.GetW()) - a.GetW()*(b.GetX()*c.GetZ() - c.GetX()*b.GetZ()),
		a.GetX()*(b.GetY()*c.GetW() - c.GetY()*b.GetW()) - a.GetY()*(b.GetX()*c.GetW() - c.GetX()*b.GetW()) + a.GetW()*(b.GetX()*c.GetY() - c.GetX()*b.GetY()),
		-a.GetX()*(b.GetY()*c.GetZ() - c.GetY()*b.GetZ()) + a.GetY()*(b.GetX()*c.GetZ() - c.GetX()*b.GetZ()) - a.GetZ()*(b.GetX()*c.GetY() - c.GetX()*b.GetY())
		);
}

inline void Transpose4x4(vec4& io_a, vec4& io_b, vec4& io_c, vec4& io_d)
{
	f32 t;

	t = io_a.Ptr()[1];  io_a.Ptr()[1] = io_b.Ptr()[0];  io_b.Ptr()[0] = t;
	t = io_a.Ptr()[2];  io_a.Ptr()[2] = io_c.Ptr()[0];  io_c.Ptr()[0] = t;
	t = io_a.Ptr()[3];  io_a.Ptr()[3] = io_d.Ptr()[0];  io_d.Ptr()[0] = t;
	t = io_b.Ptr()[2];  io_b.Ptr()[2] = io_c.Ptr()[1];  io_c.Ptr()[1] = t;
	t = io_b.Ptr()[3];  io_b.Ptr()[3] = io_d.Ptr()[1];  io_d.Ptr()[1] = t;
	t = io_c.Ptr()[3];  io_c.Ptr()[3] = io_d.Ptr()[2];  io_d.Ptr()[2] = t;
}



} // namespace FFTL

