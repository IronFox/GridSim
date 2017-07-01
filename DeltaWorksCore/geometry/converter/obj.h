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
#include "../../container/string_list.h"
#include "../../image/converter/magic.h"
#include "../../general//orthographic_comparator.h"

namespace DeltaWorks
{

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
			friend hash_t		Hash(const PoolVertex&v)
								{
									return HashValue(floatToHash(v.p.x))
											.Add(floatToHash(v.p.y))
											.Add(floatToHash(v.p.z));
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

		typedef std::set<IndexedPoolVertex>	PoolSet;


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
									if (!cmp.IsDecided())
										cmp.AddComparisonResult(Vec::compare(n,other.n));
									if (!cmp.IsDecided())
										cmp.AddComparisonResult(Vec::compare(t,other.t));
									return cmp;
								}
			friend hash_t		Hash(const ObjVertexNT&v)
								{
									return HashValue(floatToHash(v.p.x))
											.Add(floatToHash(v.p.y))
											.Add(floatToHash(v.p.z))
											.Add(floatToHash(v.n.x))
											.Add(floatToHash(v.n.y))
											.Add(floatToHash(v.n.z))
											.Add(floatToHash(v.t.x))
											.Add(floatToHash(v.t.y));
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
									return OrthographicComparison(Vec::compare(p,other.p)).AddComparisonResult(Vec::compare(n,other.n));
								}
			friend hash_t		Hash(const ObjVertexN&v)
								{
									return HashValue(floatToHash(v.p.x))
											.Add(floatToHash(v.p.y))
											.Add(floatToHash(v.p.z))
											.Add(floatToHash(v.n.x))
											.Add(floatToHash(v.n.y))
											.Add(floatToHash(v.n.z));
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
			CGS::SubGeometryA<>					*target;
			String								name;
			Container::Buffer<TObjFace>			face_buffer;
		};

		typedef std::shared_ptr<ObjFaceGroup>	PObjFaceGroup;


		template <typename Vertex>
			class VertexHashTable : public Container::GenericHashTable<Vertex,UINT32>
			{
			public:
				typedef Container::GenericHashTable<Vertex,UINT32> Super;
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
					out.SetSize(Super::count());
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

	namespace Converter
	{
		class ObjMaterialSection
		{
		public:
			PObjFaceGroup								target;
			VertexHashTable<ObjVertexNT>				tex_vertices;
			VertexHashTable<ObjVertexN>					vertices;
			//Sorted<List::Vector<ObjVertexNT>,OperatorSort>	tex_vertices;
			//Sorted<List::Vector<ObjVertexN>,OperatorSort>	vertices;
			
			Container::Buffer<TObjTriangle>				triangles;
			Container::Buffer<TObjTriangleT>			tex_triangles;
			Container::Buffer<TObjQuad>					quads;
			Container::Buffer<TObjQuadT>				tex_quads;
			
														ObjMaterialSection():target(NULL)
														{}
		};

		typedef std::shared_ptr<ObjMaterialSection>		PObjMaterialSection;

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
			Container::Vector0<PObjMaterialSection>		sections;
			PObjMaterialSection							selected_section;
			ObjTexture									*texture,
														*normal_map;
			TVec3<>										scale;
			CGS::MaterialInfo							info;
			
														ObjMaterial():selected_section(NULL),texture(NULL),normal_map(NULL)
														{
															scale = Vector<>::one;
														}
		};
		typedef std::shared_ptr<ObjMaterial>	PObjMaterial;


		struct ItQuad
		{
			PoolSet::iterator	v0,v1,v2,v3;
			count_t				set = 0;

			/**/				ItQuad()	{}
			/**/				ItQuad(const PoolSet::iterator&it0, const PoolSet::iterator&it1, const PoolSet::iterator&it2):v0(it0),v1(it1),v2(it2),set(3)	{}
			/**/				ItQuad(const PoolSet::iterator&it0, const PoolSet::iterator&it1, const PoolSet::iterator&it2, const PoolSet::iterator&it3):v0(it0),v1(it1),v2(it2),v3(it3),set(4)	{}

