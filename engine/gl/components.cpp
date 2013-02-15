#include "../../global_root.h"
#include "components.h"



namespace Engine
{
	namespace GUI
	{
	
		Layout								Button::globalLayout,
											Panel::globalLayout,
											Edit::globalLayout,
											ComboBox::globalLayout,
											Menu::globalLayout;
		ScrollBarLayout						ScrollBarLayout::global;
		CheckBox::TStyle					CheckBox::globalStyle;
		SliderLayout						SliderLayout::global;

	
	
	
		bool		Panel::GetChildSpace(Rect<float>&outRect)	const
		{
			if (children.isEmpty())
				return false;
			const shared_ptr<Component>&first = children.first();
			outRect = first->currentRegion;
				
			for (index_t i = 1; i < children.count(); i++)
			{
				const shared_ptr<Component>&child = children[i];
				outRect.include(child->currentRegion);
			}
			return true;
		}
		
		void			Panel::Append(const shared_ptr<Component>&component)
		{
			DBG_ASSERT__(!children.contains(component));
			component->anchored.set(true,false,false,true);
			if (children.isNotEmpty())
				component->offset.top = children.last()->offset.top-children.last()->height;
			else
				component->offset.top = 0;
			component->offset.left = 0;
			component->SetWindow(windowLink);
			children << component;

			SignalLayoutChange();
		}
		
		void			Panel::AppendRight(const shared_ptr<Component>&component)
		{
			DBG_ASSERT__(!children.contains(component));
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
			component->SetWindow(windowLink);
			children << component;

			SignalLayoutChange();
		}
		
		bool					Panel::Add(const shared_ptr<Component>&component)
		{
			if (!component || children.contains(component))
				return false;
			children << component;
			component->SetWindow(windowLink);
			SignalLayoutChange();
			return true;
		}
		
		bool					Panel::Erase(const shared_ptr<Component>&component)
		{
			if (children.findAndErase(component))
			{
				component->SetWindow(shared_ptr<Window>());
				SignalLayoutChange();
				return true;
			}
			return false;
		}
		
		bool					Panel::Erase(index_t index)
		{
			if (index < children.count())
			{
				children[index]->SetWindow(shared_ptr<Window>());
				children.erase(index);
				SignalLayoutChange();
				return true;
			}
			return false;
		}
		
		
		bool					Panel::MoveChildUp(const shared_ptr<Component>&component)
		{
			index_t index;
			if ((index = children.indexOf(component)) != -1)
				return MoveChildUp(index);
			return false;
		}
		
		bool					Panel::MoveChildUp(index_t index)
		{
			if (index+1 >= children.count())
				return false;
			Buffer<shared_ptr<Component>,4>::AppliedStrategy::swap(children[index],children[index+1]);
			SignalVisualChange();
			return true;
		}
		

		
		bool					Panel::MoveChildDown(const shared_ptr<Component>&component)
		{
			index_t index;
			if ((index = children.indexOf(component)) != -1)
				return MoveChildDown(index);
			return false;
		}
		
		bool					Panel::MoveChildDown(index_t index)
		{
			if (!index || index >= children.count())
				return false;
			Buffer<shared_ptr<Component>,4>::AppliedStrategy::swap(children[index-1],children[index]);
			SignalVisualChange();
			return true;
		}

		bool					Panel::MoveChildToTop(const shared_ptr<Component>&component)
		{
			index_t index;
			if ((index = children.indexOf(component))!=-1)
				return MoveChildToTop(index);
			return false;
		}
		
		bool					Panel::MoveChildToTop(index_t index)
		{
			if (index+1 < children.count())
			{
				shared_ptr<Component> cmp = children[index];
				children.erase(index);
				children << cmp;
				SignalVisualChange();
				return true;
			}
			return false;
		}
		
		bool					Panel::MoveChildToBottom(const shared_ptr<Component>&component)
		{
			index_t index;
			if ((index = children.indexOf(component)) != -1)
				return MoveChildToBottom(index);
			return false;
		}
		
		bool					Panel::MoveChildToBottom(index_t index)
		{
			if (index && index < children.count())
			{
				shared_ptr<Component> cmp = children[index];
				children.erase(index);
				children.insert(0,cmp);
				SignalVisualChange();
				return true;
			}
			return false;
		}
		
		void		Panel::UpdateLayout(const Rect<float>&parent_region)
		{
			Component::UpdateLayout(parent_region);
			for (index_t i = 0; i < children.count(); i++)
				children[i]->UpdateLayout(cellLayout.client);
		}
		
		float		Panel::GetClientMinWidth()	const
		{
			float rs = 0;
			
			for (index_t i = 0; i < children.count(); i++)
			{
				float c = children[i]->GetMinWidth(true);
				if (c > rs)
					rs = c;
			}
			return rs;
		}
		
		float		Panel::GetClientMinHeight()	const
		{
			float rs = 0;
			
			for (index_t i = 0; i < children.count(); i++)
			{
				float c = children[i]->GetMinHeight(true);
				if (c > rs)
					rs = c;
			}
			return rs;
		}
		
		/*virtual override*/ void		Panel::OnColorPaint(ColorRenderer&renderer, bool parentIsEnabled)
		{
			renderer.PushColor();
				Component::OnColorPaint(renderer,parentIsEnabled);
				bool subEnabled = parentIsEnabled && IsEnabled();
				renderer.Clip(cellLayout.client);
					for (index_t i = 0; i < children.count(); i++)
						if (children[i]->IsVisible())
						{
							renderer.PeekColor();
							children[i]->OnColorPaint(renderer,subEnabled);
						}
				renderer.Unclip();
			renderer.PopColor();
		}
		
		/*virtual override*/ void		Panel::OnNormalPaint(NormalRenderer&renderer, bool parentIsEnabled)
		{
			renderer.PushNormalScale();
				Component::OnNormalPaint(renderer,parentIsEnabled);
				bool subEnabled = parentIsEnabled && IsEnabled();
			
				renderer.Clip(cellLayout.client);
					for (index_t i = 0; i < children.count(); i++)
						if (children[i]->IsVisible())
						{
							renderer.PeekNormalScale();
							children[i]->OnNormalPaint(renderer,subEnabled);
						}
				renderer.Unclip();
			renderer.PopNormalScale();
		}

		/*virtual override*/ PComponent			Panel::GetComponent(float x, float y, ePurpose purpose, bool&outIsEnabled)
		{
			outIsEnabled &= IsEnabled();
			for (index_t i = children.count()-1; i < children.count(); i--)
			{
				const shared_ptr<Component>&child = children[i];
				if (child->IsVisible() && child->cellLayout.border.contains(x,y))
					return child->GetComponent(x,y,purpose,outIsEnabled);
			}
			return shared_from_this();
		}

		
		

		
		void	ScrollBarLayout::LoadFromFile(const String&filename, float scale)
		{
			/*
				TTexture				backCenter,
										backBottom,
										cursorCenter,
										cursorBottom,
										bottomButton;
				float					buttonIndent;
			*/
			
			FileSystem::Folder	folder(FileSystem::extractFilePath(filename));
			
			XML::Container	xml;
			xml.LoadFromFile(filename);
				

			String attrib;
			
			XML::Node	*xback = xml.find("scrollbar/background"),
						*xcursor = xml.find("scrollbar/cursor"),
						*xbutton = xml.find("scrollbar/button");
			if (!xback || !xcursor || !xbutton)
				throw IO::DriveAccess::FileFormatFault(globalString(__func__": Required XML nodes scrollbar/background, scrollbar/cursor, and/or scrollbar/button not found"));

			TTexture::load(xback,"center",folder,backCenter,scale);

			TTexture::load(xback,"bottom",folder,backBottom,scale);
			TTexture::load(xcursor,"center",folder,cursorCenter,scale);
			TTexture::load(xcursor,"bottom",folder,cursorBottom,scale);
			TTexture::load(xbutton,"background",folder,bottomButton,scale);

			if (!xbutton->query("indent",attrib) || !convert(attrib.c_str(),buttonIndent))
				throw IO::DriveAccess::FileFormatFault(globalString(__func__": Failed to find or parse scrollbar/button attribute 'indent'"));
			
			
			buttonIndent *= scale;
			minWidth = vmax(vmax(vmax(backCenter.width,backBottom.width),vmax(cursorCenter.width,cursorBottom.width)),bottomButton.width);
			minHeight = 2*(bottomButton.height-buttonIndent+backBottom.height);
		}
		
		
		
		void	SliderLayout::LoadFromFile(const String&filename, float scale)
		{
			/*
				TTexture				backCenter,
										backBottom,
										cursorCenter,
										cursorBottom,
										bottomButton;
				float					buttonIndent;
			*/
			
			FileSystem::Folder	folder(FileSystem::extractFilePath(filename));
			
			XML::Container	xml;
			xml.LoadFromFile(filename);
				

			String attrib;
			
			XML::Node	*xbar = xml.find("slider/bar"),
						*xslider = xml.find("slider/slider");
			if (!xbar || !xslider)
				throw IO::DriveAccess::FileFormatFault(globalString(__func__": Required XML nodes slider/bar and/or slider/slider not found"));

			TTexture::load(xbar,"left",folder,barLeft,scale);
			TTexture::load(xbar,"center",folder,barCenter,scale);
			TTexture::load(xslider,"background",folder,slider,scale);

			minWidth = vmax(2*barLeft.width+barCenter.width,slider.width);
			minHeight = vmax(vmax(barLeft.height,barCenter.height),slider.height);
		}
		
		
		
		
		
		
		
		
				
		void		Slider::_Setup()
		{
			cursorGrabbed = false;
			sliderLayout = &SliderLayout::global;
			current = 0;
			max = 1;
		}
		

		void		Slider::OnSlide()
		{
			onSlide();
		}
		
		
		
		
		void		Slider::_UpdateCursorRegion()
		{
			if (!sliderLayout)
				return;
			float width = sliderLayout->slider.width;
			slideFrom = cellLayout.client.left()+width/2;
			slideRange = cellLayout.client.x.max-cellLayout.client.left()-width;
			float center = slideFrom+current/max*slideRange;
			cursorRegion.left() = center-width/2;
			cursorRegion.x.max = center+width/2;
			cursorRegion.top() = cellLayout.client.y.center()+sliderLayout->slider.height/2;
			cursorRegion.bottom() = cursorRegion.top()-sliderLayout->slider.height;
			slider.region = cursorRegion;			
		}
		
		
		
