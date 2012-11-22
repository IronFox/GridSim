#ifndef engineJoystickH
#define engineJoystickH
#include "../../math/basic.h"
#include "input.h"

#pragma comment(lib,"Winmm.lib")

namespace Engine
{

	class	Joystick
	{
	private:
	#if SYSTEM==WINDOWS
		JOYINFOEX	info;
		JOYCAPS		caps;
	#endif
	public:
			InputMap	&map;
			float		x,
						y,
						r,
						throttle,
						dead_zone;
			
			
						Joystick(InputMap&input_map);
			void		regAnalogInputs();			//!< Registers analog input types to the input map						
			void		update();
	};
	
	
	
	extern Joystick	joystick;



}


#endif
