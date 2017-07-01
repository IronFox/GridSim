#ifndef engine_particle_factoryTplH
#define engine_particle_factoryTplH


namespace Engine
{

	template <class GL, class Particle>
		void	Particles<GL,Particle>::setup()
		{
			material.setLayers(1);
			material.layers[0].combiner = 0x2100;
			material.layers[0].clamp_x = false;
			material.layers[0].clamp_y = false;
			Vec::set(material.diffuse,1);
			Vec::set(material.specular.rgb,0.6);
			material.specular.alpha = 1.f;
		}

	template <class GL, class Particle>
		void	Particles<GL,Particle>::setTexture(const typename GL::Texture::Reference&texture)
		{
			material.textures[0] = texture;
		}


	template <class GL, class Particle>
		void  Particles<GL,Particle>::renderAsLines(const TVec3<>&delta0, const TVec3<>&delta1)
		{
			if (!renderer)
			{
				setRenderer(GL::global_instance);
				if (!renderer)
					FATAL__("trying to render without renderer");
			}


			renderer->bindMaterialIgnoreShader(material);
	    
			TTextureVertex	c0,c1,c2,c3,
							c4,c5,c6,c7;
			c0.w = c1.w = c2.w = c3.w = c4.w = c5.w = c6.w = c7.w = 1.f;
			Vec::def(c0.coord,0,0);
			Vec::def(c1.coord,1,0);
			Vec::def(c2.coord,1,1);
			Vec::def(c3.coord,0,1);
	    
			Vec::def(c4.coord,0,0);
			Vec::def(c5.coord,1,0);
			Vec::def(c6.coord,1,1);
			Vec::def(c7.coord,0,1);
	    
			TVec3<> p0,p1,p2,p3,p4,p5,p6,p7;

			{
				TVec3<> d,horz,vertical;
				Vec::sub(delta1,delta0,d);
				Vec::normalize0(d);
				Vec::crossVertical(d,horz);
				Vec::cross(horz,d,vertical);
				Vec::normalize0(vertical);
				Vec::normalize0(horz);

				Vec::add(d,vertical,p0);
				Vec::mult(p0,-1);
	        
				Vec::sub(d,vertical,p1);
	        
				Vec::add(d,vertical,p2);
	        
				Vec::sub(vertical,d,p3);
	        
				Vec::add(d,horz,p4);
				Vec::mult(p4,-1);

				Vec::sub(d,horz,p5);

				Vec::add(d,horz,p6);

				Vec::sub(horz,d,p7);
			}

			for (typename Super::iterator particle = Super::begin(); particle != Super::end(); ++particle)
			{
				TVec3<> center0,center1,d,horz,vertical;
				Vec::add((particle)->position,delta0,center0);
				Vec::add((particle)->position,delta1,center1);
	        
				Vec::mad(center0,p0,(particle)->size,c0.xyz);
				Vec::mad(center1,p1,(particle)->size,c1.xyz);
				Vec::mad(center1,p2,(particle)->size,c2.xyz);
				Vec::mad(center0,p3,(particle)->size,c3.xyz);
	        
				Vec::mad(center0,p4,(particle)->size,c4.xyz);
				Vec::mad(center1,p5,(particle)->size,c5.xyz);
				Vec::mad(center1,p6,(particle)->size,c6.xyz);
				Vec::mad(center0,p7,(particle)->size,c7.xyz);


				GL::face(c0,c1,c2,c3);
				GL::face(c4,c5,c6,c7);

			}
			renderer->unbindAll();
		}


	template <class GL, class Particle>
		template <class Float>
			void Particles<GL,Particle>::render(const Aspect<Float>&aspect)
			{
				if (!renderer)
				{
					setRenderer(GL::global_instance);
					if (!renderer)
						FATAL__("trying to render without renderer");
				}

				renderer->bindMaterialIgnoreShader(material);

				TMatrix4<typename GL::FloatType>matrix;
				TVec3<typename GL::FloatType>	corner0 = {-1,-1,0},
												corner1 = {1,-1,0},
												corner2 = {1,1,0},
												corner3 = {-1,1,0};
				TTextureVertex c0,c1,c2,c3;
				c0.w = c1.w = c2.w = c3.w = 1.f;
				Vec::def(c0.coord,0,0);
				Vec::def(c1.coord,1,0);
				Vec::def(c2.coord,1,1);
				Vec::def(c3.coord,0,1);

				Mat::copyOrientation(aspect.viewInvert,matrix);
				Vec::clear(matrix.w.xyz);
				matrix.w.w = 1.f;
				GL::enterSubSystem(matrix);
	    
					for (typename Super::iterator particle = Super::begin(); particle != Super::end(); ++particle)
					{
						typename TVec3<GL::FloatType> final;
						Mat::rotate(aspect.view,(particle)->position,final);
						Vec::mad(final,corner0,(particle)->size,c0.xyz);
						Vec::mad(final,corner1,(particle)->size,c1.xyz);
						Vec::mad(final,corner2,(particle)->size,c2.xyz);
						Vec::mad(final,corner3,(particle)->size,c3.xyz);
						GL::face(c0,c1,c2,c3);
					}

				GL::exitSubSystem();
				renderer->unbindAll();
			}

