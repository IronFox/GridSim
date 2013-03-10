#include "map.h"






static bool queryVector(const XML::Node&node, const String&attrib_name, TVec3<float>&vector)
{
	String value;
	if (!node.query(attrib_name,value))
		return false;
	Array<String>	components;
	explode(',',value,components);
	vector = Vector<float>::zero;
	bool success = components.count() == 3;
	for (index_t i = 0; i < components.count() && i < 3; i++)
		success = convert(components[i].trim().c_str(),vector.v[i]) && success;
	return success;
}

template <typename T>
	bool query(const XML::Node&node, const String&attrib_name, T&target)
	{
		String value;
		if (!node.query(attrib_name,value))
			return false;
		return convert(value.c_str(),target);
	}


static float queryFloat(const XML::Node&node, const String&attrib_name)
{
	String value;
	float result;
	if (!node.query(attrib_name,value))
		throw IO::DriveAccess::FileDataFault("Unable to find attribute '"+attrib_name+"' in node '"+node.name+"'");
	if (!convert(value.c_str(),result))
		throw IO::DriveAccess::FileDataFault("Unable to parse attribute '"+attrib_name+"' value '"+String(value)+"' of node '"+node.name+"' to float");
	return result;
}

static int queryInt(const XML::Node&node, const String&attrib_name)
{
	String value;
	int result;
	if (!node.query(attrib_name,value))
		return 0;
	convert(value.c_str(),result);
	return result;
}





void	Map::ObjectInstance::parse(const XML::Node&node,float scale, const FileSystem::Folder&loading_context)
{
	String string;
	node.query("type",string);
	string.convertToLowerCase();
	if (string == "item")
		type = Item;
	elif (string == "physical")
		type = Physical;
	elif (string == "holographic")
		type = Holographic;
	
	node.query("src",source_name);
	instance = composition->loadGeometry(source_name, scale, loading_context,constraints);
	if (!instance)
		composition->warnings<< "warning: unable to find import '"+source_name+"' as requested by object '"+name+"'";
	node.query("spawn_count",string);
	convert(string.c_str(),spawn_count);
	node.query("appear_after_seconds",string);
	convert(string.c_str(),appear_delay);
	node.query("respawn_after_seconds",string);
	convert(string.c_str(),respawn_delay);
	Entity::parse(node,scale,loading_context);
	for (index_t i = 0; i < node.children.count(); i++)
	{
		const XML::Node&enode = node.children[i];
		if (enode.name == "effect")
			collision_effects.append(Effect(composition)).parse(enode,scale,loading_context);
	}
	
	if (instance)
	{
		Mat::copy(system,instance->matrix);
		instance->update();
		sub_line = "Object:"+FileSystem::extractFileName(source_name);
	}
}


void	Map::TextureResource::insert(const String&filename, name64_t as_name, bool bump_map,float bump_strength /*=0.01f*/, float blur_radius/* = 0.0f */)
{
	if (container.isSet(as_name))
		throw Program::UniquenessViolation("64bit name '"+name2str(as_name)+"' is already defined");

	Image	image;
	Magic::loadFromFile(image,filename);

	insert(image, as_name, bump_map,bump_strength, blur_radius);
}

void Map::TextureResource::insert(Image&image, name64_t as_name, bool bump_map,float bump_strength /*=0.01f*/, float blur_radius/* = 0.0f */)
{

	if (blur_radius > 0.f)
	{
		image.blurChannel(0,blur_radius,true);
		image.blurChannel(1,blur_radius,true);
		image.blurChannel(2,blur_radius,true);
	}

	if (!bump_map)
		insert(image,as_name);
	else
	{
		Image	normal_map;
		image.toNormalMap(normal_map,1.0f,1.0f,bump_strength,true,0);
		insert(normal_map,as_name);
	}
}

void Map::TextureResource::insert(const Image&image, name64_t as_name)
{
	CGS::TextureA*new_texture = container.define(as_name);
	new_texture->name = as_name;
	new_texture->face_field.setSize(1);
	TextureCompression::compress(image, new_texture->face_field.first(),TextureCompression::NoCompression);	//why compress?
	new_texture->updateHash();
}



bool	Map::ObjectInstance::cast(const TVec3<>&from, const TVec3<>&direction, float&distance)	const
{
	return instance && instance->detectOpticalIntersection(from,direction,distance);
}



Map::Composition::Composition(Composition*parent_):parent(parent_),has_data(false)
{
	clear();
}

void Map::Composition::clear()
{

	{
		environment.cube_archive = "";
		environment.cube_path = "";
		environment.color = Vector<>::zero;
		environment.global_gravity = Vector<>::zero;
		environment.atmosphere_density = 1;
		environment.lights.reset();
		environment.gravity_centers.clear(0);
		
		tracks.clear(0);
		xtracks.clear(0);
		spline_tracks.clear(0);
		hex_grids.clear(0);
		geometries.clear();
		objects.clear(0);
		effects.clear(0);
		has_data = false;
	}
	
}

/*
void			Map::Composition::initObject(CGS::Child<>*child)
{
	if (!child->vs_hull_field.length())
		return;

	typedef CGS::Child<>::VsDef	Def;
	Mesh<Def>&obj = child->vs_hull_field.first();
	if (obj.map)
		DISCARD(obj.map);

	obj.map = obj.buildMap(O_ALL,4);
		
	for (unsigned i = 0; i < obj.vertex_field.length(); i++)
		_clear(obj.vertex_field[i].normal);
	for (unsigned i = 0; i < obj.triangle_field.length(); i++)
	{
		MeshTriangle<Def>&t = obj.triangle_field[i];
		_oTriangleNormal(t.v0->position,t.v1->position,t.v2->position,t.normal);
		_add(t.v0->normal,t.normal);
		_add(t.v1->normal,t.normal);
		_add(t.v2->normal,t.normal);
		_normalize0(t.normal);
	}
	for (unsigned i = 0; i < obj.quad_field.length(); i++)
	{
		MeshQuad<Def>&q = obj.quad_field[i];
		_oTriangleNormal(q.v0->position,q.v1->position,q.v2->position,q.normal[0]);
		_add(q.v0->normal,q.normal[0]);
		_add(q.v1->normal,q.normal[0]);
		_add(q.v2->normal,q.normal[0]);
		_oTriangleNormal(q.v0->position,q.v2->position,q.v3->position,q.normal[1]);
		_add(q.v0->normal,q.normal[1]);
		_add(q.v2->normal,q.normal[1]);
		_add(q.v3->normal,q.normal[1]);
		_normalize0(q.normal[0]);
		_normalize0(q.normal[1]);
	}
	for (unsigned i = 0; i < obj.vertex_field.length(); i++)
		_normalize0(obj.vertex_field[i].normal);
	
}*/

