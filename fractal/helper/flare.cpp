#include "../../global_root.h"
#include "flare.h"



namespace Flare
{
	class CustomLightSource : public Engine::LightData
	{
	public:
		/**/		CustomLightSource(const FlareCenter&center)
		{
			Update(center);
		}
		/**/		CustomLightSource()	{}
		void		Update(const FlareCenter&center)
		{
			position = center.coordinates;
			diffuse.rgb = center.color;
			specular.rgb = center.color;
			ambient.rgb = center.color;
			size = center.size;
		}
	};




	static PointerTable<index_t>			light_map;
	static Buffer<LightInfo,0>				lights;
	static Array<CustomLightSource>			customLights;

	static bool								lights_changed = true,
											occlusion_queried = false;
	static Array<Engine::LightData*>		light_field;
	static Buffer<OpenGL::Query,32,AdoptStrategy>	query_objects;
	static OpenGL::Texture					sun_flare,
											hex_flare,
											solid_hex_flare,
											ring_flare,
											sharper_ring_flare,
											red_dot_flare,
											blue_dot_flare,
											dot_flare,
											sharper_dot_flare;
	static Buffer<TFlareSprite>				sprites;
	

	void _FetchFromDisplay()
	{
		display.getSceneLights(light_field,false);
	}
	void _FetchCustom()
	{
		light_field.setSize(customLights.count());
		for (index_t i = 0; i < customLights.count(); i++)
			light_field[i] = customLights + i;
	}
	static void (*_FetchLightSources)()	= _FetchFromDisplay;



	void			SignalLightsChanged()
	{
		lights_changed = true;
	}
	



	void			Map(const Camera<float>&camera, bool check_occlusion, bool (*isOccluded)(const Engine::LightData*))
	{
		float zNear,zFar;
		camera.ExtractDepthRange(zNear,zFar);
		
		float barrier = zNear + (zFar-zNear)*0.99;
		
		
		if (lights_changed)
		{
			_FetchLightSources();
			//if (!info->clipped)
				//ShowMessage(light_field.length());
			
			foreach (lights,light)
				light->tagged = false;
			
			index_t lightIndex;
			for (index_t i = 0; i < light_field.length(); i++)
				if (light_map.query(light_field[i],lightIndex))
					lights[lightIndex].tagged = true;
				else
				{
					lightIndex = lights.count();
					LightInfo&light = lights.append();
					light.reference = light_field[i];
					light.tagged = true;
					light_map.set(light_field[i],lightIndex);
				}
			for (index_t i = 0; i < lights.count(); i++)
				if (!lights[i].tagged)
				{
					light_map.Unset(lights[i].reference);
					lights.erase(i--);
				}
			while (lights.count() > query_objects.fillLevel())
				query_objects.append().create();
			
			lights_changed = false;
		}
		
		{
			if (check_occlusion)
				display.queryBegin();
			for (index_t i = 0; i < lights.count(); i++)
			{
				LightInfo&info = lights[i];
				LightData*light = info.reference;
				info.occluded = false;
				TVec3<> sample_point;
				info.was_clipped=info.clipped;
				switch (light->GetType())
				{
					case Light::Omni:
						info.clipped = !camera.PointToScreen(light->GetPosition(),info.projected);
						info.intensity = 1;
						info.distance = Vec::distance(camera.GetAbsoluteLocation(),light->GetPosition());
						sample_point = light->GetPosition();
					break;
					case Light::Direct:
					{
						info.clipped = !camera.VectorToScreen(light->GetPosition(),info.projected);	//the direction of a direct light is actually the position. But it's a direction, not a point
						info.intensity = 1;
						info.distance = 0.5*info.GetSize();
						if (!info.clipped)
						{
							Vec::mad(camera.GetAbsoluteLocation(),light->GetPosition(),barrier,sample_point);
							/*float projected[3];
							_c2(info->projected,projected);
							projected[2] = 0.95;
							camera.ReverseProject(projected,sample_point);*/
							//ShowMessage(_distance(sample_point,camera.GetAbsoluteLocation()));
						}
					}
					break;
					case Light::Spot:
					{
						TVec3<> projected_direction;
						info.clipped = !camera.PointToScreen(light->GetPosition(),info.projected);
						Mat::rotate(camera.view,light->GetSpotDirection(),projected_direction);
						info.intensity = vpow(vmax(projected_direction.z,0),light->GetSpotExponent());
						info.distance = Vec::distance(camera.GetAbsoluteLocation(),light->GetPosition());
						sample_point = light->GetPosition();
					}
					break;
					default:
						info.clipped = true;
				}
				if (!info.clipped)
					info.clipped = info.projected.x < -1.1 || info.projected.x > 1.1 || info.projected.y < -1.1 || info.projected.y > 1.1;
				info.intensity *= vmin(Vec::sum(light->GetDiffuse())/2.0f,1);
					
				if (!info.clipped && info.intensity > 0)
				{
					if (isOccluded)
						info.occluded = isOccluded(info.reference);
					else
						info.occluded = false;
					if (check_occlusion && !info.occluded)
						display.castPointQuery(query_objects[i],sample_point);
					/*else
						info->occluded = false;*/
				}
			}
			if (check_occlusion)
				display.queryEnd();
		}
		occlusion_queried = check_occlusion;
	}
	