	template <class GL, class Particle>
		void Particles<GL,Particle>::translate(const TVec3<>&delta)
		{
			for (typename Super::iterator particle = Super::begin(); particle != Super::end(); ++particle)
				Vec::add((particle)->position,delta);
		}

	template <class GL, class Particle>
		void Particles<GL,Particle>::translate(float dx, float dy, float dz)
		{
			TVec3<> delta = {dx,dy,dz};
			translate(delta);
		}

	template <class GL, class Particle>
		Particle& Particles<GL,Particle>::add(const TVec3<>&position, float size)
		{
			Particle&particle = Super::append();
			Vec::copy(position,particle.xyz);
			particle.size = size;
			return particle;
		}

	template <class GL, class Particle> 
		void Particles<GL,Particle>::setRenderer(GL*renderer_)
		{
			if (renderer_ == renderer)
				return;
			renderer = renderer_;
		}
		
	template <class GL, class Particle> 
		void Particles<GL,Particle>::setRenderer(GL&renderer)
		{
			setRenderer(&renderer);
		}
		
	template <class GL, class Particle> 
		GL*	Particles<GL,Particle>::getRenderer()	const
		{
			return renderer;
		}



	template <class GL, typename Particle>
		template <class Float>
			void ColoredParticles<GL, Particle>::render(const Aspect<Float>&aspect)
			{
				if (!renderer)
				{
					setRenderer(GL::global_instance,false);
					if (!renderer)
						FATAL__("trying to render without renderer");
				}

				renderer->bindMaterialIgnoreShader(material);

				TMatrix4<typename GL::FloatType>matrix;
				TVec3<typename GL::FloatType>	corner0 = {-1,-1,0},
												corner1 = {1,-1,0},
												corner2 = {1,1,0},
												corner3 = {-1,1,0};
				TColorTextureVertex c0,c1,c2,c3;
				c0.w = c1.w = c2.w = c3.w = 1.f;
				Vec::def(c0.coord,0,0);
				Vec::def(c1.coord,1,0);
				Vec::def(c2.coord,1,1);
				Vec::def(c3.coord,0,1);


				Mat::copyOrientation(aspect.modelviewInvert,matrix);
				Vec::clear(matrix.w.xyz);
				matrix.w.w = 1.f;
				GL::enterSubSystem(matrix);

				for (typename Super::iterator particle = Super::begin(); particle != Super::end(); ++particle)
				{
					typename TVec3<GL::FloatType>  final;
					Mat::rotate(aspect.modelview,particle->position,final);
					Vec::mad(final,corner0,particle->size,c0.xyz);
					Vec::mad(final,corner1,particle->size,c1.xyz);
					Vec::mad(final,corner2,particle->size,c2.xyz);
					Vec::mad(final,corner3,particle->size,c3.xyz);
					Vec::copy(particle->color[0],c2.color);
					Vec::copy(particle->color[1],c3.color);
					Vec::copy(particle->color[2],c0.color);
					Vec::copy(particle->color[3],c1.color);
					GL::face(c0,c1,c2,c3);
				}

				GL::exitSubSystem();
				renderer->unbindAll();
			}


	template <class GL, typename Particle>
		template <class Float>
			void ColoredParticles<GL, Particle>::render(const Aspect<Float>&aspect, const TVec3<>&color_override)
			{
				if (!renderer)
				{
					setRenderer(GL::global_instance);
					if (!renderer)
						FATAL__("trying to render without renderer");
				}

				renderer->bindMaterialIgnoreShader(material);

				TMatrix4<typename GL::FloatType>matrix;
				TVec3<typename GL::FloatType>	corner0 = {-1,-1,0},
												corner1 = {1,-1,0},
												corner2 = {1,1,0},
												corner3 = {-1,1,0};
				TColorTextureVertex c0,c1,c2,c3;
				c0.w = c1.w = c2.w = c3.w = 1.f;
				Vec::def(c0.coord,0,0);
				Vec::def(c1.coord,1,0);
				Vec::def(c2.coord,1,1);
				Vec::def(c3.coord,0,1);


				Mat::copyOrientation(aspect.modelviewInvert,matrix);
				Vec::clear(matrix.w.xyz);
				matrix.w.w = 1.f;
				GL::enterSubSystem(matrix);

				for (typename Super::iterator particle = Super::begin(); particle != Super::end(); ++particle)
				{
					typename TVec3<GL::FloatType>  final;
					Mat::rotate(aspect.modelview,particle->position,final);
					Vec::mad(final,corner0,particle->size,c0.xyz);
					Vec::mad(final,corner1,particle->size,c1.xyz);
					Vec::mad(final,corner2,particle->size,c2.xyz);
					Vec::mad(final,corner3,particle->size,c3.xyz);
					Vec::stretch(particle->color[0],color_override,c2.color);
					Vec::stretch(particle->color[1],color_override,c3.color);
					Vec::stretch(particle->color[2],color_override,c0.color);
					Vec::stretch(particle->color[3],color_override,c1.color);
					GL::face(c0,c1,c2,c3);
				}

				GL::exitSubSystem();
				renderer->unbindAll();
			}


}

#endif