CGS::Tile*		Map::Composition::loadGeometry(const String&filename, float scale, const FileSystem::Folder&load_context)
{
	//FileSystem::Folder	local_folder(FileSystem::workingDirectory());
	const FileSystem::File*file = load_context.findFile(filename);
	if (!file)
	{
		warnings << "Error: Unable to locate '"+filename+"'";
		
		return NULL;
	}
	CGS::Tile*geo;
	if (geometries.query(file->getLocation(),geo))
		return geo;

	CGS::Geometry<>	local;

	texture_resource.signalNowLoading(local);

	if (local.loadFromFile(file->getLocation().c_str(),&texture_resource))
	{
		if (!nearingOne(scale))
			local.scale(scale);
		geo = geometries.define(file->getLocation());
		local.extractDimensions(geo->dim);
		geo->geometry.adoptData(local);
		geo->filename = file->getLocation();

		return geo;
	}
	warnings << "Error: Unable to load '"+filename+"'";
	return NULL;
}

Map::Instance	Map::Composition::loadGeometry(const String&filename, float scale, const FileSystem::Folder&load_context, Box<>&constraints)
{
	CGS::Tile*tile = loadGeometry(filename,scale,load_context);
	if (tile)
	{
		constraints = tile->dim;
		return tile->geometry.createInstance();
	}
	return Instance();
}

bool		Map::Composition::loadFromFile(const String&filename, float default_scale /*=1.0f*/, LoadContext*context/*=NULL*/)
{
	CFSFolder	folder(FileSystem::getWorkingDirectory());
	return loadFromFile(folder.findFile(filename),default_scale, context);
}




bool		Map::Composition::loadFromFile(const CFSFile*file, float default_scale /*=1.0f*/, LoadContext*context/*=NULL*/)
{
	if (!file)
		return false;
	try
	{
		container.loadFromFile(file->getLocation());
	}
	catch (...)
	{
		return false;
	}
	load_file = *file;
	has_data = false;
	
	CFSFolder folder(load_file.getFolder());
	
	if (!context)
		context = &void_context;
	
	context->printMessage("Reading file '"+load_file.getLocation()+"'");
	String string;
	context->printMessage(" Clearing...");
	clear();
	XML::Node*node,*sub;

	float scale;
	if (node = container.find("map"))
	{
		if (!query(*node,"scale",scale))
			scale = default_scale;
	}
	else
		scale = default_scale;
	context->printMessage(" Locating map/info");
	if (node = container.find("map/info"))
	{
		context->printMessage("Found map/info");
		if (sub = node->find("identity"))
			info.identity = sub->inner_content;
		if (sub = node->find("artist"))
			info.artist = sub->inner_content;
		if (sub = node->find("abstract"))
			info.abstract = sub->inner_content;
	}
	else
		warnings << "warning: XML@map/info not found";

	context->printMessage(" Reading environment");
	if (node = container.find("map/environment"))
	{
		if (node->query("cube",string))
			if (index_t p = string.indexOf(':'))
			{
				environment.cube_archive = string.subString(0,p-1);
				environment.cube_path = string.subString(p);
			}
		if (node->query("color",string))
		{
			Array<String>	components;
			explode(',',string,components);
			for (index_t k = 0; k < components.count() && k < 3; k++)
				convert(components[k].c_str(),environment.color.v[k]);
		}
		if (node->query("gravity",string))
		{
			Array<String>	components;
			explode(',',string,components);
			for (index_t k = 0; k < components.count() && k < 3; k++)
				convert(components[k].c_str(),environment.global_gravity.v[k]);
		}
		if (node->query("atmosphere_density",string))
			convert(string.c_str(),	environment.atmosphere_density);
		
		for (index_t i = 0; i < node->children.count(); i++)
		{
			const XML::Node&entry = node->children[i];
			if (entry.name == "light")
				environment.lights.append().setComposition(this).parse(entry,scale,folder);
			elif (entry.name == "gravity_center")
				environment.gravity_centers.append().setComposition(this).parse(entry,scale,folder);
		}
	}

	context->printMessage(" Reading objects");
	if (node = container.find("map/objects"))
	{
		for (index_t i = 0; i < node->children.count(); i++)
		{
			const XML::Node&entry = node->children[i];
			if (entry.name == "object")
			{
				objects.append().setComposition(this).parse(entry,scale,folder);
			}
		}
	}
	context->printMessage(" Reading tracks");
	if (node = container.find("map/tracks"))
	{
		for (index_t i = 0; i < node->children.count(); i++)
		{
			const XML::Node&entry = node->children[i];
			if (entry.name == "track")
			{
				tracks << Track(this);
				tracks.last().parse(entry,scale,folder);
			}
		}
			
	}
	context->printMessage(" Reading x-tracks");
	if (node = container.find("map/xtracks"))
	{
		for (index_t i = 0; i < node->children.count(); i++)
		{
			const XML::Node&entry = node->children[i];
			if (entry.name == "track")
			{
				xtracks << XTrack(this);
				xtracks.last().parse(entry,scale,folder);
			}
		}
			
	}
	context->printMessage(" Reading spline-tracks");
	if (node = container.find("map/spline_tracks"))
	{
		for (index_t i = 0; i < node->children.count(); i++)
		{
			const XML::Node&entry = node->children[i];
			if (entry.name == "track")
			{
				spline_tracks << SplineTrack(this);
				spline_tracks.last().parse(entry,scale,folder);
			}
		}
	}

	context->printMessage(" Reading hex-grids");
	if (node = container.find("map/hex_grids"))
	{
		for (index_t i = 0; i < node->children.count(); i++)
		{
			const XML::Node&entry = node->children[i];
			if (entry.name == "grid")
			{
				hex_grids << HexGrid(this);
				hex_grids.last().parse(entry,scale,folder);
			}
		}
	}


	context->printMessage(" Reading effects");
	
	if (node = container.find("map/effects"))
	{
		for (index_t i = 0; i < node->children.count(); i++)
		{
			const XML::Node&entry = node->children[i];
			if (entry.name == "volume")
				effects.append().setComposition(this).parse(entry,scale,folder);
		}
	}
	context->printMessage(" Done reading");

	has_data = true;
	
	return true;
	
}





