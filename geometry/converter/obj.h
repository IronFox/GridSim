#ifndef objH
#define objH

/******************************************************************

E:\include\importer\obj.h

******************************************************************/

#include "../converter.h"
#include "../../io/file_system.h"
#include "../../io/string_file.h"
#include "../../io/log.h"
#include "../../container/buffer.h"
#include "../../container/hashtable.h"
#include "../../container/arraylist.h"
#include "../../container/string_list.h"
#include "../../container/binarytree.h"
#include "../../image/converter/magic.h"
#include "../../general//orthographic_comparator.h"

namespace Converter
{
	inline hash_t floatToHash(float v)
	{
		return (hash_t)((INT64)fmod(v/TypeInfo<float>::error,std::numeric_limits<INT64>::max()));
	}


	typedef CGS::StdDef	Def;

	struct TFace;
		struct TObjFace;
	class PoolVertex;
		class ObjPoolVertex;
	class ObjFaceGroup;

	class ObjMaterial;

	class ObjTexture;



	class PoolVertex
	{
	public:
			#undef index
			TVec3<Def::FloatType>		p;
			//index_t				index;
			
								PoolVertex() /*:index(UNSIGNED_UNDEF)*/
								{
									p = Vector<Def::FloatType>::zero;
								}
								PoolVertex(Def::FloatType x, Def::FloatType y, Def::FloatType z) /*:index(UNSIGNED_UNDEF)*/
								{
									Vec::def(p,x,y,z);
								}
								PoolVertex(const TVec3<Def::FloatType>&p_) /*:index(UNSIGNED_UNDEF)*/
								{
									p = p_;
								}

			bool				operator>(const PoolVertex&other)	const throw()
								{
									return Vec::compare(p,other.p)>0;
								}
			bool				operator<(const PoolVertex&other)	const throw()
								{
									return Vec::compare(p,other.p)<0;
								}
			int					compareTo(const PoolVertex&other) const throw()
								{
									return Vec::compare(p,other.p);
								}
			bool				operator==(const PoolVertex&other) const throw()
								{
									return Vec::similar(p,other.p);
								}
			bool				operator!=(const PoolVertex&other) const throw()
								{
									return !Vec::similar(p,other.p);
								}
			hash_t				toHash()	const
								{
									return HashValue(floatToHash(p.x))
											.add(floatToHash(p.y))
											.add(floatToHash(p.z));
								}
	};

	class IndexedPoolVertex : public PoolVertex
	{
	public:
			index_t				index;
								IndexedPoolVertex() :index(InvalidIndex)
								{
									p = Vector<Def::FloatType>::zero;
								}
								IndexedPoolVertex(Def::FloatType x, Def::FloatType y, Def::FloatType z) :index(InvalidIndex)
								{
									Vec::def(p,x,y,z);
								}
								IndexedPoolVertex(const TVec3<Def::FloatType>&p_) :index(InvalidIndex)
								{
									p = p_;
								}

			bool				operator>(const PoolVertex&other)	const throw()
								{
									return Vec::compare(p,other.p)>0;
								}
			bool				operator<(const PoolVertex&other)	const throw()
								{
									return Vec::compare(p,other.p)<0;
								}
			bool				operator==(const PoolVertex&other) const throw()
								{
									return Vec::similar(p,other.p);
								}
			bool				operator!=(const PoolVertex&other) const throw()
								{
									return !Vec::similar(p,other.p);
								}

	};

	class ObjVertexRoot
	{};

	class ObjVertexNT:public ObjVertexRoot
	{
	public:
			TVec3<Def::FloatType>	p,n;
			TVec2<Def::FloatType>	t;
								
			//UINT32						index;

			
								ObjVertexNT()
								{}
								
								ObjVertexNT(Def::FloatType v)
								{
									Vec::set(p,v);
									Vec::set(n,v);
									Vec::set(t,v);
								}
								
								ObjVertexNT(const TVec3<Def::FloatType>&p_, const TVec3<Def::FloatType>&n_, const TVec2<Def::FloatType>&t_)
								{
									p = p_;
									n = n_;
									t = t_;
								}
			bool				operator>(const ObjVertexNT&other)	const throw()
								{
									return compareTo(other)>0;
								}
			bool				operator<(const ObjVertexNT&other)	const throw()
								{
									return compareTo(other)<0;
								}
			bool				operator==(const ObjVertexNT&other) const throw()
								{
									return Vec::similar(p,other.p) && Vec::similar(n,other.n) && Vec::similar(t,other.t);
								}
			bool				operator!=(const ObjVertexNT&other) const throw()
								{
									return !Vec::similar(p,other.p) || !Vec::similar(n,other.n) || !Vec::similar(t,other.t);
								}
			int					compareTo(const ObjVertexNT&other) const throw()
								{
									OrthographicComparison cmp(Vec::compare(p,other.p));
									if (!cmp.decided())
										cmp.addComparisonResult(Vec::compare(n,other.n));
									if (!cmp.decided())
										cmp.addComparisonResult(Vec::compare(t,other.t));
									return cmp;
								}
			hash_t				toHash()	const
								{
									return HashValue(floatToHash(p.x))
											.add(floatToHash(p.y))
											.add(floatToHash(p.z))
											.add(floatToHash(n.x))
											.add(floatToHash(n.y))
											.add(floatToHash(n.z))
											.add(floatToHash(t.x))
											.add(floatToHash(t.y));
								}

	};

