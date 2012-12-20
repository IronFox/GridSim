#include "../../global_root.h"
#include "components.h"



namespace Engine
{
	namespace GUI
	{
	
		Layout								Button::global_layout,
											Panel::global_layout,
											Edit::global_layout,
											ComboBox::global_layout,
											Menu::global_layout;
		ScrollBarLayout						ScrollBarLayout::global;
		CheckBox::TStyle					CheckBox::global_style;
		SliderLayout						SliderLayout::global;

	
		static void fillRect(const Rect<float>&rect)
		{
			glBegin(GL_QUADS);
				glVertex2f(rect.left,rect.bottom);
				glVertex2f(rect.right,rect.bottom);
				glVertex2f(rect.right,rect.top);
				glVertex2f(rect.left,rect.top);
			glEnd();
		}

	
	
		bool		Panel::getChildSpace(Rect<float>&out_rect)	const
		{
			if (children.isEmpty())
				return false;
			const shared_ptr<Component>&first = children.first();
			out_rect = first->current_region;
				
			for (index_t i = 1; i < children.count(); i++)
			{
				const shared_ptr<Component>&child = children[i];
				out_rect.include(child->current_region);
			}
			return true;
		}
		
		void			Panel::append(const shared_ptr<Component>&component)
		{
			component->anchored.set(true,false,false,true);
			if (children.isNotEmpty())
				component->offset.top = children.last()->offset.top-children.last()->height;
			else
				component->offset.top = 0;
			component->offset.left = 0;
			component->setWindow(window_link);
			children << component;
		}
		
		void			Panel::appendRight(const shared_ptr<Component>&component)
		{
			if (children.isNotEmpty())
			{
				component->anchored = children.last()->anchored;
				component->offset.top = children.last()->offset.top;
				component->offset.left = children.last()->offset.left+children.last()->width;
			}
			else
			{
				component->anchored.set(true,false,false,true);
				component->offset.top = 0;
				component->offset.left = 0;
			}
			component->setWindow(window_link);
			children << component;

			shared_ptr<Window> wnd = window();
			if (wnd)
				wnd->layout_changed = true;

		}
		
		bool					Panel::add(const shared_ptr<Component>&component)
		{
			if (!component || children.contains(component))
				return false;
			children << component;
			component->setWindow(window_link);
			return true;
		}
		
		bool					Panel::erase(const shared_ptr<Component>&component)
		{
			return children.findAndErase(component);
		}
		
		bool					Panel::erase(index_t index)
		{
			if (index < children.count())
			{
				children.erase(index);
				return true;
			}
			return false;
		}
		
		
		bool					Panel::moveUp(const shared_ptr<Component>&component)
		{
			index_t index;
			if ((index = children.indexOf(component)) != -1)
				return moveUp(index);
			return false;
		}
		
		bool					Panel::moveUp(index_t index)
		{
			if (index+1 >= children.count())
				return false;
			Buffer<shared_ptr<Component>,4>::AppliedStrategy::swap(children[index],children[index+1]);
			return true;
		}
		

		
		bool					Panel::moveDown(const shared_ptr<Component>&component)
		{
			index_t index;
			if ((index = children.indexOf(component)) != -1)
				return moveDown(index);
			return false;
		}
		
		bool					Panel::moveDown(index_t index)
		{
			if (!index || index >= children.count())
				return false;
			Buffer<shared_ptr<Component>,4>::AppliedStrategy::swap(children[index-1],children[index]);
			return true;
		}

		bool					Panel::moveTop(const shared_ptr<Component>&component)
		{
			index_t index;
			if ((index = children.indexOf(component))!=-1)
				return moveTop(index);
			return false;
		}
		
		bool					Panel::moveTop(index_t index)
		{
			if (index+1 < children.count())
			{
				shared_ptr<Component> cmp = children[index];
				children.erase(index);
				children << cmp;
				return true;
			}
			return false;
		}
		
		bool					Panel::moveBottom(const shared_ptr<Component>&component)
		{
			index_t index;
			if ((index = children.indexOf(component)) != -1)
				return moveBottom(index);
			return false;
		}
		
		bool					Panel::moveBottom(index_t index)
		{
			if (index && index < children.count())
			{
				shared_ptr<Component> cmp = children[index];
				children.erase(index);
				children.insert(0,cmp);
				return true;
			}
			return false;
		}
		
		void		Panel::updateLayout(const Rect<float>&parent_region)
		{
			Component::updateLayout(parent_region);
			for (index_t i = 0; i < children.count(); i++)
				children[i]->updateLayout(cell_layout.client);
		}
		
		float		Panel::clientMinWidth()	const
		{
			float rs = 0;
			
			for (index_t i = 0; i < children.count(); i++)
			{
				float c = children[i]->minWidth(true);
				if (c > rs)
					rs = c;
			}
			return rs;
		}
		
		float		Panel::clientMinHeight()	const
		{
			float rs = 0;
			
			for (index_t i = 0; i < children.count(); i++)
			{
				float c = children[i]->minHeight(true);
				if (c > rs)
					rs = c;
			}
			return rs;
		}
		
		void		Panel::onColorPaint()
		{
			Component::onColorPaint();
			shared_ptr<Operator>	op = requireOperator();

			op->focus(cell_layout.client);
			for (index_t i = 0; i < children.count(); i++)
				if (children[i]->visible)
					children[i]->onColorPaint();
			op->unfocus();
		}
		
		void		Panel::onNormalPaint()
		{
			Component::onNormalPaint();
			shared_ptr<Operator>	op = requireOperator();
			
			op->focus(cell_layout.client);
			for (index_t i = 0; i < children.count(); i++)
				if (children[i]->visible)
					children[i]->onNormalPaint();
			op->unfocus();
		}
		
		Component::eEventResult		Panel::onMouseHover(float x, float y, TExtEventResult&ext)
		{
			for (index_t i = children.count()-1; i < children.count(); i--)
			{
				const shared_ptr<Component>&child = children[i];
				if (child->visible && child->cell_layout.border.contains(x,y))
				{
					if (!child->enabled)
						return Unsupported;
					return child->onMouseHover(x,y,ext);
				}
			}
			return Unsupported;
		}
		
		Component::eEventResult		Panel::onMouseDown(float x, float y, TExtEventResult&ext)
		{
			for (index_t i = children.count()-1; i < children.count(); i--)
			{
				const shared_ptr<Component>&child = children[i];
				if (child->visible && child->cell_layout.border.contains(x,y))
				{
					if (child->enabled)
						return child->onMouseDown(x,y,ext);
					return Unsupported;
				}
			}
			return Unsupported;
		}
		
		Component::eEventResult	Panel::onMouseWheel(float x, float y, short delta)
		{
			for (index_t i = children.count()-1; i < children.count(); i--)
			{
				const shared_ptr<Component>&child = children[i];
				if (child->visible && child->enabled && child->current_region.contains(x,y))
					return child->onMouseWheel(x,y,delta);
			}
			return Unsupported;
		}
		

		
		void	ScrollBarLayout::loadFromFile(const String&filename, float scale)
		{
			/*
				TTexture				back_center,
										back_bottom,
										cursor_center,
										cursor_bottom,
										bottom_button;
				float					button_indent;
			*/
			
			FileSystem::Folder	folder(FileSystem::extractFilePath(filename));
			
			XML::Container	xml;
			xml.loadFromFile(filename);
				

			String attrib;
			
			XML::Node	*xback = xml.find("scrollbar/background"),
						*xcursor = xml.find("scrollbar/cursor"),
						*xbutton = xml.find("scrollbar/button");
			if (!xback || !xcursor || !xbutton)
				throw IO::DriveAccess::FileFormatFault(globalString(__func__": Required XML nodes scrollbar/background, scrollbar/cursor, and/or scrollbar/button not found"));

			TTexture::load(xback,"center",folder,back_center,scale);

			TTexture::load(xback,"bottom",folder,back_bottom,scale);
			TTexture::load(xcursor,"center",folder,cursor_center,scale);
			TTexture::load(xcursor,"bottom",folder,cursor_bottom,scale);
			TTexture::load(xbutton,"background",folder,bottom_button,scale);

			if (!xbutton->query("indent",attrib) || !convert(attrib.c_str(),button_indent))
				throw IO::DriveAccess::FileFormatFault(globalString(__func__": Failed to find or parse scrollbar/button attribute 'indent'"));
			
			
			button_indent *= scale;
			min_width = vmax(vmax(vmax(back_center.width,back_bottom.width),vmax(cursor_center.width,cursor_bottom.width)),bottom_button.width);
			min_height = 2*(bottom_button.height-button_indent+back_bottom.height);
		}
		
		
		
		void	SliderLayout::loadFromFile(const String&filename, float scale)
		{
			/*
				TTexture				back_center,
										back_bottom,
										cursor_center,
										cursor_bottom,
										bottom_button;
				float					button_indent;
			*/
			
			FileSystem::Folder	folder(FileSystem::extractFilePath(filename));
			
			XML::Container	xml;
			xml.loadFromFile(filename);
				

			String attrib;
			
			XML::Node	*xbar = xml.find("slider/bar"),
						*xslider = xml.find("slider/slider");
			if (!xbar || !xslider)
				throw IO::DriveAccess::FileFormatFault(globalString(__func__": Required XML nodes slider/bar and/or slider/slider not found"));

			TTexture::load(xbar,"left",folder,bar_left,scale);
			TTexture::load(xbar,"center",folder,bar_center,scale);
			TTexture::load(xslider,"background",folder,slider,scale);

			min_width = vmax(2*bar_left.width+bar_center.width,slider.width);
			min_height = vmax(vmax(bar_left.height,bar_center.height),slider.height);
		}
		
		
		
		
		
		
		
		
				
		void		Slider::setup()
		{
			cursor_grabbed = false;
			slider_layout = &SliderLayout::global;
			current = 0;
			max = 1;
		}
		

		void		Slider::onSlide()
		{
			on_slide();
		}
		
		
		
		
		void		Slider::updateCursorRegion()
		{
			if (!slider_layout)
				return;
			float width = slider_layout->slider.width;
			slide_from = cell_layout.client.left+width/2;
			slide_range = cell_layout.client.right-cell_layout.client.left-width;
			float center = slide_from+current/max*slide_range;
			cursor_region.left = center-width/2;
			cursor_region.right = center+width/2;
			cursor_region.top = cell_layout.client.centerY()+slider_layout->slider.height/2;
			cursor_region.bottom = cursor_region.top-slider_layout->slider.height;
			slider.region = cursor_region;			
		}
		
		
		
