#ifndef openalH
#define openalH

#include "al.h"
#include "alc.h"
#include "efx.h"
#include "efx-creative.h"
#include "xram.h"
#include "CWaves.h"


#include "../global_root.h"
#include "../math/vector.h"

#pragma comment(lib,"OpenAL32.lib")



namespace OpenAL
{
	class WaveBuffer
	{
	protected:
			ALuint				buffer_handle;
			friend WaveBuffer	load(const String&filename, String*);
			friend void			discard(WaveBuffer&wbo);
			
	public:
						WaveBuffer():buffer_handle(0)
						{}
	
			bool		isEmpty()	const
						{
							return buffer_handle==0;
						}
			ALuint		getHandle()	const
						{
							return buffer_handle;
						}
	};
	
	typedef WaveBuffer	Wave;
	
	
	class Source
	{
	protected:
			ALuint			handle;
			
							Source(const Source&other)
							{}
			void			operator=(const Source&other)
							{}
	public:
							Source();
							Source(Source&&fleeting);
	virtual					~Source();
	
			bool			create();
			bool			isCreated()	const;
			void			destroy();
			
			void			setWaveBuffer(const WaveBuffer&wbo);
			void			replaceWaveBuffer(const WaveBuffer&wbo);
			void			enqueueWaveBuffer(const WaveBuffer&wbo);
			void			clearQueue();
			void			rewind();
			bool			play();
			void			stop();
			bool			isPlaying()	const;
			void			pause();
			bool			isPaused()	const;
			void			setLooping(bool do_loop);
			bool			isLooping()	const;
			void			setVelocity(const TVec3<>&velocity);
			void			setVelocity(float x, float y, float z);
			void			getVelocity(TVec3<>&velocity_out)	const;
			void			locate(float x, float y, float z, bool relative=false);
			void			locate(const TVec3<>&position, bool relative=false);
			void			getLocation(TVec3<>&position_out);
			void			getPosition(TVec3<>&position_out);
			bool			relativeLocation()	const;
			void			setPitch(float pitch);
			float			pitch()		const;
			void			setGain(float gain);
			float			gain()		const;
			void			setVolume(float volume);	//!< Identical to setGain(). volume should be in the range [0,1]
			float			volume()		const;
			void			setReferenceDistance(float distance);
			float			referenceDistance()	const;

			void			swap(Source&other)
			{
				swp(handle,other.handle);
			}
	};
	
	typedef Source	Player;

	bool				init();
	bool				isInitialized();
	
	WaveBuffer			load(const String&filename, String*error_out = NULL);
	void				discard(WaveBuffer&wbo);
	
	void				setSpeedOfSound(float speed);
	float				speedOfSound();
	
	
	bool				playOnce(float x, float y, float z, const WaveBuffer&buffer);
	bool				playOnce(const TVec3<>&position, const WaveBuffer&buffer);
	
	
	namespace Listener
	{
			void			setOrientation(const TVec3<>&direction, const TVec3<>&up, bool negate_direction=false, bool negate_up=false);
			const TVec3<>&	direction();
			const TVec3<>&	up();
			void			setVelocity(float x, float y, float z);
			void			setVelocity(const TVec3<>&velocity);
			const TVec3<>&	velocity();
			void			locate(const TVec3<>&position);
			const TVec3<>&	location();
			const TVec3<>&	position();
	}



}




#endif