	void			RecheckOcclusion(const Camera<>&camera)
	{
		float zNear,zFar;
		camera.ExtractDepthRange(zNear,zFar);
		
		float barrier = zNear + (zFar-zNear)*0.99;
		
		if (occlusion_queried)
			for (index_t i = 0; i < lights.count(); i++)
			{
				LightInfo&light = lights[i];
				if (!light.clipped && !light.occluded && light.intensity > 0)
				{
					light.occluded = !display.resolveQuery(query_objects[i]);
					//cout << "resolved occlusion ("<<(light->occluded?"true":"false")<<")"<<endl;
				}
			}
		
	
		display.queryBegin();
		for (index_t i = 0; i < lights.count(); i++)
		{
			LightInfo&info = lights[i];
			LightData*light = info.reference;
			if (info.clipped || info.occluded || info.intensity<=0)
				continue;
			TVec3<> sample_point;
			switch (light->GetType())
			{
				case Light::Omni:
					sample_point = light->GetPosition();
				break;
				case Light::Direct:
					Vec::mad(camera.GetAbsoluteLocation(),light->GetPosition(),barrier,sample_point);
				break;
				case Light::Spot:
					sample_point = light->GetPosition();
				break;
			}
			display.castPointQuery(query_objects[i],sample_point);
			//cout << "cast query ("<<zNear<<"->"<<zFar<<"=>"<<barrier<<") "<<_toString(sample_point)<<endl;
		}
		display.queryEnd();
		occlusion_queried = true;
	}
	
	static void	generateCentralFlareSprite(TFlareSprite&target, OpenGL::Texture&texture_target)
	{
		static Image	upper_blend(256,256,4),
						lower_blend(512,16,4),
						image(512,256,4);
		image.setChannel(0,255);
		image.setChannel(1,255);
		image.setChannel(2,255);
		image.setChannel(3,0);
		igRadial(lower_blend, CGColor(1,1,1,1), CGColor(1,1,1,0));
		igExtRadial(upper_blend,CGColor(1,1,1,1), CGColor(1,1,1,0));
		image.paint(&lower_blend,0,120);
		image.paint(&upper_blend,128,0);
		
		texture_target.load(image);
		target.texture = &texture_target;
		target.width = 2;
		target.height = 1;
		target.position = 1;
		target.intensity = 1;
		target.primary = true;
	}

	static void		generateInnerGlow(TFlareSprite&target, OpenGL::Texture&texture_target, float r, float g, float b, float intensity, float size, float position)
	{
		static Image	image(128,128,4);
		
		igRadial(image,CGColor(r,g,b),CGColor(r,g,b,0));
		texture_target.load(image);
		target.texture = &texture_target;
		target.width = target.height = size;
		target.position = 1.0-position;
		target.intensity = intensity;
		target.primary = false;
	}

	static void		generateOuterRing(TFlareSprite&target, OpenGL::Texture&texture_target, float r, float g, float b, float intensity, float size, float position)
	{
		static Image	image(128,128,4);
		
		igRing(image,0.3,CGColor(r,g,b,0),CGColor(r,g,b),CGColor(r,g,b,0),CGColor(r,g,b,0));
		texture_target.load(image);
		target.texture = &texture_target;
		target.width = target.height = size;
		target.position = 1.0-position;
		target.intensity = intensity;
		target.primary = false;
	}

	
	
	