	class ObjVertexN:public ObjVertexRoot
	{
	public:
			TVec3<Def::FloatType>p,n;
			//UINT32					index;
			
			
								ObjVertexN()
								{}
								
								ObjVertexN(Def::FloatType v)
								{
									Vec::set(p,v);
									Vec::set(n,v);
								}
															
								
				
								
								ObjVertexN(const TVec3<Def::FloatType>&p_, const TVec3<Def::FloatType>&n_)
								{
									p = p_;
									n = n_;
								}
			bool				operator>(const ObjVertexN&other)	const throw()
								{
									return compareTo(other)>0;
								}
			bool				operator<(const ObjVertexN&other)	const throw()
								{
									return compareTo(other)<0;
								}
			bool				operator==(const ObjVertexN&other)	const throw()
								{
									return Vec::similar(p,other.p) && Vec::similar(n,other.n);
								}
			bool				operator!=(const ObjVertexN&other)	const throw()
								{
									return !Vec::similar(p,other.p) || !Vec::similar(n,other.n);
								}
			int					compareTo(const ObjVertexN&other) const throw()
								{
									return OrthographicComparison(Vec::compare(p,other.p)).addComparisonResult(Vec::compare(n,other.n));
								}
			hash_t				toHash()	const
								{
									return HashValue(floatToHash(p.x))
											.add(floatToHash(p.y))
											.add(floatToHash(p.z))
											.add(floatToHash(n.x))
											.add(floatToHash(n.y))
											.add(floatToHash(n.z));
								}
	};


	struct TObjTriangle
	{
			UINT32			v[3];
	};

	struct TObjTriangleT
	{
			UINT32			v[3];
	};

	struct TObjQuad
	{
			UINT32			v[4];
	};

	struct TObjQuadT
	{
			UINT32			v[4];
	};

	struct TPoolTriangle
	{
			UINT32			v[3];
	};

	struct TPoolQuad
	{
			UINT32			v[4];
	};


	class ObjFaceGroup
	{
	public:
			CGS::SubGeometryA<>							*target;
			String										name;
			Buffer<TObjFace>							face_buffer;
	};

	struct VertexHash
	{
	template <typename T>
	static	hash_t		hash(const T&vertex)		{return vertex.toHash();}
	};

	template <typename Vertex>
		class VertexHashTable : public GenericHashTable<Vertex,UINT32,VertexHash>
		{
		public:
				typedef GenericHashTable<Vertex,UINT32,VertexHash> Super;
				/*
										Vec::stretch(q.v[0]->t,m->scale.xy,q.v[0]->t);
						Vec::stretch(q.v[1]->t,m->scale.xy,q.v[1]->t);
						Vec::stretch(q.v[2]->t,m->scale.xy,q.v[2]->t);
*/
				//Buffer<Vertex>									list;

				UINT32											add(const Vertex&v)
				{
					UINT32 rs;
					if (Super::query(v,rs))
						return rs;
					rs = (UINT32)Super::count();
					//list << v;
					Super::set(v,rs);
					return rs;
				}
				UINT32											add(const Vertex&v, UINT32 index)
				{
					UINT32 rs;
					if (Super::query(v,rs))
						return rs;
					rs = index;
					//list << v;
					Super::set(v,rs);
					return rs;
				}

				void											exportToField(Array<Vertex>&out)	const
				{
					out.setSize(Super::count());
					{
						Array<Vertex>		out_field;
						Array<index_t>		out_index_field;
						Super::exportTo(out_field,out_index_field);
						Concurrency::parallel_for((index_t)0,out_index_field.length(),[&out,&out_field,&out_index_field](index_t i)
						{
							out[out_index_field[i]] = out_field[i];
						});
					}
				}


		};
}
DECLARE_T__(Converter::VertexHashTable,Swappable);

namespace Converter
{
	class ObjMaterialSection
	{
	public:
			ObjFaceGroup								*target;
			VertexHashTable<ObjVertexNT>				tex_vertices;
			VertexHashTable<ObjVertexN>					vertices;
			//Sorted<List::Vector<ObjVertexNT>,OperatorSort>	tex_vertices;
			//Sorted<List::Vector<ObjVertexN>,OperatorSort>	vertices;
			