		void		Slider::updateLayout(const Rect<float>&parent_region)
		{
			Component::updateLayout(parent_region);
			updateCursorRegion();
			if (slider_layout)
			{
				bar_left.region = cell_layout.client;
				bar_left.region.right = bar_left.region.left+slider_layout->bar_left.width;
				bar_left.region.top = cell_layout.client.centerY()+slider_layout->bar_left.height/2;
				bar_left.region.bottom = bar_left.region.top-slider_layout->bar_left.height;
				bar_left.color = &slider_layout->bar_left.color;
				bar_left.normal = &slider_layout->bar_left.normal;
				bar_left.orientation = 0;
				
				bar_right.region = cell_layout.client;
				bar_right.region.left = bar_right.region.right-slider_layout->bar_left.width;
				bar_right.region.top = cell_layout.client.centerY()+slider_layout->bar_left.height/2;
				bar_right.region.bottom = bar_right.region.top-slider_layout->bar_left.height;
				bar_right.color = &slider_layout->bar_left.color;
				bar_right.normal = &slider_layout->bar_left.normal;
				bar_right.orientation = 2;
				
				
				bar_center.region = cell_layout.client;
				bar_center.region.left = bar_left.region.right;
				bar_center.region.right = bar_right.region.left;
				bar_center.region.top = cell_layout.client.centerY()+slider_layout->bar_center.height/2;
				bar_center.region.bottom = bar_center.region.top-slider_layout->bar_center.height;
				bar_center.color = &slider_layout->bar_center.color;
				bar_center.normal = &slider_layout->bar_center.normal;
				bar_center.orientation = 0;
				
				//ShowMessage(bar_center.region.toString());
				
				slider.region = cursor_region;
				slider.color = &slider_layout->slider.color;
				slider.normal = &slider_layout->slider.normal;
				slider.orientation = 0;
			}
		}
		
		float		Slider::clientMinWidth()	const
		{
			if (!slider_layout)
				return 0;
			return slider_layout->min_width;
		}
		
		float		Slider::clientMinHeight()	const
		{
			if (!slider_layout)
				return 0;
			return slider_layout->min_height;
		}
		
		

		void		Slider::onColorPaint()
		{
			glDisable(GL_BLEND);
			glWhite();
			Component::onColorPaint();

			Display<OpenGL>&display = requireOperator()->getDisplay();
			glWhite();
			TFreeCell::paintColor(display,bar_left);
			TFreeCell::paintColor(display,bar_center);
			TFreeCell::paintColor(display,bar_right);
			
			/*if (cursor_grabbed)
				glWhite(0.7);
			else
				glWhite();*/
			
			TFreeCell::paintColor(display,slider);
			
		}
		
		void		Slider::onNormalPaint()
		{
			Component::onNormalPaint();

			Display<OpenGL>&display = requireOperator()->getDisplay();
			TFreeCell::paintNormal(display,bar_left,false);
			TFreeCell::paintNormal(display,bar_center,false);
			TFreeCell::paintNormal(display,bar_right,false);
			
			TFreeCell::paintNormal(display,slider,cursor_grabbed);
		}
		
		Component::eEventResult	Slider::onMouseDrag(float x, float y)
		{
			if (cursor_grabbed)
			{
				float	relative = x-cursor_region.left,
						delta = (relative-cursor_hook[0])*max/slide_range;
				current = clamped(current+delta,0,max);
				
				updateCursorRegion();
				
				onSlide();
				return RequestingRepaint;
			}
			return Handled;
		}
		
		Component::eEventResult	Slider::onMouseDown(float x, float y, TExtEventResult&ext)
		{
			if (!enabled)
				return Unsupported;
			ext.caught_by = shared_from_this();
			if (cursor_region.contains(x,y) && max > 0)
			{
				cursor_hook[0] = x-cursor_region.left;
				cursor_hook[1] = y-cursor_region.bottom;
				cursor_grabbed = true;
				return RequestingRepaint;
			}
			return Unsupported;
				
		}
		
		Component::eEventResult		Slider::onMouseUp(float x, float y)
		{
			bool changed =  cursor_grabbed;
			cursor_grabbed = false;
			return changed?RequestingRepaint:Handled;
		}
		
		Component::eEventResult		Slider::onMouseWheel(float x, float y, short delta)
		{
			float new_current = clamped(current + delta/max,0,1);

			if (current != new_current)
			{
				current = new_current;
				updateCursorRegion();
				onSlide();
				return RequestingRepaint;
			}
			return Handled;
		}
		
		
		
		
		
		
		
		
		
		
				
		
		void		ScrollBar::setup()
		{
			scrollable.reset();
			horizontal = false;
			up_pressed = false;
			down_pressed = false;
			cursor_grabbed = false;
			scroll_layout = &ScrollBarLayout::global;
			auto_visibility = false;
		}
		
		void		ScrollBar::onScroll()
		{
			if (shared_ptr<Scrollable> scrollable_ = scrollable.lock())
			{
				if (horizontal)
				{
					scrollable_->horizontal = scroll_data;
					scrollable_->onHorizontalScroll();
				}
				else
				{
					scrollable_->vertical = scroll_data;
					scrollable_->onVerticalScroll();
				}
			}
			on_scroll();
		}
		
		
		
		void		ScrollBar::updateLayout(const Rect<float>&parent_region)
		{
			Component::updateLayout(parent_region);
			if (scroll_layout)
			{
				float 	current = clamped(scroll_data.current,0,1);
				if (!horizontal)
				{
					up_button.region = cell_layout.client;
					up_button.region.bottom = up_button.region.top-scroll_layout->bottom_button.height;
					up_button.color = &scroll_layout->bottom_button.color;
					up_button.normal = &scroll_layout->bottom_button.normal;
					up_button.orientation = 2;
					
					down_button.region = cell_layout.client;
					down_button.region.top = down_button.region.bottom+scroll_layout->bottom_button.height;
					down_button.color = &scroll_layout->bottom_button.color;
					down_button.normal = &scroll_layout->bottom_button.normal;
					down_button.orientation = 0;
				
					background_top.region = cell_layout.client;
					background_top.region.bottom = background_top.region.top-scroll_layout->back_bottom.height;
					background_top.color = &scroll_layout->back_bottom.color;
					background_top.normal = &scroll_layout->back_bottom.normal;
					background_top.orientation = 2;
					
					
					background_bottom.region = cell_layout.client;
					background_bottom.region.top = background_bottom.region.bottom+scroll_layout->back_bottom.height;
					background_bottom.color = &scroll_layout->back_bottom.color;
					background_bottom.normal = &scroll_layout->back_bottom.normal;
					background_bottom.orientation = 0;
					
					background_center.region = cell_layout.client;
					background_center.region.top = background_top.region.bottom;
					background_center.region.bottom = background_bottom.region.top;
					background_center.color = &scroll_layout->back_center.color;
					background_center.normal = &scroll_layout->back_center.normal;
					background_center.orientation = 0;

					
					cursor_range = cell_layout.client.height()-2*scroll_layout->bottom_button.height+2*scroll_layout->button_indent;
					float	display_range = scroll_data.max-scroll_data.min,
							cursor_length = display_range>scroll_data.window?cursor_range*scroll_data.window/display_range:cursor_range;
					
					if (cursor_length < 2*scroll_layout->cursor_bottom.height)
						cursor_length = 2*scroll_layout->cursor_bottom.height;
					cursor_range -= cursor_length;
					cursor_top.region.left = cell_layout.client.left;
					cursor_top.region.right = cell_layout.client.right;
					cursor_top.region.top = cell_layout.client.top-scroll_layout->bottom_button.height+scroll_layout->button_indent-cursor_range*current;
					cursor_top.region.bottom = cursor_top.region.top-scroll_layout->cursor_bottom.height;
					cursor_top.color = &scroll_layout->cursor_bottom.color;
					cursor_top.normal = &scroll_layout->cursor_bottom.normal;
					cursor_top.orientation = 2;
					
					cursor_center.region.left = cell_layout.client.left;
					cursor_center.region.right = cell_layout.client.right;
					cursor_center.region.top = cursor_top.region.bottom;
					cursor_center.region.bottom = cursor_center.region.top-cursor_length+2*scroll_layout->cursor_bottom.height;
					cursor_center.color = &scroll_layout->cursor_center.color;
					cursor_center.normal = &scroll_layout->cursor_center.normal;
					cursor_center.orientation = 0;
					
					cursor_bottom.region.left = cell_layout.client.left;
					cursor_bottom.region.right = cell_layout.client.right;
					cursor_bottom.region.top = cursor_center.region.bottom;
					cursor_bottom.region.bottom = cursor_bottom.region.top-scroll_layout->cursor_bottom.height;
					cursor_bottom.color = &scroll_layout->cursor_bottom.color;
					cursor_bottom.normal = &scroll_layout->cursor_bottom.normal;
					cursor_bottom.orientation = 0;
					
					cursor_region.set(cell_layout.client.left,cursor_bottom.region.bottom,cell_layout.client.right,cursor_top.region.top);
				}
				else
				{
					current = 1.0f-current;
					down_button.region = cell_layout.client;
					down_button.region.left = down_button.region.right-scroll_layout->bottom_button.height;
					down_button.color = &scroll_layout->bottom_button.color;
					down_button.normal = &scroll_layout->bottom_button.normal;
					down_button.orientation = 3;
					
					up_button.region = cell_layout.client;
					up_button.region.right = up_button.region.left+scroll_layout->bottom_button.height;
					up_button.color = &scroll_layout->bottom_button.color;
					up_button.normal = &scroll_layout->bottom_button.normal;
					up_button.orientation = 1;
				
					background_top.region = cell_layout.client;
					background_top.region.left = background_top.region.right-scroll_layout->back_bottom.height;
					background_top.color = &scroll_layout->back_bottom.color;
					background_top.normal = &scroll_layout->back_bottom.normal;
					background_top.orientation = 3;
					
					
					background_bottom.region = cell_layout.client;
					background_bottom.region.right = background_bottom.region.left+scroll_layout->back_bottom.height;
					background_bottom.color = &scroll_layout->back_bottom.color;
					background_bottom.normal = &scroll_layout->back_bottom.normal;
					background_bottom.orientation = 1;
					
					background_center.region = cell_layout.client;
					background_center.region.right = background_top.region.left;
					background_center.region.left = background_bottom.region.right;
					background_center.color = &scroll_layout->back_center.color;
					background_center.normal = &scroll_layout->back_center.normal;
					background_center.orientation = 1;
					
					
					cursor_range = cell_layout.client.width()-2*scroll_layout->bottom_button.height+2*scroll_layout->button_indent;
					float	display_range = scroll_data.max-scroll_data.min,
							cursor_length = display_range>scroll_data.window?cursor_range*scroll_data.window/display_range:cursor_range;
					
					if (cursor_length < 2*scroll_layout->cursor_bottom.height)
						cursor_length = 2*scroll_layout->cursor_bottom.height;
					cursor_range -= cursor_length;
					cursor_top.region.top = cell_layout.client.top;
					cursor_top.region.bottom = cell_layout.client.bottom;
					cursor_top.region.right = cell_layout.client.right-scroll_layout->bottom_button.height+scroll_layout->button_indent-cursor_range*current;
					cursor_top.region.left = cursor_top.region.right-scroll_layout->cursor_bottom.height;
					cursor_top.color = &scroll_layout->cursor_bottom.color;
					cursor_top.normal = &scroll_layout->cursor_bottom.normal;
					cursor_top.orientation = 3;
					
					cursor_center.region.bottom = cell_layout.client.bottom;
					cursor_center.region.top = cell_layout.client.top;
					cursor_center.region.right = cursor_top.region.left;
					cursor_center.region.left = cursor_center.region.right-cursor_length+2*scroll_layout->cursor_bottom.height;
					cursor_center.color = &scroll_layout->cursor_center.color;
					cursor_center.normal = &scroll_layout->cursor_center.normal;
					cursor_center.orientation = 1;
					
					cursor_bottom.region.bottom = cell_layout.client.bottom;
					cursor_bottom.region.top = cell_layout.client.top;
					cursor_bottom.region.right = cursor_center.region.left;
					cursor_bottom.region.left = cursor_bottom.region.right-scroll_layout->cursor_bottom.height;
					cursor_bottom.color = &scroll_layout->cursor_bottom.color;
					cursor_bottom.normal = &scroll_layout->cursor_bottom.normal;
					cursor_bottom.orientation = 1;
					
					cursor_region.set(cursor_bottom.region.left,cell_layout.client.bottom,cursor_top.region.right,cell_layout.client.top);
				}
			
			}
		}
		