		void		Slider::UpdateLayout(const Rect<float>&parent_region)
		{
			Component::UpdateLayout(parent_region);
			_UpdateCursorRegion();
			if (sliderLayout)
			{
				barLeft.region = cellLayout.client;
				barLeft.region.x.max = barLeft.region.left()+sliderLayout->barLeft.width;
				barLeft.region.top() = cellLayout.client.y.center()+sliderLayout->barLeft.height/2;
				barLeft.region.bottom() = barLeft.region.top()-sliderLayout->barLeft.height;
				barLeft.color = &sliderLayout->barLeft.color;
				barLeft.normal = &sliderLayout->barLeft.normal;
				barLeft.orientation = 0;
				
				barRight.region = cellLayout.client;
				barRight.region.left() = barRight.region.x.max-sliderLayout->barLeft.width;
				barRight.region.top() = cellLayout.client.y.center()+sliderLayout->barLeft.height/2;
				barRight.region.bottom() = barRight.region.top()-sliderLayout->barLeft.height;
				barRight.color = &sliderLayout->barLeft.color;
				barRight.normal = &sliderLayout->barLeft.normal;
				barRight.orientation = 2;
				
				
				barCenter.region = cellLayout.client;
				barCenter.region.left() = barLeft.region.x.max;
				barCenter.region.x.max = barRight.region.left();
				barCenter.region.top() = cellLayout.client.y.center()+sliderLayout->barCenter.height/2;
				barCenter.region.bottom() = barCenter.region.top()-sliderLayout->barCenter.height;
				barCenter.color = &sliderLayout->barCenter.color;
				barCenter.normal = &sliderLayout->barCenter.normal;
				barCenter.orientation = 0;
				
				//ShowMessage(barCenter.region.toString());
				
				slider.region = cursorRegion;
				slider.color = &sliderLayout->slider.color;
				slider.normal = &sliderLayout->slider.normal;
				slider.orientation = 0;
			}
		}
		
		float		Slider::GetClientMinWidth()	const
		{
			if (!sliderLayout)
				return 0;
			return sliderLayout->minWidth;
		}
		
		float		Slider::GetClientMinHeight()	const
		{
			if (!sliderLayout)
				return 0;
			return sliderLayout->minHeight;
		}
		
		

		void		Slider::OnColorPaint(ColorRenderer&renderer, bool parentIsEnabled)
		{
			Component::OnColorPaint(renderer,parentIsEnabled);

			renderer.Paint(barLeft);
			renderer.Paint(barCenter);
			renderer.Paint(barRight);
			renderer.MarkNewLayer();
			renderer.Paint(slider);
		}
		
		void		Slider::OnNormalPaint(NormalRenderer&renderer, bool parentIsEnabled)
		{
			Component::OnNormalPaint(renderer,parentIsEnabled);

			//Display<OpenGL>&display = requireOperator()->getDisplay();
			renderer.Paint(barLeft,false);
			renderer.Paint(barCenter,false);
			renderer.Paint(barRight,false);
			renderer.MarkNewLayer();
			renderer.Paint(slider,cursorGrabbed);
		}
		
		Component::eEventResult	Slider::OnMouseDrag(float x, float y)
		{
			if (cursorGrabbed)
			{
				float	relative = x-cursorRegion.left(),
						delta = (relative-cursorHook[0])*max/slideRange;
				current = clamped(current+delta,0,max);
				
				_UpdateCursorRegion();
				
				OnSlide();
				return RequestingRepaint;
			}
			return Handled;
		}
		
		Component::eEventResult	Slider::OnMouseDown(float x, float y, TExtEventResult&ext)
		{
			if (!enabled)
				return Unsupported;
			if (cursorRegion.contains(x,y) && max > 0)
			{
				cursorHook[0] = x-cursorRegion.left();
				cursorHook[1] = y-cursorRegion.bottom();
				cursorGrabbed = true;
				return RequestingRepaint;
			}
			return Unsupported;
				
		}
		
		Component::eEventResult		Slider::OnMouseUp(float x, float y)
		{
			bool changed =  cursorGrabbed;
			cursorGrabbed = false;
			return changed?RequestingRepaint:Handled;
		}
		
		Component::eEventResult		Slider::OnMouseWheel(float x, float y, short delta)
		{
			float new_current = clamped(current + delta/max,0,1);

			if (current != new_current)
			{
				current = new_current;
				_UpdateCursorRegion();
				OnSlide();
				return RequestingRepaint;
			}
			return Handled;
		}
		
		
		
		
		
		
		
		
		
		
				
		
		void		ScrollBar::_Setup()
		{
			scrollable.reset();
			horizontal = false;
			upPressed = false;
			downPressed = false;
			cursorGrabbed = false;
			scrollLayout = &ScrollBarLayout::global;
			autoVisibility = false;
		}

		void		ScrollBar::ScrollTo(float v)
		{
			scrollData.current = v;
			OnScroll();
		}
		
		void		ScrollBar::OnScroll()
		{
			if (shared_ptr<Scrollable> scrollable_ = scrollable.lock())
			{
				if (horizontal)
				{
					scrollable_->horizontal = scrollData;
					scrollable_->OnHorizontalScroll();
				}
				else
				{
					scrollable_->vertical = scrollData;
					scrollable_->OnVerticalScroll();
				}
			}
			onScroll();
		}
		
		
		
		void		ScrollBar::UpdateLayout(const Rect<float>&parent_region)
		{
			Component::UpdateLayout(parent_region);
			if (scrollLayout)
			{
				float 	current = clamped(scrollData.current,0,1);
				if (!horizontal)
				{
					upButton.region = cellLayout.client;
					upButton.region.bottom() = upButton.region.top()-scrollLayout->bottomButton.height;
					upButton.color = &scrollLayout->bottomButton.color;
					upButton.normal = &scrollLayout->bottomButton.normal;
					upButton.orientation = 2;
					
					downButton.region = cellLayout.client;
					downButton.region.top() = downButton.region.bottom()+scrollLayout->bottomButton.height;
					downButton.color = &scrollLayout->bottomButton.color;
					downButton.normal = &scrollLayout->bottomButton.normal;
					downButton.orientation = 0;
				
					backgroundTop.region = cellLayout.client;
					backgroundTop.region.bottom() = backgroundTop.region.top()-scrollLayout->backBottom.height;
					backgroundTop.color = &scrollLayout->backBottom.color;
					backgroundTop.normal = &scrollLayout->backBottom.normal;
					backgroundTop.orientation = 2;
					
					
					backgroundBottom.region = cellLayout.client;
					backgroundBottom.region.top() = backgroundBottom.region.bottom()+scrollLayout->backBottom.height;
					backgroundBottom.color = &scrollLayout->backBottom.color;
					backgroundBottom.normal = &scrollLayout->backBottom.normal;
					backgroundBottom.orientation = 0;
					
					backgroundCenter.region = cellLayout.client;
					backgroundCenter.region.top() = backgroundTop.region.bottom();
					backgroundCenter.region.bottom() = backgroundBottom.region.top();
					backgroundCenter.color = &scrollLayout->backCenter.color;
					backgroundCenter.normal = &scrollLayout->backCenter.normal;
					backgroundCenter.orientation = 0;

					
					cursorRange = cellLayout.client.height()-2*scrollLayout->bottomButton.height+2*scrollLayout->buttonIndent;
					float	display_range = scrollData.max-scrollData.min,
							cursorLength = display_range>scrollData.window?cursorRange*scrollData.window/display_range:cursorRange;
					
					if (cursorLength < 2*scrollLayout->cursorBottom.height)
						cursorLength = 2*scrollLayout->cursorBottom.height;
					cursorRange -= cursorLength;
					cursorTop.region.left() = cellLayout.client.left();
					cursorTop.region.x.max = cellLayout.client.x.max;
					cursorTop.region.top() = cellLayout.client.top()-scrollLayout->bottomButton.height+scrollLayout->buttonIndent-cursorRange*current;
					cursorTop.region.bottom() = cursorTop.region.top()-scrollLayout->cursorBottom.height;
					cursorTop.color = &scrollLayout->cursorBottom.color;
					cursorTop.normal = &scrollLayout->cursorBottom.normal;
					cursorTop.orientation = 2;
					
					cursorCenter.region.left() = cellLayout.client.left();
					cursorCenter.region.x.max = cellLayout.client.x.max;
					cursorCenter.region.top() = cursorTop.region.bottom();
					cursorCenter.region.bottom() = cursorCenter.region.top()-cursorLength+2*scrollLayout->cursorBottom.height;
					cursorCenter.color = &scrollLayout->cursorCenter.color;
					cursorCenter.normal = &scrollLayout->cursorCenter.normal;
					cursorCenter.orientation = 0;
					
					cursorBottom.region.left() = cellLayout.client.left();
					cursorBottom.region.x.max = cellLayout.client.x.max;
					cursorBottom.region.top() = cursorCenter.region.bottom();
					cursorBottom.region.bottom() = cursorBottom.region.top()-scrollLayout->cursorBottom.height;
					cursorBottom.color = &scrollLayout->cursorBottom.color;
					cursorBottom.normal = &scrollLayout->cursorBottom.normal;
					cursorBottom.orientation = 0;
					
					cursorRegion.set(cellLayout.client.left(),cursorBottom.region.bottom(),cellLayout.client.x.max,cursorTop.region.top());
				}
				else
				{
					current = 1.0f-current;
					downButton.region = cellLayout.client;
					downButton.region.left() = downButton.region.x.max-scrollLayout->bottomButton.height;
					downButton.color = &scrollLayout->bottomButton.color;
					downButton.normal = &scrollLayout->bottomButton.normal;
					downButton.orientation = 3;
					
					upButton.region = cellLayout.client;
					upButton.region.x.max = upButton.region.left()+scrollLayout->bottomButton.height;
					upButton.color = &scrollLayout->bottomButton.color;
					upButton.normal = &scrollLayout->bottomButton.normal;
					upButton.orientation = 1;
				
					backgroundTop.region = cellLayout.client;
					backgroundTop.region.left() = backgroundTop.region.x.max-scrollLayout->backBottom.height;
					backgroundTop.color = &scrollLayout->backBottom.color;
					backgroundTop.normal = &scrollLayout->backBottom.normal;
					backgroundTop.orientation = 3;
					
					
					backgroundBottom.region = cellLayout.client;
					backgroundBottom.region.x.max = backgroundBottom.region.left()+scrollLayout->backBottom.height;
					backgroundBottom.color = &scrollLayout->backBottom.color;
					backgroundBottom.normal = &scrollLayout->backBottom.normal;
					backgroundBottom.orientation = 1;
					
					backgroundCenter.region = cellLayout.client;
					backgroundCenter.region.x.max = backgroundTop.region.left();
					backgroundCenter.region.left() = backgroundBottom.region.x.max;
					backgroundCenter.color = &scrollLayout->backCenter.color;
					backgroundCenter.normal = &scrollLayout->backCenter.normal;
					backgroundCenter.orientation = 1;
					
					
					cursorRange = cellLayout.client.width()-2*scrollLayout->bottomButton.height+2*scrollLayout->buttonIndent;
					float	display_range = scrollData.max-scrollData.min,
							cursorLength = display_range>scrollData.window?cursorRange*scrollData.window/display_range:cursorRange;
					
					if (cursorLength < 2*scrollLayout->cursorBottom.height)
						cursorLength = 2*scrollLayout->cursorBottom.height;
					cursorRange -= cursorLength;
					cursorTop.region.top() = cellLayout.client.top();
					cursorTop.region.bottom() = cellLayout.client.bottom();
					cursorTop.region.x.max = cellLayout.client.x.max-scrollLayout->bottomButton.height+scrollLayout->buttonIndent-cursorRange*current;
					cursorTop.region.left() = cursorTop.region.x.max-scrollLayout->cursorBottom.height;
					cursorTop.color = &scrollLayout->cursorBottom.color;
					cursorTop.normal = &scrollLayout->cursorBottom.normal;
					cursorTop.orientation = 3;
					
					cursorCenter.region.bottom() = cellLayout.client.bottom();
					cursorCenter.region.top() = cellLayout.client.top();
					cursorCenter.region.x.max = cursorTop.region.left();
					cursorCenter.region.left() = cursorCenter.region.x.max-cursorLength+2*scrollLayout->cursorBottom.height;
					cursorCenter.color = &scrollLayout->cursorCenter.color;
					cursorCenter.normal = &scrollLayout->cursorCenter.normal;
					cursorCenter.orientation = 1;
					
					cursorBottom.region.bottom() = cellLayout.client.bottom();
					cursorBottom.region.top() = cellLayout.client.top();
					cursorBottom.region.x.max = cursorCenter.region.left();
					cursorBottom.region.left() = cursorBottom.region.x.max-scrollLayout->cursorBottom.height;
					cursorBottom.color = &scrollLayout->cursorBottom.color;
					cursorBottom.normal = &scrollLayout->cursorBottom.normal;
					cursorBottom.orientation = 1;
					
					cursorRegion.set(cursorBottom.region.left(),cellLayout.client.bottom(),cursorTop.region.x.max,cellLayout.client.top());
				}
			
			}
		}
		