			bool				IsSet(index_t k) const {return k < set;}
			PoolSet::iterator	Get(index_t k) const
			{
				switch (k)
				{
					case 0:
					return v0;
					case 1:
					return v1;
					case 2:
					return v2;
					default:
					return v3;
				}

			}
		};

		struct TFace
		{
			ItQuad	v,n,t;
		};

		struct TObjFace
		{
			int					v[4], n[4], t[4];
			unsigned			smooth_group;
			PObjMaterial		material;
		};




		class ObjConverter:public Root
		{
		protected:

			Container::Buffer<TVec3<Def::FloatType> >		vertexBuffer,
												normal_buffer;
			Container::Buffer<TVec2<Def::FloatType> >		texcoord_buffer;
			CFSFolder							object_system;
			Container::Vector0<PObjFaceGroup>	group_buffer;
			PObjFaceGroup						current_group;
			unsigned							current_smooth_group, smooth_groups;
			Container::IndexTable<unsigned>		smooth_group_table;
			// ArrayList<TObjFace,512> 			*current_smooth_group;
			// unsigned							current_smooth_group_index;
			Container::Vector0<PObjMaterial>		material_list;
			Container::StringTable<PObjMaterial>	material_table;
			Container::StringTable<PObjFaceGroup>	object_name_table;
			Container::StringSet					object_name_table64, 
												texture_name_table64;
			PObjMaterial						current_material;
			Container::GenericHashContainer<PathString,ObjTexture>
												texture_field;
			UINT32								broken_vertex_links,
												broken_normal_links,
												broken_texcoord_links;
			
			Container::PointerTable<String>		texture_names,alpha_names;
			

			
			void				LoadObjColor(const String&line,TVec4<Def::FloatType>&target) throw();
			PathString			ParseMap(CFSFolder&folder, const String&line, TVec3<>&scale) throw();
			void				LoadObjMaterialLibrary(const CFSFile*file) throw();
			void				parseObjPoint(Container::Buffer<TVec3<Def::FloatType> >&target, char*start, char*end) throw();
			void				parseObjPoint(Container::Buffer<TVec2<Def::FloatType> >&target, char*start, char*end) throw();
			void				parseObjFaceVertex(char*start, char*end, int&v, int&n, int&t) throw();
			void				parseObjFace(char*start, char*end) throw();
			void				parseObjLine(char*start, char*end) throw();
			ObjTexture*			LoadObjTexture(const PathString&color_map, const PathString&alpha_map) throw();
			ObjTexture*			LoadObjNormalMap(const PathString&normal_map) throw();
			void				resetImport() throw();
				
			static void			writeObject(const CGS::Geometry<>&geometry, const Array<Container::PointerContainer<Array<TFace> > >&conversion_table,const CGS::SubGeometryA<>&object,StringFile&out) throw();
			void				ExportTexture(const CGS::TextureA*texture, const PathString&outname, StringFile&out) throw();

			inline void			logMessage(const String&message) throw()
			{
				if (log_file)
					(*log_file) << "OBJ: "<<message<<nl;
			}
			
			inline void			sendMessage(const String&msg) throw()
			{
				current_callback->converterProgressUpdate(msg);
			}
			inline void			setProgress(float progress) throw()
			{
				current_callback->converterProgressBarUpdate(progress);
			}
								
			inline int			positiveIndex(int index, count_t count)	const throw()
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
			inline TVec3<Def::FloatType>*GetNormal(int index) throw()
			{
				if (!index)
					return NULL;
				if (index > 0)
					return normal_buffer+(index-1);
				return normal_buffer+normal_buffer.Count()+index;
			}
			inline TVec2<Def::FloatType>*getTexcoord(int index) throw()
			{
				if (!index)
					return NULL;
				if (index > 0)
					return texcoord_buffer+(index-1);
				return texcoord_buffer+texcoord_buffer.Count()+index;
			}
								
			virtual	bool			Read(CGS::Geometry<>&target, const PathString&filename) throw() override;
			virtual	bool			Write(const CGS::Geometry<>&source, const PathString&filename) throw() override;
	
		public:
			LogFile					*log_file;
			
			/**/					ObjConverter():log_file(NULL)
									{}
	
		};

		extern ObjConverter 		obj_converter;
	}
}

DECLARE_T__(Converter::VertexHashTable,Swappable);

#endif
