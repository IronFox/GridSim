#ifndef auto_display_configureH
#define auto_display_configureH

#include <io/xml.h>
#include <io/file_system.h>
#include "display.h"


namespace Engine
{
	inline bool validFileNameChar(char c)
	{
		return CharFunctions::isalnum(c) || c == ' ' || c == '_';
	}

	namespace Detail
	{
		template <typename GL>
			void	UpdateXML(Display<GL>&display, UINT32 flags, const RECT&restoredLocation, const PathString&xmlfilename)
			{
				XML::Container xconfig;
				XML::Node&xdisplay = xconfig.Create("config/display");
				xdisplay.Set("width",restoredLocation.right - restoredLocation.left);
				xdisplay.Set("height",restoredLocation.bottom - restoredLocation.top);
				if (flags & DisplayConfig::IsMaximized)
					xdisplay.Set("state","maximized");
				elif (flags & DisplayConfig::IsFullscreen)
					xdisplay.Set("state","fullScreen");
				FileSystem::ForceCreateDirectory(FileSystem::ExtractFileDir(xmlfilename));
				xconfig.SaveToFile(xmlfilename);
			}
	}



	template <typename GL>
		UINT32	CreateDisplay(Display<GL>&display, const PathString&configFolderPath, const String&displayName, Resolution resolution, DisplayConfig::f_on_resize onResize, const DisplayConfig::Icon&icon = DisplayConfig::Icon())
		{
			Resolution screenRes = display.GetScreenSize();
			resolution.width = std::min(resolution.width,screenRes.width);
			resolution.height = std::min(resolution.height,screenRes.height);
			String xmlBaseName = displayName;
			xmlBaseName.EraseCharacters(validFileNameChar,false);
			xmlBaseName.ConvertToLowerCase();
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
						if (value == "fullScreen")
							flags |= DisplayConfig::IsFullscreen;
						//elif (value == "minimized")
						//	flags |= DisplayConfig::IsMinimized;
					}
				}
			}
			catch (const Except::IO::DriveAccess::FileOpenFault&)
			{
				updateFile = true;
			}
			
			DisplayConfig config(displayName,[xmlFileName,onResize,&display, resolution](const Resolution&newRes, UINT32 flags){
				
				Timer::Time tUpdateXML = 0;
				Timer::Time t0 = timer.Now();
				Timer::Time t1 = t0;
				display.SignalWindowResize(flags);
				Timer::Time tResizeDisplay = timer.Now() - t1;
				t1 += tResizeDisplay;
				if (flags & Engine::DisplayConfig::ResizeDragHasEnded)
				{
					Detail::UpdateXML(display,flags, display.GetPreMaxiMinimizeLocation(), xmlFileName);
					tUpdateXML = timer.Now() - t1;
					t1 += tUpdateXML;
				}
				onResize(newRes,flags);
				Timer::Time tOnResize = timer.Now() - t1;
				t1 += tOnResize;
				#ifdef _DEBUG
				if (t1 - t0 > timer.GetTicksPerSecond()/2)
				{
					ShowOnce("Warning: Resize time threshold exceeded: display="+String(timer.ToSeconds(tResizeDisplay))+", XML="+String(timer.ToSeconds(tUpdateXML))+", client="+String(timer.ToSeconds(tOnResize)));
				}
				#endif
			});
			config.icon = icon;
			display.SetBorderStyle(DisplayConfig::ResizableBorder);
			display.SetSize(resolution.width, resolution.height);
			if (!display.Create(config))
				FATAL__("Unable to create window (" + display.GetErrorStr() + ")");
			if (updateFile)
				Detail::UpdateXML(display,flags,display.GetPreMaxiMinimizeLocation(), xmlFileName);
			if (flags & DisplayConfig::IsMaximized)
				context.MaximizeWindow();
			//elif (flags & DisplayConfig::IsMinimzed)
			//	context.MinimizeWindow();
			onResize(resolution,context.GetDisplayConfigFlags());
			return flags;
		}
		
		

};


#endif
