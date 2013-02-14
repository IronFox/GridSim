#include "../global_root.h"
#include "vector.h"
#include "vclasses.h"

namespace Math
{
	template TRange<float>;
	template TRange<int>;
	template TRange<double>;

	template Rect<float>;
	template Rect<int>;
	template Rect<double>;

	template Box<float>;
	template Box<int>;
	template Box<double>;

	template Quad<float>;
	template Quad<int>;
	template Quad<double>;

	template Vec2<float>;
	template Vec2<int>;
	template Vec2<double>;

	template Vec3<float>;
	template Vec3<int>;
	template Vec3<double>;

	template Vec4<float>;
	template Vec4<int>;
	template Vec4<double>;

	template Line<float>;
	template Line<int>;
	template Line<double>;
}