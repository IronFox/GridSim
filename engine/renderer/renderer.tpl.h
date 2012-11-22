#ifndef rendererTplH
#define rendererTplH

namespace Engine
{

	#if 0
	template <class GL>
		void	convert(const CMaterialConfiguration&config, const GL::Texture*list, CGS::CGraphMaterial&target)
		{
			CBuffer<CGS::CGraphMaterial::CNode,2>	nodes;
			
			nodes.appendAdopt().setType(CGS::CGraphMaterial::type.def[CGS::CGraphMaterial::NodeType::Phong]);
			
			CBuffer<CGS::CGraphMaterial::CNode,2>	normal_nodes;
		
			if (list)
				for (unsigned i = 0; i < config.layers(); i++)
				{
					if (list[i].isEmpty())
						continue;
					const TMaterialLayer&layer = config.layer(i);
					
					if (layer.texcoord.length && ((layer.flags&CustomSystemFlag)!=0))
						additional_transformations << "gl_TexCoord["<<i<<"] = gl_TextureMatrix["<<i<<"] * gl_MultiTexCoord"<<i<<";\n";
					if (layer.flags&NormalMapFlag)
					{
						CGS::CGraphMaterial::CNode&node = normal_nodes.appendAdopt();
						switch (list[i].textureType())
						{
							case Texture1D:
								node.setType(CGS::CGraphMaterial::type.def[CGS::CGraphMaterial::NodeType::Lookup1d]);
							break;
							case Texture2D:
								node.setType(CGS::CGraphMaterial::type.def[CGS::CGraphMaterial::NodeType::Lookup2d]);
							break;
							case TextureCube:
								node.setType(CGS::CGraphMaterial::type.def[CGS::CGraphMaterial::NodeType::LookupCube]);
							break;
						
						
						
						
						num_normal_maps++;
						fragment_transformations << "normal += ";
						if (layer.flags&TangentNormalFlag)
						{
							code_out.requires_tangents = true;
							fragment_transformations << "tangent_space * (";
						}
						else
							fragment_transformations << "gl_NormalMatrix * (";
						CString sampled = sampleTexture(i,list[i].textureType());
						fragment_transformations << sampled<<".rgb*2.0-1.0";
						fragment_transformations << ");\n";
					}
				}
		
		
		
		
		
		}




	#endif
}





#endif