		float		ScrollBar::clientMinWidth()	const
		{
			if (!scroll_layout)
				return 0;
			if (horizontal)
				return scroll_layout->min_height;
			return scroll_layout->min_width;
		}
		
		float		ScrollBar::clientMinHeight()	const
		{
			if (!scroll_layout)
				return 0;
			if (horizontal)
				return scroll_layout->min_width;
			return scroll_layout->min_height;
		}
		
		

		void		ScrollBar::onColorPaint()
		{
			glWhite();
			Component::onColorPaint();
			glEnable(GL_BLEND);


			//display->setDefaultBlendFunc();
			Display<OpenGL>&display = requireOperator()->getDisplay();
			
			TFreeCell::paintColor(display,background_top);
			TFreeCell::paintColor(display,background_center);
			TFreeCell::paintColor(display,background_bottom);
			
			if (cursor_grabbed)
				glWhite(0.7);
			else
				glWhite();
			TFreeCell::paintColor(display,cursor_top);
			TFreeCell::paintColor(display,cursor_center);
			TFreeCell::paintColor(display,cursor_bottom);
			
			//if (up_pressed)
				glWhite();
			/*else
				glWhite(0.7);*/
			TFreeCell::paintColor(display,up_button);
			
			/*if (down_pressed)
				glWhite();
			else
				glWhite(0.7);*/
			TFreeCell::paintColor(display,down_button);
		}
		
		void		ScrollBar::onNormalPaint()
		{
			Component::onNormalPaint();

			Display<OpenGL>&display = requireOperator()->getDisplay();


			TFreeCell::paintNormal(display,background_top,false);
			TFreeCell::paintNormal(display,background_center,false);
			TFreeCell::paintNormal(display,background_bottom,false);
			
			TFreeCell::paintNormal(display,cursor_top,cursor_grabbed);
			TFreeCell::paintNormal(display,cursor_center,cursor_grabbed);
			TFreeCell::paintNormal(display,cursor_bottom,cursor_grabbed);
			
			TFreeCell::paintNormal(display,up_button,false);
			TFreeCell::paintNormal(display,down_button,false);
		
		}
		
		Component::eEventResult	ScrollBar::onMouseDrag(float x, float y)
		{
			if (cursor_grabbed && cursor_range > 0)
			{
				if (horizontal)
				{
					float	relative = x-cursor_region.left,
							delta = (relative-cursor_hook[0])/cursor_range;
					scroll_data.current+=delta;
					scroll_data.current = clamped(scroll_data.current,0,1);
				}
				else
				{
					float	relative = y-cursor_region.bottom,
							delta = (relative-cursor_hook[1])/cursor_range;
					scroll_data.current-=delta;
					scroll_data.current = clamped(scroll_data.current,0,1);
				}
				onScroll();
				return RequestingReshape;
			}
			return Handled;
		}
		
		Component::eEventResult	ScrollBar::onMouseDown(float x, float y, TExtEventResult&ext)
		{
			if (!enabled)
				return Unsupported;
			ext.caught_by = shared_from_this();
			if (cursor_region.contains(x,y) && cursor_range > 0)
			{
				cursor_hook[0] = x-cursor_region.left;
				cursor_hook[1] = y-cursor_region.bottom;
				cursor_grabbed = true;
			}
			elif (up_button.region.contains(x,y))
			{
				up_pressed = true;
				if ((scroll_data.max-scroll_data.min) > 0)
				{
					scroll_data.current = clamped(scroll_data.current - 20/(scroll_data.max-scroll_data.min),0,1);
					onScroll();
				}
			}
			elif (down_button.region.contains(x,y))
			{
				down_pressed = true;
				if ((scroll_data.max-scroll_data.min) > 0)
				{
					scroll_data.current = clamped(scroll_data.current + 20/(scroll_data.max-scroll_data.min),0,1);
					onScroll();
				}
			}
				
			return RequestingReshape;
		}
		
		Component::eEventResult		ScrollBar::onMouseUp(float x, float y)
		{
			bool changed =  cursor_grabbed;
			up_pressed = false;
			down_pressed = false;
			cursor_grabbed = false;
			return changed?RequestingRepaint:Handled;
		}
		
		Component::eEventResult		ScrollBar::onMouseWheel(float x, float y, short delta)
		{
			float new_current = horizontal?
									clamped(scroll_data.current + delta/(scroll_data.max-scroll_data.min),0,1):
									clamped(scroll_data.current - delta/(scroll_data.max-scroll_data.min),0,1);
			if (scroll_data.current != new_current)
			{
				scroll_data.current = new_current;
				onScroll();
				return RequestingReshape;
			}
			return Handled;
		}
		
		
			/*
					if (!enabled)
				return NULL;
			bool new_pressed = down && current_region.contains(x,y);
			requests_update |= new_pressed != pressed;
			pressed = new_pressed;
			return down?this:NULL;
		}
		
		Component*	Button::onMouseDown(float x, float y)
		{
			if (!enabled)
				return NULL;
			requests_update |= !down;
			down = true;
			pressed = true;
			return this;
		}
		
		void	Button::onMouseUp(float x, float y)
		{
			requests_update |= pressed;
			pressed = false;
			down = false;
*/
		
		bool					ScrollBox::erase(const shared_ptr<Component>&component)
		{
			visible_children.findAndErase(component);
			return children.findAndErase(component);
		}
		
		bool					ScrollBox::erase(index_t index)
		{
			if (index >= children.count())
				return false;
			visible_children.findAndErase(children[index]);
			children.erase(index);
			return true;
		}
		
		shared_ptr<const Component>		ScrollBox::child(index_t index) const
		{
			if (index < children.count())
				return children[index];
			index -= children.count();
			if (index == 0)
				return horizontal_bar;
			if (index == 1)
				return vertical_bar;
			return shared_ptr<const Component>();
		}
		
		shared_ptr<Component>		ScrollBox::child(index_t index)
		{
			if (index < children.count())
				return children[index];
			index -= children.count();
			if (index == 0)
				return horizontal_bar;
			if (index == 1)
				return vertical_bar;
			return shared_ptr<Component>();
		}

		count_t	ScrollBox::countChildren() const
		{
			return children.count()+2;
		}
		

		
		void		ScrollBox::updateLayout(const Rect<float>&parent_region)
		{
			Panel::updateLayout(parent_region);
			
			if (horizontal_bar->scrollable.expired())
				horizontal_bar->scrollable = toScrollable();
			if (vertical_bar->scrollable.expired())
				vertical_bar->scrollable = toScrollable();

			Rect<float>	current;
			if (children.isEmpty())
			{
				current.set(0,0,1,1);
				horizontal.current = 0.5;
				vertical.current = 0.5;
			}
			else
			{
				float	w = cell_layout.client.width(),
						h = cell_layout.client.height();
				Rect<float>	region(0,0,w,h);
				for (index_t i = 0; i < children.count(); i++)
				{
					//children[i]->updateLayout(cell_layout.client);
					Rect<float>	child_region;
					const shared_ptr<Component>&child = children[i];
					if (vertical_bar->visible)
					{
						child->anchored.top = true;
						child->anchored.bottom = false;
					}
					if (horizontal_bar->visible)
					{
						child->anchored.left = true;
						child->anchored.right = false;
					}
					
					child->locate(region,child_region);
					

					if (!i)
						current = child_region;
					else
						current.include(child_region);
				}
			}
			
			horizontal.min = 0;
			horizontal.max = current.width();
			vertical.min = 0;
			vertical.max = current.height();
			
			
			/*	horizontal.min -= cell_layout.client.left;
				horizontal.max -= cell_layout.client.left;
				vertical.min -= cell_layout.client.top;
				vertical.max -= cell_layout.client.top;*/
			
			horizontal.window = cell_layout.client.width();
			vertical.window = cell_layout.client.height();
			
			if (horizontal_bar->auto_visibility)
				horizontal_bar->visible = horizontal.max>horizontal.window;
			if (vertical_bar->auto_visibility)
				vertical_bar->visible = vertical.max>vertical.window;
			
			if (vertical_bar->visible)
				horizontal.window-=vertical_bar->width;
			if (horizontal_bar->visible)
				vertical.window-=horizontal_bar->height;
			
			if (!horizontal_bar->visible && horizontal_bar->auto_visibility)
			{
				horizontal_bar->visible = horizontal.max>horizontal.window;
				if (horizontal_bar->visible)
					vertical.window-=horizontal_bar->height;
			}
			if (!vertical_bar->visible && vertical_bar->auto_visibility)
			{
				vertical_bar->visible = vertical.max>vertical.window;
				if (vertical_bar->visible)
				{
					horizontal.window-=vertical_bar->height;
					if (!horizontal_bar->visible && horizontal_bar->auto_visibility)
					{
						horizontal_bar->visible = horizontal.max>horizontal.window;
						if (horizontal_bar->visible)
							vertical.window-=horizontal_bar->height;
					}
				}
			}

			horizontal_bar->scroll_data = horizontal;
			vertical_bar->scroll_data = vertical;
			horizontal_bar->anchored.set(true,true,true,false);
			horizontal_bar->offset.set(0,0,vertical_bar->visible?-vertical_bar->minWidth(false):0,0);
			vertical_bar->anchored.set(false,true,true,true);
			vertical_bar->offset.set(0,horizontal_bar->visible?horizontal_bar->minHeight(false):0,0,0);
			horizontal_bar->updateLayout(cell_layout.client);
			vertical_bar->updateLayout(cell_layout.client);
			effective_client_region.set(cell_layout.client.left,
										horizontal_bar->visible?/*floor*/(horizontal_bar->current_region.top):cell_layout.client.bottom,
										vertical_bar->visible?/*ceil*/(vertical_bar->current_region.left):cell_layout.client.right,
										cell_layout.client.top);
			//effective_client_region = cell_layout.client;
			
			float	hrange = (horizontal.max-effective_client_region.width()),
					vrange = (vertical.max-effective_client_region.height());
			if (hrange < 0)
			{
				hrange = 0;
				horizontal.current = horizontal_bar->scroll_data.current = 0;
			}
			if (vrange < 0)
			{
				vrange = 0;
				vertical.current = vertical_bar->scroll_data.current = 0;
			}

			float	offset_x = horizontal_bar->visible?-(horizontal.current * hrange):0,
					offset_y = vertical_bar->visible?(vertical.current * vrange):0;
			
			Rect<float>	space = effective_client_region;
			space.translate(offset_x,offset_y);
			
			visible_children.reset();
			for (index_t i = 0; i < children.count(); i++)
			{
				const shared_ptr<Component>&child = children[i];
				if (!child->visible)
					continue;
				child->updateLayout(space);
				if (effective_client_region.intersects(child->current_region))
					visible_children << child;
			}
		}
		