void	Map::Entity::updateSystem()
{
	if (flags&NoPosition)
		return;
	//system = Matrix<>::eye4;

			
	if (flags&Directional)
	{
		Vec::mult(x_align,-1,system.z.xyz);
		Vec::normalize0(system.z.xyz);
		Vec::cross(y_align,system.z.xyz,system.x.xyz);
		Vec::normalize0(system.x.xyz);
		Vec::cross(system.z.xyz,system.x.xyz,system.y.xyz);
		Vec::normalize0(system.y.xyz);
	}
	else
	{
		system.y.xyz = y_align;
		Vec::normalize0(system.y.xyz);
		Vec::cross(x_align,y_align,system.z.xyz);
		Vec::normalize0(system.z.xyz);
		Vec::cross(y_align,system.z.xyz,system.x.xyz);
		Vec::normalize0(system.x.xyz);
	}
	system.w.xyz = position;
	Mat::resetBottomRow(system);

	Mat::invertSystem(system,inverse);
}

bool	Map::Entity::cast(const TVec3<>&from, const TVec3<>&direction, float&distance) const
{
	TVec3<> b,f;
	Mat::transform(inverse,from,b);
	Mat::rotate(inverse,direction,f);

			float	a_ = Vec::dot(f),
					b_ = 2*Vec::dot(f,b),
					c_ = Vec::dot(b)-1,
					rs[2];
			BYTE num_rs = solveSqrEquation(a_,b_,c_,rs);
			if (num_rs)
			{
				float alpha = smallestPositiveResult(rs,num_rs);
				if (alpha < distance)
				{
					distance = alpha;
					return true;
				}
				return false;
			}
			return false;


	TVec3<> delta;
	Vec::sub(position,from,delta);
	float dlen = Vec::length(delta);
	if (dlen > distance)
		return false;
	float similarity = Vec::dot(delta,direction)/(dlen);
	if (similarity > 0.99)
	{
		distance = dlen;
		return true;
	}
	return false;
}



void	Map::Entity::parse(const XML::Node&node,float scale, const FileSystem::Folder&)
{
	order = 0;
	position = Vector<>::zero;

	node.query("name",name);
	name.trimThis();
	
	if (!(flags&NoPosition))
		queryVector(node,"position",position);
	
	x_align = Vector<>::x_axis;
	y_align = Vector<>::y_axis;

	if (!(flags&NoOrientation))
	{
		if ((flags&Directional) && queryVector(node,"direction",direction))
			Vec::normalize0(direction);
		elif (!(flags&Directional) && queryVector(node,"x_align",x_align))
			Vec::normalize0(x_align);

		if (queryVector(node,"y_align",y_align))
			Vec::normalize0(y_align);
	}
	if (!(flags&NoOrder))
	{
		order = queryInt(node,"order");
		sub_line = "order "+IntToStr(order);
	}
	if (scale != 1)
		Vec::mult(position,scale);
	updateSystem();
}


void	Map::Effect::parse(const XML::Node&node,float scale, const FileSystem::Folder&loading_context)
{
	String string;
	
	node.query("strength",string);
	convert(string.c_str(),strength);
	
	node.query("type",string);
	string.convertToLowerCase();
	
	if (string == "score")
		type = Score;
	elif (string == "wind")
		type = Wind;
	elif (string == "gravity")
		type = Gravity;
	elif (string == "refuel")
		type = Refuel;
	elif (string == "repair")
		type = Repair;
	elif (string == "damage")
		type = Damage;
	elif (string == "kill")
		type = Kill;
	Entity::parse(node,scale,loading_context);
}

Map::Shape::~Shape()
{}

void	Map::Shape::updateTriangulation()
{
	_oTriangulate(polygon_points.pointer(),polygon_points.count(),triangulation);
	if (triangulation.vertex_field.length())
	{
		Entity::constraints.set(triangulation.vertex_field.first().position);
		
		for (index_t i = 1; i < triangulation.vertex_field.length(); i++)
			_oDetDimension(triangulation.vertex_field[i].position,Entity::constraints);
	}
}

bool	Map::Shape::cast(const TVec3<>&from, const TVec3<>&direction, float&distance) const
{
	TVec3<> inner_from,inner_direction;
	Mat::transform(Entity::inverse,from,inner_from);
	Mat::rotate(Entity::inverse,direction,inner_direction);
	
	for (index_t i = 0; i < triangulation.triangle_field.length(); i++)	//least efficient way possible :P
		if (_oDetectOpticalIntersection(triangulation.triangle_field[i].v0->position,triangulation.triangle_field[i].v1->position,triangulation.triangle_field[i].v2->position,inner_from,inner_direction,distance))
			return true;
	return false;
}

void	Map::Shape::updateShape()
{
	if (type != Polygon)
	{
		polygon_points.reset();
		switch (type)
		{
			case Ellipse:
			case Circle:
			{
				unsigned num_points = 40;
				for (unsigned i = 0; i < num_points; i++)
					polygon_points << Vec2<>(sin((float)i/(float)num_points*2*M_PI)*primary_diameter/2,cos((float)i/(float)num_points*2*M_PI)*secondary_diameter/2);
			}
			break;
			case Rectangle:
			case Square:
				polygon_points	<< Vec2<>(-primary_diameter/2,-secondary_diameter/2)
								<< Vec2<>(primary_diameter/2,-secondary_diameter/2)
								<< Vec2<>(primary_diameter/2,secondary_diameter/2)
								<< Vec2<>(-primary_diameter/2,secondary_diameter/2);
			break;
		}
	}
}


void	Map::Shape::parse(const XML::Node&node,float scale, const FileSystem::Folder&loading_context)
{
	Entity::parse(node,scale,loading_context);
	
	String string;
	node.query("type",string);
	string.convertToLowerCase();
	if (string == "ellipse")
	{
		type = Ellipse;
		primary_diameter = scale;//queryFloat(node,"radius0")*2;
		secondary_diameter = scale;//queryFloat(node,"radius1")*2;
		if (primary_diameter <= getError<float>())
			primary_diameter = scale;
		if (secondary_diameter <= getError<float>())
			secondary_diameter = scale;
			
		/*for (unsigned i = 0; i < 20; i++)
			polygon_points.append(SHIELDED(new Vec2<float>(sin((float)i/20.0f*2*M_PI)*primary_diameter/2,cos((float)i/20.0f*2*M_PI)*secondary_diameter/2)));*/
	}
	elif (string == "circle")
	{
		type = Circle;
		primary_diameter = queryFloat(node,"radius")*2*scale;
		secondary_diameter = primary_diameter;
	}
	elif (string == "rectangle")
	{
		type = Rectangle;
		primary_diameter = queryFloat(node,"width")*scale;
		secondary_diameter = queryFloat(node,"height")*scale;
	}
	elif (string == "square")
	{
		type = Square;
		primary_diameter = queryFloat(node,"width")*scale;
		secondary_diameter = primary_diameter;
	}
	elif (string == "polygon")
	{
		type = Polygon;
		primary_diameter = scale;
		secondary_diameter = scale;
		
		for (index_t i = 0; i < node.children.count(); i++)
		{
			const XML::Node&pnode = node.children[i];
			if (pnode.name == "vertex")
			{
				Vec2<float>	&v =  polygon_points.append();
				String value;
				pnode.query("position",value);
				Array<String>	values;
				explode(',',value,values);
				if (values.count())
					convert(values[0].trim().c_str(),v.x);
				if (values.count() >= 2)
					convert(values[1].trim().c_str(),v.y);
				Vec::mult(v,scale);
			}
		}
	}
	updateShape();
	
	
}