		float		ScrollBar::GetClientMinWidth()	const
		{
			if (!scrollLayout)
				return 0;
			if (horizontal)
				return scrollLayout->minHeight;
			return scrollLayout->minWidth;
		}
		
		float		ScrollBar::GetClientMinHeight()	const
		{
			if (!scrollLayout)
				return 0;
			if (horizontal)
				return scrollLayout->minWidth;
			return scrollLayout->minHeight;
		}
		
		

		/*virtual override*/	void		ScrollBar::OnColorPaint(ColorRenderer&renderer, bool parentIsEnabled)
		{
			Component::OnColorPaint(renderer,parentIsEnabled);
			renderer.Paint(backgroundTop);
			renderer.Paint(backgroundCenter);
			renderer.Paint(backgroundBottom);
			
			renderer.MarkNewLayer();
			renderer.PushColor();
			if (cursorGrabbed)
				renderer.ModulateColor(0.7f);
			renderer.Paint(cursorTop);
			renderer.Paint(cursorCenter);
			renderer.Paint(cursorBottom);
			renderer.PopColor();
			renderer.MarkNewLayer();
			
			renderer.Paint(upButton);
			renderer.Paint(downButton);
		}
		
		/*virtual override*/	void		ScrollBar::OnNormalPaint(NormalRenderer&renderer, bool parentIsEnabled)
		{
			Component::OnNormalPaint(renderer,parentIsEnabled);

			renderer.Paint(backgroundTop,false);
			renderer.Paint(backgroundCenter,false);
			renderer.Paint(backgroundBottom,false);
			renderer.MarkNewLayer();
			
			renderer.Paint(cursorTop,cursorGrabbed);
			renderer.Paint(cursorCenter,cursorGrabbed);
			renderer.Paint(cursorBottom,cursorGrabbed);
			
			renderer.MarkNewLayer();
			renderer.Paint(upButton,false);
			renderer.Paint(downButton,false);
		}
		
		/*virtual override*/	Component::eEventResult	ScrollBar::OnMouseDrag(float x, float y)
		{
			if (cursorGrabbed && cursorRange > 0)
			{
				if (horizontal)
				{
					float	relative = x-cursorRegion.left(),
							delta = (relative-cursorHook[0])/cursorRange;
					scrollData.current+=delta;
					scrollData.current = clamped(scrollData.current,0,1);
				}
				else
				{
					float	relative = y-cursorRegion.bottom(),
							delta = (relative-cursorHook[1])/cursorRange;
					scrollData.current-=delta;
					scrollData.current = clamped(scrollData.current,0,1);
				}
				OnScroll();
				return RequestingReshape;
			}
			return Handled;
		}
		
		/*virtual override*/	Component::eEventResult	ScrollBar::OnMouseDown(float x, float y, TExtEventResult&ext)
		{
			if (cursorRegion.contains(x,y) && cursorRange > 0)
			{
				cursorHook[0] = x-cursorRegion.left();
				cursorHook[1] = y-cursorRegion.bottom();
				cursorGrabbed = true;
			}
			elif (upButton.region.contains(x,y))
			{
				upPressed = true;
				if ((scrollData.max-scrollData.min) > 0)
				{
					scrollData.current = clamped(scrollData.current - 20/(scrollData.max-scrollData.min),0,1);
					OnScroll();
				}
			}
			elif (downButton.region.contains(x,y))
			{
				downPressed = true;
				if ((scrollData.max-scrollData.min) > 0)
				{
					scrollData.current = clamped(scrollData.current + 20/(scrollData.max-scrollData.min),0,1);
					OnScroll();
				}
			}
				
			return RequestingReshape;
		}
		
		/*virtual override*/	Component::eEventResult		ScrollBar::OnMouseUp(float x, float y)
		{
			bool changed =  cursorGrabbed;
			upPressed = false;
			downPressed = false;
			cursorGrabbed = false;
			return changed?RequestingRepaint:Handled;
		}
		
		/*virtual override*/	Component::eEventResult		ScrollBar::OnMouseWheel(float x, float y, short delta)
		{
			float new_current = horizontal?
									clamped(scrollData.current + delta/(scrollData.max-scrollData.min),0,1):
									clamped(scrollData.current - delta/(scrollData.max-scrollData.min),0,1);
			if (scrollData.current != new_current)
			{
				scrollData.current = new_current;
				OnScroll();
				return RequestingReshape;
			}
			return Handled;
		}
		
		
			/*
					if (!enabled)
				return NULL;
			bool new_pressed = down && currentRegion.contains(x,y);
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

		void					ScrollBox::ScrollTo(float x, float y)
		{
			horizontalBar->ScrollTo(x);
			verticalBar->ScrollTo(y);
		}
		
		void					ScrollBox::scrollToX(float x)
		{
			horizontalBar->ScrollTo(x);
		}

		void					ScrollBox::scrollToY(float y)
		{
			verticalBar->ScrollTo(y);
		}

		
		/*virtual override*/	bool					ScrollBox::Erase(const shared_ptr<Component>&component)
		{
			if (children.findAndErase(component))
			{
				visible_children.findAndErase(component);
				SignalLayoutChange();
				component->SetWindow(shared_ptr<Window>());
				return true;
			}
			return false;
		}
		
		/*virtual override*/	bool					ScrollBox::Erase(index_t index)
		{
			if (index >= children.count())
				return false;
			visible_children.findAndErase(children[index]);
			children[index]->SetWindow(shared_ptr<Window>());
			children.erase(index);
			SignalLayoutChange();
			return true;
		}
		
		/*virtual override*/	shared_ptr<const Component>		ScrollBox::GetChild(index_t index) const
		{
			if (index < children.count())
				return children[index];
			index -= children.count();
			if (index == 0)
				return horizontalBar;
			if (index == 1)
				return verticalBar;
			return shared_ptr<const Component>();
		}
		
		/*virtual override*/	shared_ptr<Component>		ScrollBox::GetChild(index_t index)
		{
			if (index < children.count())
				return children[index];
			index -= children.count();
			if (index == 0)
				return horizontalBar;
			if (index == 1)
				return verticalBar;
			return shared_ptr<Component>();
		}