		Component::eEventResult		ScrollBox::onMouseWheel(float x, float y, short delta)
		{
			if (horizontal_bar->visible && horizontal_bar->enabled && horizontal_bar->current_region.contains(x,y))
			{
				if (horizontal_bar->scrollable.expired())
					horizontal_bar->scrollable = toScrollable();
				return horizontal_bar->onMouseWheel(x,y,delta);
			}
			if (vertical_bar->visible && horizontal_bar->enabled && vertical_bar->current_region.contains(x,y))
			{
				if (vertical_bar->scrollable.expired())
					vertical_bar->scrollable = toScrollable();
				return vertical_bar->onMouseWheel(x,y,delta);
			}
			if (effective_client_region.contains(x,y))
			{
				
				for (index_t i = visible_children.count()-1; i < visible_children.count(); i--)
				{
					const shared_ptr<Component>&child = visible_children[i];
					if (child->current_region.contains(x,y))
					{
						if (!child->enabled)
							break;
						eEventResult rs = child->onMouseWheel(x,y,delta);
						if (rs == Unsupported)
							break;
						return rs;
					}
				}
			}
			if (vertical_bar->visible && vertical_bar->enabled)
			{
				if (vertical_bar->scrollable.expired())
					vertical_bar->scrollable = toScrollable();
				return vertical_bar->onMouseWheel(x,y,delta);
			}
			if (horizontal_bar->visible && horizontal_bar->enabled)
			{
				if (horizontal_bar->scrollable.expired())
					horizontal_bar->scrollable = toScrollable();
				return horizontal_bar->onMouseWheel(x,y,delta);
			}
			return Unsupported;
		}
		
		float		ScrollBox::clientMinWidth()	const
		{
			float w = horizontal_bar->visible || horizontal_bar->auto_visibility?horizontal_bar->minWidth(false):Panel::clientMinWidth();
			if (vertical_bar->visible)
				w += vertical_bar->minWidth(false);
			return w;
		}
		
		float		ScrollBox::clientMinHeight()	const
		{
			float h;
			if (vertical_bar->visible || vertical_bar->auto_visibility)
				h = vertical_bar->minHeight(false);
			else
			{
				h = 0;
				for (index_t i = 0; i < children.count(); i++)
					h += children[i]->height;
			}
			if (horizontal_bar->visible)
				h += horizontal_bar->minHeight(false);
			return h;
		}
		
		void		ScrollBox::onColorPaint()
		{
			
			Component::onColorPaint();

			shared_ptr<Operator> op = requireOperator();

			op->focus(effective_client_region);
			
			for (index_t i = 0; i < visible_children.count(); i++)
				visible_children[i]->onColorPaint();
			op->unfocus();
			
			if (horizontal_bar->visible)
				horizontal_bar->onColorPaint();
			if (vertical_bar->visible)
				vertical_bar->onColorPaint();
		}
		
		void		ScrollBox::onNormalPaint()
		{
			
			Component::onNormalPaint();
			shared_ptr<Operator> op = requireOperator();

			op->focus(effective_client_region);
			
			
			for (index_t i = 0; i < visible_children.count(); i++)
				visible_children[i]->onNormalPaint();
			op->unfocus();

			if (horizontal_bar->visible)
				horizontal_bar->onNormalPaint();
			if (vertical_bar->visible)
				vertical_bar->onNormalPaint();
		}

		
		
		
		Component::eEventResult		ScrollBox::onMouseHover(float x, float y, TExtEventResult&ext)
		{
	 		if (horizontal_bar->visible && horizontal_bar->cell_layout.border.contains(x,y))
			{
				if (horizontal_bar->scrollable.expired())
					horizontal_bar->scrollable = toScrollable();
				return horizontal_bar->enabled?horizontal_bar->onMouseHover(x,y,ext):Unsupported;
			}
			if (vertical_bar->visible && vertical_bar->cell_layout.border.contains(x,y))
			{
				if (vertical_bar->scrollable.expired())
					vertical_bar->scrollable = toScrollable();
				return vertical_bar->enabled?vertical_bar->onMouseHover(x,y,ext):Unsupported;
			}
			if (effective_client_region.contains(x,y))
			{
				for (index_t i = visible_children.count()-1; i < visible_children.count(); i--)
				{
					const shared_ptr<Component>&child = visible_children[i];
					if (child->cell_layout.border.contains(x,y))
					{
						if (!child->enabled)
							return Unsupported;
						return child->onMouseHover(x,y,ext);
					}
				}
			}
	 		return Unsupported;
		}
		
		Component::eEventResult		ScrollBox::onMouseDown(float x, float y, TExtEventResult&ext)
		{
			Component::eEventResult result;
			if (horizontal_bar->visible && horizontal_bar->enabled && horizontal_bar->current_region.contains(x,y) && (result = horizontal_bar->onMouseDown(x,y,ext)))
			{
				if (horizontal_bar->scrollable.expired())
					horizontal_bar->scrollable = toScrollable();
				return result;
			}
			if (vertical_bar->visible && vertical_bar->enabled && vertical_bar->current_region.contains(x,y) && (result = vertical_bar->onMouseDown(x,y,ext)))
			{
				if (vertical_bar->scrollable.expired())
					vertical_bar->scrollable = toScrollable();
				return result;
			}
			if (effective_client_region.contains(x,y))
			{
				for (index_t i = visible_children.count()-1; i < visible_children.count(); i--)
				{
					const shared_ptr<Component>&child = visible_children[i];
					if (child->cell_layout.border.contains(x,y))
					{
						if (!child->enabled)
							return Unsupported;
						return child->onMouseDown(x,y,ext);
					}
				}
			}
			return Unsupported;
		}
		
		void			ScrollBox::append(const shared_ptr<Component>&component)
		{
			component->anchored.set(true,false,false,true);
			if (children.isNotEmpty())
				component->offset.top = children.last()->offset.top-children.last()->height;
			else
				component->offset.top = 0;
			component->offset.left = 0;
			component->setWindow(window_link);
			children << component;
			shared_ptr<Window> wnd = window();
			if (wnd)
				wnd->layout_changed = true;
		}
		
		
		void	Edit::setup()
		{
			go_left = 0;
			go_right = 0;
			tick_interval = 0.1;
			readonly = false;
			mask_input = false;
			cursor = 0;
			sel_start = 0;
			view_begin = view_end = 0;
			layout = global_layout.reference();
			width = minWidth(false);
			height = minHeight(false);
		}

		
		void						Edit::onColorPaint()
		{
			if (readonly || !enabled)
				glGrey(0.5);
			else
				glWhite();
			
			shared_ptr<Operator> op = requireOperator();

			Component::onColorPaint();
			glWhite();
			size_t end = vmin(view_end,text.length());
			if (end>view_begin)
			{
				op->focus(cell_layout.client);
				textout.color(1,1,1);
				float	bottom = cell_layout.client.centerY()-textout.getFont().getHeight()/2+font_offset,
						top = cell_layout.client.centerY()+textout.getFont().getHeight()/2;
				glDisable(GL_BLEND);
				if (mask_input)
				{
					float psize;
					glGetFloatv(GL_POINT_SIZE,&psize);
					float csize = textout.getFont().getHeight()*0.75;
					glPointSize(csize*0.8);
					glEnable(GL_POINT_SMOOTH);
					glWhite();
					glDisable(GL_TEXTURE_2D);
					glBegin(GL_POINTS);
					for (size_t i = view_begin; i < end; i++)
						glVertex2f(cell_layout.client.left+csize*(0.5+i),cell_layout.client.centerY());
					glEnd();
					glPointSize(psize);
				}
				else
				{
					textout.locate(cell_layout.client.left,bottom);
					textout.print(text.root()+view_begin,end-view_begin);
				}
				if (sel_start != cursor)
				{
					glDisable(GL_BLEND);
					glDisable(GL_TEXTURE_2D);
					glBegin(GL_QUADS);
						size_t	sel_begin = sel_start,
								sel_end = cursor;
						if (sel_begin > sel_end)
							swp(sel_begin,sel_end);
						if (sel_begin < view_begin)
							sel_begin = view_begin;
						if (sel_end < view_begin)
							sel_end = view_begin;
						float	left = cell_layout.client.left+textWidth(text.root()+view_begin,sel_begin-view_begin),
								right = cell_layout.client.left+textWidth(text.root()+view_begin,sel_end-view_begin);
						//glColor4f(0.4,0.6,1,0.7);
						glColor4f(0.2,0.3,0.5,0.7);
						glVertex2f(left-2,bottom);
						glVertex2f(right+2,bottom);
						glVertex2f(right+2,top);
						glVertex2f(left-2,top);
					glEnd();
					glEnable(GL_BLEND);
					//display->setDefaultBlendFunc();
					
					if (mask_input)
					{
						float psize;
						glGetFloatv(GL_POINT_SIZE,&psize);
						float csize = textout.getFont().getHeight()*0.75;
						glPointSize(csize*0.8);
						glEnable(GL_POINT_SMOOTH);
						glDisable(GL_TEXTURE_2D);
						glWhite();
						glBegin(GL_POINTS);
						for (index_t i = sel_begin; i < sel_end; i++)
							glVertex2f(cell_layout.client.left+csize*(0.5+i),cell_layout.client.centerY());
						glEnd();
						glPointSize(psize);
					}
					else
					{
						textout.color(1,1,1,1);
						textout.locate(cell_layout.client.left+textout.unscaledLength(text.root()+view_begin,sel_begin-view_begin),bottom);
						textout.print(text.root()+sel_begin,sel_end-sel_begin);
					}
				}
					
				glEnable(GL_BLEND);
				
				op->unfocus();
			}
			if (isFocused() && cursor_ticks < 5)
			{
				glWhite();
				op->getDisplay().useTexture(NULL);
				glBegin(GL_LINES);
					glVertex2f(cursor_offset,cell_layout.client.centerY()-textout.getFont().getHeight()/2);
					glVertex2f(cursor_offset,cell_layout.client.centerY()+textout.getFont().getHeight()/2+font_offset);
				glEnd();
			}
		}
		
