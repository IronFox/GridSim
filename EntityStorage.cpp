#include <global_root.h>
#include "EntityStorage.h"



static void UpdateColor(Entity&e)
{
	BYTE r = (BYTE)(e.guid.Data1%191);
	BYTE g = (BYTE)(e.guid.Data2%191);
	BYTE b = (BYTE)(e.guid.Data3%191);
	e.color = float3(r,g,b)/255.f;
}


static int CompareGUIDs(const GUID&a, const GUID&b)
{
	return memcmp(&a, &b, sizeof(GUID));
}

bool operator>(const GUID&a, const GUID&b)
{
	return CompareGUIDs(a,b) > 0;
}
bool operator>=(const GUID&a, const GUID&b)
{
	return CompareGUIDs(a, b) >= 0;
}
bool operator<(const GUID&a, const GUID&b)
{
	return CompareGUIDs(a, b) < 0;
}
bool operator<=(const GUID&a, const GUID&b)
{
	return CompareGUIDs(a, b) <= 0;
}



void				EntityStorage::AssertIsSorted()	const
{
	for (index_t i = 1; i < entities.Count(); i++)
	{
		ASSERT2__(entities[i].guid > entities[i - 1].guid, i, entities.Count());
	}
}


bool	EntityStorage::operator==(const EntityStorage&other) const
{
	if (Count() != other.Count())
		return false;
	const_iterator a = begin();
	const_iterator b = other.begin();
	while (a != end())
	{
		if (*a != *b)
			return false;
		++a;
		++b;
	}
	return true;
	//foreach (list,e)
	//{
	//	const auto e2 = other.FindEntity(e->guid);
	//	if (!e2 || (*e) != (*e2))
	//		return false;
	//}
	//foreach (other, e)
	//{
	//	const auto e2 = FindEntity(e->guid);
	//	if (!e2)
	//		return false;
	//}
	//return true;
}


Entity*	EntityStorage::CreateEntity(const GUID&guid)
{
	//ASSERT_EQUAL__(list.Count(),map.Count());
	index_t at = GetIndexOf(guid);
	if (at < Count() && entities[at].guid == guid)
		return nullptr;
	Entity&rs = entities.Insert(at);

	//map.Set(guid, list.Count());
	//Entity&rs = list.Append();
	rs.guid = guid;
	UpdateColor(rs);
	AssertIsSorted();

	//ASSERT_EQUAL__(list.Count(),map.Count());
	return &rs;
}

bool EntityStorage::CreateEntity(const Entity &e)
{
	//ASSERT_EQUAL__(list.Count(),map.Count());
	index_t at = GetIndexOf(e.guid);
	if (at < Count() && entities[at].guid == e.guid)
		return false;
	Entity&rs = entities.Insert(at,e);

	AssertIsSorted();

	//map.Set(e.guid, list.Count());
	//list << e;
	UpdateColor(rs);
	//ASSERT_EQUAL__(list.Count(),map.Count());
	return true;
}

void	EntityStorage::InsertAll(const EntityStorage&other)
{
	foreach (other.entities,e)
	{
		InsertEntity(*e);
	}
}


void EntityStorage::InsertEntity(const Entity &e)
{
	if (!CreateEntity(e))
		FATAL__("Unable to insert entity "+ToString( e.guid )+". GUID exists");
}

Entity * EntityStorage::FindEntity(const GUID &id)
{
	index_t at = GetIndexOf(id);
	if (at >= Count())
		return nullptr;
	Entity*candidate = entities + at;
	if (candidate->guid == id)
		return candidate;
	return nullptr;

	//ASSERT_EQUAL__(list.Count(),map.Count());
	//index_t at;
	//if (map.Query(id,at))
	//	return list + at;
	//return nullptr;
}

const Entity * EntityStorage::FindEntity(const GUID &id) const
{
	index_t at = GetIndexOf(id);
	if (at >= Count())
		return nullptr;
	const Entity*candidate = entities + at;
	if (candidate->guid == id)
		return candidate;
	return nullptr;

	//ASSERT_EQUAL__(list.Count(),map.Count());
	//index_t at;
	//if (map.Query(id,at))
	//	return list + at;
	//return nullptr;
}

void EntityStorage::RemoveEntity(const Entity *e)
{
	if (!entities.Owns(e))
		return;
	//if (!map.Unset(e->guid))
	//	return;
	//index_t i = e - list.pointer();
	//map.VisitAllValues([i](index_t&other)
	//{
	//	if (other > i)
	//		other--;
	//});
	entities.Erase(e - entities.pointer());
	AssertIsSorted();
	//ASSERT_EQUAL__(list.Count(),map.Count());
}

void EntityStorage::Hash(Hasher&hash) const
{
	hash.AppendPOD(Count());
	foreach (entities,e)
		e->Hash(hash);
}

index_t				EntityStorage::GetIndexOf(const GUID&guid) const
{
	index_t begin = 0, end = Count();
	while (begin < end)
	{
		index_t middle = (begin + end) / 2;
		const Entity&e = entities[middle];
		int cmp = CompareGUIDs(guid, e.guid);
		if (cmp < 0)
		{
			end = middle;
			continue;
		}
		if (cmp > 0)
		{
			begin = middle + 1;
			continue;
		}
		return middle;

	}
	return begin;

}