void			Map::HexGrid::getCornersOfCell(TVec2<> out_corners[6], index_t cell_x, index_t cell_y)	const
{
	const float cell_width = cell_height / sin(M_PI/3.f),
				cell_indent = cell_width * 3.f/4.f;
	float2	coords(cell_indent * cell_x + cell_width*0.5f,cell_height*cell_y + (cell_x%2)*cell_height*0.5f + cell_height*1.5f);

	out_corners[0] = coords;
	out_corners[1] = coords;
	out_corners[2] = coords;
	out_corners[3] = coords;
	out_corners[4] = coords;
	out_corners[5] = coords;

	out_corners[0].x -= cell_width*0.5f;
	out_corners[1].x -= cell_width*0.25f;
	out_corners[1].y -= cell_height*0.5f;
	out_corners[2].x += cell_width*0.25f;
	out_corners[2].y -= cell_height*0.5f;
	out_corners[3].x += cell_width*0.5f;
	out_corners[4].x += cell_width*0.25f;
	out_corners[4].y += cell_height*0.5f;
	out_corners[5].x -= cell_width*0.25f;
	out_corners[5].y += cell_height*0.5f;

}

void					Map::HexGrid::updateConstraints()
{
	const float cell_width = cell_height / sin(M_PI/3.f),
				cell_indent = cell_width * 3.f/4.f;

	constraints.setMin(float3(cell_indent*grid_offset.x,0,cell_height*grid_offset.y));
	constraints.setMax(float3(cell_indent*(grid_offset.x+grid.width())+cell_width*0.25f,0,cell_height*(grid_offset.y+grid.height())+cell_height*0.5f));
}


/*virtual override*/	void					Map::HexGrid::parse(const XML::Node&node,float scale, const FileSystem::Folder&loading_context)
{
	Entity::parse(node,scale,loading_context);


	count_t	res_x = queryInt(node,"resolution_x"),
			res_y = queryInt(node,"resolution_y");
	grid.setSize(res_x,res_y);
	grid_offset.x = queryInt(node,"offset_x");
	grid_offset.y = queryInt(node,"offset_y");
	cell_height = queryFloat(node,"cell_height")*scale;

	for (index_t i = 0; i < node.children.count(); i++)
	{
		const XML::Node&xcell = node.children[i];
		if (xcell.name != "cell")
			continue;
		index_t x = queryInt(xcell,"x"),
				y = queryInt(xcell,"y");
		if (x >= grid.width() || y >= grid.height())
			continue;
		Cell&cell = grid.get(x,y);
		cell.has_floor = true;
		cell.scale = queryFloat(xcell,"scale");
		String type,filename;
		if (xcell.query("type",type))
		{
			if (type == "tree")
			{
				/*later*/
			}
			elif (type == "geometry" && xcell.query("filename",filename))
			{
				cell.geometry_instance = composition->loadGeometry(filename, scale, loading_context,cell.object_bounds);
				if (cell.geometry_instance)
				{
					getMatrixOfCell(cell.geometry_instance->matrix,x,y);
					Vec::mad(cell.geometry_instance->matrix.w.xyz,cell.geometry_instance->matrix.y.xyz,-cell.object_bounds.y.min);
				}
			}
		}
	}

	updateConstraints();
	updateHull();

}


void		Map::HexGrid::updateHull()
{
	const float cell_width = cell_height / sin(M_PI/3.f),
				cell_indent = cell_width * 3.f/4.f;
	static Array<SurfaceDescription>	lods(1);
	static SurfaceDescription			phHull;
	SurfaceDescription&desc = lods.first();
	desc.Clear();
	phHull.Clear();

	TMatrix4<>	system = this->system;
	Vec::mad(system.w.xyz,system.x.xyz,cell_indent * grid_offset.x);
	Vec::mad(system.w.xyz,system.z.xyz,cell_height*(grid_offset.y-1));


	TVec2<> out_corners[6];

	for (index_t x = 0; x < grid.width(); x++)
		for (index_t y = 0; y < grid.height(); y++)
		{
			const Cell&cell = grid.get(x,y);
			if (cell.has_floor)
			{
				getCornersOfCell(out_corners,x,y);
				const UINT32	v_offset = (UINT32)desc.vertices.count(),
								pv_offset = (UINT32)phHull.vertices.count();
				SurfaceDescription::TVertex	vtx,vtx_p0,vtx_p1;

				float2	center(cell_indent * x + cell_width*0.5f,cell_height*y + (x%2)*cell_height*0.5f + cell_height*1.5f);


				for (unsigned i = 0; i < 6; i++)
				{
					Mat::transform(system,float3(out_corners[i].x,0,out_corners[i].y),vtx_p0.position);
					vtx_p1 = vtx_p0;
					vtx_p1.position.y = 0.f;
					desc.vertices.append().position = vtx_p0.position;
					desc.vertices.last().position.y = 0;
					desc.vertices.append().position = vtx_p0.position - float3::reinterpret(system.y.xyz)*0.1f;
					float3 p = desc.vertices.last().position;
					desc.vertices.append().position = p;
					Mat::transform(system,float3(center.x + (out_corners[i].x-center.x)*0.95f,0,center.y + (out_corners[i].y-center.y)*0.95f),desc.vertices.append().position);
					p = desc.vertices.last().position;
					desc.vertices.append().position = p;
					desc.vertices.append().position = p;

					p = desc.vertices.fromEnd(5).position;
					desc.vertices.append().position = p;
					p = desc.vertices.fromEnd(5).position;
					desc.vertices.append().position = p;
					p = desc.vertices.fromEnd(5).position;
					desc.vertices.append().position = p;
					p = desc.vertices.fromEnd(5).position;
					desc.vertices.append().position = p;
					p = desc.vertices.fromEnd(5).position;
					desc.vertices.append().position = p;

					phHull.vertices << vtx_p0 << vtx_p1;

					UINT32	t = (UINT32)(v_offset+i*11+6),
							n = (UINT32)(v_offset+((i+1)%6)*11),
							pt = (UINT32)(pv_offset+i*2),
							pn = (UINT32)(pv_offset+((i+1)%6)*2);
					desc.quadIndices	<< t << t+1 << n+1 << n
										<< t+2 << t+3 << n+3 << n+2;
					phHull.quadIndices << pt << pt+1 << pn+1 << pn;
				}

				for (unsigned i = 1; i < 5; i++)
				{
					UINT32	t = (UINT32)(v_offset+i*11),
							n = (UINT32)(v_offset+((i+1)%6)*11),
							pt = (UINT32)(pv_offset+i*2),
							pn = (UINT32)(pv_offset+((i+1)%6)*2);
					desc.triangleIndices << v_offset+4 << n+4 << t+4;
					phHull.triangleIndices << pv_offset << pn << pt;
				}

			}


		}

	desc.GenerateNormals();
	SurfaceNetwork::CompileFromDescriptions(hull,lods,phHull,1.f,0,0);
	hull.material_field.first().info.fully_reflective = true;
	hull.material_field.first().info.specular = float4(0.9,0.9,1,1);

}