		Component::eEventResult		Edit::onTick()
		{
			count_t new_ticks = (cursor_ticks+1)%10;
			bool refresh = new_ticks < 5 != cursor_ticks < 5;
			cursor_ticks  = new_ticks;

			
			if (go_right && !view_right_most)
			{
				view_begin++;
				if (cursor < text.length())
					cursor++;
				updateView();
				return RequestingRepaint;
			}
			if (go_left && view_begin)
			{
				view_begin--;
				if (cursor)
					cursor--;
				updateView();
				return RequestingRepaint;
			}
			return refresh?RequestingRepaint:Handled;
		}

		
		Component::eEventResult		Edit::onMouseHover(float x, float y, TExtEventResult&ext)
		{
			ext.caught_by = shared_from_this();
			if (cell_layout.client.contains(x,y))
				ext.custom_cursor = Mouse::CursorType::EditText;
			return Handled;
		}
		
		Component::eEventResult		Edit::onMouseDown(float x, float y, TExtEventResult&ext)
		{
			ext.caught_by = shared_from_this();
			if (cell_layout.client.contains(x,y))
			{
				//ShowMessage(String(x)+", "+String(y)+" is in "+cell_layout.client.toString());
				float rx = x - cell_layout.client.left;
				size_t end = vmin(view_end-1+view_right_most,text.length());
				index_t index=view_begin;
				for (; index < end; index++)
				{
					float width = charWidth(text[index]);
					if (rx < width/2)
						break;
					rx -= width;
				}
				cursor = index;
				if (!input.pressed[Key::Shift])
					sel_start = cursor;
				ext.custom_cursor = Mouse::CursorType::EditText;
				
				cursor_offset = cell_layout.client.left+textWidth(text.root()+view_begin,cursor-view_begin);			
				return RequestingRepaint;
			}
			return Handled;
		}
		
		Component::eEventResult	Edit::onMouseDrag(float x, float y)
		{
			float rx = x - cell_layout.client.left;
			go_left = rx < 0;
			size_t end = vmin(view_end-1+view_right_most,text.length());
			index_t index=view_begin;
			for (; index < end; index++)
			{
				float width = charWidth(text[index]);
				if (rx < width/2)
					break;
				rx -= width;
			}
			go_right = index == end && rx > 0;
			cursor = index;
			cursor_offset = cell_layout.client.left+textWidth(text.root()+view_begin,cursor-view_begin);			
			return RequestingRepaint;
		}
		
		Component::eEventResult	Edit::onMouseUp(float x, float y)
		{
			go_left = false;
			go_right = false;
		
			return Handled;
		}

		
		Component::eEventResult	Edit::onKeyDown(Key::Name key)
		{
			switch (key)
			{
				case Key::Left:
					if (cursor && text.length())
					{
						if (!input.pressed[Key::Ctrl])
							cursor--;
						else
						{
							if (cursor >= text.length())
								cursor = text.length()-1;
							while (cursor && isWhitespace(text[cursor-1]))
								cursor--;
							while (cursor && !isWhitespace(text[cursor-1]))
								cursor--;
							if (isWhitespace(text[cursor]))
								cursor++;
						}
						if (!input.pressed[Key::Shift])
							sel_start = cursor;
						updateView();
						return RequestingRepaint;
					}
				break;
				case Key::Right:
					if (cursor<text.length())
					{
						if (!input.pressed[Key::Ctrl])
							cursor++;
						else
						{
							while (cursor<text.length() && !isWhitespace(text[cursor]))
								cursor++;
							while (cursor<text.length() && isWhitespace(text[cursor]))
								cursor++;
							//if (cursor && !isWhitespace(text[cursor-1])
						}
						
						if (!input.pressed[Key::Shift])
							sel_start = cursor;
						updateView();
						return RequestingRepaint;
					}
				break;
				case Key::Home:
					if (cursor)
					{
						cursor = 0;
						if (!input.pressed[Key::Shift])
							sel_start = cursor;
						updateView();
						return RequestingRepaint;
					}
				break;
				case Key::End:
					if (cursor<text.length() && !read_only)
					{
						cursor = text.length();
						if (!input.pressed[Key::Shift])
							sel_start = cursor;
						updateView();
						return RequestingRepaint;
					}
				break;
				case Key::BackSpace:
					if ((cursor || sel_start)  && !read_only)
					{
						if (sel_start == cursor)
						{
							sel_start--;
							if (text.erase(--cursor,1))
							{
								updateView();
								on_change();
								return RequestingRepaint;
							}
							
						}
						else
							if (sel_start > cursor)
							{
								if (text.erase(cursor,sel_start-cursor))
								{
									sel_start = cursor;
									updateView();
									on_change();
									return RequestingRepaint;
								}
							}
							else
								if (text.erase(sel_start,cursor-sel_start))
								{
									cursor = sel_start;
									updateView();
									on_change();
									return RequestingRepaint;
								}
					}
				break;
				case Key::Delete:
					if (!read_only)
					{
						if (sel_start == cursor)
						{
							if (text.erase(cursor,1))
							{
								updateView();
								on_change();
								return RequestingRepaint;
							}
							
						}
						else
							if (sel_start > cursor)
							{
								if (text.erase(cursor,sel_start-cursor))
								{
									sel_start = cursor;
									updateView();
									on_change();
									return RequestingRepaint;
								}
							}
							else
								if (text.erase(sel_start,cursor-sel_start))
								{
									cursor = sel_start;
									updateView();
									on_change();
									return RequestingRepaint;
								}
					}
				break;
				case Key::C:
					if (input.pressed[Key::Ctrl] && sel_start != cursor && !masked_view)
					{
						size_t	begin = sel_start,
								end = cursor;
						if (begin > end)
							swp(begin,end);
						String sub = String(text.root()+begin,end-begin);
						if (!System::copyToClipboard(NULL,sub.c_str()))
							ErrMessage("failed to copy");
					}
				break;
				case Key::X:
					if (input.pressed[Key::Ctrl] && sel_start != cursor && !masked_view)
					{
						size_t	begin = sel_start,
								end = cursor;
						if (begin > end)
							swp(begin,end);
						String sub = String(text.root()+begin,end-begin);
						if (System::copyToClipboard(NULL,sub.c_str()) && !read_only)
						{
							text.erase(begin,end-begin);
							if (cursor > begin)
								cursor = begin;
							sel_start = cursor;
							updateView();
							on_change();
							return RequestingRepaint;
						}
					}
				break;
				case Key::V:
					if (input.pressed[Key::Ctrl] && !read_only)
					{
						char buffer[0x1000];
						if (System::getFromClipboardIfText(NULL,buffer,sizeof(buffer)))
						{
							if (cursor!=sel_start)
							{
								if (cursor > sel_start)
								{
									text.erase(sel_start,cursor-sel_start);
									cursor = sel_start;
								}
								else
									text.erase(cursor,sel_start-cursor);
							}
							size_t len = strlen(buffer);
							text.insert(cursor,buffer,len);
							cursor+=len;
							sel_start = cursor;
							updateView();
							on_change();
							return RequestingRepaint;
						}
					}
				break;
				case Key::Enter:
				case Key::Return:
					on_enter();
				return Handled;
				default:
					return Unsupported;
			}
			return Handled;
		}
		
		Component::eEventResult	Edit::onKeyUp(Key::Name key)
		{
		
			return Unsupported;
		}
		
		void	Edit::updateView()
		{
			if (cursor > text.length())
				cursor = text.length();
			if (view_begin >= text.length())
				view_begin = text.length()?text.length()-1:0;
			if (cursor < view_begin)
				view_begin = cursor;
			else
				while (textWidth(text.root()+view_begin,cursor-view_begin)>cell_layout.client.width())
					view_begin++;
			view_end = view_begin+1;
			while (view_end < text.length() && textWidth(text.root()+view_begin,view_end-view_begin)<cell_layout.client.width())
				view_end++;
			view_right_most = textWidth(text.root()+view_begin,view_end-view_begin)<cell_layout.client.width();
			//if (textout.unscaledLength(text.root()+view_begin,view_end-view_begin)>=cell_layout.client.width())
				//view_end--;
			cursor_offset = cell_layout.client.left+textWidth(text.root()+view_begin,cursor-view_begin);
		}
		
		void	Edit::updateLayout(const Rect<float>&parent_region)
		{
			Component::updateLayout(parent_region);
			updateView();
		}
		
		
		
		Component::eEventResult	Edit::onChar(char c)
		{
			if (read_only || (acceptChar && !acceptChar(c)))
				return Unsupported;
			if (cursor!=sel_start)
			{
				if (cursor > sel_start)
				{
					text.erase(sel_start,cursor-sel_start);
					cursor = sel_start;
				}
				else
					text.erase(cursor,sel_start-cursor);
			}

			text.insert(cursor++,c);
			sel_start = cursor;
			updateView();
			on_change();
			return RequestingRepaint;
		}
		
		
		
		
		

		void		Button::onNormalPaint()
		{
			glMatrixMode(GL_TEXTURE);
				glPushMatrix();
				if (!enabled)
					glScalef(0.1,0.1,1);
				elif (pressed)
					glScalef(-1,-1,1);
			glMatrixMode(GL_MODELVIEW);
			
			Component::onNormalPaint();
			
			glMatrixMode(GL_TEXTURE);
				glPopMatrix();
			glMatrixMode(GL_MODELVIEW);
		}
		
		void		Button::onColorPaint()
		{
			if (pressed || !enabled)
				glGrey(0.5);
			else
				glWhite();
			
			Component::onColorPaint();
			glWhite();
			const Rect<float>&rect=cell_layout.client;
			
			glDisable(GL_BLEND);
			textout.locate(rect.centerX()-textout.unscaledLength(caption)*0.5+pressed,rect.centerY()-textout.getFont().getHeight()/2+font_offset);//textout.getFont().getHeight()*0.5);
			textout.color(1.0-0.2*(pressed||!enabled),1.0-0.2*(pressed||!enabled),1.0-0.2*(pressed||!enabled));
			textout.print(caption);
			glEnable(GL_BLEND);
			
		}
		
