#include "../../global_root.h"
#include "joystick.h"

namespace Engine
{
	Joystick::Joystick(InputMap&map_):map(map_),x(0),y(0),r(0),throttle(0),dead_zone(0.05)
	{
		#if SYSTEM == WINDOWS
			memset(&info,0,sizeof(info));
			info.dwSize = sizeof(info);
			info.dwFlags = JOY_RETURNALL;
			joyGetDevCaps(JOYSTICKID1,&caps,sizeof(caps));
		#else
		
		
		#endif
	}
	
	void Joystick::regAnalogInputs()
	{
		map.regAnalog("JoystickX",x,-1,1);
		map.regAnalog("JoystickY",y,-1,1);
		map.regAnalog("JoystickThrottle",throttle,-1,1);
		map.regAnalog("JoystickRudder",r,-1,1);
	}
	
	void Joystick::update()
	{
		x = 0;
		y = 0;
		r = 0;
		throttle = 0;
		#if SYSTEM==WINDOWS
			if (joyGetPosEx(JOYSTICKID1,&info) != JOYERR_NOERROR)
				return;
			x = (float)(info.dwXpos-caps.wXmin)/(float)(caps.wXmax-caps.wXmin)*2-1;
			y = (float)(info.dwYpos-caps.wYmin)/(float)(caps.wYmax-caps.wYmin)*2-1;
			if (fabs(x) < dead_zone)
				x = 0;
			if (fabs(y) < dead_zone)
				y = 0;
			if (caps.wCaps & JOYCAPS_HASZ)
			{
				throttle = -((float)(info.dwZpos-caps.wZmin)/(float)(caps.wZmax-caps.wZmin)*2.0f-1.0f);
				//ShowMessage("throttle is at "+String(throttle)+" as a result of ("+String(info.dwZpos)+"-"+String(caps.wZmin)+")/("+String(caps.wZmax)+"-"+String(caps.wZmin)+")");
			}
			if (caps.wCaps & JOYCAPS_HASR)
				r = (float)(info.dwRpos-caps.wRmin)/(float)(caps.wRmax-caps.wRmin)*2-1;
			if (fabs(r) < dead_zone)
				r = 0;
			for (unsigned i = 0; i < 32; i++)
			{
				bool down = (info.dwButtons&(1<<i))!=0;
				if (down != map.pressed[Key::JoystickButton0+i])
					if (down)
						map.keyDown(Key::JoystickButton0+i);
					else
						map.keyUp(Key::JoystickButton0+i);
			}
		#endif
	}
	
	Joystick		joystick(input);
}