		/*virtual override*/	count_t	ScrollBox::CountChildren() const
		{
			return children.count()+2;
		}
		

		
		/*virtual override*/	void		ScrollBox::UpdateLayout(const Rect<float>&parent_region)
		{
			Component::UpdateLayout(parent_region);
			
			if (horizontalBar->scrollable.expired())
				horizontalBar->scrollable = toScrollable();
			if (verticalBar->scrollable.expired())
				verticalBar->scrollable = toScrollable();

			Rect<float>	current;
			if (children.isEmpty())
			{
				current.set(0,0,1,1);
				horizontal.current = 0.5;
				vertical.current = 0.5;
			}
			else
			{
				float	w = cellLayout.client.width(),
						h = cellLayout.client.height();
				Rect<float>	region(0,0,w,h);
				for (index_t i = 0; i < children.count(); i++)
				{
					//children[i]->UpdateLayout(cellLayout.client);
					Rect<float>	child_region;
					const shared_ptr<Component>&child = children[i];
					if (verticalBar->IsVisible())
					{
						child->anchored.top = true;
						child->anchored.bottom = false;
					}
					if (horizontalBar->IsVisible())
					{
						child->anchored.left = true;
						child->anchored.right = false;
					}
					
					child->Locate(region,child_region);
					

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
			
			
			/*	horizontal.min -= cellLayout.client.left();
				horizontal.max -= cellLayout.client.left();
				vertical.min -= cellLayout.client.top();
				vertical.max -= cellLayout.client.top();*/
			
			horizontal.window = cellLayout.client.width();
			vertical.window = cellLayout.client.height();
			
			if (horizontalBar->autoVisibility)
				horizontalBar->SetVisible(horizontal.max>horizontal.window);
			if (verticalBar->autoVisibility)
				verticalBar->SetVisible(vertical.max>vertical.window);
			
			if (verticalBar->IsVisible())
				horizontal.window-=verticalBar->width;
			if (horizontalBar->IsVisible())
				vertical.window-=horizontalBar->height;
			
			if (!horizontalBar->IsVisible() && horizontalBar->autoVisibility)
			{
				horizontalBar->SetVisible(horizontal.max>horizontal.window);
				if (horizontalBar->IsVisible())
					vertical.window-=horizontalBar->height;
			}
			if (!verticalBar->IsVisible() && verticalBar->autoVisibility)
			{
				verticalBar->SetVisible(vertical.max>vertical.window);
				if (verticalBar->IsVisible())
				{
					horizontal.window-=verticalBar->height;
					if (!horizontalBar->IsVisible() && horizontalBar->autoVisibility)
					{
						horizontalBar->SetVisible(horizontal.max>horizontal.window);
						if (horizontalBar->IsVisible())
							vertical.window-=horizontalBar->height;
					}
				}
			}

			horizontalBar->scrollData = horizontal;
			verticalBar->scrollData = vertical;
			horizontalBar->anchored.set(true,true,true,false);
			horizontalBar->offset.set(0,0,verticalBar->IsVisible()?-verticalBar->GetMinWidth(false):0,0);
			verticalBar->anchored.set(false,true,true,true);
			verticalBar->offset.set(0,horizontalBar->IsVisible()?horizontalBar->GetMinHeight(false):0,0,0);
			horizontalBar->UpdateLayout(cellLayout.client);
			verticalBar->UpdateLayout(cellLayout.client);
			effectiveClientRegion.set(cellLayout.client.left(),
										horizontalBar->IsVisible()?/*floor*/(horizontalBar->currentRegion.top()):cellLayout.client.bottom(),
										verticalBar->IsVisible()?/*ceil*/(verticalBar->currentRegion.left()):cellLayout.client.x.max,
										cellLayout.client.top());
			//effectiveClientRegion = cellLayout.client;
			
			float	hrange = (horizontal.max-effectiveClientRegion.width()),
					vrange = (vertical.max-effectiveClientRegion.height());
			if (hrange < 0)
			{
				hrange = 0;
				horizontal.current = horizontalBar->scrollData.current = 0;
			}
			if (vrange < 0)
			{
				vrange = 0;
				vertical.current = verticalBar->scrollData.current = 0;
			}

			float	offset_x = horizontalBar->IsVisible()?-(horizontal.current * hrange):0,
					offset_y = verticalBar->IsVisible()?(vertical.current * vrange):0;
			
			Rect<float>	space = effectiveClientRegion;
			space.translate(offset_x,offset_y);
			
			visible_children.reset();
			for (index_t i = 0; i < children.count(); i++)
			{
				const shared_ptr<Component>&child = children[i];
				if (!child->IsVisible())
					continue;
				child->UpdateLayout(space);
				if (effectiveClientRegion.intersects(child->currentRegion))
					visible_children << child;
			}
		}

		/*virtual override*/	bool		ScrollBox::CanHandleMouseWheel()	const
		{
			return (horizontalBar->IsVisible() && horizontalBar->IsEnabled()) || (verticalBar->IsVisible() && horizontalBar->IsEnabled());
		}

		/*virtual override*/	Component::eEventResult		ScrollBox::OnMouseWheel(float x, float y, short delta)
		{
			if (horizontalBar->IsVisible() && horizontalBar->IsEnabled() && horizontalBar->currentRegion.contains(x,y))
			{
				if (horizontalBar->scrollable.expired())
					horizontalBar->scrollable = toScrollable();
				return horizontalBar->OnMouseWheel(x,y,delta);
			}
			if (verticalBar->IsVisible() && horizontalBar->IsEnabled() && verticalBar->currentRegion.contains(x,y))
			{
				if (verticalBar->scrollable.expired())
					verticalBar->scrollable = toScrollable();
				return verticalBar->OnMouseWheel(x,y,delta);
			}
			if (effectiveClientRegion.contains(x,y))
			{
				
				for (index_t i = visible_children.count()-1; i < visible_children.count(); i--)
				{
					const shared_ptr<Component>&child = visible_children[i];
					if (child->currentRegion.contains(x,y))
					{
						if (!child->IsEnabled())
							break;
						eEventResult rs = child->OnMouseWheel(x,y,delta);
						if (rs == Unsupported)
							break;
						return rs;
					}
				}
			}
			if (verticalBar->IsVisible() && verticalBar->IsEnabled())
			{
				if (verticalBar->scrollable.expired())
					verticalBar->scrollable = toScrollable();
				return verticalBar->OnMouseWheel(x,y,delta);
			}
			if (horizontalBar->IsVisible() && horizontalBar->IsEnabled())
			{
				if (horizontalBar->scrollable.expired())
					horizontalBar->scrollable = toScrollable();
				return horizontalBar->OnMouseWheel(x,y,delta);
			}
			return Unsupported;
		}
		
		/*virtual override*/	float		ScrollBox::GetClientMinWidth()	const
		{
			float w = horizontalBar->IsVisible() || horizontalBar->autoVisibility?horizontalBar->GetMinWidth(false):Panel::GetClientMinWidth();
			if (verticalBar->IsVisible())
				w += verticalBar->GetMinWidth(false);
			return w;
		}
		
		/*virtual override*/	float		ScrollBox::GetClientMinHeight()	const
		{
			float h;
			if (verticalBar->IsVisible() || verticalBar->autoVisibility)
				h = verticalBar->GetMinHeight(false);
			else
			{
				h = 0;
				for (index_t i = 0; i < children.count(); i++)
					h = std::max(children[i]->GetMinHeight(true),h);
			}
			if (horizontalBar->IsVisible())
				h += horizontalBar->GetMinHeight(false);
			return h;
		}
		
		/*virtual override*/ void		ScrollBox::OnColorPaint(ColorRenderer&renderer, bool parentIsEnabled)
		{
			renderer.PushColor();
			Component::OnColorPaint(renderer,parentIsEnabled);

			renderer.Clip(effectiveClientRegion);
			
			bool subEnabled = parentIsEnabled && IsEnabled();

			for (index_t i = 0; i < visible_children.count(); i++)
			{
				renderer.PeekColor();
				visible_children[i]->OnColorPaint(renderer,subEnabled);
			}
			renderer.Unclip();
			//renderer.MarkNewLayer();
			
			if (horizontalBar->IsVisible())
			{
				renderer.PeekColor();
				horizontalBar->OnColorPaint(renderer,subEnabled);
			}
			if (verticalBar->IsVisible())
			{
				renderer.PeekColor();
				verticalBar->OnColorPaint(renderer,subEnabled);
			}
			renderer.PopColor();
		}
		
		/*virtual override*/ void		ScrollBox::OnNormalPaint(NormalRenderer&renderer, bool parentIsEnabled)
		{
			renderer.PushNormalScale();
			Component::OnNormalPaint(renderer,parentIsEnabled);
			
			renderer.Clip(effectiveClientRegion);
			
			bool subEnabled = parentIsEnabled && IsEnabled();
			
			for (index_t i = 0; i < visible_children.count(); i++)
			{
				renderer.PeekNormalScale();
				visible_children[i]->OnNormalPaint(renderer,subEnabled);
			}
			renderer.Unclip();
			//renderer.MarkNewLayer();

			if (horizontalBar->IsVisible())
			{
				renderer.PeekNormalScale();
				horizontalBar->OnNormalPaint(renderer,subEnabled);
			}
			if (verticalBar->IsVisible())
			{
				renderer.PeekNormalScale();
				verticalBar->OnNormalPaint(renderer,subEnabled);
			}
			renderer.PopNormalScale();

		}

		/*virtual override*/ PComponent			ScrollBox::GetComponent(float x, float y, ePurpose purpose, bool&outIsEnabled)
		{
			outIsEnabled &= IsEnabled();
		
			PComponent result;
			if (horizontalBar->IsVisible() && horizontalBar->IsEnabled() && horizontalBar->currentRegion.contains(x,y) && (result = horizontalBar->GetComponent(x,y,purpose,outIsEnabled)))
			{
				if (horizontalBar->scrollable.expired())
					horizontalBar->scrollable = toScrollable();
				return result;
			}
			if (verticalBar->IsVisible() && verticalBar->IsEnabled() && verticalBar->currentRegion.contains(x,y) && (result = verticalBar->GetComponent(x,y,purpose,outIsEnabled)))
			{
				if (verticalBar->scrollable.expired())
					verticalBar->scrollable = toScrollable();
				return result;
			}
			if (effectiveClientRegion.contains(x,y))
			{
				for (index_t i = visible_children.count()-1; i < visible_children.count(); i--)
				{
					const shared_ptr<Component>&child = visible_children[i];
					if (child->cellLayout.border.contains(x,y))
					{
						bool subEnabled = outIsEnabled;
						result = child->GetComponent(x,y,purpose,subEnabled);
						if (purpose == MouseWheelRequest && CanHandleMouseWheel() && outIsEnabled && (!subEnabled || !result || !result->CanHandleMouseWheel()))
							return shared_from_this();	//override child if it cannot or will not handle mouse wheel
						outIsEnabled = subEnabled;
						return result;
					}
				}
			}
			return shared_from_this();
		}
		
		void			ScrollBox::Append(const shared_ptr<Component>&component)
		{
			component->anchored.set(true,false,false,true);
			if (children.isNotEmpty())
				component->offset.top = children.last()->offset.top-children.last()->height;
			else
				component->offset.top = 0;
			component->offset.left = 0;
			component->SetWindow(windowLink);
			children << component;
			SignalLayoutChange();
		}
		
		
		void	Edit::_Setup()
		{
			goLeft = 0;
			goRight = 0;
			tickInterval = 0.1;
			readonly = false;
			maskInput = false;
			cursor = 0;
			selectionStart = 0;
			viewBegin = viewEnd = 0;
			layout = globalLayout.Refer();
			width = GetMinWidth(false);
			height = GetMinHeight(false);
		}

		void Edit::SetText(const String&new_text)
		{
			text = new_text;
			selectionStart = 0;
			cursor = 0;
			viewBegin = viewEnd = 0;
			_UpdateView();
			SignalVisualChange();
		}
		void Edit::SetText(const ReferenceExpression<char>&new_text)
		{
			text = new_text;
			selectionStart = 0;
			viewBegin = viewEnd = 0;
			_UpdateView();
			SignalVisualChange();
		}

		void						Edit::SetText(const char*new_text)
		{
			SetText(ReferenceExpression<char>(new_text));
		}


		/*virtual override*/ void			Edit::OnColorPaint(ColorRenderer&renderer, bool parentIsEnabled)
		{
			const float fontHeight = renderer.GetFont().getHeight();
			renderer.PushColor();
			if (readonly && enabled && parentIsEnabled)
				renderer.ModulateColor(0.5f);	//if not enabled, will be toned by Component::OnColorPaint
			
			Component::OnColorPaint(renderer,parentIsEnabled);

			renderer.PeekColor();

			size_t end = vmin(viewEnd,text.length());
			if (end>viewBegin)
			{
				renderer.Clip(cellLayout.client);
				float	bottom = cellLayout.client.y.center()-fontHeight/2+font_offset,
						top = cellLayout.client.y.center()+fontHeight/2;
				if (maskInput)
				{
					float csize = fontHeight*0.75;
					renderer.SetPointSize(csize*0.8);
					for (size_t i = viewBegin; i < end; i++)
						renderer.PaintPoint(cellLayout.client.left()+csize*(0.5+i),cellLayout.client.y.center());
				}
				else
				{
					renderer.SetTextPosition(cellLayout.client.left(),bottom);
					renderer.WriteText(text.root()+viewBegin,end-viewBegin);
				}
				if (selectionStart != cursor)
				{
					renderer.ModulateColor(0.2,0.3,0.5,0.7);
					size_t	sel_begin = selectionStart,
							sel_end = cursor;
					if (sel_begin > sel_end)
						swp(sel_begin,sel_end);
					if (sel_begin < viewBegin)
						sel_begin = viewBegin;
					if (sel_end < viewBegin)
						sel_end = viewBegin;
					float	left = cellLayout.client.left()+_GetTextWidth(text.root()+viewBegin,sel_begin-viewBegin),
							right = cellLayout.client.left()+_GetTextWidth(text.root()+viewBegin,sel_end-viewBegin);
						//glColor4f(0.4,0.6,1,0.7);
					renderer.FillRect(Rect<>(left-2,bottom,right+2,top));
					renderer.PeekColor();
					
					if (maskInput)
					{
						float csize = fontHeight*0.75;
						renderer.SetPointSize(csize*0.8);
						for (index_t i = sel_begin; i < sel_end; i++)
							renderer.PaintPoint(cellLayout.client.left()+csize*(0.5+i),cellLayout.client.y.center());
					}
					else
					{
						renderer.SetTextPosition(cellLayout.client.left()+renderer.GetUnscaledWidth(text.root()+viewBegin,sel_begin-viewBegin),bottom);
						renderer.WriteText(text.root()+sel_begin,sel_end-sel_begin);
					}
				}
					
				renderer.Unclip();
			}
			if (IsFocused() && enabled && parentIsEnabled && cursorTicks < 5)
			{
				renderer.MarkNewLayer();
				renderer.RenderLine(cursorOffset,cellLayout.client.y.center()-fontHeight/2,
									cursorOffset,cellLayout.client.y.center()+fontHeight/2+font_offset);
			}
			renderer.PopColor();
		}
		
		Component::eEventResult		Edit::OnTick()
		{
			count_t new_ticks = (cursorTicks+1)%10;
			bool refresh = new_ticks < 5 != cursorTicks < 5;
			cursorTicks  = new_ticks;

			
			if (goRight && !viewRightMost)
			{
				viewBegin++;
				if (cursor < text.length())
					cursor++;
				_UpdateView();
				return RequestingRepaint;
			}
			if (goLeft && viewBegin)
			{
				viewBegin--;
				if (cursor)
					cursor--;
				_UpdateView();
				return RequestingRepaint;
			}
			return refresh?RequestingRepaint:Handled;
		}

		
		Component::eEventResult		Edit::OnMouseHover(float x, float y, TExtEventResult&ext)
		{
			if (cellLayout.client.contains(x,y))
				ext.customCursor = Mouse::CursorType::EditText;
			return Handled;
		}
		
		Component::eEventResult		Edit::OnMouseDown(float x, float y, TExtEventResult&ext)
		{
			if (cellLayout.client.contains(x,y))
			{
				//ShowMessage(String(x)+", "+String(y)+" is in "+cellLayout.client.toString());
				float rx = x - cellLayout.client.left();
				size_t end = vmin(viewEnd-1+viewRightMost,text.length());
				index_t index=viewBegin;
				for (; index < end; index++)
				{
					float width = _GetCharWidth(text[index]);
					if (rx < width/2)
						break;
					rx -= width;
				}
				cursor = index;
				if (!input.pressed[Key::Shift])
					selectionStart = cursor;
				ext.customCursor = Mouse::CursorType::EditText;
				
				cursorOffset = cellLayout.client.left()+_GetTextWidth(text.root()+viewBegin,cursor-viewBegin);			
				return RequestingRepaint;
			}
			return Handled;
		}
		
		Component::eEventResult	Edit::OnMouseDrag(float x, float y)
		{
			float rx = x - cellLayout.client.left();
			goLeft = rx < 0;
			size_t end = vmin(viewEnd-1+viewRightMost,text.length());
			index_t index=viewBegin;
			for (; index < end; index++)
			{
				float width = _GetCharWidth(text[index]);
				if (rx < width/2)
					break;
				rx -= width;
			}
			goRight = index == end && rx > 0;
			cursor = index;
			cursorOffset = cellLayout.client.left()+_GetTextWidth(text.root()+viewBegin,cursor-viewBegin);			
			return RequestingRepaint;
		}
		
		Component::eEventResult	Edit::OnMouseUp(float x, float y)
		{
			goLeft = false;
			goRight = false;
		
			return Handled;
		}

		
		Component::eEventResult	Edit::OnKeyDown(Key::Name key)
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
							selectionStart = cursor;
						_UpdateView();
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
							selectionStart = cursor;
						_UpdateView();
						return RequestingRepaint;
					}
				break;
				case Key::Home:
					if (cursor)
					{
						cursor = 0;
						if (!input.pressed[Key::Shift])
							selectionStart = cursor;
						_UpdateView();
						return RequestingRepaint;
					}
				break;
				case Key::End:
					if (cursor<text.length() && !readOnly)
					{
						cursor = text.length();
						if (!input.pressed[Key::Shift])
							selectionStart = cursor;
						_UpdateView();
						return RequestingRepaint;
					}
				break;
				case Key::BackSpace:
					if ((cursor || selectionStart)  && !readOnly)
					{
						if (selectionStart == cursor)
						{
							selectionStart--;
							if (text.erase(--cursor,1))
							{
								_UpdateView();
								onChange();
								return RequestingRepaint;
							}
							
						}
						else
							if (selectionStart > cursor)
							{
								if (text.erase(cursor,selectionStart-cursor))
								{
									selectionStart = cursor;
									_UpdateView();
									onChange();
									return RequestingRepaint;
								}
							}
							else
								if (text.erase(selectionStart,cursor-selectionStart))
								{
									cursor = selectionStart;
									_UpdateView();
									onChange();
									return RequestingRepaint;
								}
					}
				break;
				case Key::Delete:
					if (!readOnly)
					{
						if (selectionStart == cursor)
						{
							if (text.erase(cursor,1))
							{
								_UpdateView();
								onChange();
								return RequestingRepaint;
							}
							
						}
						else
							if (selectionStart > cursor)
							{
								if (text.erase(cursor,selectionStart-cursor))
								{
									selectionStart = cursor;
									_UpdateView();
									onChange();
									return RequestingRepaint;
								}
							}
							else
								if (text.erase(selectionStart,cursor-selectionStart))
								{
									cursor = selectionStart;
									_UpdateView();
									onChange();
									return RequestingRepaint;
								}
					}
				break;
				case Key::C:
					if (input.pressed[Key::Ctrl] && selectionStart != cursor && !maskedView)
					{
						size_t	begin = selectionStart,
								end = cursor;
						if (begin > end)
							swp(begin,end);
						String sub = String(text.root()+begin,end-begin);
						if (!System::copyToClipboard(NULL,sub.c_str()))
							ErrMessage("failed to copy");
					}
				break;
				case Key::X:
					if (input.pressed[Key::Ctrl] && selectionStart != cursor && !maskedView)
					{
						size_t	begin = selectionStart,
								end = cursor;
						if (begin > end)
							swp(begin,end);
						String sub = String(text.root()+begin,end-begin);
						if (System::copyToClipboard(NULL,sub.c_str()) && !readOnly)
						{
							text.erase(begin,end-begin);
							if (cursor > begin)
								cursor = begin;
							selectionStart = cursor;
							_UpdateView();
							onChange();
							return RequestingRepaint;
						}
					}
				break;
				case Key::V:
					if (input.pressed[Key::Ctrl] && !readOnly)
					{
						char buffer[0x1000];
						if (System::getFromClipboardIfText(NULL,buffer,sizeof(buffer)))
						{
							if (cursor!=selectionStart)
							{
								if (cursor > selectionStart)
								{
									text.erase(selectionStart,cursor-selectionStart);
									cursor = selectionStart;
								}
								else
									text.erase(cursor,selectionStart-cursor);
							}
							size_t len = strlen(buffer);
							text.insert(cursor,buffer,len);
							cursor+=len;
							selectionStart = cursor;
							_UpdateView();
							onChange();
							return RequestingRepaint;
						}
					}
				break;
				case Key::Enter:
				case Key::Return:
					onEnter();
				return Handled;
				default:
					{
						Key::Name k = key;
						if ((k < Key::A || k > Key::Z) && (k < Key::N0 || k > Key::N9) && k != Key::SZ && k != Key::AE && k != Key::UE && k != Key::OE && k != Key::Space && k != Key::Period && k != Key::Comma && k != Key::Minus && k != Key::Mesh)
							return Unsupported;	//really have no use for these. Cascade away.
						//else goto Handled...
					}
			}
			return Handled;
		}
		