		Component::eEventResult	Button::onMouseDrag(float x, float y)
		{
			bool new_pressed = down && current_region.contains(x,y);
			bool changed = pressed != new_pressed;
			pressed = new_pressed;
			return changed?RequestingRepaint:Handled;
		}
		
		Component::eEventResult	Button::onMouseDown(float x, float y, TExtEventResult&ext)
		{
			if (!enabled)
				return Unsupported;
			ext.caught_by = shared_from_this();
			/*if (!current_region.contains(x,y))
				return NULL;*/
			bool changed = !down;
			down = true;
			pressed = true;
			return changed?RequestingRepaint:Handled;
		}
		
		Component::eEventResult	Button::onMouseUp(float x, float y)
		{
			if (pressed)
				onExecute();
			bool changed = pressed;
			pressed = false;
			down = false;
			return changed?RequestingRepaint:Handled;
		}
		
		Component::eEventResult	Button::onKeyDown(Key::Name key)
		{
			switch (key)
			{
				case Key::Space:
				case Key::Enter:
				case Key::Return:
					on_execute();
				return Handled;
			}
			return Unsupported;
		}
		
		Component::eEventResult	Button::onKeyUp(Key::Name key)
		{
		
			return Unsupported;
		}
		
		
		
		float		Button::clientMinWidth()	const
		{
			return textout.unscaledLength(caption);
		}
		
		float		Button::clientMinHeight()	const
		{

			return textout.getFont().getHeight();
		}
		
		void		CheckBox::onNormalPaint()
		{
			Component::onNormalPaint();
			if (style && !style->box_normal.isEmpty())
			{
				const Rect<float>&rect=cell_layout.client;

				shared_ptr<Operator> op = requireOperator();

				op->getDisplay().useTexture(style->box_normal);
				float cy = rect.centerY();
				float size = boxSize();
				glBegin(GL_QUADS);
					glTexCoord2f(0,0); glVertex2f(rect.left,cy-size/2);
					glTexCoord2f(1,0); glVertex2f(rect.left+size,cy-size/2);
					glTexCoord2f(1,1); glVertex2f(rect.left+size,cy+size/2);
					glTexCoord2f(0,1); glVertex2f(rect.left,cy+size/2);
				glEnd();
			}
		}
		
		void		CheckBox::onColorPaint()
		{
			Component::onColorPaint();
			
			const Rect<float>&rect=cell_layout.client;
			float cy = rect.centerY();
			float size = boxSize();
			shared_ptr<Operator> op = requireOperator();
			Engine::Display<OpenGL>&display = op->getDisplay();
			if (style && !style->box_color.isEmpty())
			{

				display.useTexture(style->box_color);
				glBegin(GL_QUADS);
					glTexCoord2f(0,0); glVertex2f(rect.left,cy-size/2);
					glTexCoord2f(1,0); glVertex2f(rect.left+size,cy-size/2);
					glTexCoord2f(1,1); glVertex2f(rect.left+size,cy+size/2);
					glTexCoord2f(0,1); glVertex2f(rect.left,cy+size/2);
				glEnd();
			}
			
			if (checked && style && !style->check_mark.isEmpty())
			{
				display.useTexture(style->check_mark);
				glBlack();
				glBegin(GL_QUADS);
					glTexCoord2f(0,0); glVertex2f(rect.left,cy-size/2);
					glTexCoord2f(1,0); glVertex2f(rect.left+size,cy-size/2);
					glTexCoord2f(1,1); glVertex2f(rect.left+size,cy+size/2);
					glTexCoord2f(0,1); glVertex2f(rect.left,cy+size/2);
				glEnd();
			}
			
			if (pressed && style && !style->highlight_mark.isEmpty())
			{
				display.useTexture(style->highlight_mark);
				glColor3f(1,0.6,0);
				glBegin(GL_QUADS);
					glTexCoord2f(0,0); glVertex2f(rect.left,cy-size/2);
					glTexCoord2f(1,0); glVertex2f(rect.left+size,cy-size/2);
					glTexCoord2f(1,1); glVertex2f(rect.left+size,cy+size/2);
					glTexCoord2f(0,1); glVertex2f(rect.left,cy+size/2);
				glEnd();
			}
		
			glDisable(GL_BLEND);
			textout.locate(rect.left+size+textout.getFont().getHeight()*0.2,rect.centerY()-textout.getFont().getHeight()/2+font_offset);//textout.getFont().getHeight()*0.5);
			textout.color(1.0-0.2*(!enabled),1.0-0.2*(!enabled),1.0-0.2*(!enabled));
			textout.print(caption);
			glEnable(GL_BLEND);
			glWhite();
		}


		


		Component::eEventResult	CheckBox::onMouseDrag(float x, float y)
		{
			bool new_pressed = down && current_region.contains(x,y);
			bool changed = new_pressed != pressed;
			pressed = new_pressed;
			return changed?RequestingRepaint:Handled;
		}
		
		Component::eEventResult	CheckBox::onMouseDown(float x, float y, TExtEventResult&ext)
		{
			if (!enabled)
				return Unsupported;
			ext.caught_by = shared_from_this();
			bool changed = !down;
			down = true;
			pressed = true;
			return changed?RequestingRepaint:Handled;
		}
		
		Component::eEventResult	CheckBox::onMouseUp(float x, float y)
		{
			if (pressed)
			{
				checked = !checked;
				on_change();
			}
			bool changed = pressed;
			pressed = false;
			down = false;
			return changed?RequestingRepaint:Handled;
		}
		
		
		float		CheckBox::minWidth(bool include_offsets)	const
		{
			float rs;
			{
				rs = textout.unscaledLength(caption)+0.2*textout.getFont().getHeight()+boxSize();
				if (anchored.right && include_offsets)
					rs -= offset.right;
				if (anchored.left && include_offsets)
					rs += offset.left;
			}
			if (layout)
			{
				rs += layout->client_edge.left+layout->client_edge.right;
				if (rs < layout->min_width)
					rs = layout->min_width;
			}
			return rs;
		}
		
		float		CheckBox::minHeight(bool include_offsets)	const
		{
			float rs;
			{
				rs = textout.getFont().getHeight();
				if (anchored.top && include_offsets)
					rs -= offset.top;
				if (anchored.bottom && include_offsets)
					rs += offset.bottom;
			}
			if (layout)
			{
				rs += layout->client_edge.bottom+layout->client_edge.top;
				if (rs < layout->min_height)
					rs = layout->min_height;
			}
			return rs;
		}
		
		
		
		
		
		void			Label::updateLayout(const Rect<float>&parent_space)
		{
			Component::updateLayout(parent_space);
			if (wrap_text)
			{
				static float last_width = -1;
				float w = cell_layout.client.width();
				if (w != last_width || text_changed)
				{
					text_changed = false;
					last_width = w;
					wrapf(caption,w,charLen,lines);
					for (index_t i = 0; i < lines.count(); i++)
						cout << "'"<<lines[i]<<"'"<<endl;
				}
			}
					
			
		}
		
		
		
		void			Label::setup()
		{
			caption = "Label";
			wrap_text = false;
			text_changed = true;
			fill_background = false;
			Vec::set(background_color,1);
			height = minHeight(false);
			width = minWidth(false);
		}
		
		float			Label::clientMinWidth()	const
		{
			if (!wrap_text)
				return textout.unscaledLength(caption);
			return 30;
		}
		
		float			Label::clientMinHeight()	const
		{
			if (!wrap_text)
				return textout.getFont().getHeight();
			return textout.getFont().getHeight()*lines.count();
		
		}
		
		Label*			Label::setText(const String&text)
		{
			caption = text;
			text_changed = true;
			if (!wrap_text)
				width = minWidth(false);
			return this;
		}

		float		Label::charLen(char c)
		{
			return textout.unscaledLength(&c,1);
		}
		
		void			Label::onColorPaint()
		{
			if (!enabled)
				glGrey(0.5);
			else
				glWhite();
			
			Component::onColorPaint();
			if (!fill_background && !caption.length())
				return;
			
			if (fill_background)
			{
				glDisable(GL_TEXTURE_2D);
				glColor3fv(background_color.v);
				fillRect(cell_layout.client);
			}
			if (caption.length())
			{
				shared_ptr<Operator> op = requireOperator();
				op->focus(cell_layout.client);
				textout.color(1,1,1);
				if (!fill_background)
					glDisable(GL_BLEND);
				if (!wrap_text)
				{
					float	bottom = cell_layout.client.centerY()-textout.getFont().getHeight()/2+font_offset;
					textout.locate(cell_layout.client.left,bottom);
					textout.print(caption);
				}
				else
				{
					for (index_t i = 0; i < lines.count(); i++)
					{
						textout.locate(cell_layout.client.left,cell_layout.client.top-(textout.getFont().getHeight()*(i+1)));
						textout.print(lines[i]);
					}
				}
				glEnable(GL_BLEND);
				op->unfocus();
			}
		}
		
		void	ComboBox::setup()
		{
			//MenuEntry::menu();	//don't create menu (called from constructor)
			selected_object.reset();
			selected_entry = 0;
			open_down = true;
			layout = global_layout.reference();
			width = minWidth(false);
			height = minHeight(false);
			//ShowMessage(width);
		}
		
		void	ComboBox::updateLayout(const Rect<float>&parent_space)
		{
			if (!selected_object && menu()->countChildren()>2)
			{
				selected_entry = 0;
				selected_object = ((MenuEntry*)(menu()->child(selected_entry).get()))->object;
				setText(selected_object->toString());
			}
			height = Label::minHeight(false);
			MenuEntry::updateLayout(parent_space);
		}
		
		
		void	MenuEntry::correctMenuWindowSize()	const
		{
			bool changed = false;
			float mw = menu_window->minWidth();
			if (menu_window->fwidth != mw)
			{
				menu_window->fwidth = mw;
				menu_window->iwidth = (size_t)round(menu_window->fwidth);
				changed = true;
			}
			Menu*	menu = (Menu*)menu_window->component_link.get();
			float mh = menu->idealHeight();
			if (menu_window->layout)
				mh += menu_window->layout->client_edge.top+menu_window->layout->client_edge.bottom;
			float h = vmin(150,mh);
			if (menu_window->fheight != h)
			{
				menu_window->fheight = h;
				menu_window->iheight = (size_t)round(menu_window->fheight);
				changed = true;
			}
					
			if (menu_window->layout_changed || changed)
				menu_window->updateLayout();
		}

		
		float	ComboBox::minWidth(bool include_offsets)	const
		{
			if (menu_window)
			{
				if (menu_window->layout_changed)
					menu_window->updateLayout();
				correctMenuWindowSize();
				return vmax(MenuEntry::minWidth(include_offsets),menu_window->minWidth());
			}
			return MenuEntry::minWidth(include_offsets);
		}
		