void 		Map::Volume::parse(const XML::Node&node,float scale, const FileSystem::Folder&loading_context)
{
	radius = queryFloat(node,"radius")*scale;
	height = queryFloat(node,"height")*scale;
	if (!height)
		height = scale;
	if (!radius)
		radius = scale;
	Entity::parse(node,scale,loading_context);
	
	String string;
	node.query("type",string);
	string.convertToLowerCase();
	if (string == "sphere")
	{
		type = Sphere;
		Entity::constraints.setAllMin(-radius);
		Entity::constraints.setAllMax(radius);
	}
	else
	{
		type = Cylinder;
		Entity::constraints.setMin(-radius,-height/2,-radius);
		Entity::constraints.setMax(radius,height/2,radius);
	}
	for (index_t i = 0; i < node.children.count(); i++)
	{
		const XML::Node&sub = node.children[i];
		if (sub.name == "effect")
		{
			effects.append(Effect(composition)).parse(sub,scale,loading_context);
		}
	}
}

bool	Map::Volume::cast(const TVec3<>&from, const TVec3<>&direction, float&distance)	const
{
	TVec3<>	b,f;
	Mat::transform(Entity::inverse,from,b);
	Mat::rotate(Entity::inverse,direction,f);
	
	switch (type)
	{
		case Cylinder:
		{
			float	a_ = Vec::dot(f.xy),
					b_ = 2*Vec::dot(f.xy,b.xy),
					c_ = Vec::dot(b.xy)-sqr(radius),
					rs[2];
			BYTE num_rs = solveSqrEquation(a_,b_,c_,rs);
			if (num_rs)
			{
				float alpha = smallestPositiveResult(rs,num_rs);
				float h = b.z+alpha*f.z;
				if (h >= -height/2-getError<float>() && h <= height/2+getError<float>())
				{
					if (distance > alpha)
					{
						distance = alpha;
						return true;
					}
					return false;
				}
			}
			if (fabs(f.z) < getError<float>())
				return false;
			float alpha = (height/2-b.z)/f.z;
			if (sqrt(sqr(b.x+f.x*alpha)+sqr(b.y+f.y*alpha))<=radius+getError<float>())
			{
				if (alpha < distance)
				{
					distance = alpha;
					return true;
				}
				return false;
			}
			alpha = (-height/2-b.z)/f.z;
			if (sqrt(sqr(b.x+f.x*alpha)+sqr(b.y+f.y*alpha))<=radius+getError<float>())
			{
				if (alpha < distance)
				{
					distance = alpha;
					return true;
				}
				return false;
			}
			return false;
		}
		break;
		case Sphere:
		{
			float	a_ = Vec::dot(f),
					b_ = 2*Vec::dot(f,b),
					c_ = Vec::dot(b)-sqr(radius),
					rs[2];
			BYTE num_rs = solveSqrEquation(a_,b_,c_,rs);
			if (num_rs)
			{
				float alpha = smallestPositiveResult(rs,num_rs);
				if (alpha < distance)
				{
					distance = alpha;
					return true;
				}
				return false;
			}
			return false;
		}
		break;
	}
	return Entity::cast(from,direction,distance);
}



Map::Light::Light():Entity("Light",Entity::Directional|Entity::NoOrder),type(Omni),spot_cutoff(45),constant_attenuation(1),linear_attenuation(0),quadratic_attenuation(0), spot_exponent(5)
{
	color = Vector<>::one;
	sub_line = "type omni";
}


Map::Light::Light(Composition*composition):Entity("Light",Entity::Directional|Entity::NoOrder,composition),type(Omni),spot_cutoff(45),constant_attenuation(1),linear_attenuation(0),quadratic_attenuation(0), spot_exponent(5)
{
	color = Vector<>::one;
	sub_line = "type omni";
}


void	Map::Light::parse(const XML::Node&node,float scale, const FileSystem::Folder&loading_context)
{
	String string;
	node.query("type",string);
	string.convertToLowerCase();
	if (string == "omni")
		type = Omni;
	elif (string == "direct")
		type = Direct;
	elif (string == "spot")
		type = Spot;

	queryVector(node,"color",color);
	query(node,"spot_cutoff",spot_cutoff);
	queryVector(node,"attenuation",attenuation);
	query(node,"spot_exponent",spot_exponent);
	
	sub_line = "type "+string;
	
	Entity::parse(node,scale,loading_context);
	//fullUpdate();
}



void	Map::GravityCenter::parse(const XML::Node&node,float scale, const FileSystem::Folder&loading_context)
{
	Entity::parse(node,scale,loading_context);
	mass = queryFloat(node,"mass")*scale*scale*scale;
}




void	Map::Track::parse(const XML::Node&node,float scale, const FileSystem::Folder&loading_context)
{
	Entity::parse(node,scale,loading_context);
	query(node,"loop",loop);
	
	if (const XML::Node*xstart_positions = node.find("start_positions"))
	{
		for (index_t i = 0; i < xstart_positions->children.count(); i++)
		{
			const XML::Node&sub = xstart_positions->children[i];
			if (sub.name == "location")
				start_positions.append(StartPosition(composition)).parse(sub,scale,loading_context);
		}
	}
	start_positions.compact();
	if (const XML::Node*xcheckpoints = node.find("checkpoints"))
	{
		for (index_t i = 0; i < xcheckpoints->children.count(); i++)
		{
			const XML::Node&sub = xcheckpoints->children[i];
			if (sub.name == "shape")
				checkpoints.append(Shape(composition)).parse(sub,scale,loading_context);
		}
	}
	checkpoints.compact();
}