		Component::eEventResult	Edit::OnKeyUp(Key::Name key)
		{
		
			return Unsupported;
		}
		
		void	Edit::_UpdateView()
		{
			if (cursor > text.length())
				cursor = text.length();
			if (viewBegin >= text.length())
				viewBegin = text.length()?text.length()-1:0;
			if (cursor < viewBegin)
				viewBegin = cursor;
			else
				while (_GetTextWidth(text.root()+viewBegin,cursor-viewBegin)>cellLayout.client.width())
					viewBegin++;
			viewEnd = viewBegin+1;
			while (viewEnd < text.length() && _GetTextWidth(text.root()+viewBegin,viewEnd-viewBegin)<cellLayout.client.width())
				viewEnd++;
			viewRightMost = _GetTextWidth(text.root()+viewBegin,viewEnd-viewBegin)<cellLayout.client.width();
			//if (textout.unscaledLength(text.root()+viewBegin,viewEnd-viewBegin)>=cellLayout.client.width())
				//viewEnd--;
			cursorOffset = cellLayout.client.left()+_GetTextWidth(text.root()+viewBegin,cursor-viewBegin);
		}
		
		void	Edit::UpdateLayout(const Rect<float>&parent_region)
		{
			Component::UpdateLayout(parent_region);
			_UpdateView();
		}
		
		
		