		Component::eEventResult			ComboBox::onMouseHover(float x, float y, TExtEventResult&ext)
		{
			return Unsupported;
		}
		
		
		void	ComboBox::onMenuClose(const shared_ptr<MenuEntry>&child)
		{
			selected_entry = menu()->indexOf(child);
			if (selected_entry)
			{
				selected_entry--;
				selected_object = child->object;
			}
			else
			{
				selected_object.reset();
			}
			if (selected_object)
				setText(selected_object->toString());
			else
				setText("");
			window()->apply(RequestingReshape);
			on_change();
		}
		
		void	ComboBox::select(index_t index)
		{
			selected_entry = index;
			if (selected_entry >= menu()->countChildren())
				selected_object.reset();
			else
				selected_object = ((MenuEntry*)menu()->child(selected_entry).get())->object;
			if (selected_object)
				setText(selected_object->toString());
			else
				setText("");
			window()->apply(RequestingReshape);
		}

		
		void	MenuEntry::updateLayout(const Rect<float>&parent_space)
		{
			Label::updateLayout(parent_space);
			
		}
		
		Component::eEventResult			MenuEntry::onMouseDown(float x, float y, TExtEventResult&ext)
		{
			ext.caught_by = shared_from_this();
			on_execute();
			
			if (menu_window)
			{
				shared_ptr<Operator> op = requireOperator();
				float delta = timer.toSecondsf(timing.now64-menu_window->hidden);
				if (delta >= 0 && delta<0.1f)
				{
					op->hideMenus();
					//Window::hideMenus();
				}
				else
				{
					ASSERT_NOT_NULL__(menu_window->component_link);
					
					Rect<float>	absolute = cell_layout.border;
					shared_ptr<Window> parent = window();
					ASSERT__(parent);
					
					#ifdef DEEP_GUI
						absolute.translate(parent->current_center.x-parent->fwidth/2,parent->current_center.y-parent->fheight/2);
					#else
						absolute.translate(parent->x-parent->fwidth/2,parent->y-parent->fheight/2);
					#endif
						

					correctMenuWindowSize();
					
					#ifdef DEEP_GUI
						if (open_down)
						{
							menu_window->current_center.x = absolute.left+menu_window->fwidth/2;
							menu_window->current_center.y = absolute.bottom-menu_window->fheight/2;
						}
						else
						{
							menu_window->current_center.x = absolute.right+menu_window->fwidth/2;
							menu_window->current_center.y = absolute.top-menu_window->fheight/2;
						}
						menu_window->current_center.x -= (menu_window->cell_layout.border.left);
						menu_window->current_center.y += (menu_window->fheight-menu_window->cell_layout.border.top);
						
						menu_window->current_center.shell_radius = 1;
						
						menu_window->origin = menu_window->destination = menu_window->current_center;
					#else
						if (open_down)
						{
							menu_window->x = absolute.left+menu_window->fwidth/2;
							menu_window->y = absolute.bottom-menu_window->fheight/2;
						}
						else
						{
							menu_window->x = absolute.right+menu_window->fwidth/2;
							menu_window->y = absolute.top-menu_window->fheight/2;
						}
						menu_window->x -= (menu_window->cell_layout.border.left);
						menu_window->y += (menu_window->fheight-menu_window->cell_layout.border.top);
					#endif
						
					requireOperator()->showMenu(menu_window);
				}
			}
			else
			{
				requireOperator()->hideMenus();
				onMenuClose(static_pointer_cast<MenuEntry,Component>(shared_from_this()));
			}
			
			return Handled;
		}
		
		Component::eEventResult			MenuEntry::onMouseHover(float x, float y, TExtEventResult&ext)
		{
			ext.caught_by = shared_from_this();
			bool changed = !Label::fill_background;
			Label::fill_background = true;
			return changed?RequestingRepaint:Handled;
		}
		
		Component::eEventResult			MenuEntry::onMouseExit()
		{
			bool changed = Label::fill_background;
			Label::fill_background = false;
			return changed?RequestingRepaint:Handled;
		}
		

		Component::eEventResult			MenuEntry::onFocusGained()
		{
			Label::fill_background = true;
			return RequestingRepaint;
		}
		
		Component::eEventResult			MenuEntry::onFocusLost()
		{
			Label::fill_background = false;
			return RequestingRepaint;
		}
		
		Component::eEventResult			MenuEntry::onKeyDown(Key::Name key)
		{
			return parent.expired()?Unsupported:parent.lock()->onKeyDown(key);
		}
		
		
		const shared_ptr<Window>&	MenuEntry::menuWindow()
		{
			if (!menu_window)
			{
				menu_window.reset(new Window(&Window::menu_style));
				shared_ptr<Menu> menu = shared_ptr<Menu>(new Menu());
				menu_window->setComponent(menu);
				menu->parent = static_pointer_cast<MenuEntry,Component>(shared_from_this());
				//menu->level = level+1;

			}
			return menu_window;
		}
		
		shared_ptr<Menu>		MenuEntry::menu()
		{
			return static_pointer_cast<Menu, Component>(menuWindow()->component_link);
		}

		shared_ptr<const Menu>	MenuEntry::menu()	const
		{
			return menu_window?static_pointer_cast<const Menu, const Component>(menu_window->component_link):shared_ptr<const Menu>();
		}
		
		void		MenuEntry::discardMenu()
		{
			if (menu_window)
			{
				shared_ptr<Operator> op = getOperator();
				if (op)	//might, in fact, be null
					op->hideMenus();
				menu_window.reset();
			}
		}
		
		void		MenuEntry::setup()
		{
			parent.reset();
			menu_window.reset();
			object.reset();
			open_down = false;
			Label::wrap_text = false;
			//level = 0;
			Vec::def(Label::background_color,0.5,0.5,1);
			height = minHeight(false);
			width = minWidth(false);
		}
		
		
		void	MenuEntry::onColorPaint()
		{
			Label::fill_background = isFocused();
			if (object && object.get() != this)
				Label::caption = object->toString();
			Label::onColorPaint();
		}
		
		void	MenuEntry::onMenuClose(const shared_ptr<MenuEntry>&child)
		{
			if (!parent.expired())
			{
				shared_ptr<Menu>	p = parent.lock();
				if (!p->parent.expired())
					p->parent.lock()->onMenuClose(static_pointer_cast<MenuEntry, Component>(shared_from_this()));
			}
		}
		
		MenuEntry::~MenuEntry()
		{
			//discardMenu();	//only hides menu in the operator, but i suspect even if the case happens where a menu is still visible when the root is erased, the operator should now be able to handle that
		}
		
		
		float		Menu::idealHeight()	const
		{
			float h = 0;
			for (index_t i = 0; i < children.count(); i++)
				h += children[i]->height;
			if (layout)
				h += layout->client_edge.top+layout->client_edge.bottom;
			return h;
		}
		
		shared_ptr<MenuEntry>		Menu::add(const String&caption)
		{
			shared_ptr<MenuEntry> entry = shared_ptr<MenuEntry>(new MenuEntry());
			entry->setText(caption);
			entry->object = entry;
			entry->parent = static_pointer_cast<Menu,Component>(shared_from_this());
			ASSERT__(add(entry));
			return entry;
		}
		
		bool		Menu::add(const shared_ptr<Component>&component)
		{
			if (!component->type_name.beginsWith("Label/MenuEntry") || !ScrollBox::add(component))
				return false;
			static_pointer_cast<MenuEntry,Component>(component)->parent = static_pointer_cast<Menu,Component>(shared_from_this());
			//((MenuEntry*)component)->level = level;
			component->setWindow(window_link);		
			arrangeItems();
			if (!window_link.expired())
				window_link.lock()->layout_changed = true;
			return true;
		}
		
		bool		Menu::erase(const shared_ptr<Component>&component)
		{
			if (ScrollBox::erase(component))
			{
				arrangeItems();
				return true;
			}
			return false;
		}
		
		bool		Menu::erase(index_t index)
		{
			if (ScrollBox::erase(index))
			{
				arrangeItems();
				return true;
			}
			return false;
		}
		
		bool		Menu::moveUp(const shared_ptr<Component>&component)
		{
			if (ScrollBox::moveUp(component))
			{
				arrangeItems();
				return true;
			}
			return false;
		}

		bool		Menu::moveUp(index_t index)
		{
			if (ScrollBox::moveUp(index))
			{
				arrangeItems();
				return true;
			}
			return false;
		}

		bool		Menu::moveDown(const shared_ptr<Component>&component)
		{
			if (ScrollBox::moveDown(component))
			{
				arrangeItems();
				return true;
			}
			return false;
		}
		
		bool		Menu::moveDown(index_t index)
		{
			if (ScrollBox::moveDown(index))
			{
				arrangeItems();
				return true;
			}
			return false;
		}
		
		bool		Menu::moveTop(const shared_ptr<Component>&component)
		{
			if (ScrollBox::moveTop(component))
			{
				arrangeItems();
				return true;
			}
			return false;
		}

		bool		Menu::moveTop(index_t index)
		{
			if (ScrollBox::moveTop(index))
			{
				arrangeItems();
				return true;
			}
			return false;
		}

		bool		Menu::moveBottom(const shared_ptr<Component>&component)
		{
			if (ScrollBox::moveBottom(component))
			{
				arrangeItems();
				return true;
			}
			return false;
		}

		bool		Menu::moveBottom(index_t index)
		{
			if (ScrollBox::moveBottom(index))
			{
				arrangeItems();
				return true;
			}
			return false;
		}
		
		void		Menu::arrangeItems()
		{
			float current=0;
			for (index_t i = 0; i < children.count(); i++)
			{
				const shared_ptr<Component>&component = children[i];
				
				component->anchored.set(true,false,true,true);
				component->offset.top = current;
				current -= component->height;
				component->offset.left = 0;
				component->offset.right = 0;
			}
		}

		void	Menu::setup()
		{
			ScrollBox::horizontal_bar->visible = false;
			ScrollBox::vertical_bar->auto_visibility = true;
			parent.reset();
			//level = 0;
			selected_entry = 0;
			layout = global_layout.reference();
		}
		
		
		Component::eEventResult			Menu::onKeyDown(Key::Name key)
		{
			switch (key)
			{
				case Key::Up:
					if (selected_entry)
					{
						selected_entry--;
						setFocused(children[selected_entry]);
						return RequestingRepaint;
					}
					return Handled;
				case Key::Down:
					if (selected_entry+1<children.count())
					{
						selected_entry++;
						setFocused(children[selected_entry]);
						return RequestingRepaint;
					}
				
			}
			return Unsupported;
		}
		
