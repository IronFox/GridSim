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
			void	UpdateXML(Display<GL>&display, const String&filename)
			{
				XML::Container xconfig;
				XML::Node&xdisplay = xconfig.create("config/display");
				xdisplay.set("width",display.width());
				xdisplay.set("height",display.height());
				FileSystem::createFolder("config");
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
				}
			}
			catch (const IO::DriveAccess::FileOpenFault&)
			{
				updateFile = true;
			}
			
			DisplayConfig config(name,[fileName,onResize,&display](const Resolution&newRes, bool is_final, bool is_full_screen){
				display.SignalWindowResize(is_final);
				if (is_final && !is_full_screen)
				{
					Detail::UpdateXML(display,fileName);
				}
				onResize(newRes,is_final,is_full_screen);
			});
			display.setSize(resolution.width, resolution.height, DisplayConfig::ResizableBorder);
			if (!display.create(config))
				FATAL__("Unable to create window (" + display.errorStr() + ")");
			if (updateFile)
				Detail::UpdateXML(display,fileName);
			onResize(resolution,true,false);
		}
		
		

};


#endif