			Buffer<TObjTriangle>						triangles;
			Buffer<TObjTriangleT>						tex_triangles;
			Buffer<TObjQuad>							quads;
			Buffer<TObjQuadT>							tex_quads;
			
														ObjMaterialSection():target(NULL)
														{}
	};

	class ObjTexture
	{
	public:
			CGS::TextureA								data,
														*target;
	};

	class ObjMaterial
	{
	public:
			String										name;
			List::Vector<ObjMaterialSection>			sections;
			ObjMaterialSection*							selected_section;
			ObjTexture									*texture,
														*normal_map;
			TVec3<>										scale;
			CGS::MaterialInfo							info;
			
														ObjMaterial():selected_section(NULL),texture(NULL),normal_map(NULL)
														{
															scale = Vector<>::one;
														}
	};

	struct TFace
	{
			TVec4<IndexedPoolVertex*>	v,
										n,
										t;
	};

	struct TObjFace
	{
			int					v[4], n[4], t[4];
			unsigned			smooth_group;
			ObjMaterial			*material;
	};




	class ObjConverter:public Root
	{
	protected:

			Buffer<TVec3<Def::FloatType> >		vertexBuffer,
												normal_buffer;
			Buffer<TVec2<Def::FloatType> >		texcoord_buffer;
			CFSFolder							object_system;
			List::Vector<ObjFaceGroup>			group_buffer;
			ObjFaceGroup						*current_group;
			unsigned							current_smooth_group, smooth_groups;
			IndexTable<unsigned>				smooth_group_table;
			// ArrayList<TObjFace,512> 			*current_smooth_group;
			// unsigned							current_smooth_group_index;
			List::Vector<ObjMaterial>			material_list;
			StringTable<ObjMaterial*>			material_table;
			StringTable<ObjFaceGroup*>			object_name_table;
			StringSet							object_name_table64, 
												texture_name_table64;
			ObjMaterial*						current_material;
			HashContainer<ObjTexture>			texture_field;
			UINT32								broken_vertex_links,
												broken_normal_links,
												broken_texcoord_links;
			
			PointerTable<String>				texture_names,alpha_names;
			

			
			void				loadObjColor(const String&line,TVec4<Def::FloatType>&target) throw();
			String				parseMap(CFSFolder&folder, const String&line, TVec3<>&scale) throw();
			void				loadObjMaterialLibrary(const CFSFile*file) throw();
			void				parseObjPoint(Buffer<TVec3<Def::FloatType> >&target, char*start, char*end) throw();
			void				parseObjPoint(Buffer<TVec2<Def::FloatType> >&target, char*start, char*end) throw();
			void				parseObjFaceVertex(char*start, char*end, int&v, int&n, int&t) throw();
			void				parseObjFace(char*start, char*end) throw();
			void				parseObjLine(char*start, char*end) throw();
			ObjTexture*		loadObjTexture(const String&color_map, const String&alpha_map) throw();
			ObjTexture*		loadObjNormalMap(const String&normal_map) throw();
			void				resetImport() throw();
				
	static	void				writeObject(const CGS::Geometry<>&geometry, const Array<PointerContainer<Array<TFace> > >&conversion_table,const CGS::SubGeometryA<>&object,StringFile&out) throw();
			void				exportTexture(const CGS::TextureA*texture, const String&outname, StringFile&out) throw();

	inline	void				logMessage(const String&message) throw()
								{
									if (log_file)
										(*log_file) << "OBJ: "<<message<<nl;
								}
			
	inline	void				sendMessage(const String&msg) throw()
								{
									current_callback->converterProgressUpdate(msg);
								}
	inline	void				setProgress(float progress) throw()
								{
									current_callback->converterProgressBarUpdate(progress);
								}
								
	inline int					positiveIndex(int index, count_t count)	const throw()
								{
									if (index >= 0)
										return index;
									return (int)count+index+1;
								}

	inline TVec3<Def::FloatType>*getVertex(int index) throw()
								{
									if (!index)
										return NULL;
									return vertexBuffer+(index-1);
								}
	inline TVec3<Def::FloatType>*getNormal(int index) throw()
								{
									if (!index)
										return NULL;
									if (index > 0)
										return normal_buffer+(index-1);
									return normal_buffer+normal_buffer.fillLevel()+index;
								}
	inline TVec2<Def::FloatType>*getTexcoord(int index) throw()
								{
									if (!index)
										return NULL;
									if (index > 0)
										return texcoord_buffer+(index-1);
									return texcoord_buffer+texcoord_buffer.fillLevel()+index;
								}
								
	virtual	bool				read(CGS::Geometry<>&target, const String&filename) throw();
	virtual	bool				write(const CGS::Geometry<>&source, const String&filename) throw();
	
	public:
			LogFile				*log_file;
			
								ObjConverter():log_file(NULL)
								{}
	
	};

	extern ObjConverter 		obj_converter;
}


#endif