		Component::eEventResult	Edit::OnChar(char c)
		{
			if (readOnly || (acceptChar && !acceptChar(c)))
				return Unsupported;
			if (cursor!=selectionStart)
			{
				if (cursor > selectionStart)
				{
					text.erase(selectionStart,cursor-selectionStart);
					cursor = selectionStart;
				}
				else
					text.erase(cursor,selectionStart-cursor);
			}

			text.insert(cursor++,c);
			selectionStart = cursor;
			_UpdateView();
			onChange();
			return RequestingRepaint;
		}
		
		
		
		
		

		/*virtual override*/ void		Button::OnNormalPaint(NormalRenderer&renderer, bool parentIsEnabled)
		{
			renderer.PushNormalScale();
				if (AppearsPressed())
					renderer.ScaleNormals(-1,-1,1);
			
				Component::OnNormalPaint(renderer,parentIsEnabled);

			renderer.PopNormalScale();
		}
		
		/*virtual override*/ void		Button::OnColorPaint(ColorRenderer&renderer, bool parentIsEnabled)
		{
			bool pressed = AppearsPressed();
			renderer.PushColor();
				if (pressed && enabled && parentIsEnabled)
					renderer.ModulateColor(0.5f);
			Component::OnColorPaint(renderer,parentIsEnabled);
			renderer.PopColor();

			if (caption.isNotEmpty())
			{
				const Rect<float>&rect=cellLayout.client;
				renderer.SetTextPosition(rect.x.center()-ColorRenderer::textout.unscaledLength(caption)*0.5+pressed,rect.y.center()-ColorRenderer::textout.getFont().getHeight()/2+font_offset);
				renderer.PushColor();
				renderer.ModulateColor(1.0-0.2*(pressed||!enabled));
				renderer.WriteText(caption);
				renderer.PopColor();
			}		
		}
		
		/*virtual override*/ Component::eEventResult	Button::OnMouseDrag(float x, float y)
		{
			bool new_pressed = down && currentRegion.contains(x,y);
			bool changed = pressed != new_pressed;
			pressed = new_pressed;
			return changed?RequestingRepaint:Handled;
		}
		
		Component::eEventResult	Button::OnMouseDown(float x, float y, TExtEventResult&ext)
		{
			bool changed = !down;
			down = true;
			pressed = true;
			return changed?RequestingRepaint:Handled;
		}
		
		Component::eEventResult	Button::OnMouseUp(float x, float y)
		{
			if (pressed)
				OnExecute();
			bool changed = pressed;
			pressed = false;
			down = false;
			return changed?RequestingRepaint:Handled;
		}
		
		Component::eEventResult	Button::OnKeyDown(Key::Name key)
		{
			switch (key)
			{
				case Key::Space:
				case Key::Enter:
				case Key::Return:
					onExecute();
				return Handled;
			}
			return Unsupported;
		}
		
		Component::eEventResult	Button::OnKeyUp(Key::Name key)
		{
			return Unsupported;
		}
		
		
		
		float		Button::GetClientMinWidth()	const
		{
			return ColorRenderer::textout.unscaledLength(caption);
		}
		
		float		Button::GetClientMinHeight()	const
		{

			return ColorRenderer::textout.getFont().getHeight();
		}
		
		/*virtual override*/void		CheckBox::OnNormalPaint(NormalRenderer&renderer, bool parentIsEnabled)
		{
			Component::OnNormalPaint(renderer,parentIsEnabled);
			if (style && !style->boxNormal.isEmpty())
			{
				renderer.TextureRect(GetBoxRect(),style->boxNormal);
			}
		}
		
		/*virtual override*/void		CheckBox::OnColorPaint(ColorRenderer&renderer, bool parentIsEnabled)
		{
			renderer.PushColor();
			Component::OnColorPaint(renderer,parentIsEnabled);
			
			const Rect<> rect=GetBoxRect();
			if (style && !style->boxColor.isEmpty())
			{
				renderer.TextureRect(rect,style->boxColor);
				renderer.MarkNewLayer();
			}
			if (checked && style && !style->checkMark.isEmpty())
			{
				renderer.TextureRect(rect,style->checkMark);
				renderer.MarkNewLayer();
			}
			
			if (pressed && style && !style->highlightMark.isEmpty())
			{
				renderer.ModulateColor(1,0.6,0);
				renderer.TextureRect(rect,style->highlightMark);
				renderer.PeekColor();
				renderer.MarkNewLayer();
			}
		
			if (caption.isNotEmpty())
			{
				const float h= ColorRenderer::textout.getFont().getHeight();
				const float size = GetBoxSize();
				renderer.SetTextPosition(rect.left()+size+h*0.2f,rect.y.center()-h/2.f+font_offset);//textout.getFont().getHeight()*0.5);
				renderer.ModulateColor(1.0-0.2*(!enabled));
				renderer.WriteText(caption);
			}
			renderer.PopColor();
		}


		


		Component::eEventResult	CheckBox::OnMouseDrag(float x, float y)
		{
			bool new_pressed = down && currentRegion.contains(x,y);
			bool changed = new_pressed != pressed;
			pressed = new_pressed;
			return changed?RequestingRepaint:Handled;
		}
		
		Component::eEventResult	CheckBox::OnMouseDown(float x, float y, TExtEventResult&ext)
		{
			bool changed = !down;
			down = true;
			pressed = true;
			return changed?RequestingRepaint:Handled;
		}
		
		Component::eEventResult	CheckBox::OnMouseUp(float x, float y)
		{
			if (pressed)
			{
				checked = !checked;
				onChange();
			}
			bool changed = pressed;
			pressed = false;
			down = false;
			return changed?RequestingRepaint:Handled;
		}
		
		
		float		CheckBox::GetMinWidth(bool includeOffsets)	const
		{
			float rs;
			{
				rs = ColorRenderer::textout.unscaledLength(caption)+0.2*ColorRenderer::textout.getFont().getHeight()+GetBoxSize();
				if (anchored.right && includeOffsets)
					rs -= offset.right;
				if (anchored.left && includeOffsets)
					rs += offset.left;
			}
			if (layout)
			{
				rs += layout->clientEdge.left+layout->clientEdge.right;
				if (rs < layout->minWidth)
					rs = layout->minWidth;
			}
			return rs;
		}
		
		float		CheckBox::GetMinHeight(bool includeOffsets)	const
		{
			float rs;
			{
				rs = ColorRenderer::textout.getFont().getHeight();
				if (anchored.top && includeOffsets)
					rs -= offset.top;
				if (anchored.bottom && includeOffsets)
					rs += offset.bottom;
			}
			if (layout)
			{
				rs += layout->clientEdge.bottom+layout->clientEdge.top;
				if (rs < layout->minHeight)
					rs = layout->minHeight;
			}
			return rs;
		}
		
		
		
		
		
		void			Label::UpdateLayout(const Rect<float>&parent_space)
		{
			Component::UpdateLayout(parent_space);
			if (wrapText)
			{
				float w = cellLayout.client.width();
				if (w != lastWidth || textChanged)
				{
					textChanged = false;
					lastWidth = w;
					wrapf(caption,w,_CharLen,lines);
					//for (index_t i = 0; i < lines.count(); i++)
					//	cout << "'"<<lines[i]<<"'"<<endl;
					height = GetMinHeight(false);
				}
			}
					
			
		}
		
		
		
		void			Label::_Setup()
		{
			lastWidth = -1;
			caption = "Label";
			wrapText = false;
			textChanged = true;
			fillBackground = false;
			Vec::set(backgroundColor,1);
			Vec::set(textColor,1);
			height = GetMinHeight(false);
			width = GetMinWidth(false);
		}
		
		float			Label::GetClientMinWidth()	const
		{
			if (!wrapText)
				return ColorRenderer::textout.unscaledLength(caption);
			return 30;
		}
		
		float			Label::GetClientMinHeight()	const
		{
			if (!wrapText)
				return ColorRenderer::textout.getFont().getHeight();
			return ColorRenderer::textout.getFont().getHeight()*lines.count();
		
		}
		
		Label*			Label::SetText(const String&text)
		{
			caption = text;
			textChanged = true;
			if (!wrapText)
				width = GetMinWidth(false);
			SignalLayoutChange();
			return this;
		}
		Label*			Label::SetColor(const TVec4<>&color)
		{
			textColor = color;
			SignalVisualChange();
			return this;
		}

		float		Label::_CharLen(char c)
		{
			return ColorRenderer::textout.unscaledLength(&c,1);
		}
		
		void			Label::OnColorPaint(ColorRenderer&renderer, bool parentIsEnabled)
		{
			renderer.PushColor();
			
			Component::OnColorPaint(renderer, parentIsEnabled);

			if (!fillBackground && caption.isEmpty())
			{
				renderer.PopColor();
				return;
			}
			
			if (fillBackground)
			{
				renderer.ModulateColor(backgroundColor);
				renderer.FillRect(cellLayout.client);
				renderer.PeekColor();
				renderer.MarkNewLayer();
			}
			if (caption.isNotEmpty())
			{
				renderer.Clip(cellLayout.client);
				renderer.ModulateColor(textColor);
				if (!wrapText)
				{
					float	bottom = cellLayout.client.y.center()-ColorRenderer::textout.getFont().getHeight()/2+font_offset;
					renderer.SetTextPosition(cellLayout.client.left(),bottom);
					renderer.WriteText(caption);
				}
				else
				{
					for (index_t i = 0; i < lines.count(); i++)
					{
						renderer.SetTextPosition(cellLayout.client.left(),cellLayout.client.top()-(ColorRenderer::textout.getFont().getHeight()*(i+1)));
						renderer.WriteText(lines[i]);
					}
				}
				renderer.Unclip();
			}
			renderer.PopColor();
		}
		
		void	ComboBox::_Setup()
		{
			//MenuEntry::GetMenu();	//don't create menu (called from constructor)
			selectedObject.reset();
			selectedEntry = 0;
			openDown = true;
			layout = globalLayout.Refer();
			width = GetMinWidth(false);
			height = GetMinHeight(false);
			//ShowMessage(width);
		}
		