	static TFlareSprite& placeSprite(float fc, float size, float intensity, const OpenGL::Texture&texture, bool primary = false)
	{
		TFlareSprite&sprite = sprites.append();
		sprite.texture = &texture;
		sprite.width = size;
		sprite.height = size;
		sprite.intensity = intensity;
		sprite.position = fc;
		sprite.primary = primary;
		sprite.sensitive = false;
		Vec::set(sprite.color,1);
		return sprite;
		/*
		float at[2];
		_mult2(sun,fc,at);
		_addValue2(at,1);
		_div2(at,2);
		display.useTexture(texture);
		float	w = size/display.pixelAspect(),
				h = size;
		glWhite(intensity*sun_intensity);
		glBegin(GL_QUADS);
			glTexCoord2f(0,0);glVertex2f(at[0]-w,at[1]-h);
			glTexCoord2f(1,0);glVertex2f(at[0]+w,at[1]-h);
			glTexCoord2f(1,1);glVertex2f(at[0]+w,at[1]+h);
			glTexCoord2f(0,1);glVertex2f(at[0]-w,at[1]+h);
		glEnd();
		display.useTexture(NULL);*/
	}


	static TFlareSprite& placeColoredSprite(float fc, float size, float r, float g, float b, float a, const OpenGL::Texture&texture)
	{
	
		TFlareSprite&sprite = sprites.append();
		sprite.texture = &texture;
		sprite.width = size;
		sprite.height = size;
		sprite.intensity = a;
		sprite.position = fc;
		sprite.primary = false;
		sprite.sensitive = false;
		Vec::def(sprite.color,r,g,b);
		return sprite;
	
	/*
		float at[2];
		_mult2(sun,fc,at);
		_addValue2(at,1);
		_div2(at,2);
		display.useTexture(texture);
		float	w = size/display.pixelAspect(),
				h = size;
		glColor4f(r,g,b,a*sun_intensity);
		glBegin(GL_QUADS);
			glTexCoord2f(0,0);glVertex2f(at[0]-w,at[1]-h);
			glTexCoord2f(1,0);glVertex2f(at[0]+w,at[1]-h);
			glTexCoord2f(1,1);glVertex2f(at[0]+w,at[1]+h);
			glTexCoord2f(0,1);glVertex2f(at[0]-w,at[1]+h);
		glEnd();
		display.useTexture(NULL);*/
	}

