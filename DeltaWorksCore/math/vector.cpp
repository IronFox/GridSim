#include "../global_root.h"
#include "vector.h"
#include "vclasses.h"

namespace DeltaWorks
{
	namespace Math
	{
		template TFloatRange < float > ;
		template TIntRange < int > ;
		template TFloatRange < double > ;
		template TFloatRange < half > ;

		template Rect < float > ;
		template Rect < int > ;
		template Rect < double > ;
		template Rect < half > ;

		template Box < float > ;
		template Box < int > ;
		template Box < double > ;
		template Box < half > ;

		template Quad < float > ;
		template Quad < int > ;
		template Quad < double > ;
		template Quad < half > ;

		template Vec2 < float > ;
		template Vec2 < int > ;
		template Vec2 < double > ;
		template Vec2 < half > ;

		template Vec3 < float > ;
		template Vec3 < int > ;
		template Vec3 < double > ;
		template Vec3 < half > ;

		template Vec4 < float > ;
		template Vec4 < int > ;
		template Vec4 < double > ;
		template Vec4 < half > ;


		template Line < float > ;
		template Line < int > ;
		template Line < double > ;
		template Line < half > ;


		namespace
		{
			struct		VecTester
			{
				/**/	VecTester()
				{
					Vec::assertConsistency<float>();
					Vec::assertConsistency<double>();
					Vec::assertConsistency<half>();
					Vec::assertConsistency<bool>();
				}

			}	tester;

		}


	}
}