		void	ComboBox::UpdateLayout(const Rect<float>&parent_space)
		{
			if (!selectedObject && GetMenu()->CountChildren()>2)
			{
				selectedEntry = 0;
				selectedObject = ((MenuEntry*)(GetMenu()->GetChild(selectedEntry).get()))->object;
				SetText(selectedObject->toString());
			}
			height = Label::GetMinHeight(false);
			MenuEntry::UpdateLayout(parent_space);
		}
		
		
		void	MenuEntry::CorrectMenuWindowSize()	const
		{
			bool changed = false;
			float mw = menuWindow->GetMinWidth();
			if (menuWindow->fsize.x != mw)
			{
				menuWindow->fsize.x = mw;
				menuWindow->size.width = (size_t)round(menuWindow->fsize.x);
				changed = true;
			}
			Menu*	menu = (Menu*)menuWindow->rootComponent.get();
			float mh = menu->GetIdealHeight();
			if (menuWindow->layout)
				mh += menuWindow->layout->clientEdge.top+menuWindow->layout->clientEdge.bottom;
			float h = vmin(150,mh);
			if (menuWindow->fsize.y != h)
			{
				menuWindow->fsize.y = h;
				menuWindow->size.height = (size_t)round(menuWindow->fsize.y);
				changed = true;
			}
					
			if (menuWindow->layoutChanged || changed)
			{
				menuWindow->UpdateLayout();
			}
		}

		
		float	ComboBox::GetMinWidth(bool includeOffsets)	const
		{
			PWindow menuWindow = GetMenuWindow();
			if (menuWindow)
			{
				if (menuWindow->layoutChanged)
					menuWindow->UpdateLayout();
				CorrectMenuWindowSize();
				return vmax(MenuEntry::GetMinWidth(includeOffsets),menuWindow->GetMinWidth());
			}
			return MenuEntry::GetMinWidth(includeOffsets);
		}

		
		void	ComboBox::OnMenuClose(const shared_ptr<MenuEntry>&child)
		{
			selectedEntry = GetMenu()->GetIndexOfChild(child);
			if (selectedEntry)
			{
				selectedEntry--;
				selectedObject = child->object;
			}
			else
			{
				selectedObject.reset();
			}
			if (selectedObject)
				SetText(selectedObject->toString());
			else
				SetText("");
			SignalLayoutChange();
			onChange();
		}
		
		void	ComboBox::Select(index_t index)
		{
			selectedEntry = index;
			if (selectedEntry >= GetMenu()->CountChildren())
				selectedObject.reset();
			else
				selectedObject = ((MenuEntry*)GetMenu()->GetChild(selectedEntry).get())->object;
			if (selectedObject)
				SetText(selectedObject->toString());
			else
				SetText("");
			SignalLayoutChange();
		}

		
		void	MenuEntry::UpdateLayout(const Rect<float>&parent_space)
		{
			Label::UpdateLayout(parent_space);
			
		}
		
		Component::eEventResult			MenuEntry::OnMouseDown(float x, float y, TExtEventResult&ext)
		{
			onExecute();
			
			if (menuWindow)
			{
				shared_ptr<Operator> op = RequireOperator();
				float delta = timer.toSecondsf(timing.now64-menuWindow->hidden);
				if (delta >= 0 && delta<0.1f)
				{
					op->HideMenus();
					//Window::hideMenus();
				}
				else
				{
					ASSERT_NOT_NULL__(menuWindow->rootComponent);
					
					Rect<float>	absolute = cellLayout.border;
					shared_ptr<Window> parent = GetWindow();
					ASSERT__(parent);
					
					#ifdef DEEP_GUI
						absolute.translate(parent->current_center.x-parent->fsize.x/2,parent->current_center.y-parent->fsize.y/2);
					#else
						absolute.translate(parent->x-parent->fsize.x/2,parent->y-parent->fsize.y/2);
					#endif
						

					CorrectMenuWindowSize();
					
					#ifdef DEEP_GUI
						if (openDown)
						{
							menuWindow->current_center.x = absolute.left()+menuWindow->fsize.x/2;
							menuWindow->current_center.y = absolute.bottom()-menuWindow->fsize.y/2;
						}
						else
						{
							menuWindow->current_center.x = absolute.x.max+menuWindow->fsize.x/2;
							menuWindow->current_center.y = absolute.top()-menuWindow->fsize.y/2;
						}
						menuWindow->current_center.x -= (menuWindow->cellLayout.border.left());
						menuWindow->current_center.y += (menuWindow->fsize.y-menuWindow->cellLayout.border.top());
						
						menuWindow->current_center.shellRadius = 1;
						
						menuWindow->origin = menuWindow->destination = menuWindow->current_center;
					#else
						if (openDown)
						{
							menuWindow->x = absolute.left()+menuWindow->fsize.x/2;
							menuWindow->y = absolute.bottom()-menuWindow->fsize.y/2;
						}
						else
						{
							menuWindow->x = absolute.x.max+menuWindow->fsize.x/2;
							menuWindow->y = absolute.top()-menuWindow->fsize.y/2;
						}
						menuWindow->x -= (menuWindow->cellLayout.border.left());
						menuWindow->y += (menuWindow->fsize.y-menuWindow->cellLayout.border.top());
					#endif
						
					RequireOperator()->ShowMenu(menuWindow);
				}
			}
			else
			{
				RequireOperator()->HideMenus();
				OnMenuClose(static_pointer_cast<MenuEntry,Component>(shared_from_this()));
			}
			
			return Handled;
		}
		
		
		Component::eEventResult			MenuEntry::OnMouseExit()
		{
			bool changed = Label::fillBackground;
			Label::fillBackground = false;
			return changed?RequestingRepaint:Handled;
		}
		

		Component::eEventResult			MenuEntry::OnFocusGained()
		{
			Label::fillBackground = true;
			return RequestingRepaint;
		}
		
		Component::eEventResult			MenuEntry::OnFocusLost()
		{
			Label::fillBackground = false;
			return RequestingRepaint;
		}
		
		Component::eEventResult			MenuEntry::OnKeyDown(Key::Name key)
		{
			return parent.expired()?Unsupported:parent.lock()->OnKeyDown(key);
		}
		
		
		const shared_ptr<Window>&	MenuEntry::RetrieveMenuWindow()
		{
			if (!menuWindow)
			{
				menuWindow.reset(new Window(false, &Window::menuStyle));
				shared_ptr<Menu> menu = shared_ptr<Menu>(new Menu());
				menuWindow->SetComponent(menu);
				menu->parent = static_pointer_cast<MenuEntry,Component>(shared_from_this());
				//menu->level = level+1;

			}
			return menuWindow;
		}
		
		shared_ptr<Menu>		MenuEntry::GetMenu()
		{
			return static_pointer_cast<Menu, Component>(RetrieveMenuWindow()->rootComponent);
		}

		shared_ptr<const Menu>	MenuEntry::GetMenu()	const
		{
			return menuWindow?static_pointer_cast<const Menu, const Component>(menuWindow->rootComponent):shared_ptr<const Menu>();
		}
		
		void		MenuEntry::DiscardMenu()
		{
			if (menuWindow)
			{
				shared_ptr<Operator> op = GetOperator();
				if (op)	//might, in fact, be null
					op->HideMenus();
				menuWindow.reset();
			}
		}
		
		void		MenuEntry::_Setup()
		{
			parent.reset();
			menuWindow.reset();
			object.reset();
			openDown = false;
			Label::wrapText = false;
			//level = 0;
			Vec::def(Label::backgroundColor,0.5,0.5,1);
			height = GetMinHeight(false);
			width = GetMinWidth(false);
		}
		
		
		void	MenuEntry::OnColorPaint(ColorRenderer&renderer, bool parentIsEnabled)
		{
			Label::fillBackground = IsFocused();
			if (object && object.get() != this)
				Label::SetText(object->toString());
			Label::OnColorPaint(renderer,parentIsEnabled);
		}
		
		void	MenuEntry::OnMenuClose(const shared_ptr<MenuEntry>&child)
		{
			if (!parent.expired())
			{
				shared_ptr<Menu>	p = parent.lock();
				if (!p->parent.expired())
					p->parent.lock()->OnMenuClose(static_pointer_cast<MenuEntry, Component>(shared_from_this()));
			}
		}
		
		MenuEntry::~MenuEntry()
		{
			//DiscardMenu();	//only hides menu in the operator, but i suspect even if the case happens where a menu is still visible when the root is erased, the operator should now be able to handle that
		}
		
		
		float		Menu::GetIdealHeight()	const
		{
			float h = 0;
			for (index_t i = 0; i < children.count(); i++)
				h += children[i]->height;
			if (layout)
				h += layout->clientEdge.top+layout->clientEdge.bottom;
			return h;
		}
		
		shared_ptr<MenuEntry>		Menu::Add(const String&caption)
		{
			shared_ptr<MenuEntry> entry = shared_ptr<MenuEntry>(new MenuEntry());
			entry->SetText(caption);
			entry->object = entry;
			entry->parent = static_pointer_cast<Menu,Component>(shared_from_this());
			ASSERT__(Add(entry));
			return entry;
		}
		
		bool		Menu::Add(const shared_ptr<Component>&component)
		{
			if (!component->typeName.beginsWith("Label/MenuEntry") || !ScrollBox::Add(component))
				return false;
			static_pointer_cast<MenuEntry,Component>(component)->parent = static_pointer_cast<Menu,Component>(shared_from_this());
			//((MenuEntry*)component)->level = level;
			component->SetWindow(windowLink);		
			_ArrangeItems();
			SignalLayoutChange();
			return true;
		}
		
		bool		Menu::Erase(const shared_ptr<Component>&component)
		{
			if (ScrollBox::Erase(component))
			{
				_ArrangeItems();
				return true;
			}
			return false;
		}
		
		bool		Menu::Erase(index_t index)
		{
			if (ScrollBox::Erase(index))
			{
				_ArrangeItems();
				return true;
			}
			return false;
		}
		
		bool		Menu::MoveChildUp(const shared_ptr<Component>&component)
		{
			if (ScrollBox::MoveChildUp(component))
			{
				_ArrangeItems();
				return true;
			}
			return false;
		}

		bool		Menu::MoveChildUp(index_t index)
		{
			if (ScrollBox::MoveChildUp(index))
			{
				_ArrangeItems();
				return true;
			}
			return false;
		}

		bool		Menu::MoveChildDown(const shared_ptr<Component>&component)
		{
			if (ScrollBox::MoveChildDown(component))
			{
				_ArrangeItems();
				return true;
			}
			return false;
		}
		
		bool		Menu::MoveChildDown(index_t index)
		{
			if (ScrollBox::MoveChildDown(index))
			{
				_ArrangeItems();
				return true;
			}
			return false;
		}
		
		bool		Menu::MoveChildToTop(const shared_ptr<Component>&component)
		{
			if (ScrollBox::MoveChildToTop(component))
			{
				_ArrangeItems();
				return true;
			}
			return false;
		}

		bool		Menu::MoveChildToTop(index_t index)
		{
			if (ScrollBox::MoveChildToTop(index))
			{
				_ArrangeItems();
				return true;
			}
			return false;
		}

		bool		Menu::MoveChildToBottom(const shared_ptr<Component>&component)
		{
			if (ScrollBox::MoveChildToBottom(component))
			{
				_ArrangeItems();
				return true;
			}
			return false;
		}

