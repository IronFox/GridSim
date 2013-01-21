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

	template <typename GL>
		void	createDisplay(Display<GL>&display, const String&name, Resolution resolution, DisplayConfig::f_on_resize onResize)
		{
			String file_name = name;
			file_name.eraseCharacters(validFileNameChar,false);
			XML::Container xconfig;
			try
			{
				xconfig.loadFromFile("./config/"+file_name+".xml");
				XML::Node*xdisplay = xconfig.find("config/display");
				if (xdisplay)
				{
					String value;
					if (xdisplay->query("width",value))
						convert(value.c_str(),resolution.width);
					if (xdisplay->query("height",value))
						convert(value.c_str(),resolution.height);
				}
			}
			catch (const IO::DriveAccess::FileOpenFault&)
			{}
			
			DisplayConfig config(name,[file_name,onResize,&display](UINT width, UINT height, bool is_final){
				if (is_final)
				{
					XML::Container xconfig;
					XML::Node&xdisplay = xconfig.create("config/display");
					xdisplay.set("width",display.width());
					xdisplay.set("height",display.height());
					FileSystem::createFolder("config");
					xconfig.saveToFile("./config/"+file_name+".xml");
				}
				onResize(width,height,is_final);
			});
			display.setSize(resolution.width, resolution.height, DisplayConfig::ResizableBorder);
			
			if (!display.create(config))
				FATAL__("Unable to create window (" + display.errorStr() + ")");
		}
		
		

};


#endif
