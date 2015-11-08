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
			void	UpdateXML(Display<GL>&display, UINT32 flags, const Resolution&restoredResolution, const PathString&xmlfilename)
			{
				XML::Container xconfig;
				XML::Node&xdisplay = xconfig.Create("config/display");
				xdisplay.Set("width",restoredResolution.width);
				xdisplay.Set("height",restoredResolution.height);
				if (flags & DisplayConfig::IsMaximized)
					xdisplay.Set("state","maximized");
				FileSystem::CreateFolder("config");
				xconfig.SaveToFile(xmlfilename);
			}
	}



	template <typename GL>
		void	CreateDisplay(Display<GL>&display, const PathString&configFolderPath, const String&displayName, Resolution resolution, DisplayConfig::f_on_resize onResize, const DisplayConfig::Icon&icon = DisplayConfig::Icon())
		{
			Resolution screenRes = display.getScreenSize();
			resolution.width = std::min(resolution.width,screenRes.width);
			resolution.height = std::min(resolution.height,screenRes.height);
			String xmlBaseName = displayName;
			xmlBaseName.eraseCharacters(validFileNameChar,false);
			xmlBaseName.convertToLowerCase();
			PathString xmlFileName = configFolderPath + FOLDER_SLASH + xmlBaseName + ".xml";
			XML::Container xconfig;
			bool updateFile = false;
			UINT32 flags = DisplayConfig::ResizeDragHasEnded;
			try
			{
				xconfig.LoadFromFile(xmlFileName);
				XML::Node*xdisplay = xconfig.Find("config/display");
				if (xdisplay)
				{
					String value;
					if (!xdisplay->Query("width",value)|| !convert(value.c_str(),resolution.width))
						updateFile = true;
					
					if (!xdisplay->Query("height",value)|| !convert(value.c_str(),resolution.height))
						updateFile = true;

					if (xdisplay->Query("state",value))
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
			
			DisplayConfig config(displayName,[xmlFileName,onResize,&display, resolution](const Resolution&newRes, UINT32 flags){
				static Resolution restoredResolution = resolution;
				display.SignalWindowResize(flags);
				if (flags & Engine::DisplayConfig::ResizeDragHasEnded)
				{
					if (!(flags & (Engine::DisplayConfig::IsMaximized | Engine::DisplayConfig::IsMinimzed | Engine::DisplayConfig::IsFullscreen)))
					{
						restoredResolution = newRes;
					}
					Detail::UpdateXML(display,flags, restoredResolution, xmlFileName);
				}
				onResize(newRes,flags);
			});
			config.icon = icon;
			display.setSize(resolution.width, resolution.height, DisplayConfig::ResizableBorder);
			if (!display.create(config))
				FATAL__("Unable to create window (" + display.errorStr() + ")");
			if (updateFile)
				Detail::UpdateXML(display,flags,resolution, xmlFileName);
			if (flags & DisplayConfig::IsMaximized)
				context.MaximizeWindow();
			//elif (flags & DisplayConfig::IsMinimzed)
			//	context.MinimizeWindow();
			onResize(resolution,context.GetDisplayConfigFlags());
		}
		
		

};


#endif