	static void placeColoredString(float fc0, float length, float size, float r, float g, float b, float a, const OpenGL::Texture&texture)
	{
		unsigned res = (unsigned)(2*length/size);
		if (!res)
			res = 1;
		a/=res;
		
			for (unsigned i = 0; i < res; i++)
			{
				TFlareSprite&sprite = sprites.append();
				sprite.texture = &texture;
				sprite.width = size;
				sprite.height = size;
				sprite.position = (fc0+(float)i/(res-1)*length);
				sprite.primary = false;
				Vec::def(sprite.color,r,g,b);			
				if (!i || i == res-1)
					sprite.intensity = a/2;
				else
					sprite.intensity = a;
				/*
				float at[2];
				_mult2(sun,fc0+(float)i/(res-1)*length,at);
				_addValue2(at,1);
				_div2(at,2);
				glTexCoord2f(0,0);glVertex2f(at[0]-w,at[1]-h);
				glTexCoord2f(1,0);glVertex2f(at[0]+w,at[1]-h);
				glTexCoord2f(1,1);glVertex2f(at[0]+w,at[1]+h);
				glTexCoord2f(0,1);glVertex2f(at[0]-w,at[1]+h);*/
			}
		
		placeColoredSprite(fc0+length/2,length/2,r,g,b,a*2,dot_flare);

	}
	
	
	void			Render(float ambient_light, float delta_t)
	{
	
		if (!sprites.fillLevel())
		{
			//sprites.resize(5);
			
			unsigned counter[8];
			memset(counter,0,sizeof(counter));
			
			Image	image(512,512,3);
			Image	image2(512,512,4);
			//image.clear();
			image.Fill(0,0,0);
			
			Random random;
			
			for (unsigned i = 0; i < 10; i++)
			{
				float angle = random.getFloat();
				TVec3<> c = {0.5f,random.getFloat()*0.25f+0.5f,random.getFloat()+0.5f};
				Vec::setLen0(c,1.2f);
				Vec::clamp(c,0,1);
				
				while (counter[(unsigned)(8*angle)] > i/8)
					angle = random.getFloat();
				counter[(unsigned)(8*angle)]++;
				float width = random.getFloat(0,12);
				igSolarStripe(image2, 0.0002, cos(angle*M_PI), sin(angle*M_PI),  width ,sqr(random.getFloat(0.5,1))/(width/2+1),CGColor(c.red,c.green,c.blue,1),CGColor(c.red,c.green,c.blue,0));
				image.paint(&image2,0,0,Image::AlphaAdd);
			}
			
			
			igExtRadial(image2,CGColor(1.15,1.15,1.15),CGColor(0,0,0));
			
			image.paint(&image2,0,0,Image::Add);

			sun_flare.load(image);
			
			igRing(image,0.1,CGColor(1,1,1,0),CGColor(1,0.5,0.5,1),CGColor(0.5,0.5,1,0.5),CGColor(1,1,1,0));
			
			ring_flare.load(image);
			
			igRing(image,0.05,CGColor(1,1,1,0),CGColor(1,0.5,0.5,1),CGColor(0.5,0.5,1,0.5),CGColor(1,1,1,0));
			
			sharper_ring_flare.load(image);
			
			
			
			image.setSize(256,256,4);
			
			igHexagon(image,0.3,CGColor(1,1,1,-0.1),CGColor(1,1,1,1),CGColor(1,1,1,-0.1));
			
			hex_flare.load(image);
			

			igExtRadial(image,CGColor(1.0,0.3,0.2,1),CGColor(0,0,0,1),false);
			red_dot_flare.load(image);
			
			igExtRadial(image,CGColor(0.2,0.6,1.0,1),CGColor(0,0,0,1),false);
			blue_dot_flare.load(image);
			
			
			igRing(image,0.5,CGColor(1,1,1,0),CGColor(1,1,1,0),CGColor(1,1,1,1),CGColor(1,1,1,1));
			dot_flare.load(image);
			
			igRing(image,0.2,CGColor(1,1,1,0),CGColor(1,1,1,0),CGColor(1,1,1,1),CGColor(1,1,1,1));
			sharper_dot_flare.load(image);

			
			
			
			
			
			
			
			placeSprite(1,0.5,0.8,sun_flare,true).sensitive = true;
			placeSprite(1,0.15,0.02,sharper_ring_flare);
			placeColoredSprite(1.105,0.1,1,0,0,0.07,hex_flare).sensitive = true;
			placeColoredSprite(1.08,0.07,1,0,0,0.07,hex_flare).sensitive = true;
			placeSprite(-0.75,0.15,0.01,ring_flare);
		
			placeSprite(-1.3,0.08,0.6,blue_dot_flare);
			placeSprite(-1.31,0.08,0.6,red_dot_flare);
			placeSprite(-1.35,0.04,0.03,sharper_dot_flare);
			placeSprite(-1.2,0.025,0.01,sharper_dot_flare);
		
			placeColoredSprite(0.75,0.007,0.5,1.1,0.5,0.09,dot_flare);
			placeColoredSprite(0.743,0.007,0.5,1.1,0.5,0.09,dot_flare);
	
			placeColoredString(0.53,0.05,0.003,0.5,1.1,0.5,0.2,dot_flare);
			placeColoredString(0.44,0.03,0.0038,0.5,1.1,0.5,0.3,dot_flare);
			placeColoredString(0.42,0.02,0.0038,1.1,0.3,0.3,0.3,dot_flare);
		

			placeColoredString(0.36,0.05,0.004,0.5,1.1,0.5,0.15,dot_flare);
		

			placeColoredSprite(0.24,0.004,0.5,1.1,0.5,0.18,dot_flare);
			placeColoredSprite(0.12,0.003,0.5,1.1,0.5,0.35,dot_flare);			
			
			
		}

		display.unbindAll();
		
		tCTVertex c0,c1,c2,c3;
		c0.w = 1;
		c0.z = 0;
		Vec::set(c0.color,1);
		c1 = c2 = c3 = c0;
		Vec::def(c0.coord,0,0);
		Vec::def(c1.coord,1,0);
		Vec::def(c2.coord,1,1);
		Vec::def(c3.coord,0,1);

		display.disableLighting();
		display.setFlareBlendFunc();
		
		glMatrixMode(GL_PROJECTION);
			glPushMatrix();
			glLoadIdentity();
		glMatrixMode(GL_TEXTURE);
			glPushMatrix();
			glLoadIdentity();
		glMatrixMode(GL_MODELVIEW);
			glPushMatrix();
			glLoadIdentity();
		
		index_t i = 0;
		foreach (lights,light)
		{
			if (!light->clipped && !light->occluded && light->intensity > 0 && occlusion_queried)
			{
				light->occluded = !display.resolveQuery(query_objects[i]);
				//cout << "resolved occlusion ("<<(light->occluded?"true":"false")<<")"<<endl;
			}
			i++;
			
			if (!light->occluded)
			{
				if (light->was_clipped)
				{
					light->visibility = 1;
					light->was_clipped = false;
				}
				else
					light->visibility = vmin(light->visibility+delta_t*2,1);
				//cout << "not occluded"<<endl;
			}
			elif (!light->clipped)
			{
				if (light->was_clipped)
				{
					light->visibility = 0;
					light->was_clipped = false;
				}
				else
					light->visibility = vmax(light->visibility-delta_t*2,0);
				//cout << "occluded"<<endl;
			}
			//cout << light->visibility << "/"<<light->intensity<<" @"<<_toString(light->projected,2)<<endl;
			if (light->clipped || light->visibility*light->intensity <= getError<float>())
				continue;
			float intensity = vmax(light->visibility*light->intensity*(1.5f-Vec::length(light->projected)),0)/2;
			c0.color.rgb = light->reference->GetDiffuse();
			TVec3<> base_color;
			TVec4<>	color;
			
			base_color.red = 1.0-((1.0-c0.color.red)*0.7);
			base_color.green = 1.0-((1.0-c0.color.green)*0.7);
			base_color.blue = 1.0-((1.0-c0.color.blue)*0.7);
			//_mult(c0.color,1.2);
			//_clamp(c0.color,0,1);
			//cout << _toString(base_color)<<endl;
			float base_size = light->visibility*light->intensity*vmin(Vec::dot(c0.color.rgb)*light->GetSize()/light->distance,2.0);//(light->reference->getConstantAttenuation()+light->reference->getLinearAttenuation()*light->distance+light->reference->getQuadraticAttenuation()*sqr(light->distance));
			for (index_t j = 0; j < sprites.fillLevel(); j++)
			{
				if (sprites[j].texture->IsEmpty() || !sprites[j].primary)
					continue;
				TVec2<OpenGL::FloatType> at;
				Vec::mult(light->projected,sprites[j].position,at);
				float	w = sprites[j].width/display.pixelAspect(),
						h = sprites[j].height;
				if (true) //sprites[j].distance_dependent)
				{
					w*=base_size;
					h*=base_size;
				}
				
				
				Vec::def(c0.xy,at.x-w,at.y-h);
				Vec::def(c1.xy,at.x+w,at.y-h);
				Vec::def(c2.xy,at.x+w,at.y+h);
				Vec::def(c3.xy,at.x-w,at.y+h);
				//*vmax(1.2f-_length2(light->projected)*(float)!sprites[j].primary,0)/1.2
				color.alpha = light->visibility*light->intensity*sprites[j].intensity;
				if (sprites[j].sensitive)
				{
					color.alpha *= clamped((1.2-ambient_light),0,1.2);
				}
				Vec::stretch(base_color,sprites[j].color,color.rgb);
				glColor4fv(color.v);
				display.useTexture(sprites[j].texture);
				/*if (!j)
					cout << display.renderState()<<endl;*/
				glBegin(GL_QUADS);
					glTexCoord2f(0,0); glVertex2fv(c0.v);
					glTexCoord2f(1,0); glVertex2fv(c1.v);
					glTexCoord2f(1,1); glVertex2fv(c2.v);
					glTexCoord2f(0,1); glVertex2fv(c3.v);
				glEnd();
			}
		}
		display.setDefaultBlendFunc();
		display.useTexture(NULL);
		//flares_rendered = true;
		glMatrixMode(GL_MODELVIEW);
			glPopMatrix();
		glMatrixMode(GL_TEXTURE);
			glPopMatrix();
		glMatrixMode(GL_PROJECTION);
			glPopMatrix();
		glMatrixMode(GL_MODELVIEW);
		
		occlusion_queried = false;
	
	
	
	}

