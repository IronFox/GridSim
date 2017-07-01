#include "../global_root.h"
#include "vertex_table.h"


namespace DeltaWorks
{


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
		Vec::def(ip,(__int64)M::Round(position[0]/tolerance),(__int64)M::Round(position[1]/tolerance),(__int64)M::Round(position[2]/tolerance));
	
	
		using namespace GlobalHashFunctions;
		hash_t hashed = Hash(ip);
	
		bool did_occupy;
		Table::Carrier*carrier = Table::find(hashed, ip, true,&did_occupy);
	
		if (!did_occupy)
			return carrier->cast();
		
		index_t result = position_map.Count();
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
		Vec::def(ip,(__int64)M::Round(position.x/tolerance),(__int64)M::Round(position.y/tolerance),(__int64)M::Round(position.z/tolerance));
	
		using namespace GlobalHashFunctions;
		hash_t hashed = Hash(ip);
	
		bool did_occupy;
		Table::Carrier*carrier = Table::find(hashed, ip, true,&did_occupy);
	
		if (!did_occupy)
			return carrier->cast();
		
		index_t result = position_map.Count();
		position_map<<position;
		
		carrier->cast() = result;
		return result;
	}

	const VertexTable::TPosition&		VertexTable::vertex(index_t index)	const
	{
		return position_map[index];
	}
}

