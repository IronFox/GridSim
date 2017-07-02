<?php
	$strBeforeFunctionType = "inline";
	$strBeforeFunctionName = "__fastcall";
	$strBeforeFunctionBody = "throw()";
	$strNamespace = 'Vec';
	$strVector = 'TVec';
	$strOutFile = 'vector_operations';
?>
functions:

	addValue,addVal(const[] v {Vector to add to}, value {Value to add},[] result)
		{Adds the specified value to each element of <paramref>v</paramref> and stores the result in the respective element of <paramref>result</paramref>}
		:iterate
			result:i = v:i + value;
	
	addValue,addVal([]v {Vector to add to}, value {Value to add})
		{Adds the specified value to each element of <paramref>v</paramref> and stores the result back in the respective element of <paramref>v</paramref>}
		:iterate
			v:i += value;
	
	
	add(const[] u, const[] v, const[] w, [] result)
		:iterate
			result:i = u:i + v:i + w:i;
	
	add(const[] v, const[] w, [] result)
		:iterate
			result:i = v:i+w:i;
	
	add([] v, const[] w)
		:iterate
			v:i += w:i;
	
	subtract,sub(const[] v, const[] w, [] result)
		:iterate
			result:i = v:i - w:i;
	
	subtract,sub([] v, const[] w)
		:iterate
			v:i -= w:i;
			
	subtractValue,subVal(const[] v {Vector to subtract from}, value {Value to subtract},[] result)
		{Subtracts the specified value from each element of <paramref>v</paramref> and stores the result in the respective element of <paramref>result</paramref>}
		:iterate
			result:i = v:i - value;
	
	subtractValue,subVal([]v {Vector to subtract from}, value {Value to subtract})
		{Subtracts the specified value from each element of <paramref>v</paramref> and stores the result back in the respective element of <paramref>v</paramref>}
		:iterate
			v:i -= value;			
			
			
	multiply,mult,mul(const[] v, factor, [] result)
		:iterate
			result:i = v:i * factor;
	
	multiply,mult,mul([] v, factor)
		:iterate
			v:i *= factor;
	
	divide,div(const[] v, value, [] result)
		:iterate
			result:i = v:i / value;
	
	divide,div([] v, value)
		:iterate
			v:i /= value;
	
	resolve(const[] v, const[] w, [] result)
		:iterate
			result:i = v:i / w:i;
			
	stretch(const[] v,const[] w, []result)
		{Writes the component-wise product of v and w to result}
		:iterate
			result:i = v:i * w:i;
	
	interpolate(const []v, const [] w, f, [] result)
		:float i_ = :float(1)-f;
		:iterate [i_, v, w, f, result]
			result:i = v:i * i_ + w:i * f;
				
	center(const[] v, const[] w, []result)
		{Writes the arithmetic center of v and w to result}
		:iterate
			result:i = (v:i + w:i)/:float(2);
	
	center(const[] u, const[] v, const[] w, []result)
		{Writes the arithmetic center of u, v, and w to result}
		:iterate
			result:i = (u:i + v:i + w:i)/:float(3);
	
	multAdd,mad([] current, const[] vector, scalar)
		{Adds <paramref>vector</paramref> * <paramref>scalar</paramref> to <paramref>current</paramref>}
		:iterate
			current:i += vector:i * scalar;
			
	multAdd,mad(const[] base, const[] vector, scalar, [] result)
		{Writes <paramref>base</paramref> + <paramref>vector</paramref> * <paramref>scalar</paramref> to <paramref>result</paramref>}
		:iterate
			result:i = base:i + vector:i * scalar;
	
	dot(const[] v, const[] w) -> :float
		{Calculates the dot product of <paramref>v</paramref> and <paramref>w</paramref>}
		return :sum( v:i*w:i );
	
	dot(const[] v) -> :float
		{Calculates the dot product <paramref>v</paramref> * <paramref>v</paramref>}
		return :sum( v:i*v:i );
	
	sum(const[] v) -> :float
		return :sum( v:i );
	
	zero(const[] v) -> bool
		return :and( !v:i );
		
	length(const[] v{Vector to determine the length of}) -> :float	{Length of <paramref>v</paramref>}
		{Determines the length of <paramref>v</paramref>}
		return sqrt(:<dot>(v));

	
	quadraticDistance(const[] v, const[] w) -> :float
		return :sum( Math::sqr(v:i - w:i) );
		
	distance(const[] v, const[] w) -> :float
		{Calculates the distance between <paramref>v0</paramref> and <paramref>v1</paramref>}
		return sqrt( :<quadraticDistance>(v,w) );

		


	reflectN(const[] base{The base point of the reflecting surface}, const[] normal {The normal of the reflecting surface. Must be normalized}, const[] p{The point being reflected}, [] out {[out] Result})
		{Calculates the reflection of a point across the plane, specified by base and (normalized) normal}
		:float fc = 2*( :<dot>( base,normal ) - :<dot>(p,normal) );
		:<mad>( p,normal,fc,out );

	reflectN(const[] base{The base point of the reflecting surface}, const[] normal {The normal of the reflecting surface. Must be normalized}, [] p{[inout] The point being reflected})
		{Calculates the reflection of a point across the plane, specified by base and (normalized) normal}
		:float fc = 2*( :<dot>( base,normal ) - :<dot>(p,normal) );
		:<mad>( p,normal,fc);

	reflect(const[] base{The base point of the reflecting surface}, const[] normal {The normal of the reflecting surface. May be of any length > 0}, const[] p {The point being reflected}, [] out {[out] Result})
		{Calculates the reflection of a point across the plane, specified by base and normal}
		:float fc = 2*( :<dot>( base,normal ) - :<dot>(p,normal) ) / :<dot>( normal );
		:<mad>( p,normal,fc,out );

	reflect(const[] base{The base point of the reflecting surface}, const[] normal {The normal of the reflecting surface. May be of any length > 0}, [] p {The point being reflected})
		{Calculates the reflection of a point across the plane, specified by base and normal}
		:float fc = 2*( :<dot>( base,normal ) - :<dot>(p,normal) ) / :<dot>( normal );
		:<mad>( p,normal,fc );

	reflectVectorN(const[] normal{The normal of the reflecting surface. Must be normalized}, const[] v{The vector being reflected. May be of any length.}, [] out {Reflection result})
		{Calculates the reflection of a direction vector across the plane, specified by its (normalized) normal}
		:float fc = 2*( - :<dot>(v,normal) );
		:<mad>( v,normal,fc,out );

	reflectVectorN(const[] normal{The normal of the reflecting surface. Must be normalized}, [] v{[inout] The vector being reflected. May be of any length.})
		{Calculates the reflection of a direction vector across the plane, specified by its (normalized) normal}
		:float fc = 2*( - :<dot>(v,normal) );
		:<mad>( v,normal,fc);

	reflectVector(const[] normal{The normal of the reflecting surface. May be of any length > 0}, const[] v{The vector being reflected. May be of any length.}, [] out {[out] Reflection result})
		{Calculates the reflection of a direction vector across the plane, specified by its (normalized) normal}
		:float fc = 2*( - :<dot>(v,normal) )/:<dot>( normal );
		:<mad>( v,normal,fc,out );

	reflectVector(const[] normal{The normal of the reflecting surface. May be of any length > 0}, [] v{[inout] The vector being reflected. May be of any length.})
		{Calculates the reflection of a direction vector across the plane, specified by its (normalized) normal}
		:float fc = 2*( - :<dot>(v,normal) )/:<dot>( normal );
		:<mad>( v,normal,fc );
		



	resolveUCBS(const[] p0, const[] p1, const[] p2, const[] p3, t, [] result)
		:float	//i = 1-t,
				f0 = (-t*t*t+3*t*t-3*t+1)/6,
				f1 = (3*t*t*t-6*t*t+4)/6,
				f2 = (-3*t*t*t+3*t*t+3*t+1)/6,
				f3 = (t*t*t)/6;
		:iterate[f0, f1, f2, f3, p0, p1, p2, p3, result]
			result:i = p0:i*f0 + p1:i*f1 + p2:i*f2 + p3:i*f3;
		

	resolveUCBSaxis(const[] p0, const[] p1, const[] p2, const[] p3, t, [] result)
		:float	//i = 1-t,
				f0 = (t*t-2*t+1)/2,
				f1 = (-2*t*t+2*t+1)/2,
				f2 = (t*t)/2;
		:iterate[f0,f1,f2,p0,p1,p2,p3,result]
			result:i = (p1:i-p0:i)*f0 + (p2:i-p1:i)*f1 + (p3:i-p2:i)*f2;


	resolveBezierCurvePoint(const[] p0, const[] p1, const[] p2, const[] p3, t, [] result)
		:float	i = 1-t,
				f3 = t*t*t,
				f2 = t*t*i*3,
				f1 = t*i*i*3,
				f0 = i*i*i;
		:iterate[f0,f1,f2,f3,p0,p1,p2,p3,result]
			result:i = p0:i*f0 + p1:i*f1 + p2:i*f2 + p3:i*f3;

			
	resolveBezierCurveAxis(const[] p0, const[] p1, const[] p2, const[] p3, t, [] result)
		:float	i = 1-t,
				f2 = t*t,
				f1 = t*i*2,
				f0 = i*i;
		:iterate[f0,f1,f2,p0,p1,p2,p3,result]
			result:i = (p1:i - p0:i)*f0 + (p2:i - p1:i)*f1 + (p3:i - p2:i)*f2;
		:<normalize0>(result);

	SplitBezierCurveAxis(const[] p0, const[] p1, const[] p2, const[] p3, t, [] out0, [] out1, [] out2, [] out3)
		:iterate[t,p0,p1,p2,p3,out0,out1,out2,out3]
			{
				:float x01 = (p1:i - p0:i)*t + p0:i;
				:float x12 = (p2:i - p1:i)*t + p1:i;
				:float x23 = (p3:i - p2:i)*t + p2:i;
				:float x012 = (x12 - x01) * t + x01;
				:float x123 = (x23 - x12) * t + x12;
				:float x0123 = (x123 - x012) * t + x012;
				out0:i = p0:i;
				out1:i = x01;
				out2:i = x012;
				out3:i = x0123;
			}
	
	compare<16>(const[] v0, const[] v1, tolerance) -> char {-1 if <paramref>v0</paramref> is lexicographically less than <paramref>v1</paramref>, 0 if they are identical, +1 if <paramref>v0</paramref> is lexicographically greater than <paramref>v1</paramref>}
		{Compares two vectors for lexicographic order}
		:iterate
			{
				:float delta = v0:i - v1:i;
				if (delta < -tolerance)
					return -1;
				if (delta > tolerance)
					return 1;
			}
		return 0;

	compare<16>(const[] v0, const[] v1) -> char {-1 if <paramref>v0</paramref> is lexicographically less than <paramref>v1</paramref>, 0 if they are identical, +1 if <paramref>v0</paramref> is lexicographically greater than <paramref>v1</paramref>}
		{Compares two vectors for lexicographic order}
		return :<compare>(v0,v1,TypeInfo<:float>::error);

	equal(const[] v, const[] w) -> bool
		return :and( v:i == w:i );
	
	
	similar(const[] v, const[] w, tolerance {maximum distance between <paramref>v</paramref> and <paramref>w</paramref> to still be considered similar}) -> bool {true, if <paramref>v</paramref> and <paramref>w</paramref> are similar in accordance to the specified distance, false otherwise}
		{Determines whether <paramref>v</paramref> and <paramref>w</paramref> are similar (within the specified tolerance proximity)}
		return :sum( Math::sqr(v:i - w:i) ) < tolerance*tolerance;
		
	similar(const[] v, const[] w) -> bool {true, if <paramref>v</paramref> and <paramref>w</paramref> are similar, false otherwise}
		{Determines whether <paramref>v</paramref> and <paramref>w</paramref> are similar (within a type-specific tolerance proximity)}
		return :<similar>(v,w,TypeInfo<:float>::error);
		
	
	maxAxisDistance(const[] v, const[] w) -> :float {Greatest axial distance between v and w}
		{Determines the greatest difference along any axis between <paramref>v</paramref> and <paramref>w</paramref>}
		using std::fabs;
		return :max( fabs(v:i-w:i) );
	
	planePointDistanceN(const[] base {plane base point}, const[] normal {plane normal; must be normalized}, const[] p {point to determine the distance of}) -> :float {absolute distance}
		{Determines the absolute distance between a plane and a point. The specified normal is assumed to be of length 1}
		return (:<dot>(p,normal) - :<dot>(base,normal));

	
	planePointDistance(const[] base {plane base point}, const[] normal {plane normal; may be of any length greater 0}, const[] p {point to determine the distance of})->:float {absolute distance}
		{Determines the absolute distance between a plane and a point}
		return (:<dot>(p,normal) - :<dot>(base,normal)) / :<length>(normal);
	
	

	
	scale(const[] center, factor, [] current)
		{Scales the distance between <paramref>center</paramref> and <paramref>current</paramref> by the specified <paramref>factor</paramref>}
		:iterate
			current:i = center:i+(current:i-center:i)*factor;
		
	scaleAbsolute(const[] center, distance, [] current)
		{Scales the distance between <paramref>center</paramref> and <paramref>current</paramref> to the specified absolute <paramref>distance</paramref>. Behavior is undefined if <paramref>center</paramref> and <paramref>current</paramref> are identical.}
		:float len = :<distance>(current,center);
		:<scale>(center, distance/len, current);
		
	scaleAbsolute0(const[] center, distance, [] current)
		{Scales the distance between <paramref>center</paramref> and <paramref>current</paramref> to the specified absolute <paramref>distance</paramref>. Also checks if <paramref>center</paramref> and <paramref>current</paramref> are identical and reacts appropriately.}
		:float len = :<distance>(current,center);
		if (len > TypeInfo<:float>::error)
		{
			:<scale>(center, distance/len, current);
		}
		else
			current[0] = len;
	

	normalize([] vector)
		:float len = sqrt(:<dot>(vector));
		:iterate [vector, len]
			vector:i /= len;

	normalize0([] vector)
		:float len = :<dot>(vector);
		if (isnan(len) || len == 0)
		{
			vector:0 = 1;
			:iterate+
				vector:i = 0;
			return;
		}
		len = sqrt(len);
		:iterate [vector, len]
			vector:i /= len;
		

	abs([] v)
		using std::fabs;
		:iterate
			v:i = fabs(v:i);
	
	setLen([] current {vector to normalize, must not be of length 0}, length{new length})
		{Updates the length of <paramref>current</paramref> so that its length matches <paramref>length</paramref>. <paramref>current</paramref> may be of any length greater 0}
		:float len = sqrt( :<dot>(current) );
		:float fc = :float(length)/len;
		:<multiply>(current, fc);
		
	setLen0([] current {vector to normalize, must not be of length 0}, length{new length})
		{Updates the length of <paramref>current</paramref> so that its length matches <paramref>length</paramref>. <paramref>current</paramref> may be of any length greater 0}
		:float len = sqrt( :<dot>(current) );
		using std::fabs;
		if (fabs(len) > TypeInfo<:float>::error)
		{
			:float fc = :float(length)/len;
			:<multiply>(current, fc);
		}
		else
			current:0 = len;
		
	
	
	angleOne(x,y) -> :float {Absolute angle of @v in the range [0,1]. The result is 0 for (0,0)}
		{Determines the rotational angle of <paramref>v</paramref>. The returned angle is measured counter-clockwise starting at 0 for points along the positive x axis}
		if (!x && !y)
			return 0;
		:float rs = (:float)vatan2(y,x)/(2*M_PI);
		if (rs < 0)
			rs = ((:float)1)+rs;
		return rs;

	
	angleOne(const[2] v) -> :float {absolute angle of @v in the range [0,1]}
		{Determines the rotational angle of <paramref>v</paramref>}
		return :<angleOne>(v:0,v:1);
	
	angle360(x,y) -> :float {absolute angle of @v in the range [0,360]}
		{Determines the rotational angle of <paramref>v</paramref>}
		return :<angleOne>(x,y) *360;

	
	angle360(const[2] v) -> :float {absolute angle of @v in the range [0,360]}
		{Determines the rotational angle of <paramref>v</paramref>}
		return :<angleOne>(v) *360;
	
	angle2PI(x,y) -> :float {absolute angle of @v in the range [0,2*PI]}
		{Determines the rotational angle of <paramref>v</paramref>. The returned angle is measured counter-clockwise starting at 0 for points along the positive x axis}
		if (!x && !y)
			return 0;
		:float rs = (:float)vatan2(y,x);
		if (rs < 0)
			rs = ((:float)M_PI*2)+rs;
		return rs;
		
	angle2PI(const[2] v{vector to determine the angle of; not required to be normalized}) -> :float {absolute angle of @v in the range [0,2*PI]}
		{Determines the rotational angle of <paramref>v</paramref>}
		return :<angle2PI>(v:0,v:1);
	
	
	angle(const[] v{first vector; not required to be normalized, but must be non-0}, const[] w{second vector; not required to be normalized, but must be non-0}) -> :float {angle in the range [0,180]}
		{Determines the angle between <paramref>v</paramref> and <paramref>w</paramref>}
		return acos(Math::clamp(:<dot>(v,w)/(:<length>(v)*:<length>(w)),-1,1))*180/M_PI;
	
	
	isNAN(const[] v) -> bool
		{Tests if at least one component of <paramref>v</paramref> is NAN (Not A Number). Not applicable to integer types}
		return :or( isnan(v:i) );
		
	intensity(const[] v {first vector; not required to be normalized, but must be non-0}, const[] w {second vector; not required to be normalized, but must be non-0}) -> :float
		{Determines the light intensity (the cos of the angle) between <paramref>v</paramref> and <paramref>w</paramref>}
		return :<dot>(v,w)/(:<length>(v)*:<length>(w));
	
	set<16>([] v, value)
		{Sets all components of <paramref>v</paramref> to <paramref>value</paramref>}
		:iterate
			v:i = value;
	
	clamp([] v, min, max)
		{Clamps all components of <paramref>v</paramref> to the range [<paramref>min</paramref>, <paramref>max</paramref> ]}
		:iterate
			v:i = M::clamped(v:i,min,max);
	
	clear([] v)
		{Sets all components of <paramref>v</paramref> to 0}
		:iterate
			v:i = 0;
	
	min(const[] v) -> :float
		{Determines the minimum coordinate value of <paramref>v</paramref>}
		return :min( v:i );

	max(const[] v) -> :float
		{Determines the maximum coordinate value of <paramref>v</paramref>}
		return :max( v:i );
	
	max(const[] v, const[] w, [] result)
		{Writes the component-wise maximum of <paramref>v</paramref> and <paramref>w</paramref> to <paramref>result</paramref>}
		:iterate
			result:i = Math::Max( v:i, w:i);

	min(const[] v, const[] w, [] result)
		{Writes the component-wise minimum of <paramref>v</paramref> and <paramref>w</paramref> to <paramref>result</paramref>}
		:iterate
			result:i = Math::Min( v:i, w:i);
			
	oneLess(const[] v, const[] w) -> bool
		{Tests if at least one element of <paramref>v</paramref> is less than the respective element of <paramref>w</paramref>}
		return :or( v:i < w:i );
	
	oneGreater(const[] v, const[] w) -> bool
		{Tests if at least one element of <paramref>v</paramref> is greater than the respective element of <paramref>w</paramref>}
		return :or( v:i > w:i );
	
	allLess(const[] v, const[] w) -> bool
		{Tests if all elements of <paramref>v</paramref> are less than the respective element of <paramref>w</paramref>}
		return :and( v:i < w:i );
	
	allGreater(const[] v, const[] w) -> bool
		{Tests if all elements of <paramref>v</paramref> are greater than the respective element of <paramref>w</paramref>}
		return :and( v:i > w:i );
	
	
			
	vectorSort([] v, [] w)
		{Swaps an element of <paramref>v</paramref> with the respective element of <paramref>w</paramref> if it is greater. Once done each element of <paramref>v</paramref> is less or equal than the respective one of <paramref>w</paramref>}
		:iterate
			if (w:i<v:i)
				swp(v:i,w:i);
				
	sphereCoords(planar_angle, height_angle, radius, [3] result)
		result:1 = sin(height_angle*M_PI/180)*radius;
		:float	r2 = cos(height_angle*M_PI/180)*radius;
		result:0 = cos(planar_angle*M_PI/180)*r2;
		result:2 = sin(planar_angle*M_PI/180)*r2;
	
	sphereCoordsRad(planar_angle, height_angle, [3] result)
		result:1 = sin(height_angle);
		:float	r2 = cos(height_angle);
		result:0 = cos(planar_angle)*r2;
		result:2 = sin(planar_angle)*r2;
		
	
	copy,c<16>(const[] origin {array to copy from}, [] destination {array to copy to})
		{Copies elements from <paramref>origin</paramref> to <paramref>destination</paramref>}
		:iterate
			destination:i = (:float)origin:i;
	
	
	cross(const[3] v, const[3] w, [3] result)
		result:0 = v:1*w:2 - v:2*w:1;
		result:1 = v:2*w:0 - v:0*w:2;
		result:2 = v:0*w:1 - v:1*w:0;

		
	crossDot(const[3] u, const[3] v, const[3] w) -> :float
		return	(u:1*v:2 - u:2*v:1) * w:0
				+(u:2*v:0 - u:0*v:2) * w:1
				+(u:0*v:1 - u:1*v:0) * w:2;


	crossVertical(const[3] v, [3] result)
		result:0 = -v:2;
		result:1 = 0;
		result:2 = v:0;
	

	crossZ(const[3] v, [3] result)
		result:0 = v:1;
		result:1 = -v:0;
		result:2 = 0;

	
	normal(const[3] v, [3] result)
		:makeVector(help);
		using std::fabs;
		help:0 = fabs((:float)v:1)+1;
		help:1 = (:float)v:2;
		help:2 = (:float)v:0;
		:<cross>(v,help,result);
		

	makeNormalTo([3] v, const[3] reference)
		:makeVector(temp);
		:<cross>(v,reference,temp);
		:<cross>(reference,temp,v);
	
	intercept(const[] position, const[] velocity, interception_velocity, [] result{normalized direction of intersection, if any; the value of this variable remains unchanged if no interseption could be determined}) -> :float {time of intersection, or 0 if no such could be determined}
		{Calculates the time and direction of interception of an object at <paramref>position</paramref> moving with a specific constant speed and direction <paramref>velocity</paramref>}
		:float	rs[2],
				a = :<dot>(velocity) - interception_velocity*interception_velocity,
				b = 2*:<dot>(position,velocity),
				c = :<dot>(position);
		BYTE num_rs = solveSqrEquation(a, b, c, rs);
		if (!num_rs)
			return 0;
		:float	t = smallestPositiveResult(rs,num_rs);
		if (t<=0)
			return 0;
		:float f = :float(1)/t;
		:<mad>(velocity,position,f,result);
		:<normalize>(result);
		return t;
	
	toString(const[] vector) -> String
		return '('+ :glue( ", ",String(vector:i))+')';

