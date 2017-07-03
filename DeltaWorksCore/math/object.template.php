<?php
	import('vector');
	
	$strBeforeFunctionType = "inline";
	$strBeforeFunctionName = "__fastcall";
	$strBeforeFunctionBody = "throw()";
	$strVector = 'TVec';
	$strOutFile = 'object_operations';
	$strNamespace = 'Obj';
?>
functions:

	detectOpticalIntersection(const [3] p0{First triangle vertex}, const [3] p1{Second triangle vertex}, const [3] p2{Third triangle vertex}, const [3] b{Ray base position}, const [3] d{Normalized ray direction vector}, &distance{distance In/result distance scalar. This value will be updated if a closer - positive - intersection was detected}) -> bool {true if a positive intersection closer than the specified distance was detected, false otherwise. The specified distance value remains unchanged if the result is false}
		{
			Checks if there is a valid closer intersection of the specified triangle and ray.
			detectOpticalIntersection() calculates the intersection distance (if any) of the specified triangle and ray.
			If such an intersection occurs and it's closer than the specified
			distance then the distance variable will be updated and the result be true.
			A valid intersection occurs if the ray intersects the triangle plane within the triangle's boundaries and with a positive ray factor.
			An intersection with a negative ray factor - thus behind the ray's base position - will be ignored.
			The function detects front- and backface intersections.
		}
		:vector(d0,d1,n);
		:<sub>(p1,p0,d0);
		:<sub>(p2,p0,d1);
		:<cross>(d0,d1,n);
		:float	sub_alpha = :<dot>(n,d);
		if (!sub_alpha)
			return false;
		:vector(dif);
		:<sub>(p0,b,dif);
		:float	alpha = :<dot>(n,dif)/sub_alpha;
		if (alpha < 0 || alpha > distance)
			return false;
		:vector(n0,n1,cross_point);
		:<cross>(n,d0,n0);
		:<cross>(n,d1,n1);
		:<mad>(b,d,alpha,cross_point);
		:<sub>(cross_point,p0,dif);
		:float	beta = :<dot>(n0,dif)/:<dot>(n0,d1),
				gamma = :<dot>(n1,dif)/:<dot>(n1,d0);
		using Math::GetError;
		if (beta > -GetError<:float>() && gamma > -GetError<:float>() && beta + gamma < (:float)1+ GetError<:float>())
		{
			distance = alpha;
			return true;
		}
		return false;
	
	
	tetrahedronVolume,TetrahedronVolume(const [3] p0, const [3] p1, const [3] p2, const [3] p3) -> :float {signed volume}
		{
			Calculates the signed volume of the specified tetrahedron
		}
		:vector(n);
		:float vn;
		:<triangleNormal>(p0,p1,p2,n);
		vn = :<dot>(n);
		using std::fabs;
		using Math::GetError;
		if (fabs(vn) <= GetError<:float>())
			return 0;
		return (:<dot>(p0,n)-:<dot>(p3,n)) / (vn*6);		

	

	detectOpticalQuadIntersection(const [3] p0, const [3] p1, const [3] p2, const [3] b, const [3] d, &distance) -> bool
		{
			Checks if there is a valid closer intersection of the specified quad and ray.
			The unspecified fourth point is implied to be opposite of p0, reflected along the edge between p1 and p2.
			detectOpticalQuadIntersection() calculates the intersection distance (if any) of the specified quad and ray.
			If such an intersection occurs and it's closer than the specified
			distance then the distance variable will be updated and the result be true.
			A valid intersection occurs if the ray intersects the quad plane within the quad's boundaries and with a positive ray factor.
			An intersection with a negative ray factor - thus behind the ray's base position - will be ignored.
			The function detects front- and backface intersections.
		}
		:vector(d0,d1,n);
		:<sub>(p1,p0,d0);
		:<sub>(p2,p0,d1);
		:<cross>(d0,d1,n);
		:float sub_alpha = :<dot>(n,d);
		if (!sub_alpha)
			return false;
		:vector(dif);
		:<sub>(p0,b,dif);
		:float alpha = :<dot>(n,dif) / sub_alpha;
		if (alpha < 0 || alpha > distance)
			return false;
		:vector(n0,n1,cross_point);
		:<cross>(n,d0,n0);
		:<cross>(n,d1,n1);
		:<mad>(b,d,alpha,cross_point);
		:<sub>(cross_point,p0,dif);
		:float	beta = :<dot>(n0,dif) / :<dot>(n0,d1),
				gamma = :<dot>(n1,dif) / :<dot>(n1,d0);
		if (beta > -TypeInfo<:float>::error && gamma > -TypeInfo<:float>::error && beta < (:float)1+ TypeInfo<:float>::error && gamma < (:float)1+ TypeInfo<:float>::error)
		{
			distance = alpha;
			return true;
		}
		return false;
	


	zeroTetrahedronVolume(const [3]p0{First tetrahedron vertex}, const [3] p1{Second tetrahedron vertex}, const [3] p2{Third tetrahedron vertex}) -> :float {signed volume}
		{Calculates the signed volume of the specified tetrahedron with the fourth vertex being the point of origin.<br>_oZeroTetrahedronVolume() calculates the signed volume using the three specified vertices and the point of origin as the fourth vertex.<br>		The sign of the resulting scalar depends on the orientation of the three specified vertices as seen from the point of origin: clockwise=positive, counter clockwise=negative.<br>		This function is quite fast and can be used to quickly determine the volume of a closed hull, simply by calculating the sum of all the triangular tetrahedron volumes.}
		:vector(n);
		:float	vn;
		:<cross>(p0,p1,n);
		vn = :<dot>(n);
		using std::fabs;
		using Math::GetError;
		if (fabs(vn) <= GetError<:float>())
			return 0;
		return :<dot>(p2,n) / (vn*6);

	zeroTriangleSize(const [2] p0, const [2] p1) -> :float
		return (p0:0*p1:1 - p0:1*p1:0)/2;
		
	signedTriangleSize(const [2] p0, const [2] p1, const [2] p2) -> :float
		return	(p0:0*p1:1+p1:0*p2:1+p2:0*p0:1
				-p1:0*p0:1-p2:0*p1:1-p0:0*p2:1)/2;
				
	detEdgeIntersection(const [2] a0, const [2] a1, const [2] b0, const [2] b1, &alpha, &beta) -> bool
		:vector(d0,d1,d2);
		:<sub>(b0,a0,d0);
		:<sub>(a1,a0,d1);
		:<sub>(b1,b0,d2);
		using Math::GetError;
		:float	sub = d2:0 * d1:1 - d2:1 * d1:0,
				error = GetError<:float>();
		using std::fabs;
		if (fabs(sub)<error)
			return false;
		:float 	a = (d0:1*d2:0-d0:0*d2:1)/sub,
				b = (d0:1*d1:0-d0:0*d1:1)/sub;
		alpha = a;
		beta = b;
		return a >= -error && a <= (:float)1+error && b >= -error && b <= (:float)1+error;
		
	detectOpticalSphereIntersection(const [3] center, radius, const [3] b, const [3] d) -> bool
		:vector(delta);
		:<sub>(b,center,delta);
		:float	pa = 1,
				pb = 2*:<dot>(d,delta),
				pc = :<dot>(delta)-M::sqr(radius),
				rs[2];
		BYTE num_rs = Math::solveSqrEquation(pa,pb,pc,rs);
		return num_rs > 0;
		
	detectOpticalSphereIntersection(const [3] center, radius, const [3] b, const [3] d, &distance) -> bool
		:vector(delta);
		:<sub>(b,center,delta);
		:float	pa = 1,
				pb = 2*:<dot>(d,delta),
				pc = :<dot>(delta)-M::sqr(radius),
				rs[2];
		BYTE num_rs = Math::solveSqrEquation(pa,pb,pc,rs);
		if (!num_rs)
			return false;
		:float	alpha = smallestPositiveResult(rs,num_rs);
		if (alpha >= distance)
			return false;
		distance = alpha;
		return true;

	SphereContainsPoint(const [] center, radius, const [] p) -> bool
		return :<quadraticDistance>(center,p) <= M::sqr(radius);
	
	
	detectSphereEdgeIntersection,DetectSphereEdgeIntersection(const [] center, radius, const [] e0, const [] e1) -> bool
		if (:<SphereContainsPoint>(center,radius,e0) || :<SphereContainsPoint>(center,radius,e1))
			return true;
		:vector(d);
		:<sub>(e1,e0,d);
		:vector(delta);
		:<sub>(e0,center,delta);
		:float	pa = :<dot>(d,d),
				pb = 2*:<dot>(d,delta),
				pc = :<dot>(delta)-M::sqr(radius),
				rs[2];
		BYTE num_rs = Math::solveSqrEquation(pa,pb,pc,rs);
		if (!num_rs)
			return false;
		:float	alpha = smallestPositiveResult(rs,num_rs);
		return alpha >= (:float)0 && alpha <= (:float)1;
		
	
	detectSphereEdgeIntersection,DetectSphereEdgeIntersection(const [] center, radius, const [] e0, const [] e1, [2] distances{Intersection distances} ) -> bool {True, if there is an intersection, or all intersection points lie inside the sphere, false otherwise}
		{
			Detects whether or not there is an intersection between the given sphere and edge. Returns the intersection distance factors between e0 (=0) and e1 (=1)
		}
		:vector(d);
		:<sub>(e1,e0,d);
		:vector(delta);
		:<sub>(e0,center,delta);
		:float	pa = :<dot>(d,d),
				pb = 2*:<dot>(d,delta),
				pc = :<dot>(delta)-M::sqr(radius),
				rs[2];
		BYTE num_rs = Math::solveSqrEquation(pa,pb,pc,rs);
		if (!num_rs)
			return false;
		if (num_rs == 1)
		{
			distances:0 = distances:1 = rs[0];
			return rs[0] >= (:float)0 && rs[0] <= (:float)1;
		}
		distances:0 = std::min(rs[0],rs[1]);
		distances:1 = std::max(rs[0],rs[1]);
		return (distances:0 <= (:float)1 && distances:1 >= (:float)0);

	detTriangleRayIntersection(const [3] t0, const [3] t1, const [3] t2, const [3] b, const [3] f {Ray direction vector: any length greater 0}, [3] result) -> bool { true if a point of intersection could be determined, false otherwise. The out array will remain unchanged if the function returns false.}
		{Similar to _oDetTriangleEdgeIntersection(), _oDetTriangleRayIntersection() determines the factors (a, b, c) of the intersection of the specified triangle and ray (if any).<br>Unless the ray and triangle are parallel the resulting factors are written to the specified out array: out[0] = a, out[1] = b, out[2] = c.<br>The solved intersection equation is: x (point of intersection) = t0 + (t1-t0)*a + (t2-t0)*b = b + f*c}
		:vector(d0,d1,n,dif,n0,n1,cross_point);
		:<sub>(t1,t0,d0);
		:<sub>(t2,t0,d1);
		:<cross>(d0,d1,n);
		:<sub>(t0,b,dif);
		:float	sub_alpha = :<dot>(n,f);
		if (!sub_alpha)
			return false;
		:float	alpha = static_cast<:float>(:<dot>(n,dif))/static_cast<:float>(sub_alpha);
		:<cross>(n,d0,n0);
		:<cross>(n,d1,n1);
		:<mad>(b,f,alpha,cross_point);
		:<sub>(cross_point,t0,dif);
		:float	beta = static_cast<:float>(:<dot>(n0,dif))/static_cast<:float>(:<dot>(n0,d1)),
				gamma = static_cast<:float>(:<dot>(n1,dif))/static_cast<:float>(:<dot>(n1,d0));
		result:0 = gamma;
		result:1 = beta;
		result:2 = alpha;
		return true;

	detDeltaTriangleRayIntersection(const [3] t_base, const [3] edge_x, const [3] edge_y, const [3] b, const [3] f {Ray direction vector: any length greater 0}, [3] result) -> bool { true if a point of intersection could be determined, false otherwise. The out array will remain unchanged if the function returns false.}
		{
			@copydoc detTriangleRayIntersectionE

			Variant of detTriangleRayIntersection, using edges delta, rather than absolute positions
		}
		:vector(n,dif,n0,n1,cross_point);
		:<cross>(edge_x,edge_y,n);
		:<sub>(t_base,b,dif);
		:float	sub_alpha = :<dot>(n,f);
		if (!sub_alpha)
			return false;
		:float	alpha = static_cast<:float>(:<dot>(n,dif))/static_cast<:float>(sub_alpha);
		:<cross>(n,edge_x,n0);
		:<cross>(n,edge_y,n1);
		:<mad>(b,f,alpha,cross_point);
		:<sub>(cross_point,t_base,dif);
		:float	beta = static_cast<:float>(:<dot>(n0,dif))/static_cast<:float>(:<dot>(n0,edge_y)),
				gamma = static_cast<:float>(:<dot>(n1,dif))/static_cast<:float>(:<dot>(n1,edge_x));
		result:0 = gamma;
		result:1 = beta;
		result:2 = alpha;
		return true;

		
	triangleNormal(const [3] p0, const [3] p1, const [3] p2, [3] result)
		:vector(v,w);
		:<sub>(p1,p0,v);
		:<sub>(p2,p0,w);
		:<cross>(v,w,result);
		

	quadraticTriangleDistance(const [3] t0,const [3] t1,const [3] t2, const [3] p, [3] normal) -> :float
		:<triangleNormal>(t0,t1,t2,normal);
		:vector(d0,d1,dif,n0,n1,cross_point);
		:<sub>(t1,t0,d0);
		:<sub>(t2,t0,d1);
		:<sub>(p,t0,dif);
		:float	sub_alpha = :<dot>(normal);
		using std::fabs;
		using Math::GetError;
		if (fabs(sub_alpha) <= GetError<:float>())
			return :<quadraticDistance>(t0,p);
		:float	alpha = :<dot>(normal,dif)/sub_alpha;
		:<cross>(normal,d0,n0);
		:<cross>(normal,d1,n1);
		:<mad>(p,normal,alpha,cross_point);
		:<sub>(cross_point,t0,dif);
		:float	beta = :<dot>(n0,dif)/:<dot>(n0,d1),
				gamma = :<dot>(n1,dif)/:<dot>(n1,d0);
		if (beta >= 0)
		{
			if (gamma >= 0)
			{
				if (beta+gamma <= 1)
					return :<quadraticDistance>(cross_point,p);
				:vector(d2,n2);
				:<sub>(t2,t1,d2);
				:float	fc = (:<dot>(cross_point,d2)-:<dot>(t1,d2))/:<dot>(d2);
				if (fc < 0)
					return :<quadraticDistance>(t1,p);
				if (fc > 1)
					return :<quadraticDistance>(t2,p);
				{
					:vector(d0,d1);
					:<sub>(t1,p,d1);
					:<sub>(t2,t1,d0);
					:vector(r);
					:<cross>(d0,d1,r);
					return :<dot>(r)/:<dot>(d0);
				}
			}
			:float	fc = (:<dot>(cross_point,d1)-:<dot>(t0,d1))/:<dot>(d1);
			if (fc < 0)
				return :<quadraticDistance>(t0,p);
			if (fc > 1)
				return :<quadraticDistance>(t2,p);
			{
				:vector(dx);
				:<sub>(t0,p,dx);
				:vector(r);
				:<cross>(d1,dx,r);
				return :<dot>(r)/:<dot>(d1);
			}
		}
		:float	fc = (:<dot>(cross_point,d0)-:<dot>(t0,d0))/:<dot>(d0);
		if (fc < 0)
			return :<quadraticDistance>(t0,p);
		if (fc > 1)
			return :<quadraticDistance>(t1,p);
		{
			:vector(dx);
			:<sub>(t0,p,dx);
			:vector(r);
			:<cross>(dx,d0,r);
			return :<dot>(r)/:<dot>(d0);
		}
		
	triangleDistance(const [3] t0,const [3] t1,const [3] t2, const [3] p, [3] normal) -> :float
		return vsqrt(:<quadraticTriangleDistance>(t0,t1,t2,p,normal));
	
		