void	Map::XTrack::parse(const XML::Node&node,float scale, const FileSystem::Folder&loading_context)
{
	Entity::parse(node,scale,loading_context);
	
	query(node,"goal",goalNode);
	
	if (const XML::Node*xnodes = node.find("nodes"))
	{
		for (index_t i = 0; i < xnodes->children.count(); i++)
		{
			const XML::Node&sub = xnodes->children[i];
			if (sub.name == "node")
			{
				nodes.append(XNode(composition)).parse(sub,scale,loading_context);
				node_table.set(nodes.last().id,nodes.count()-1);
			}
		}
	}
	if (const XML::Node*xsegments = node.find("segments"))
	{
		for (index_t i = 0; i < xsegments->children.count(); i++)
		{
			const XML::Node&sub = xsegments->children[i];
			if (sub.name == "segment")
				segments.append(XSegment(composition)).parse(sub,scale,loading_context);
		}
	}


	nodes.compact();
	segments.compact();

	for (index_t i = 0; i < segments.count(); i++)
		segments[i].link(*this);


}


void Map::XNode::parse(const XML::Node&node,float scale, const FileSystem::Folder&loading_context)
{
	Entity::parse(node,scale,loading_context);
	//Vec::mult(Entity::direction,-1);

	CGS::TrackNode::coordinates.remainder = position;
	CGS::TrackNode::coordinates.sector = Vector<Composite::sector_t>::zero;
	//CGS::TrackNode::direction = -float3::reinterpret(Entity::direction);
	Vec::copy(Entity::system.z.xyz,CGS::TrackNode::direction);
	Vec::copy(Entity::system.y.xyz,CGS::TrackNode::up);


	query(node,"id",id);
	query(node,"width",width);	width *= scale;
	query(node,"height",height); height *= scale;

	CGS::TrackNode::scale.x = width;
	CGS::TrackNode::scale.y = height;

	if (!query(node,"function",(int&)function))
		function = NoFunction;

	String filename;
	if (query(node,"inbound",filename))
		inbound.setStub(composition->loadGeometry(filename, scale, loading_context));

	if (query(node,"outbound",filename))
		outbound.setStub(composition->loadGeometry(filename, scale, loading_context));
	//updateSystem();
	CGS::TrackNode::update(system,true);
}

void	Map::XSegment::parseConnector(const XML::Node*node, BYTE index)
{
	if (!node)
	{
		composition->warnings << String("Connector xml section not specified for connector '"+String((int)index)+"'");
		return;
	}
	query(*node,"id",node_id[index]);
	query(*node,"slot",node_slot[index].slot_index);
	query(*node,"flipped",node_slot[index].flipped);
	query(*node,"outbound",node_slot[index].is_outbound);

}

void Map::XSegment::parse(const XML::Node&node,float scale, const FileSystem::Folder&loading_context)
{
	Entity::parse(node,scale,loading_context);

	String string;
	FileSystem::File file;
	if (query(node,"tile",string) && loading_context.findFile(string,file))
	{
		composition->texture_resource.signalNowLoading(bent_geometry);
		ASSERT__(bent_geometry.loadFromFile(file.getLocation(),&composition->texture_resource));
		TVec3<>	center;
		ASSERT__(queryVector(node,"tile_center",center));
		Vec::mult(center,scale);
		if (scale != 1)
			bent_geometry.scale(scale);
		bent_geometry.root_system.setPosition(center);
	}

	if (node.query("start_position",string))
	{
		if (string == "forward")
		{
			function = StartPosition;
		}
		elif (string == "backward")
		{
			function = RevertedStartPosition;
		}
		else
		{
			function = NoFunction;
		}
	}
	else
	{
		function = NoFunction;
	}


	parseConnector(node.find("from"),0);
	parseConnector(node.find("to"),1);
}

void Map::XSegment::link(XTrack&track)
{
	//node_slot[0].is_outbound = !node_slot[0].is_outbound;
	//node_slot[1].flipped = !node_slot[1].flipped;

	index_t	index;
	ASSERT__(track.node_table.query(node_id[0],index));
	track.nodes[index].setSegment(node_slot[0],this);

	ASSERT__(track.node_table.query(node_id[1],index));
	track.nodes[index].setSegment(node_slot[1],this);

	this->updateControls(50000);
}

/*static*/	void	Map::SplineTrack::parseConnector(const XML::Node&xnode, SurfaceNetwork::Segment::Connector&connector)
{
	ASSERT__(query(xnode,"id",connector.node));
	ASSERT__(query(xnode,"slot",connector.subdivisionStep));
	ASSERT__(query(xnode,"outbound",connector.outbound));
}


bool	Map::SplineTrack::findClosestSegment(const TVec3<>&reference_position, index_t&out_index, TVec2<>&out_factor, float&out_distance, SurfaceDescription::InterpolatedSlice&out_slice)	const
{
	float min_distance = std::numeric_limits<float>::max();
	bool got_one = false;

	const float3&ref_p = float3::reinterpret(reference_position);

	for (index_t i = 0; i < network.segments.count(); i++)
	{
		if (Vec::quadraticDistance(reference_position,segmentBoundingSphere[i].xyz) > sqr(segmentBoundingSphere[i].w*1.2f))
			continue;

		const SurfaceNetwork::Segment&seg = network.segments[i];
		float	lower = 0.f,
				upper = 1.f;
		SurfaceDescription::InterpolatedSlice	lower_slice,upper_slice,pivot_slice;
		const SurfaceNetwork::Node	&n0 = network.nodes[seg.connector[0].node],
									&n1 = network.nodes[seg.connector[1].node];
		SurfaceDescription::TConnector	c0,
										c1;
		DBG_VERIFY__(n0.BuildConnector(seg.connector[0].outbound,seg.connector[0].subdivisionStep,false,c0));
		DBG_VERIFY__(n1.BuildConnector(seg.connector[1].outbound,seg.connector[1].subdivisionStep,true,c1));

		SurfaceDescription::Interpolate(c0, c1,seg.controls, lower, lower_slice);
		SurfaceDescription::Interpolate(c0, c1,seg.controls, upper, upper_slice);
		float3	delta;
		Vec::sub(reference_position,lower_slice.position,delta);
		if (Vec::dot(lower_slice.direction,delta) < 0)
			continue;
		Vec::sub(reference_position,upper_slice.position,delta);
		if (Vec::dot(upper_slice.direction,delta) > 0)
			continue;
		float3 pivot_normal;

		for (index_t j = 0; j < 10; j++)
		{
			float pivot = (upper+lower)*0.5f;
			SurfaceDescription::Interpolate(c0, c1,seg.controls, pivot, pivot_slice);
			float3	delta;
			Vec::cross(pivot_slice.right,pivot_slice.up,pivot_normal);
			Vec::sub(reference_position,pivot_slice.position,delta);
			if (Vec::dot(pivot_normal,delta) < 0)
			{
				lower = pivot;
				//lower_slice = pivot_slice;
			}
			else
			{
				upper = pivot;
				//upper_slice = pivot_slice;
			}
		}
		float radius = std::max(fabs(pivot_slice.scale.x),fabs(pivot_slice.scale.y));
		float3 projected_ref = ref_p - pivot_normal * Vec::dot(ref_p-pivot_slice.position,pivot_normal);
		
		//if (Vec::quadraticDistance(projected_ref,pivot_slice.position) < radius*radius)
		{
			float closest_distance = std::numeric_limits<float>::max();
			float closest_at = 0;
			for (index_t j = 0; j < 3; j++)
			{
				float fj = (float(j)+0.5f)/3.f;
				TVec3<>	p,n,d;
				pivot_slice.MakePoint(fj,p,n);
				float dist = Vec::quadraticDistance(p,projected_ref);
				Vec::sub(projected_ref,p,d);
				//float3 delta = projected_ref - p;
				if (dist < closest_distance && Vec::dot(n,d)>-0.1)
				{
					closest_distance = dist;
					closest_at = fj;
				}
			}
				float extend = 1.f/6.f;
				for (index_t j = 0; j < 5; j++)
				{
					float step = extend*2.f/3.f;
					float center = clamped(closest_at,extend,1.f-extend);
					for (float f = center-extend; f <= center+extend; f+= step)
					{
						TVec3<>	p,n,d;
						pivot_slice.MakePoint(f,p,n);
						Vec::sub(projected_ref,p,d);
						float dist = Vec::quadraticDistance(p,projected_ref);
						if (dist < closest_distance && Vec::dot(n,d)>-0.1)
						{
							closest_distance = dist;
							closest_at = f;
							//out_position = p;
						}
					}
					extend /= 3.f;
				}
			if (closest_distance < min_distance)
			{
				out_slice = pivot_slice;

				out_index = i;
				out_factor.y = (upper+lower)*0.5f;
				out_factor.x = closest_at;

				min_distance = closest_distance;
				got_one = true;
			}
		}
	}
	out_distance = sqrt(min_distance);
	return got_one;
}


