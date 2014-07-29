#include "../global_root.h"
#include "vertex_table.h"





VertexTable::VertexTable(float tolerance_):tolerance(tolerance_)
{}

		
void	VertexTable::clear()
{
	Table::clear();
	position_map.reset();
}

void	VertexTable::setTolerance(float new_tolerance)
{
	clear();
	tolerance = new_tolerance;
}
		
index_t	VertexTable::map(const float position[3])
{
	TVertexIntPosition ip;
	Vec::def(ip,(__int64)Round(position[0]/tolerance),(__int64)Round(position[1]/tolerance),(__int64)Round(position[2]/tolerance));
	
	
	hash_t hashed = VertexHash::hash(ip);
	
	bool did_occupy;
	Table::Carrier*carrier = Table::find(hashed, ip, true,&did_occupy);
	
	if (!did_occupy)
		return carrier->cast();
		
	index_t result = position_map.fillLevel();
	TPosition&fp = position_map.append();
	fp.x = position[0];
	fp.y = position[1];
	fp.z = position[2];
		
	carrier->cast() = result;
	return result;
}

index_t	VertexTable::map(const TPosition&position)
{
	TVertexIntPosition ip;
	Vec::def(ip,(__int64)Round(position.x/tolerance),(__int64)Round(position.y/tolerance),(__int64)Round(position.z/tolerance));
	
	hash_t hashed = VertexHash::hash(ip);
	
	bool did_occupy;
	Table::Carrier*carrier = Table::find(hashed, ip, true,&did_occupy);
	
	if (!did_occupy)
		return carrier->cast();
		
	index_t result = position_map.fillLevel();
	position_map<<position;
		
	carrier->cast() = result;
	return result;
}

const VertexTable::TPosition&		VertexTable::vertex(index_t index)	const
{
	return position_map[index];
}