		bool		Menu::MoveChildToBottom(index_t index)
		{
			if (ScrollBox::MoveChildToBottom(index))
			{
				_ArrangeItems();
				return true;
			}
			return false;
		}
		
		void		Menu::_ArrangeItems()
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

		void	Menu::_Setup()
		{
			ScrollBox::horizontalBar->SetVisible(false);
			ScrollBox::verticalBar->autoVisibility = true;
			parent.reset();
			//level = 0;
			selectedEntry = 0;
			layout = globalLayout.Refer();
		}
		
		
		Component::eEventResult			Menu::OnKeyDown(Key::Name key)
		{
			switch (key)
			{
				case Key::Up:
					if (selectedEntry)
					{
						selectedEntry--;
						SetFocused(children[selectedEntry]);
						return RequestingRepaint;
					}
					return Handled;
				case Key::Down:
					if (selectedEntry+1<children.count())
					{
						selectedEntry++;
						SetFocused(children[selectedEntry]);
						return RequestingRepaint;
					}
				
			}
			return Unsupported;
		}
		
		
		Component::eEventResult		MenuEntry::OnMouseHover(float x, float y, TExtEventResult&)
		{
			bool changed = !Label::fillBackground;
			Label::fillBackground = true;
			if (changed)
			{
				PMenu menu = parent.lock();
				if (menu)
					menu->_SelectMenuEntry(shared_from_this());
				return RequestingRepaint;
			}
			return Handled;
		}

		void			Menu::_SelectMenuEntry(const PComponent&c)
		{
			if (selectedComponent == c)
				return;
			index_t index = Panel::children.indexOf(c);
			if (index != InvalidIndex)
			{
				selectedEntry = index;
				selectedComponent = c;
				SetFocused(children[selectedEntry]);
				SignalVisualChange();
			}
		}


		static void	LoadLayout(XML::Node*xtheme, FileSystem::Folder&folder, const String&path, Layout&layout, float outer_scale)
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
					layout.LoadFromFile(file.getLocation(),scale*outer_scale);
					return;
				}
			if (node->query("copy",string))
			{
				if (string == "button")
					layout.override = &Button::globalLayout;
				elif (string == "panel")
					layout.override = &Panel::globalLayout;
				elif (string == "scrollbox")
					layout.override = &ScrollBox::globalLayout;
				elif (string == "window/common")
					layout.override = &Window::commonStyle;
				elif (string == "window/menu")
					layout.override = &Window::menuStyle;
				elif (string == "window/hint")
					layout.override = &Window::hintStyle;
				else
					throw Program::UnsupportedRequest("Unknown resource layout '"+string+"'");
			}
		}
		
		static void LoadLayout(XML::Node*xtheme, FileSystem::Folder&folder, const String&path, ScrollBarLayout&layout, float outer_scale)
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
					layout.LoadFromFile(file.getLocation(),scale*outer_scale);
		}
		
		static void LoadLayout(XML::Node*xtheme, FileSystem::Folder&folder, const String&path, SliderLayout&layout, float outer_scale)
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
					layout.LoadFromFile(file.getLocation(),scale*outer_scale);
		}
		
		static void LoadLayout(XML::Node*xtheme, FileSystem::Folder&folder, const String&path, CheckBox::TStyle&layout, float outer_scale)
		{
			XML::Node*node = xtheme->find(path);
			if (!node)
				return;	//graceful ignore
			String string;

			FileSystem::File	file;
			
			if (node->query("color",string) && folder.findFile(string,file))
				loadColor(file.getLocation(),layout.boxColor);
			if (node->query("bump",string) && folder.findFile(string,file))
				loadBump(file.getLocation(),layout.boxNormal);
			if (node->query("check",string) && folder.findFile(string,file))
				loadColor(file.getLocation(),layout.checkMark);
			if (node->query("highlight",string) && folder.findFile(string,file))
				loadColor(file.getLocation(),layout.highlightMark);
		}
		
		
		void		LoadTheme(const String&filename,float outer_scale)
		{
			XML::Container	xml;
			xml.LoadFromFile(filename);
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
					ColorRenderer::textout.getFont().loadFromFile(file.getLocation(),scale*outer_scale);
						//FAIL("Failed to load font from font file '"+file.getLocation()+"'");
			}
			else
				throw IO::DriveAccess::FileFormatFault(globalString("XML theme file lacks theme/font node"));
			
			LoadLayout(xtheme,folder,"button",Button::globalLayout,outer_scale);
			LoadLayout(xtheme,folder,"panel",Panel::globalLayout,outer_scale);
			LoadLayout(xtheme,folder,"combobox",ComboBox::globalLayout,outer_scale);
			LoadLayout(xtheme,folder,"edit",Edit::globalLayout,outer_scale);
			LoadLayout(xtheme,folder,"scrollbox",ScrollBox::globalLayout,outer_scale);
			LoadLayout(xtheme,folder,"scrollbar/style",ScrollBarLayout::global,outer_scale);
			LoadLayout(xtheme,folder,"window/common",Window::commonStyle,outer_scale);
			LoadLayout(xtheme,folder,"window/menu",Window::menuStyle,outer_scale);
			LoadLayout(xtheme,folder,"window/hint",Window::hintStyle,outer_scale);
			LoadLayout(xtheme,folder,"checkbox/style",CheckBox::globalStyle,outer_scale);
			LoadLayout(xtheme,folder,"slider/style",SliderLayout::global,outer_scale);
			
			/*
			if (Button::globalLayout.copy_from)
				Button::globalLayout = *Button::globalLayout.copy_from;
			if (Panel::globalLayout.copy_from)
				Panel::globalLayout = *Panel::globalLayout.copy_from;
			if (ComboBox::globalLayout.copy_from)
				ComboBox::globalLayout = *ComboBox::globalLayout.copy_from;
			if (Edit::globalLayout.copy_from)
				Edit::globalLayout = *Edit::globalLayout.copy_from;
			if (ScrollBox::globalLayout.copy_from)
				ScrollBox::globalLayout = *ScrollBox::globalLayout.copy_from;
			if (Window::commonStyle.copy_from)
				Window::commonStyle = *Window::commonStyle.copy_from;
			if (Window::menuStyle.copy_from)
				Window::menuStyle = *Window::menuStyle.copy_from;
			if (Window::hintStyle.copy_from)
				Window::hintStyle = *Window::hintStyle.copy_from;
			*/
			

		}
		
		
		void			ShowChoice(Operator&op, const String&title, const String&query, const Array<String>&choices, const function<void(index_t)>&onSelect)
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
				buttons[i]->SetCaption(choices[i]);
				buttons[i]->width = std::max(buttons[i]->width,100.f);
			}
				//message_button->on_execute += HideMessage;
			
			message_label->wrapText = true;
			message_label->SetText(query);
			message_label->anchored.set(true,true,true,true);
			panel->Add(message_label);
			panel->Add(buttons.first());
			buttons.first()->anchored.set(true,true,false,false);

			for (index_t i = 1; i < buttons.count(); i++)
				panel->AppendRight(buttons[i]);

			message_label->offset.bottom = buttons.first()->height;

			shared_ptr<GUI::Window>	window = Window::CreateNew(NewWindowConfig(title,WindowPosition(0,0,400,200,SizeChange::Fixed),true),panel);
			window->size.height = (size_t)(window->fsize.y = window->GetMinHeight());
			window->layoutChanged = true;
			window->visualChanged = true;
			window->UpdateLayout();
			window->size.height = (size_t)(window->fsize.y = window->GetMinHeight());
			message_label->SetText(query);
			window->UpdateLayout();
			window->size.height = (size_t)(window->fsize.y = window->GetMinHeight());

			op.ShowWindow(window);
			Component::SetFocused(buttons[0]);
			
			weak_ptr<GUI::Window>	weak_window = window;
			for (index_t i = 0; i < buttons.count(); i++)
				buttons[i]->onExecute += [weak_window,i,onSelect]()
				{
					shared_ptr<GUI::Window>	window = weak_window.lock();
					if (window)
						window->Hide();
					onSelect(i);
				};
		}


		static shared_ptr<Window>	messageWindow;
		static shared_ptr<Label>	messageLabel;
		static shared_ptr<Button>	messageButton;
		static void createMessageWindow(Operator&op)
		{
			if (messageWindow)
				return;
			
			messageLabel = shared_ptr<Label>(new Label());
			shared_ptr<Panel>	panel = shared_ptr<Panel>(new Panel());
			messageButton = shared_ptr<Button>(new Button());
			
			messageButton->anchored.set(false,true,false,false);
			messageButton->SetCaption("OK");
			messageButton->width = 100;
			messageButton->onExecute += HideMessage;
			
			messageLabel->SetText("message");
			messageLabel->wrapText = true;
			messageLabel->anchored.set(true,true,true,true);
			panel->Add(messageLabel);
			panel->Add(messageButton);
			messageLabel->offset.bottom = messageButton->height;

			//float	mx = op.getDisplay().clientWidth()/2,
			//		my = op.getDisplay().clientHeight()/2;
			messageWindow = Window::CreateNew(NewWindowConfig("Message",WindowPosition(0,0,400,200,SizeChange::Fixed),true),panel);
		}
		
		
		void	ShowMessage(Operator&op, const String&title, const String&message)
		{
			createMessageWindow(op);
			messageWindow->title = title;
			
			//message_window->UpdateLayout();
			messageLabel->SetText(message);
			
			messageWindow->UpdateLayout();
			messageWindow->size.height = (size_t)(messageWindow->fsize.y = messageWindow->GetMinHeight());
			messageWindow->layoutChanged = true;
			messageWindow->visualChanged = true;
			
			#ifdef DEEP_GUI
				message_window->current_center.x = message_window->origin.x = message_window->destination.x = 0;
				message_window->current_center.y = message_window->origin.y = message_window->destination.y = 0;
				message_window->current_center.shellRadius = 1;
				message_window->origin.shellRadius = 1;
				message_window->destination.shellRadius = 1;
			#else
				messageWindow->x = 0;
				messageWindow->y = 0;
			#endif
			//ShowMessage("inserting window");
			op.ShowWindow(messageWindow);
			Component::SetFocused(messageButton);
		}
		
		void	ShowMessage(Operator&op, const String&message)
		{
			ShowMessage(op,"Message",message);
		}
		
		bool	ShowingMessage()
		{
			if (!messageWindow)
				return false;
			shared_ptr<Operator> op = messageWindow->operatorLink.lock();
			return op && op->WindowIsVisible(messageWindow);
		}
		
		void	HideMessage()
		{
			if (messageWindow)
				messageWindow->Hide();
		}
		
	}
}

