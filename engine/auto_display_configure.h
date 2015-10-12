#ifndef auto_display_configureH
#define auto_display_configureH

#include "../io/xml.h"
#include "../io/file_system.h"
#include "display.h"


namespace Engine
{
	inline bool validFileNameChar(char c)
	{
		return Template::isalnum(c) || c == ' ' || c == '_';
	}

	namespace Detail
	{
		template <typename GL>
			void	UpdateXML(Display<GL>&display, UINT32 flags, const Resolution&restoredResolution, const String&filename)
			{
				XML::Container xconfig;
				XML::Node&xdisplay = xconfig.create("config/display");
				xdisplay.set("width",restoredResolution.width);
				xdisplay.set("height",restoredResolution.height);
				if (flags & DisplayConfig::IsMaximized)
					xdisplay.set("state","maximized");
				FileSystem::CreateFolder("config");
				xconfig.saveToFile("./config/"+filename+".xml");
			}
	}



	template <typename GL>
		void	CreateDisplay(Display<GL>&display, const String&name, Resolution resolution, DisplayConfig::f_on_resize onResize)
		{
			Resolution screenRes = display.getScreenSize();
			resolution.width = std::min(resolution.width,screenRes.width);
			resolution.height = std::min(resolution.height,screenRes.height);
			String fileName = name;
			fileName.eraseCharacters(validFileNameChar,false);
			fileName.convertToLowerCase();
			XML::Container xconfig;
			bool updateFile = false;
			UINT32 flags = DisplayConfig::ResizeDragHasEnded;
			try
			{
				xconfig.loadFromFile("./config/"+fileName+".xml");
				XML::Node*xdisplay = xconfig.find("config/display");
				if (xdisplay)
				{
					String value;
					if (!xdisplay->query("width",value)|| !convert(value.c_str(),resolution.width))
						updateFile = true;
					
					if (!xdisplay->query("height",value)|| !convert(value.c_str(),resolution.height))
						updateFile = true;

					if (xdisplay->query("state",value))
					{
						if (value == "maximized")
							flags |= DisplayConfig::IsMaximized;
						//elif (value == "minimized")
						//	flags |= DisplayConfig::IsMinimized;
					}
				}
			}
			catch (const IO::DriveAccess::FileOpenFault&)
			{
				updateFile = true;
			}
			
			DisplayConfig config(name,[fileName,onResize,&display, resolution](const Resolution&newRes, UINT32 flags){
				static Resolution restoredResolution = resolution;
				display.SignalWindowResize(flags);
				if (flags & Engine::DisplayConfig::ResizeDragHasEnded)
				{
					if (!(flags & (Engine::DisplayConfig::IsMaximized | Engine::DisplayConfig::IsMinimzed | Engine::DisplayConfig::IsFullscreen)))
					{
						restoredResolution = newRes;
					}
					Detail::UpdateXML(display,flags, restoredResolution, fileName);
				}
				onResize(newRes,flags);
			});
			display.setSize(resolution.width, resolution.height, DisplayConfig::ResizableBorder);
			if (!display.create(config))
				FATAL__("Unable to create window (" + display.errorStr() + ")");
			if (updateFile)
				Detail::UpdateXML(display,flags,resolution, fileName);
			if (flags & DisplayConfig::IsMaximized)
				context.MaximizeWindow();
			//elif (flags & DisplayConfig::IsMinimzed)
			//	context.MinimizeWindow();
			onResize(resolution,context.GetDisplayConfigFlags());
		}
		
		

};


#endif
