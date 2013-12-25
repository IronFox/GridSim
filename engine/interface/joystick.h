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
		static const UINT NumJoysticks = 16;
	//	JOYINFOEX	info[NumJoysticks];
	//	JOYCAPS		caps[NumJoysticks];
	#endif
	public:
		InputMap	&map;

		struct State
		{
			UINT		index;
			float		x,
						y,
						r,
						throttle,
						dead_zone;
			String		name;
			JOYINFOEX	info;
			JOYCAPS		caps;

			/**/		State():x(0),y(0),r(0),throttle(0),dead_zone(0.05)	{}

		private:
			void		Process();
			friend class Joystick;
		};
	private:
		Buffer<State,NumJoysticks>	active;
		bool			registered;
	public:
		count_t			CountActive()	const	{return active.count();}
		const State*	GetActive(index_t i) const {return i < active.count() ? active+i : NULL;}
		const State*	GetActive(const String&name) const;	


						Joystick(InputMap&input_map);
		void			RegAnalogInputs();			//!< Registers analog input types to the input map						
		void			UnregAnalogInputs();			//!< Unregisters analog input types to the input map						
		void			update();

		void			Reinit();
	};
	
	
	
	extern Joystick	joystick;



}


#endif
