#pragma once

#include "entity.h"
#include <io/cryptographic_hash.h>
#include <container/buffer.h>



bool operator>(const GUID&, const GUID&);
bool operator>=(const GUID&, const GUID&);
bool operator<(const GUID&, const GUID&);
bool operator<=(const GUID&, const GUID&);


class EntityStorage
{
	typedef Buffer0<Entity>	List;
	List				entities;

	void				AssertIsSorted()	const;
	index_t				GetIndexOf(const GUID&) const;
public:
	typedef List::iterator iterator;
	typedef List::const_iterator const_iterator;
	
	void				swap(EntityStorage&other)
	{
		entities.swap(other.entities);
	}
	
	/**
	Attempts to create the specified complete entity.
	@return true if the entity was created, false if the GUID was already in use. The local list remains unchanged if the method fails
	*/
	bool				CreateEntity(const Entity&);
	/**
	Forcefully inserts the specified entity.
	The method causes a fatal exception if the respective guid is already in use
	*/
	void				InsertEntity(const Entity&);
	Entity*				CreateEntity(const GUID&);
	Entity*				FindEntity(const GUID&);
	const Entity*		FindEntity(const GUID&)	const;
	void				RemoveEntity(const Entity *e);
	count_t				Count() const {return entities.size();}
	count_t				size() const {return entities.size();}
	void				Hash(Hasher&) const;
	iterator			begin() {return entities.begin();}
	iterator			end() {return entities.end();}
	const_iterator		begin() const {return entities.begin();}
	const_iterator		end() const {return entities.end();}
	void				Clear()	{ entities.clear();}
	bool				IsEmpty() const {return entities.IsEmpty();}
	bool				IsNotEmpty() const {return entities.IsNotEmpty();}
	void				Compact() {entities.Compact();}
	/**
	Inserts all entities contained by the specified other storage.
	All contained entities must not be defined in the local storage
	*/
	void				InsertAll(const EntityStorage&);

	bool				operator==(const EntityStorage&other) const;
	bool				operator!=(const EntityStorage&other) const	{return !operator==(other);}
	const Entity&		operator[](index_t i) const {return entities[i];}
};