void	Map::SplineTrack::parse(const XML::Node&node,float scale, const FileSystem::Folder&loading_context)
{
	Entity::parse(node,scale,loading_context);

	SurfaceNetwork	network;
	
	index_t	node_id;

	query(node,"goal",goalNode);
	SurfaceNetwork::minTolerance *= scale/4;
	
	if (const XML::Node*xnodes = node.find("nodes"))
	{
		for (index_t i = 0; i < xnodes->children.count(); i++)
		{
			const XML::Node&sub = xnodes->children[i];
			if (sub.name == "node")
			{
				ASSERT__(query(sub,"id",node_id));
				SurfaceNetwork::Node*node = network.nodes.setNew(node_id);
				ASSERT_NOT_NULL__(node);
				ASSERT__(queryVector(sub,"position",node->position));
				Vec::mult(node->position,scale);
				ASSERT__(queryVector(sub,"direction",node->direction));
				float3	y_align,right;
				ASSERT__(queryVector(sub,"y_align",y_align));
				Vec::cross(node->direction,y_align,right);
				Vec::cross(right,node->direction,node->up);
				Vec::normalize(node->up);
				Vec::mult(node->direction,-1.f);
				ASSERT__(query(sub,"angle0",node->angle0));
				ASSERT__(query(sub,"angle1",node->angle1));
				ASSERT__(query(sub,"texcoord0",node->texcoord0));
				ASSERT__(query(sub,"texcoord1",node->texcoord1));
				ASSERT__(query(sub,"width",node->scale.x));
				ASSERT__(query(sub,"height",node->scale.y));
				Vec::mult(node->scale,scale);

				for (int k = 0; k < 2; k++)
				{
					const XML::Node*xdivs = sub.find("divs"+String(k));
					if (xdivs)
					{
						count_t	num_divs;
						ASSERT__(query(*xdivs,"count",num_divs));
						node->subdivision[k].setSize(num_divs);
						node->segments[k].setSize(num_divs+1);
						node->segments[k].fill(InvalidIndex);
						for (index_t j = 0; j < xdivs->children.count() && j < num_divs; j++)
						{
							ASSERT_EQUAL__(xdivs->children[j].name,"div");
							ASSERT__(query(xdivs->children[j],"at",node->subdivision[k][j]));
						}

					}
					else
					{
						node->subdivision[k].free();
						node->segments[k].setSize(1);
						node->segments[k].fill(InvalidIndex);
					}
				}

			}
		}
	}
	if (const XML::Node*xsegments = node.find("segments"))
	{
		for (index_t i = 0; i < xsegments->children.count(); i++)
		{
			const XML::Node&sub = xsegments->children[i];
			if (sub.name == "segment")
			{
				const XML::Node*xfrom = sub.find("from"),
								*xto = sub.find("to");
				ASSERT_NOT_NULL__(xfrom);
				ASSERT_NOT_NULL__(xto);
				SurfaceNetwork::Segment::Connector	from,to;
				parseConnector(*xfrom,from);
				parseConnector(*xto,to);
				index_t	seg_id = network.Link(from,to,false,false);

				String	function;
				if (sub.query("start_position",function))
				{
					if (function == "forward")
						startSegments << StartSegment(seg_id,true);
					elif (function == "backward")
						startSegments << StartSegment(seg_id,false);
					else
						FATAL__("Unexpected start_position string encountered: '"+function+"'");
				}
			}
		}
	}

	network.UpdateAllSlopes(true,true);

	network.MoveCompact(this->network);

	foreach(startSegments,seg)
		seg->segment_id = network.segments[seg->segment_id].segID;

	recompileGeometry(scale);
}