	void			Rerender(const Camera<float>&camera, float screen_center_x, float screen_center_y, float screen_scale,float ambient_light)
	{
	
		if (!sprites.fillLevel())
			return;

		display.unbindAll();
		
		TVec2<float> screen_center = {screen_center_x,screen_center_y};
		
		
		
		
		
		
		
		
		
		foreach (lights,info)
		{
			LightData*light = info->reference;
			switch (light->GetType())
			{
				case Light::Omni:
				case Light::Spot:
					camera.PointToScreen(light->GetPosition(),info->projected);
				break;
				case Light::Direct:
					camera.VectorToScreen(light->GetPosition(),info->projected);	//the direction of a direct light is actually the position. But it's a direction, not a point
				break;
			}
		}
		
		
		tCTVertex c0,c1,c2,c3;
		c0.w = 1;
		c0.z = 0;
		Vec::set(c0.color,1);
		c1 = c2 = c3 = c0;
		Vec::def(c0.coord,0,0);
		Vec::def(c1.coord,1,0);
		Vec::def(c2.coord,1,1);
		Vec::def(c3.coord,0,1);

		display.disableLighting();
		display.setFlareBlendFunc();
		
		glMatrixMode(GL_PROJECTION);
			glPushMatrix();
			glLoadIdentity();
		glMatrixMode(GL_TEXTURE);
			glPushMatrix();
			glLoadIdentity();
		glMatrixMode(GL_MODELVIEW);
			glPushMatrix();
			glLoadIdentity();
		
		
		foreach (lights,light)
		{
			if (light->clipped || light->visibility*light->intensity <= getError<float>())
				continue;
			float intensity = vmax(light->visibility*light->intensity*(1.5f-Vec::distance(light->projected,screen_center)*screen_scale),0)/2;
			c0.color.rgb = light->reference->GetDiffuse();
			TVec3<> base_color;
			TVec4<>	color;
			
			base_color.red = 1.0-((1.0-c0.color.red)*0.7);
			base_color.green = 1.0-((1.0-c0.color.green)*0.7);
			base_color.blue = 1.0-((1.0-c0.color.blue)*0.7);
			//_mult(c0.color,1.2);
			//_clamp(c0.color,0,1);
			float base_size = light->visibility*light->intensity*vmin(Vec::dot(c0.color.rgb)*light->GetSize()/light->distance,2.0);//(light->reference->getConstantAttenuation()+light->reference->getLinearAttenuation()*light->distance+light->reference->getQuadraticAttenuation()*sqr(light->distance));
			for (index_t j = 0; j < sprites.fillLevel(); j++)
			{
				if (sprites[j].texture->IsEmpty())
					continue;
				TVec2<OpenGL::FloatType> at,vec;
				Vec::sub(light->projected,screen_center,vec);
				Vec::mad(screen_center,vec,sprites[j].position,at);
				float	w = sprites[j].width/display.pixelAspect()/screen_scale,
						h = sprites[j].height/screen_scale;
				if (true) //sprites[j].distance_dependent)
				{
					w*=base_size;
					h*=base_size;
				}
				
				
				Vec::def(c0.xy,at.x-w,at.y-h);
				Vec::def(c1.xy,at.x+w,at.y-h);
				Vec::def(c2.xy,at.x+w,at.y+h);
				Vec::def(c3.xy,at.x-w,at.y+h);
				//*vmax(1.2f-_length2(light->projected)*(float)!sprites[j].primary,0)/1.2
				color.alpha = light->visibility*light->intensity*sprites[j].intensity;
				if (sprites[j].sensitive)
				{
					color.alpha *= clamped((1.2-ambient_light),0,1.2);
				}
				Vec::stretch(base_color,sprites[j].color,color.rgb);
				glColor4fv(color.v);
				display.useTexture(sprites[j].texture);
				glBegin(GL_QUADS);
					glTexCoord2f(0,0); glVertex2fv(c0.v);
					glTexCoord2f(1,0); glVertex2fv(c1.v);
					glTexCoord2f(1,1); glVertex2fv(c2.v);
					glTexCoord2f(0,1); glVertex2fv(c3.v);
				glEnd();
			}
		}
		display.setDefaultBlendFunc();
		display.useTexture(NULL);
		//flares_rendered = true;
		glMatrixMode(GL_MODELVIEW);
			glPopMatrix();
		glMatrixMode(GL_TEXTURE);
			glPopMatrix();
		glMatrixMode(GL_PROJECTION);
			glPopMatrix();
		glMatrixMode(GL_MODELVIEW);

	
	
	
	}



	void			SetFlareSequence(const FlareCenter*centers, count_t numCenters)
	{
		customLights.setSize(numCenters);
		for (index_t i = 0; i < numCenters; i++)
			customLights[i].Update(centers[i]);
		_FetchLightSources = _FetchCustom;
		lights_changed = true;
	}


}