		Component::eEventResult			Menu::onMouseHover(float x, float y, TExtEventResult&ext)
		{
			eEventResult	rs=ScrollBox::onMouseHover(x,y,ext);
			if (rs != Unsupported)
			{
				index_t index = Panel::children.indexOf(ext.caught_by);
				if (index != -1)
				{
					selected_entry = index;
					setFocused(children[selected_entry]);
					return RequestingRepaint;
				}
				return rs;
			}
			return Unsupported;
		}


		static void	loadLayout(XML::Node*xtheme, FileSystem::Folder&folder, const String&path, Layout&layout, float outer_scale)
		{
			XML::Node*node = xtheme->find(path);
			if (!node)
				return;	//graceful ignore
			String string;
			float scale = 1.0f;
			if (node->query("scale",string))
				convert(string.c_str(),scale);
			layout.override = NULL;
			FileSystem::File	file;
			if (node->query("file",string))
				if (folder.findFile(string,file))
				{
					layout.loadFromFile(file.getLocation(),scale*outer_scale);
					return;
				}
			if (node->query("copy",string))
			{
				if (string == "button")
					layout.override = &Button::global_layout;
				elif (string == "panel")
					layout.override = &Panel::global_layout;
				elif (string == "scrollbox")
					layout.override = &ScrollBox::global_layout;
				elif (string == "window/common")
					layout.override = &Window::common_style;
				elif (string == "window/menu")
					layout.override = &Window::menu_style;
				elif (string == "window/hint")
					layout.override = &Window::hint_style;
				else
					throw Program::UnsupportedRequest("Unknown resource layout '"+string+"'");
			}
		}
		
		static void loadLayout(XML::Node*xtheme, FileSystem::Folder&folder, const String&path, ScrollBarLayout&layout, float outer_scale)
		{
			XML::Node*node = xtheme->find(path);
			if (!node)
				return;	//graceful ignore
			String string;
			float scale = 1.0f;
			if (node->query("scale",string))
				convert(string.c_str(),scale);
			
			FileSystem::File	file;
			if (node->query("file",string))
				if (folder.findFile(string,file))
					layout.loadFromFile(file.getLocation(),scale*outer_scale);
		}
		
		static void loadLayout(XML::Node*xtheme, FileSystem::Folder&folder, const String&path, SliderLayout&layout, float outer_scale)
		{
			XML::Node*node = xtheme->find(path);
			if (!node)
				return;	//graceful ignore
			String string;
			float scale = 1.0f;
			if (node->query("scale",string))
				convert(string.c_str(),scale);
			
			FileSystem::File	file;
			if (node->query("file",string))
				if (folder.findFile(string,file))
					layout.loadFromFile(file.getLocation(),scale*outer_scale);
		}
		
		static void loadLayout(XML::Node*xtheme, FileSystem::Folder&folder, const String&path, CheckBox::TStyle&layout, float outer_scale)
		{
			XML::Node*node = xtheme->find(path);
			if (!node)
				return;	//graceful ignore
			String string;

			FileSystem::File	file;
			
			if (node->query("color",string) && folder.findFile(string,file))
				loadColor(file.getLocation(),layout.box_color);
			if (node->query("bump",string) && folder.findFile(string,file))
				loadBump(file.getLocation(),layout.box_normal);
			if (node->query("check",string) && folder.findFile(string,file))
				loadColor(file.getLocation(),layout.check_mark);
			if (node->query("highlight",string) && folder.findFile(string,file))
				loadColor(file.getLocation(),layout.highlight_mark);
		}
		
		
		void		loadTheme(const String&filename,float outer_scale)
		{
			XML::Container	xml;
			xml.loadFromFile(filename);
			FileSystem::Folder	folder(FileSystem::extractFilePath(filename));
			String	string;
			XML::Node*xtheme = xml.find("theme");
			if (!xtheme)
				throw IO::DriveAccess::FileFormatFault(globalString("XML theme file lacks theme XML root node"));
				
			FileSystem::File	file;
			if (XML::Node*node = xtheme->find("font"))
			{
				float scale = 1.0f;
				if (node->query("scale",string))
					convert(string.c_str(),scale);
				if (!node->query("file",string))
					throw IO::DriveAccess::FileFormatFault("XML Font node lacks file attribute");
				if (folder.findFile(string,file))
					Component::textout.getFont().loadFromFile(file.getLocation(),scale*outer_scale);
						//FAIL("Failed to load font from font file '"+file.getLocation()+"'");
			}
			else
				throw IO::DriveAccess::FileFormatFault(globalString("XML theme file lacks theme/font node"));
			
			loadLayout(xtheme,folder,"button",Button::global_layout,outer_scale);
			loadLayout(xtheme,folder,"panel",Panel::global_layout,outer_scale);
			loadLayout(xtheme,folder,"combobox",ComboBox::global_layout,outer_scale);
			loadLayout(xtheme,folder,"edit",Edit::global_layout,outer_scale);
			loadLayout(xtheme,folder,"scrollbox",ScrollBox::global_layout,outer_scale);
			loadLayout(xtheme,folder,"scrollbar/style",ScrollBarLayout::global,outer_scale);
			loadLayout(xtheme,folder,"window/common",Window::common_style,outer_scale);
			loadLayout(xtheme,folder,"window/menu",Window::menu_style,outer_scale);
			loadLayout(xtheme,folder,"window/hint",Window::hint_style,outer_scale);
			loadLayout(xtheme,folder,"checkbox/style",CheckBox::global_style,outer_scale);
			loadLayout(xtheme,folder,"slider/style",SliderLayout::global,outer_scale);
			
			/*
			if (Button::global_layout.copy_from)
				Button::global_layout = *Button::global_layout.copy_from;
			if (Panel::global_layout.copy_from)
				Panel::global_layout = *Panel::global_layout.copy_from;
			if (ComboBox::global_layout.copy_from)
				ComboBox::global_layout = *ComboBox::global_layout.copy_from;
			if (Edit::global_layout.copy_from)
				Edit::global_layout = *Edit::global_layout.copy_from;
			if (ScrollBox::global_layout.copy_from)
				ScrollBox::global_layout = *ScrollBox::global_layout.copy_from;
			if (Window::common_style.copy_from)
				Window::common_style = *Window::common_style.copy_from;
			if (Window::menu_style.copy_from)
				Window::menu_style = *Window::menu_style.copy_from;
			if (Window::hint_style.copy_from)
				Window::hint_style = *Window::hint_style.copy_from;
			*/
			

		}
		
		
		void			showChoice(Operator&op, const String&title, const String&query, const Array<String>&choices, const function<void(index_t)>&onSelect)
		{
			if (choices.isEmpty())
				return;
			shared_ptr<Label>	message_label = shared_ptr<Label>(new Label());
			shared_ptr<Panel>	panel = shared_ptr<Panel>(new Panel());
			Array<shared_ptr<Button> >	buttons(choices.count());
			for (index_t i = 0; i < buttons.count(); i++)
			{
				buttons[i] = shared_ptr<Button>(new Button());
				buttons[i]->anchored.set(true,true,false,false);
				buttons[i]->setCaption(choices[i]);
				buttons[i]->width = std::max(buttons[i]->width,100.f);
			}
				//message_button->on_execute += hideMessage;
			
			message_label->setText(query);
			message_label->wrap_text = true;
			message_label->anchored.set(true,true,true,true);
			panel->add(message_label);
			panel->add(buttons.first());
			for (index_t i = 1; i < buttons.count(); i++)
				panel->appendRight(buttons[i]);

			message_label->offset.bottom = buttons.first()->height;

			float	mx = op.getDisplay().clientWidth()/2,
					my = op.getDisplay().clientHeight()/2;
			shared_ptr<GUI::Window>	window = op.createWindow(Rect<float>(mx-200,my-100,mx+200,my+100),title,panel);
			window->is_modal = true;
			window->fixed_size = true;
			window->iheight = (size_t)(window->fheight = window->minHeight());
			window->layout_changed = true;
			window->visual_changed = true;
			op.insertWindow(window);
			Component::setFocused(buttons[0]);
			
			weak_ptr<GUI::Window>	weak_window = window;
			for (index_t i = 0; i < buttons.count(); i++)
				buttons[i]->on_execute += [weak_window,i,onSelect]()
				{
					shared_ptr<GUI::Window>	window = weak_window.lock();
					if (window)
					{
						shared_ptr<GUI::Operator>&op = window->operator_link.lock();
						if (op)
							op->removeWindow(window);
					}
					onSelect(i);
				};
		}


		static shared_ptr<Window>	message_window;
		static shared_ptr<Label>	message_label;
		static shared_ptr<Button>	message_button;
		static void createMessageWindow(Operator&op)
		{
			if (message_window)
				return;
			
			message_label = shared_ptr<Label>(new Label());
			shared_ptr<Panel>	panel = shared_ptr<Panel>(new Panel());
			message_button = shared_ptr<Button>(new Button());
			
			message_button->anchored.set(false,true,false,false);
			message_button->setCaption("OK");
			message_button->width = 100;
			message_button->on_execute += hideMessage;
			
			message_label->setText("message");
			message_label->wrap_text = true;
			message_label->anchored.set(true,true,true,true);
			panel->add(message_label);
			panel->add(message_button);
			message_label->offset.bottom = message_button->height;

			float	mx = op.getDisplay().clientWidth()/2,
					my = op.getDisplay().clientHeight()/2;
			message_window = op.createWindow(Rect<float>(mx-200,my-100,mx+200,my+100),"Message",panel);
			message_window->is_modal = true;
			message_window->fixed_size = true;
		}
		
		
		void	showMessage(Operator&op, const String&title, const String&message)
		{
			createMessageWindow(op);
			message_window->title = title;
			
			//message_window->updateLayout();
			message_label->setText(message);
			
			message_window->updateLayout();
			message_window->iheight = (size_t)(message_window->fheight = message_window->minHeight());
			message_window->layout_changed = true;
			message_window->visual_changed = true;
			
			#ifdef DEEP_GUI
				message_window->current_center.x = message_window->origin.x = message_window->destination.x = 0;
				message_window->current_center.y = message_window->origin.y = message_window->destination.y = 0;
				message_window->current_center.shell_radius = 1;
				message_window->origin.shell_radius = 1;
				message_window->destination.shell_radius = 1;
			#else
				message_window->x = 0;
				message_window->y = 0;
			#endif
			//ShowMessage("inserting window");
			op.insertWindow(message_window);
			Component::setFocused(message_button);
		}
		
		void	showMessage(Operator&op, const String&message)
		{
			showMessage(op,"Message",message);
		}
		
		bool	showingMessage()
		{
			return message_window && !message_window->operator_link.expired() && message_window->operator_link.lock()->windowIsVisible(message_window);
		}
		
		void	hideMessage()
		{
			if (message_window && !message_window->operator_link.expired())
				message_window->operator_link.lock()->removeWindow(message_window);
		}
		
	}
}

