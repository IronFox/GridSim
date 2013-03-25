#ifndef engine_aspectTplH
#define engine_aspectTplH

/******************************************************************

Camera-structures (in general aspects) to simplify usage of
different types of cameras.

******************************************************************/

namespace Engine
{

	template <typename C>
	/*static*/ const Aspect<C>		Aspect<C>::identity;

template <class C>
MFUNC2 (bool) Frustum<C>::visible(const TVec3<C0>&center, const C1&radius) const
{
	return	(Vec::planePointDistanceN(near_bottom_left,bottom_normal,center)<radius)
			&&
			(Vec::planePointDistanceN(near_bottom_right,right_normal,center)<radius)
			&&
			(Vec::planePointDistanceN(near_top_right,top_normal,center)<radius)
			&&
			(Vec::planePointDistanceN(near_top_left,left_normal,center)<radius)
			&&
			(Vec::planePointDistanceN(near_bottom_left,near_normal,center)<radius)
			&&
			(Vec::planePointDistanceN(far_bottom_left,far_normal,center)<radius);
}

template <class C>
	MFUNC2 (bool) Frustum<C>::intersectsCone(const TVec3<C0>&center, const C1&radius) const
		{
			return	(Vec::planePointDistanceN(near_bottom_left,bottom_normal,center)<radius)
					&&
					(Vec::planePointDistanceN(near_bottom_right,right_normal,center)<radius)
					&&
					(Vec::planePointDistanceN(near_top_right,top_normal,center)<radius)
					&&
					(Vec::planePointDistanceN(near_top_left,left_normal,center)<radius);
		}


template <class C> MFUNC2 (typename Frustum<C>::Visibility) Frustum<C>::checkSphereVisibility(const TVec3<C0>&center, const C1&radius) const
	{
		const TVec3<C>* base[6] =
			{
				&near_bottom_left,
				&far_bottom_left,
				&near_bottom_left,
				&near_bottom_right,
				&near_top_right,
				&near_top_left
			};
		bool fully_visible = true;
		for (BYTE k = 0; k < 6; k++)
		{
			C0 dist = Vec::planePointDistance(* (base[k]),normal[k],center);
			if (dist > -radius)
				fully_visible = false;
			if (dist > radius)
				return NotVisible;
		}
		return fully_visible?FullyVisible:PartiallyVisible;
	}


template <class C> MFUNC3 (bool) Frustum<C>::visible(const C0&x, const C1&y, const C2&z) const
	{
		TVec3<C0> center={x,y,z};
		C error = getError<C>();
		return	(Vec::planePointDistanceN(near_bottom_left,bottom_normal,center)<=error)
				&&
				(Vec::planePointDistanceN(near_bottom_right,right_normal,center)<=error)
				&&
				(Vec::planePointDistanceN(near_top_right,top_normal,center)<=error)
				&&
				(Vec::planePointDistanceN(near_top_left,left_normal,center)<=error)
				&&
				(Vec::planePointDistanceN(near_bottom_left,near_normal,center)<=error)
				&&
				(Vec::planePointDistanceN(far_bottom_left,far_normal,center)<=error);
	}
	
	
	
template <class C> MFUNC1 (bool) Frustum<C>::intersects(const C0&x0, const C0&y0, const C0&z0,const C0&x1, const C0&y1, const C0&z1,const TVec3<C>&p0, const TVec3<C>&p1, const TVec3<C>&p2, const TVec3<C>&p3)
	{
		TVec3<C0>	e0 = {x0,y0,z0},
						e1 = {x1,y1,z1},
						rs;
		if (
			_oDetTriangleEdgeIntersection(p0, p1, p2, e0, e1, rs) 
			&& 
			rs[2] >= -getError<C>() 
			&& 
			rs[2] <= (C)1+getError<C>() 
			&& 
			rs[0] >= getError<C>() 
			&& 
			rs[1] >= getError<C>() 
			&& 
			rs[0]+rs[1] <= (C)1+getError<C>())
			return true;
		return _oDetTriangleEdgeIntersection(p0, p2, p3, e0, e1, rs) && rs[2] >= -getError<C>() && rs[2] <= (C)1+getError<C>() && rs[0] >= getError<C>() && rs[1] >= getError<C>() && rs[0]+rs[1] <= (C)1+getError<C>();
	}
	

template <class C> MFUNC1 (bool) Frustum<C>::intersects(const TVec3<C0> box[2], const TVec3<C>&p0, const TVec3<C>&p1, const TVec3<C>&p2, const TVec3<C>&p3)
	{
		return	intersects(box[0].x,box[0].y,box[0].z,box[2].x,box[0].y,box[0].z,p0,p1,p2,p3)
				||
				intersects(box[0].x,box[0].y,box[0].z,box[0].x,box[1].y,box[0].z,p0,p1,p2,p3)
				||
				intersects(box[0].x,box[0].y,box[0].z,box[0].x,box[0].y,box[1].z,p0,p1,p2,p3)
				||
				intersects(box[1].x,box[0].y,box[0].z,box[1].x,box[1].y,box[0].z,p0,p1,p2,p3)
				||
				intersects(box[1].x,box[0].y,box[0].z,box[1].x,box[0].y,box[1].z,p0,p1,p2,p3)
				||
				intersects(box[0].x,box[1].y,box[0].z,box[0].x,box[1].y,box[1].z,p0,p1,p2,p3)
				||
				intersects(box[0].x,box[1].y,box[0].z,box[1].x,box[1].y,box[0].z,p0,p1,p2,p3)
				||
				intersects(box[0].x,box[0].y,box[1].z,box[0].x,box[1].y,box[1].z,p0,p1,p2,p3)
				||
				intersects(box[0].x,box[0].y,box[1].z,box[1].x,box[0].y,box[1].z,p0,p1,p2,p3)
				||
				intersects(box[0].x,box[1].y,box[1].z,box[1].x,box[1].y,box[1].z,p0,p1,p2,p3)
				||
				intersects(box[1].x,box[0].y,box[1].z,box[1].x,box[1].y,box[1].z,p0,p1,p2,p3)
				||
				intersects(box[1].x,box[1].y,box[0].z,box[1].x,box[1].y,box[1].z,p0,p1,p2,p3);
	}

	
template <class C> MFUNC2 (typename Frustum<C>::Visibility) Frustum<C>::checkBoxVisibility(const TVec3<C0>&lower_corner, const TVec3<C1>&upper_corner)	const
	{
		bool all_in_box=true;
		for (BYTE k = 0; k < 8; k++)
		{
			all_in_box = !Vec::oneLess(corner[k],lower_corner) && !Vec::oneGreater(corner[k],upper_corner);
			if (!all_in_box)
				break;
		}
		if (all_in_box)
			return PartiallyVisible;
		if (
			visible(lower_corner.x,lower_corner.y,lower_corner.z)
			&&
			visible(lower_corner.x,lower_corner.y,upper_corner.z)
			&&
			visible(lower_corner.x,upper_corner.y,upper_corner.z)
			&&
			visible(lower_corner.x,upper_corner.y,lower_corner.z)
			&&
			visible(upper_corner.x,upper_corner.y,upper_corner.z)
			&&
			visible(upper_corner.x,upper_corner.y,lower_corner.z)
			&&
			visible(upper_corner.x,lower_corner.y,lower_corner.z)
			&&
			visible(upper_corner.x,lower_corner.y,upper_corner.z))
			return FullyVisible;
		
		// tested for box in frustum and frustum in box. now to check for other intersection. if no intersecion occurs then the box is invisible
		
		//test if edges of the frustum intersect box faces:
		
		C0 box[6];
		_c3(lower_corner,box);
		_c3(upper_corner,box+3);
		
		if (_oIntersectsBox(near_bottom_left,near_bottom_right,box))
			return PartiallyVisible;
		if (_oIntersectsBox(near_bottom_left,near_top_left,box))
			return PartiallyVisible;
		if (_oIntersectsBox(near_top_left,near_top_right,box))
			return PartiallyVisible;
		if (_oIntersectsBox(near_top_right,near_bottom_right,box))
			return PartiallyVisible;
			
		if (_oIntersectsBox(far_bottom_left,far_bottom_right,box))
			return PartiallyVisible;
		if (_oIntersectsBox(far_bottom_left,far_top_left,box))
			return PartiallyVisible;
		if (_oIntersectsBox(far_top_left,far_top_right,box))
			return PartiallyVisible;
		if (_oIntersectsBox(far_top_right,far_bottom_right,box))
			return PartiallyVisible;
			
		if (_oIntersectsBox(far_top_right,near_top_right,box))
			return PartiallyVisible;
		if (_oIntersectsBox(far_top_left,near_top_left,box))
			return PartiallyVisible;
		if (_oIntersectsBox(far_bottom_left,near_bottom_left,box))
			return PartiallyVisible;
		if (_oIntersectsBox(far_bottom_right,near_bottom_right,box))
			return PartiallyVisible;
			
		if (intersects(box,near_bottom_left,near_bottom_right,near_top_right,near_top_left))
			return PartiallyVisible;
		if (intersects(box,far_bottom_left,far_bottom_right,far_top_right,far_top_left))
			return PartiallyVisible;
		if (intersects(box,far_bottom_left,far_bottom_right,near_bottom_right,near_bottom_left))
			return PartiallyVisible;
		if (intersects(box,far_bottom_left,far_top_left,near_top_left,near_bottom_left))
			return PartiallyVisible;
		if (intersects(box,far_top_left,far_top_right,near_top_right,near_top_left))
			return PartiallyVisible;
		if (intersects(box,far_top_right,far_bottom_right,near_bottom_right,near_top_right))
			return PartiallyVisible;
		
		return NotVisible;
	}
	


template <class C>
	MF_DECLARE(String)		Frustum<C>::toString()	const
	{
		String rs = "volume:\r\nnear: "+Vec::toString(near_bottom_left)+", "+Vec::toString(near_bottom_right)+", "+Vec::toString(near_top_right)+", "+Vec::toString(near_top_left)+"\r\n";
		rs += "far: "+Vec::toString(far_bottom_left)+", "+Vec::toString(far_bottom_right)+", "+Vec::toString(far_top_right)+", "+Vec::toString(far_top_left)+"\r\n";
		rs += "normals: left="+Vec::toString(left_normal)+", right="+Vec::toString(right_normal)+", bottom="+Vec::toString(bottom_normal)+", top="+Vec::toString(top_normal);
		return rs;
	}

template <class C>
MFUNC2 (bool)	Frustum<C>::isVisible(const TVec3<C0>&center, const C1&radius)	const
{
	return visible(center,radius);
}


template <class C>
MF_DECLARE(void) Frustum<C>::defNormal(const TVec3<C>&p0, const TVec3<C>&p1, const TVec3<C>&p2, TVec3<C>&out)
{
	TVec3<C> edge0,edge1;
	Vec::sub(p1,p0,edge0);
	Vec::sub(p2,p0,edge1);
	Vec::cross(edge0,edge1,out);
	Vec::normalize0(out);
}

template <class C>
	template <typename SubType>
		MF_DECLARE (void) Frustum<C>::defNormalST(const TVec3<C>&p0, const TVec3<C>&p1, const TVec3<C>&p2, TVec3<C>&out)
		{
			TVec3<SubType>	edge0,edge1,tmp;
			Vec::sub(p1,p0,edge0);
			Vec::sub(p2,p0,edge1);
			Vec::cross(edge0,edge1,tmp);
			Vec::normalize0(tmp);
			Vec::copy(tmp,out);
		}
template <class C>
	MF_DECLARE(void)	Frustum<C>::flipNormals()
	{
		Vec::mult(normal[0],-1);
		Vec::mult(normal[1],-1);
		Vec::mult(normal[2],-1);
		Vec::mult(normal[3],-1);
		Vec::mult(normal[4],-1);
		Vec::mult(normal[5],-1);


	}

template <class C>
MF_DECLARE(void) Frustum<C>::updateNormals()
{
	defNormal(corner[0],corner[4],corner[1],bottom_normal);
	defNormal(corner[1],corner[5],corner[2],right_normal);
	defNormal(corner[2],corner[6],corner[3],top_normal);
	defNormal(corner[3],corner[7],corner[4],left_normal);
	//defNormal(corner[0],corner[1],corner[2],near_normal);	//near field may be too small
	{
		TVec3<C>	far_center = far[0],
					near_center = near[0];

		Vec::add(near_center,near[1]);
		Vec::add(near_center,near[2]);
		Vec::add(near_center,near[3]);
		Vec::div(near_center,(C)4);
	
		Vec::add(far_center,far[1]);
		Vec::add(far_center,far[2]);
		Vec::add(far_center,far[3]);
		Vec::div(far_center,(C)4);
	
		Vec::sub(near_center,far_center,near_normal);
		Vec::normalize0(near_normal);
	}
	
	defNormal(corner[7],corner[6],corner[5],far_normal);
}

template <class C>
	template <typename SubType>
		MF_DECLARE(void) Frustum<C>::updateNormalsST()
		{
			defNormalST<SubType>(corner[0],corner[4],corner[1],bottom_normal);
			defNormalST<SubType>(corner[1],corner[5],corner[2],right_normal);
			defNormalST<SubType>(corner[2],corner[6],corner[3],top_normal);
			defNormalST<SubType>(corner[3],corner[7],corner[4],left_normal);
			defNormalST<SubType>(corner[0],corner[1],corner[2],near_normal);
			defNormalST<SubType>(corner[7],corner[6],corner[5],far_normal);
		}

template <class C>
	MF_DECLARE(void) Frustum<C>::updateNormalsP()
	{
		defNormal(corner[0],corner[4],corner[1],bottom_normal);
		defNormal(corner[1],corner[5],corner[2],right_normal);
		defNormal(corner[2],corner[6],corner[3],top_normal);
		defNormal(corner[3],corner[7],corner[4],left_normal);
		defNormal(corner[0],corner[1],corner[2],near_normal);
		defNormal(corner[7],corner[6],corner[5],far_normal);
	}

template <class C>
	MFUNC1(Frustum<C>&)	Frustum<C>::operator=(const Frustum<C0>&other)
		{
			for (BYTE k = 0; k < 8; k++)
				Vec::copy(other.corner[k],corner[k]);
			for (BYTE k = 0; k < 6; k++)
				Vec::copy(other.normal[k],normal[k]);
			return *this;
		}


template <class C> MF_CONSTRUCTOR Aspect<C>::Aspect():region(0,0,1,1),depthTest(NormalDepthTest)
{
	LoadIdentity();
}

template <class C> MF_CONSTRUCTOR Aspect<C>::Aspect(VisualEnum::eDepthTest dtest):region(0,0,1,1),depthTest(dtest)
{
	LoadIdentity();
}


template <class C> MF_DECLARE(void)	Aspect<C>::LoadIdentity()
{
	view = Matrix<C>::eye4;
	viewInvert = Matrix<C>::eye4;
	projectionInvert = Matrix<C>::eye4;
	projection = Matrix<C>::eye4;
	Vec::clear(location);
}

template <class C> MF_DECLARE(void) Aspect<C>::UpdateInvert()
{
	Mat::invertSystem(view,viewInvert);
	Mat::invert(projection,projectionInvert);
}

template <class C> MF_DECLARE(const TVec3<C>&) Aspect<C>::GetAbsoluteLocation()	const
{
	return viewInvert.w.xyz;
}


template <class C>
MFUNC3 (void) Aspect<C>::Translate(const C0&x, const C1&y, const C2&z)
{
	TVec3<C> vec={x,y,z},out;
	Mat::rotate(view,vec,out);
	Vec::add(ref3(view+12),out);
	Mat::invertSystem(view,viewInvert);
}


template <class C>
MFUNC1 (void) Aspect<C>::Translate(const TVec3<C0>&delta)
{
	TVec3<C> out;
	Mat::rotate(view,delta,out);
	Vec::add(ref3(view+12),out);
	Mat::invertSystem(view,viewInvert);
}

template <class C>
MFUNC4 (void) Aspect<C>::Rotate(const C0&angle, const C1&x, const C2&y, const C3&z)
{
	TVec4<C> a = {x,y,z,angle},q[4];
	Vec::normalize0(a);	//normalize first 3 components
	_q2Quaternion(a,q);
	Rotate(q);
}

template <class C>
MFUNC2 (void) Aspect<C>::Rotate(const C0&angle, const TVec3<C1>&v)
{
	TVec4<C> a = {v.x,v.y,v.z,angle},q[4];
	Vec::normalize0(a);	//normalize first 3 components
	_q2Quaternion(a,q);
	Rotate(q);
}

template <class C>
MFUNC1 (void) Aspect<C>::Rotate(const TVec4<C0>&quaternion)
{
	_qRotateSystemCW(quaternion,view);
	Mat::invertSystem(view,viewInvert);
}

template <class C>
MFUNC2 (bool) Aspect<C>::PointToScreen(const TVec3<C0>&point, TVec2<C1>&screenPoint)	const
{
	TVec4<C>	temp0,temp1;
	Mat::transform(view,point,temp0.xyz);
	temp0.w = 1;
	Mat::mult(projection,temp0,temp1);
	if (vabs(temp1.w) <= TypeInfo<C>::error)
		return false;
	Vec::divide(temp1.xy,temp1.w,screenPoint);
	return temp1.z >= -temp1.w && temp1.z <= temp1.w;	//[-1,+1]
}
template <class C>
MFUNC2 (bool) Aspect<C>::Project(const TVec3<C0>&point, TVec3<C1>&projected)	const
{
	TVec4<C>	temp0,temp1;
	Mat::transform(view,point,temp0.xyz);
	temp0.w = 1;
	Mat::mult(projection,temp0,temp1);
	if (vabs(temp1.w) < getError<C>())
		return false;
	Vec::divide(temp1.xyz,temp1.w,projected);
	return projected.z >= -1 && projected.z <= 1;
}

template <class C>
MFUNC2 (bool) Aspect<C>::VectorToScreen(const TVec3<C0>&vector, TVec2<C1>&screenPoint)	const
{
	TVec4<C>	temp0,temp1;
	Mat::rotate(view,vector,temp0.xyz);
	temp0.w = 1;
	Mat::mult(projection,temp0,temp1);
	if (vabs(temp1.w) < getError<C>())
		return false;
	Vec::divide(temp1.xy,temp1.w,screenPoint);
	return temp1.w >= 0;
}

template <class C>
	MFUNC4 (void)				Aspect<C>::ScreenToVector(const C0&x, const C1&y, TVec3<C2>&position, TVec3<C3>&direction)	const
		{
			TVec2<C>	point = {x,y};
			ScreenToVector(point,position,direction);
		}

template <class C>
MFUNC3 (void) Aspect<C>::ScreenToVector(const TVec2<C0>&point, TVec3<C1>&position, TVec3<C2>&direction)	const
{
	//_c3(&viewInvert[12],position);
	TVec4<C>	p0 = {point.x,point.y,-1,1},
				temp;
	Mat::mult(projectionInvert,p0,temp);
	if (vabs(temp.w) > getError<C>())
		Vec::div(temp.xyz,temp.w);
	Mat::rotate(viewInvert,temp.xyz,direction);
	Mat::transform(viewInvert,temp.xyz,position);
}

template <class C>
MFUNC2 (void)				Aspect<C>::ReverseProject(const TVec3<C0>&point, TVec3<C1>&position)					const
	{
		TVec4<C>		p0 = {point.x,point.y,point.z,1},
						temp;
		Mat::mult(projectionInvert,p0,temp);
		if (vabs(temp.w) > getError<C>())
			Vec::div(temp.xyz,temp.w);
		Mat::transform(viewInvert,temp.xyz,position);
	}


template <class C>
	MF_DECLARE (const Frustum<C>&)	Aspect<C>::ResolveFrustum() const
	{
		return ResolveVolume();
	}

template <class C>
	MF_DECLARE (const Frustum<C>&)	Aspect<C>::GetFrustrum() const
	{
		return ResolveVolume();
	}
	

template <class C>
	MF_DECLARE (const Frustum<C>&) Aspect<C>::ResolveVolume() const
	{
		ResolveVolume(result);
		return result;
	}


template <class C>
	MF_DECLARE (void) Aspect<C>::ResolveFrustum(Volume&result) const
	{
		ResolveVolume(result);
	}

template <class C>
	MF_DECLARE (void) Aspect<C>::GetFrustrum(Volume&result) const
	{
		ResolveVolume(result);
	}

template <class C>
	template <typename SubType>
		MF_DECLARE (void) Aspect<C>::ResolveVolumeST(Volume&result_) const
		{
			Frustum<SubType>	result;

			TVec4<SubType>	temp;

			TVec4<SubType>	p = {-1,-1,-1,1};

							Mat::mult(projectionInvert,p,temp);
							if (temp.w)
								Vec::divide(temp.xyz,temp.w);
							Mat::transform(viewInvert,temp.xyz,result.corner[0]);
			Vec::def(p.xy,1,-1);
							Mat::mult(projectionInvert,p,temp);
							if (temp.w)
								Vec::divide(temp.xyz,temp.w);
							Mat::transform(viewInvert,temp.xyz,result.corner[1]);
			Vec::def(p.xy,1,1);
							Mat::mult(projectionInvert,p,temp);
							if (temp.w)
								Vec::divide(temp.xyz,temp.w);
							Mat::transform(viewInvert,temp.xyz,result.corner[2]);
			Vec::def(p.xy,-1,1);
							Mat::mult(projectionInvert,p,temp);
							if (temp.w)
								Vec::divide(temp.xyz,temp.w);
							Mat::transform(viewInvert,temp.xyz,result.corner[3]);


			Vec::def(p.xyz,-1,-1,1);
							Mat::mult(projectionInvert,p,temp);
							if (temp.w)
								Vec::divide(temp.xyz,temp.w);
							Mat::transform(viewInvert,temp.xyz,result.corner[4]);
			Vec::def(p.xy,1,-1);
							Mat::mult(projectionInvert,p,temp);
							if (temp.w)
								Vec::divide(temp.xyz,temp.w);
							Mat::transform(viewInvert,temp.xyz,result.corner[5]);
			Vec::def(p.xy,1,1);
							Mat::mult(projectionInvert,p,temp);
							if (temp.w)
								Vec::divide(temp.xyz,temp.w);
							Mat::transform(viewInvert,temp.xyz,result.corner[6]);
			Vec::def(p.xy,-1,1);
							Mat::mult(projectionInvert,p,temp);
							if (temp.w)
								Vec::divide(temp.xyz,temp.w);
							Mat::transform(viewInvert,temp.xyz,result.corner[7]);

			result.updateNormalsP();
			result_ = result;
		}

template <class C>
	MF_DECLARE (void) Aspect<C>::ResolveVolume(Volume&result) const
	{
		TVec4<C>	temp;

		TVec4<C>	p = {-1,-1,-1,1};
						Mat::mult(projectionInvert,p,temp);
						if (temp.w)
							Vec::divide(temp.xyz,temp.w);
						Mat::transform(viewInvert,temp.xyz,result.corner[0]);
		Vec::def(p.xy,1,-1);
						Mat::mult(projectionInvert,p,temp);
						if (temp.w)
							Vec::divide(temp.xyz,temp.w);
						Mat::transform(viewInvert,temp.xyz,result.corner[1]);
		Vec::def(p.xy,1,1);
						Mat::mult(projectionInvert,p,temp);
						if (temp.w)
							Vec::divide(temp.xyz,temp.w);
						Mat::transform(viewInvert,temp.xyz,result.corner[2]);
		Vec::def(p.xy,-1,1);
						Mat::mult(projectionInvert,p,temp);
						if (temp.w)
							Vec::divide(temp.xyz,temp.w);
						Mat::transform(viewInvert,temp.xyz,result.corner[3]);


		Vec::def(p.xyz,-1,-1,1);
						Mat::mult(projectionInvert,p,temp);
						if (temp.w)
							Vec::divide(temp.xyz,temp.w);
						Mat::transform(viewInvert,temp.xyz,result.corner[4]);
		Vec::def(p.xy,1,-1);
						Mat::mult(projectionInvert,p,temp);
						if (temp.w)
							Vec::divide(temp.xyz,temp.w);
						Mat::transform(viewInvert,temp.xyz,result.corner[5]);
		Vec::def(p.xy,1,1);
						Mat::mult(projectionInvert,p,temp);
						if (temp.w)
							Vec::divide(temp.xyz,temp.w);
						Mat::transform(viewInvert,temp.xyz,result.corner[6]);
		Vec::def(p.xy,-1,1);
						Mat::mult(projectionInvert,p,temp);
						if (temp.w)
							Vec::divide(temp.xyz,temp.w);
						Mat::transform(viewInvert,temp.xyz,result.corner[7]);

		result.updateNormals();
	}

template <class C> MF_DECLARE (String) Aspect<C>::toString() const
{
	return "view\n"+__toString(view)+"\nprojection\n"+__toString(projection);
}



template <class C> MF_CONSTRUCTOR OrthographicAspect<C>::OrthographicAspect(VisualEnum::eDepthTest depthTest /*= VisualEnum::NoDepthTest*/):Aspect<C>(depthTest)
{
	Vec::clear(location);
	Mat::eye(orientation);
}


MFUNC (void) OrthographicAspect<C>::UpdateProjection(const C&aspect, const C&zoom, const C&zNear, const C&zFar)
{
	C	scale = (C)1/(zFar-zNear),		//endless 0
		offset = -(zNear+zFar)/2*scale,	//endless -0.5
		pa = (C)1.0/region.aspect()/aspect;

	Vec::def(projection.x,	pa*zoom,0,0,0);
	Vec::def(projection.y,	0,zoom,0,0);
	Vec::def(projection.z,	0,0,-scale,0);
	Vec::def(projection.w,	0,0,offset,1);
	Mat::invert(projection,projectionInvert);
}

template <class C>
MFUNC2 (void) OrthographicAspect<C>::AlterDepthRange(const C0&zNear, const C1&zFar)
{
	projection.z.z = (C)-1/(zFar-zNear);
	projection.w.z = (zNear+zFar)/2*projection[10];
	Mat::invert(projection,projectionInvert);
}


template <class C>
MFUNC1 (void) OrthographicAspect<C>::UpdateProjection(const Rect<C0>&area, const C&zNear, const C&zFar)
{
	UpdateProjection(area.left,area.bottom,area.right,area.top,zNear,zFar);
}

MFUNC (void) OrthographicAspect<C>::UpdateProjection(const C&left, const C&bottom, const C&right, const C&top,const C&zNear, const C&zFar)
{
	C	xscale = (C)1/(right-left)*2,
		yscale = (C)1/(top-bottom)*2,
		zscale = (C)1/(zFar-zNear)*2,
		xoffset = (C)-(left)*xscale-1,
		yoffset = (C)-(bottom)*yscale-1,
		zoffset = (C)-(zNear+zFar)/2*zscale;

	Vec::def(projection.x,	xscale,0,0,0);
	Vec::def(projection.y, 0,yscale,0,0);
	Vec::def(projection.z, 0,0,-zscale,0);
	Vec::def(projection.w, xoffset,yoffset,zoffset,1);


	Mat::invert(projection,projectionInvert);
}



template <class C> MF_DECLARE (void) OrthographicAspect<C>::UpdateView()
{
	viewInvert.x.xyz = orientation.x;
	viewInvert.y.xyz = orientation.y;
	viewInvert.z.xyz = orientation.z;
	viewInvert.w.xyz = location;
	Mat::invertSystem(viewInvert,view);
}
template <class C> MF_DECLARE (void) OrthographicAspect<C>::UpdateScaledView()
{
	viewInvert.x.xyz = orientation.x;
	viewInvert.y.xyz = orientation.y;
	viewInvert.z.xyz = orientation.z;
	viewInvert.w.xyz = location;
	Mat::invert(viewInvert,view);
}


template <class C> MF_CONSTRUCTOR Camera<C>::Camera()
{
	Vec::clear(location);
	Vec::clear(retraction);
	Mat::eye(orientation);

	system = orientation;
	vfov = 90;
}

template <class C> MF_DECLARE (void) Camera<C>::UpdateProjection(const C&aspect, const C&zNear, const C&zFar, const C&vFov)
{
	vfov = vFov;
	C	extend = (C)1.0/vtan(vfov*M_PI/180/2),
		pa = (C)1.0/region.aspect()/aspect;

	Vec::def(projection.x,	pa*extend,0,0,0);
	Vec::def(projection.y,	0,extend,0,0);
	Vec::def(projection.z,	0,0,(C)((C)zNear+zFar)/((C)zNear-zFar),-1);	//endless -1, -1
	Vec::def(projection.w,	0,0,(C)2.0*zNear*zFar/(zNear-zFar),0);	//endless 0
	Mat::invert(projection,projectionInvert);
}

template <class C>
MFUNC2 (void) Camera<C>::ExtractDepthRange(C0&zNear, C1&zFar)	const
{
	C	z0 = projection.z.z,	// = (zNear+zFar)/(zNear-zFar)
		z1 = projection.w.z;	// = 2*zNear*zFar/(zNear-zFar)
	
	/*
	z0 = (zNear+zFar)/(zNear-zFar)
	=> (zNear-zFar)*z0 = zNear+zFar
	=> z0*zNear - z0*zFar = zNear+zFar
	=> z0*zNear - zNear = zFar + z0*zFar
	=> zNear * (z0-1) = zFar * (z0+1)
	=> zNear = zFar * (z0+1)/(z0-1)
	
	f0 := (z0+1)/(z0-1)
	
	z1 = 2*zNear*zFar / (zNear-zFar)
	=> z1 = 2*zFar*f0*zFar/(zFar*f0 - zFar)
	=> z1 = 2*zFar²*f0/(zFar*(f0 - 1))
	=> z1 * zFar * (f0-1) = 2*zFar²*f0
	=> z1 * (f0-1) = 2*zFar*f0
	=> zFar = z1 * (f0-1) / (2*f0)
	
	
	=> zNear = z1 * (f0-1)/(2*f0)*f0
	=> zNear = z1 * (f0-1)/2
	*/

	
	C f0 = (z0+1)/(z0-1);
	
	zNear = (C0)(z1*(f0-1)/2);
	zFar = (C1)(z1*(f0-1)/(2*f0));
}


template <class C>
MFUNC2 (void) Camera<C>::AlterDepthRange(const C0&zNear, const C1&zFar)
{
	projection.z.z = (C)((C)zNear+zFar)/((C)zNear-zFar);
	projection.w.z = (C)2.0*zNear*zFar/(zNear-zFar);
	Mat::invert(projection,projectionInvert);
}


template <class C> MF_DECLARE (void) Camera<C>::UpdateView()
{
	if (GlobalAspectConfiguration::worldZIsUp)
	{
		view.x.xyz = orientation.x;
		view.y.xyz = orientation.z;
		view.z.xyz = orientation.y;
		Vec::mult(view.y.xyz,-1);
	}
	else
	{
		view.x.xyz = orientation.x;
		view.y.xyz = orientation.y;
		view.z.xyz = orientation.z;
	}
	view.w.xyz = retraction;
	Mat::resetBottomRow(view);
	TVec3<C> temp;
	Mat::rotate(view,location,temp);
	Vec::subtract(view.w.xyz,temp);

	Mat::invertSystem(view,viewInvert);
}

template <class C>
	MF_DECLARE (TVec3<C>&)			Aspect<C>::GetViewingDirection()
	{
		return viewInvert.z.xyz;
	}
	
template <class C>
	MF_DECLARE (const TVec3<C>&)	Aspect<C>::GetViewingDirection()		const
	{
		return viewInvert.z.xyz;
	}

template <class C>
MFUNC3 (void) Camera<C>::Translate(const C0&x, const C1&y, const C2&z)
{
	TVec3<C>	vec = {x,y,z},out;
	Mat::Rotate(viewInvert,vec,out);
	Vec::add(location,out);
	Vec::add(viewInvert.w.xyz,out);
	Vec::subtract(view.w.xyz,vec);
}

template <class C>
MFUNC1 (void) Camera<C>::Translate(const TVec3<C0>& delta)
{
	TVec3<C>	out;
	Mat::rotate(viewInvert,delta,out);
	Vec::add(location,out);
	Vec::add(viewInvert.w.xyz,out);
	Vec::subtract(view.w.xyz,delta);
}


template <class C>
MFUNC3 (void) Camera<C>::SetPosition(const C0&x, const C1&y, const C2&z)
{
	Vec::def(location,x,y,z);
	UpdateView();
}

template <class C>
MFUNC1 (void) Camera<C>::SetPosition(const TVec3<C0>&position)
{
	Vec::copy(position,location);
	UpdateView();			//could be removed
}

template <class C> MF_CONSTRUCTOR AngularCamera<C>::AngularCamera()
{
	Vec::clear(angle);
}

template <class C> MF_DECLARE (void) AngularCamera<C>::UpdateView()
{
	C	a0 = angle.x*M_PI/180,
		a1 = angle.y*M_PI/180,
		a2 = angle.z*M_PI/180;
	if (GlobalAspectConfiguration::worldZIsUp)
	{
		swp(a1,a2);
	}

	TMatrix3<C>	mbuffer0,
				mbuffer1,
				mbuffer2,
				mbuffer3;

	Vec::def(mbuffer0.x, vcos(a1),0,-vsin(a1));
	Vec::def(mbuffer0.y, 0, 1, 0);
	Vec::def(mbuffer0.z, vsin(a1), 0, vcos(a1));

	Vec::def(mbuffer1.x, 1,0,0);
	Vec::def(mbuffer1.y, 0, vcos(a0), -vsin(a0));
	Vec::def(mbuffer1.z, 0, vsin(a0), vcos(a0));

	Vec::def(mbuffer2.x, vcos(a2),-vsin(a2),0);
	Vec::def(mbuffer2.y, vsin(a2), vcos(a2), 0);
	Vec::def(mbuffer2.z, 0, 0, 1);

	Mat::mult(mbuffer1,mbuffer2,mbuffer3);
	Mat::mult(mbuffer0,mbuffer3,system);


	Mat::transpose(system,orientation);
	Camera<C>::UpdateView();
}

template <class C>
MFUNC3 (void) AngularCamera<C>::TranslatePlanar(const C0&x, const C1&y, const C2&z)
{
	TVec3<C>	delta = {vcos(angle.y*M_PI/180)*x+vsin(angle.y*M_PI/180)*z,y,-vsin(angle.y*M_PI/180)*x+vcos(angle.y*M_PI/180)*z};
	Vec::add(location,delta);
	UpdateView(); //could be removed...
}

template <class C>
MFUNC1 (void) AngularCamera<C>::TranslatePlanar(const TVec3<C0>&delta)
{
	TranslatePlanar(delta.x,delta.y,delta.z);
}

template <class C>
MFUNC3 (void) AngularCamera<C>::AlterAngles(const C0&alpha, const C1&beta, const C2&gamma)
{
	angle.x += alpha;
	angle.y += beta;
	angle.z += gamma;
	UpdateView();
}

template <class C>
MFUNC1 (void) AngularCamera<C>::LookAtPlanar(const TVec3<C0>&vector)
{
	angle.y = Vec::angle360(location.x-vector.x,location.z-vector.z);
	UpdateView();
}

template <class C>
MFUNC1 (void) AngularCamera<C>::LookAt(const TVec3<C0>&vector)
{
	TVec3<C>	delta;
	Vec::sub(location,vector,delta);
	if (GlobalAspectConfiguration::worldZIsUp)
	{
		angle.z = 90.f + Vec::angle360(delta.x,delta.y);
		angle.x = vatan2(delta.z,vsqrt(sqr(delta.x)+sqr(delta.y)))*(C)180/M_PI;
		angle.y = 0;
	}
	else
	{
		angle.y = 90.f - Vec::angle360(delta.x,delta.z);
		angle.x = vatan2(delta.y,vsqrt(sqr(delta.x)+sqr(delta.z)))*(C)180/M_PI;
		angle.z = 0;
	}
	UpdateView();

	//const TVec3<>&	view = GetViewingDirection();
	//if (!Vec::similar(view,delta))
	//	FATAL__("Expected similarity");
}


template <class C>
MFUNC1 (void) AngularCamera<C>::AlterAngles(const TVec3<C0>&delta)
{
	Vec::add(angle,delta);
	UpdateView();
}

template <class C> MF_DECLARE(VectorCamera<C>) AngularCamera<C>::ToVectorCamera()
{
	VectorCamera<C> result(*this);
	result.direction = system.z;
	result.upAxis = system.y;

	if (GlobalAspectConfiguration::worldZIsUp)
	{
		result.direction.z =-result.direction.z;
		std::swap(result.direction.y,result.direction.z);
		result.upAxis.z = -result.upAxis.z;
		std::swap(result.upAxis.y,result.upAxis.z);
	}

	return result;
}

template <class C>
	MF_CONSTRUCTOR VectorCamera<C>::VectorCamera(const Camera<C>&c):Camera<C>(c)
	{
		Vec::def(direction,0,0,-1);
		Vec::def(upAxis,0,1,0);
		vsystem = system;
	}


template <class C> MF_CONSTRUCTOR VectorCamera<C>::VectorCamera()
{
	Vec::def(direction,0,0,-1);
	Vec::def(upAxis,0,1,0);
	vsystem = system;
}

template <class C>
MFUNC3 (void) VectorCamera<C>::TranslatePlanar(const C0&x, const C1&y, const C2&z)
{
	TVec3<C>	temp = {x,y,z};
	TranslatePlanar(temp);
}

template <class C>
MFUNC1 (void) VectorCamera<C>::TranslatePlanar(const TVec3<C0>&delta)
{
	TVec3<C>	temp0,temp1;
	Mat::mult(vsystem,delta,temp0);
	Mat::rotate(view,temp0,temp1);
	Vec::add(location,temp1);
	view.w.xyz = location;
	Mat::invertSystem(view,viewInvert);
}

template <class C>
MFUNC2 (void) VectorCamera<C>::Align(const TVec3<C0>&dir, const TVec3<C1>&vert)
{
	Vec::copy(dir,direction);
	Vec::copy(vert,upAxis);
	UpdateView();
}


template <class C>
	MF_DECLARE (void) VectorCamera<C>::AdjustUpAxis()
	{
		TVec3<C>	n0,n1;
		Vec::cross(direction,upAxis,n0);
		Vec::cross(n0,direction,n1);
		Vec::normalize0(n1);
		if (Vec::dot(n1,upAxis)>0)
			upAxis = n1;
		else
			Vec::mult(n1,-1,upAxis);
	}

template <class C>
	template <typename T>
		MF_DECLARE (void) VectorCamera<C>::SetDirection(const TVec3<T>&d, bool doUpdate)
		{
			Vec::copy(d,direction);
			/*C	intensity = vabs(_intensity(upAxis,direction));
			if (intensity > 0.9)
				AdjustUpAxis();*/
			if (doUpdate)
				UpdateView();
		}

template <class C>
	template <typename T>
		MF_DECLARE (void) VectorCamera<C>::SetUpAxis(const TVec3<T>&v,bool doUpdate)
		{
			Vec::copy(v,upAxis);
			/*C	intensity = vabs(_intensity(v,direction));
			if (intensity > 0.99)
				return;*/
			/*if (intensity>0.9)
				adjustVertical();*/
			if (doUpdate)
				UpdateView();
		}

template <class C> MF_DECLARE (void) VectorCamera<C>::UpdateView()
{
	if (GlobalAspectConfiguration::worldZIsUp)
	{
		TVec3<C> direction = this->direction;
		TVec3<C> upAxis = this->upAxis;
		std::swap(direction.y,direction.z);	direction.z =-direction.z;
		std::swap(upAxis.y,upAxis.z); upAxis.z =-upAxis.z;

		system.z = direction;
		Vec::cross(upAxis,direction,system.x);
		Vec::cross(direction,system.x,system.y);

		vsystem.y = upAxis;
		Vec::cross(upAxis,direction,vsystem.x);
		Vec::cross(vsystem.x,upAxis,vsystem.z);
	}
	else
	{
		system.z = direction;
		Vec::cross(upAxis,direction,system.x);
		Vec::cross(direction,system.x,system.y);

		vsystem.y = upAxis;
		Vec::cross(upAxis,direction,vsystem.x);
		Vec::cross(vsystem.x,upAxis,vsystem.z);
	}


	Vec::normalize(system.x);
	Vec::normalize(system.y);
	Vec::normalize(system.z);

	Mat::transpose(system,orientation);
	Camera<C>::UpdateView();
}

template <class C>
MFUNC2 (void) VectorCamera<C>::RotatePlanar(const C0&alpha, const C1&beta)
{
	TVec3<C>	axis0;
	TMatrix3<C>	matrix,temp;
	Vec::cross(upAxis,direction,axis0);
	Vec::normalize0(axis0);
	//_v3(axis0,-1,0,0);
	Mat::rotationMatrix(-alpha,axis0,matrix);
	//_q2RotMatrixCCW(alpha,axis0,matrix);
	Mat::mult(matrix,direction,axis0);
	//__mult3(matrix,orientation,temp);

	Mat::rotationMatrix(beta,upAxis,matrix);
	Mat::mult(matrix,axis0,direction);
	//__mult3(matrix,temp,orientation);
	
	
	UpdateView();
}

template <class C>
MFUNC1 (void) VectorCamera<C>::RotateDirectional(const C0&alpha, bool rebuild)
{
	TMatrix3<C>		matrix,tempm;
	TVec3<C>		v=direction;

	Vec::normalize0(v);
	Mat::rotationMatrix(alpha,v,matrix);
    //__rotationMatrix(alpha, matrix, 3);
	Mat::mult(matrix,upAxis);
	//__mult3(matrix,orientation,tempm);
	//_c9(tempm,orientation);
	if (rebuild)
		UpdateView();
}

template <class C> MF_DECLARE (AngularCamera<C>) VectorCamera<C>::ToAngularCamera()
{
//tbc - complicated ...

	return AngularCamera<C>();

}


template <class C>
Frustum<C>	Aspect<C>::result;



MF_DECLARE (RECT) rect(int left, int top, int right, int bottom)
{
	RECT rs;
	rs.left = left;
	rs.top = top;
	rs.right = right;
	rs.bottom = bottom;
	return rs;
}

}

#endif