void Map::SplineTrack::recompileGeometry(float scale)
{
	vertexBuffer.reset();
	indexBuffer.reset();

	segmentLODs.setSize(network.segments.count());
	segmentBoundingSphere.setSize(network.segments.count());



	count_t	ph_vertex_count = 0;

	for (index_t i = 0; i < SurfaceNetwork::numLODs*3; i++)
	{
		if (i == 1)
			ph_vertex_count = vertexBuffer.size();
		for (index_t j = 0; j < network.segments.count(); j++)
		{
			const SurfaceNetwork::Segment&	seg = network.segments[j];

			if (segmentLODs[j].isEmpty())
				segmentLODs[j].setSize(seg.compiledSurfaces.count());
			DBG_ASSERT_EQUAL__(seg.compiledSurfaces.count(),SurfaceNetwork::numLODs*3);
			SegmentLOD&lod = segmentLODs[j][i];

			static Buffer<index_t>	vertex_map;

			const SurfaceDescription&surf = seg.compiledSurfaces[i];
			//foreach (seg.compiledSurfaces,surf)
			{
				const UINT32 voffset = (UINT32)vertexBuffer.size();
				lod.triangleIndexOffset = indexBuffer.count();
				foreach(surf.vertices,vert)
				{
					Vertex&vtx = vertexBuffer.append();
					vtx.position = vert->position;
					vtx.normal = vert->normal;
					vtx.tangent = vert->tangent;
					vtx.texcoords = vert->tcoord;
				}
				const UINT32 vdown_offset = (UINT32)vertexBuffer.size();
				Vertex*const vtx_field = vertexBuffer.appendRow(surf.vertices.count());
				Concurrency::parallel_for(index_t(0),surf.vertices.count(),[vtx_field,&surf,scale](index_t i)
				{
					Vertex&vtx = vtx_field[i];
					const auto&vert = surf.vertices[i];
					Vec::mad(vert.position,vert.normal,-0.1f*scale,vtx.position);
					Vec::mul(vert.normal,-1.f,vtx.normal);
					Vec::mul(vert.tangent,-1.f,vtx.tangent);
					vtx.texcoords = vert.tcoord;
				});


				if (i+1 == SurfaceNetwork::numLODs*3)
				{
					Box<>	box(vertexBuffer[voffset].position);
					for (index_t k = voffset+1; k < vertexBuffer.fillLevel(); k++)
						_oDetDimension(vertexBuffer[k].position,box);
					float3 center = box.center();
					float radius = 0;
					for (index_t k = voffset; k < vertexBuffer.fillLevel(); k++)
					{
						float r = Vec::quadraticDistance(center,vertexBuffer[k].position);
						if (r > radius)
							radius = r;
					}
					segmentBoundingSphere[j].xyz = center;
					segmentBoundingSphere[j].w = sqrt(radius);
				}



				foreach(surf.triangleIndices,tri)
					indexBuffer << (UINT32)((*tri) + voffset);
				for (index_t i = 0; i < surf.triangleIndices.size(); i+=3)
				{
					indexBuffer << (UINT32)(surf.triangleIndices[i] + vdown_offset);
					indexBuffer << (UINT32)(surf.triangleIndices[i+2] + vdown_offset);
					indexBuffer << (UINT32)(surf.triangleIndices[i+1] + vdown_offset);
				}

				lod.quadIndexOffset = indexBuffer.count();
				foreach(surf.quadIndices,quad)
					indexBuffer << (UINT32)((*quad)+voffset);

				for (index_t i = 0; i < surf.quadIndices.size(); i+=4)
				{
					indexBuffer << surf.quadIndices[i+3] + vdown_offset;
					indexBuffer << surf.quadIndices[i+2] + vdown_offset;
					indexBuffer << surf.quadIndices[i+1] + vdown_offset;
					indexBuffer << surf.quadIndices[i] + vdown_offset;
				}

				vertex_map.reset();

				foreach(surf.edges, edge)
				{
					index_t last_vertex_index = InvalidIndex;
					for (index_t i = 0; i < edge->length(); i++)
					{
						index_t vindex = edge->at(i);
						if (vindex >= vertex_map.fillLevel())
						{
							vertex_map.appendRow(vindex - vertex_map.fillLevel() + 1,InvalidIndex);
						}
						index_t vertex_index = vertex_map[vindex];
						if (vertex_index == InvalidIndex)
						{
							vertex_map[vindex] = vertex_index = vertexBuffer.size();
							const SurfaceDescription::TVertex&vert = surf.vertices[vindex];
							{
								Vertex&vtx = vertexBuffer.append();
								vtx.position = vert.position;
								vtx.normal = vert.tangent;
								Vec::mul(vert.normal,-1.f,vtx.tangent);
								vtx.texcoords = vert.tcoord;

								if (edge->leftEdge)
								{
									Vec::mult(vtx.tangent,-1.f);
									Vec::mult(vtx.normal,-1.f);
								}
							}
							{
								Vertex&vtx = vertexBuffer.append();
								Vec::mad(vert.position,vert.normal,-0.1f*scale,vtx.position);
								vtx.normal = vert.tangent;
								Vec::mul(vert.normal,-1.f,vtx.tangent);
								vtx.texcoords = vert.tcoord;
								vtx.texcoords.x += 0.1f;
								if (edge->leftEdge)
								{
									Vec::mult(vtx.tangent,-1.f);
									Vec::mult(vtx.normal,-1.f);
								}
							}
						}

						if (i > 0)
						{
							indexBuffer << (UINT32)last_vertex_index << (UINT32)(last_vertex_index+1) << (UINT32)(vertex_index+1) << (UINT32)vertex_index;
						}

						last_vertex_index = vertex_index;
					}
				}


				lod.quadIndexCount = indexBuffer.count() - lod.quadIndexOffset;
			}


		}
	}

	count_t	num_triangles = 0,
			num_quads = 0;
	for (index_t i = 0; i < network.segments.count(); i++)
	{
		num_triangles += (segmentLODs[i].first().quadIndexOffset - segmentLODs[i].first().triangleIndexOffset)/3;
		num_quads += segmentLODs[i].first().quadIndexCount / 4;
	}
	
	phHull.resize(ph_vertex_count,0,num_triangles,num_quads);
	for (index_t i = 0; i < ph_vertex_count; i++)
	{
		const Vertex&from = vertexBuffer[i];
		auto&to = phHull.vertex_field[i];
		to.normal = from.normal;
		to.position = from.position;
	}

	auto triangle_out = phHull.triangle_field.pointer();
	auto quad_out = phHull.quad_field.pointer();
	const UINT32*index_field = indexBuffer.pointer();
	for (index_t i = 0; i < network.segments.count(); i++)
	{
		count_t local_triangle_count = (segmentLODs[i].first().quadIndexOffset - segmentLODs[i].first().triangleIndexOffset)/3;
		for (index_t j = 0; j < local_triangle_count; j++)
		{
			triangle_out->v0 = phHull.vertex_field + (*index_field++);
			triangle_out->v1 = phHull.vertex_field + (*index_field++);
			triangle_out->v2 = phHull.vertex_field + (*index_field++);
			triangle_out++;
		}
		count_t local_quad_count = segmentLODs[i].first().quadIndexCount / 4;
		for (index_t j = 0; j < local_quad_count; j++)
		{
			quad_out->v0 = phHull.vertex_field + (*index_field++);
			quad_out->v1 = phHull.vertex_field + (*index_field++);
			quad_out->v2 = phHull.vertex_field + (*index_field++);
			quad_out->v3 = phHull.vertex_field + (*index_field++);
			quad_out++;
		}
	}
	ASSERT_EQUAL__(index_field - indexBuffer.pointer(),segmentLODs.first()[1].triangleIndexOffset);
	ASSERT_CONCLUSION(phHull.triangle_field,triangle_out);
	ASSERT_CONCLUSION(phHull.quad_field,quad_out);
}

