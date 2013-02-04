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
		ALuint				bufferHandle;
		friend WaveBuffer	load(const String&filename, String*);
		friend void			discard(WaveBuffer&wbo);
	public:
		/**/				WaveBuffer():bufferHandle(0) {}
		inline bool			isEmpty()	const	{return bufferHandle == 0;}
		inline bool			IsEmpty()	const	{return bufferHandle == 0;}
		inline bool			isNotEmpty()	const	{return bufferHandle != 0;}
		inline bool			IsNotEmpty()	const	{return bufferHandle != 0;}
		inline ALuint		getHandle()	const	{return bufferHandle;}
		inline ALuint		GetHandle()	const	{return bufferHandle;}
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
		/**/			Source();
		/**/			Source(Source&&fleeting);
		virtual			~Source();
	
		bool			create();
		inline bool		Create()			{return create();}
		bool			isCreated()	const;
		inline bool		IsCreated() const	{return isCreated();}
		void			destroy();
		inline void		Destro()			{destroy();}
			
		void			setWaveBuffer(const WaveBuffer&wbo);
		inline void		SetWaveBuffer(const WaveBuffer&wbo)		{setWaveBuffer(wbo);}
		void			replaceWaveBuffer(const WaveBuffer&wbo);
		inline void		ReplaceWaveBuffer(const WaveBuffer&wbo)	{replaceWaveBuffer(wbo);}
		void			enqueueWaveBuffer(const WaveBuffer&wbo);
		inline void		EnqueueWaveBuffer(const WaveBuffer&wbo)	{enqueueWaveBuffer(wbo);}
		void			clearQueue();
		inline void		ClearQueue()	{clearQueue();}
		void			rewind();
		inline void		Rewind()		{rewind();}
		bool			play();
		inline bool		Play()			{play();}
		void			stop();
		inline void		Stop()			{stop();}
		bool			isPlaying()	const;
		inline bool		IsPlaying()	const	{return isPlaying();}
		void			pause();
		inline void		Pause()			{pause();}
		bool			isPaused()	const;
		inline bool		IsPaused()	const	{return isPaused();}
		void			setLooping(bool do_loop);
		inline void		SetLooping(bool doLoop)	{setLooping(doLoop);}
		bool			isLooping()	const;
		inline bool		IsLooping()	const	{return isLooping();}
		void			setVelocity(const TVec3<>&velocity);
		inline void		SetVelocity(const TVec3<>&velocity)	{setVelocity(velocity);}
		void			setVelocity(float x, float y, float z);
		inline void		SetVelocity(float x, float y, float z)	{setVelocity(x,y,z);}
		void			getVelocity(TVec3<>&velocity_out)	const;
		inline void		GetVelocity(TVec3<>&velocityOut)	const	{getVelocity(velocityOut);}
		void			locate(float x, float y, float z, bool relative=false);
		inline void		Locate(float x, float y, float z, bool relative=false)	{locate(x,y,z,relative);}
		void			locate(const TVec3<>&position, bool relative=false);
		inline void		Locate(const TVec3<>&position, bool relative=false)	{locate(position,relative);}
		void			getLocation(TVec3<>&position_out);
		inline void		GetLocation(TVec3<>&positionOut)	{getLocation(positionOut);}
		void			getPosition(TVec3<>&position_out);
		inline void		GetPosition(TVec3<>&positionOut)	{getPosition(positionOut);}
		bool			relativeLocation()	const;
		inline bool		IsRelativeLocation()	const	{return relativeLocation();}
		void			setPitch(float pitch);
		inline void		SetPitch(float pitch)	{setPitch(pitch);}
		float			pitch()		const;
		inline float	GetPitch()	const	{return pitch();}
		void			setGain(float gain);
		inline void		SetGain(float gain)	{setGain(gain);}
		float			gain()		const;
		inline float	GetGain()		const	{return gain();}
		void			setVolume(float volume);	//!< Identical to setGain(). volume should be in the range [0,1]
		inline void		SetVolume(float volume)	{setVolume(volume);}
		float			volume()		const;
		inline float	GetVolume()		const	{return volume();}
		void			setReferenceDistance(float distance);
		inline void		SetReferenceDistance(float distance)	{setReferenceDistance(distance);}
		float			referenceDistance()	const;
		inline float	GetReferenceDistance()	const	{return referenceDistance();}

		void			swap(Source&other)
		{
			swp(handle,other.handle);
		}
	};
	
	typedef Source	Player;

	bool				init();
	inline bool			Init()	{init();}
	bool				isInitialized();
	inline bool			IsInitialized()	{return isInitialized();}
	
	WaveBuffer			load(const String&filename, String*error_out = NULL);
	inline WaveBuffer	Load(const String&filename, String*errorOut = NULL)	{return load(filename,errorOut);}
	void				discard(WaveBuffer&wbo);
	inline void			Discard(WaveBuffer&wbo)	{discard(wbo);}
	
	void				setSpeedOfSound(float speed);
	inline void			SetSpeedOfSound(float speed)	{setSpeedOfSound(speed);}
	float				speedOfSound();
	inline float		GetSpeedOfSound()	{return speedOfSound();}
	
	
	bool				playOnce(float x, float y, float z, const WaveBuffer&buffer);
	inline bool			PlayOnce(float x, float y, float z, const WaveBuffer&buffer)	{return playOnce(x,y,z,buffer);}
	bool				playOnce(const TVec3<>&position, const WaveBuffer&buffer);
	inline bool			PlayOnce(const TVec3<>&position, const WaveBuffer&buffer)	{return playOnce(position,buffer);}
	
	
	namespace Listener
	{
		void					setOrientation(const TVec3<>&direction, const TVec3<>&up, bool negate_direction=false, bool negate_up=false);
		inline void				SetOrientation(const TVec3<>&direction, const TVec3<>&up, bool negateDirection=false, bool negateUp=false)
								{setOrientation(direction,up,negateDirection,negateUp);}
		const TVec3<>&			direction();
		inline const TVec3<>&	GetDirection()	{return direction();}
		const TVec3<>&			up();
		inline const TVec3<>&	GetUp()	{return up();}
		void					setVelocity(float x, float y, float z);
		inline void				SetVelocity(float x, float y, float z)	{setVelocity(x,y,z);}
		void					setVelocity(const TVec3<>&velocity);
		inline void				SetVelocity(const TVec3<>&velocity)	{setVelocity(velocity);}
		const TVec3<>&			velocity();
		inline const TVec3<>&	GetVelocity()	{return velocity();}
		void					locate(const TVec3<>&position);
		inline void				Locate(const TVec3<>&position)	{locate(position);}
		const TVec3<>&			location();
		inline const TVec3<>&	GetLocation()	{return location();}
		const TVec3<>&			position();
		inline const TVec3<>&	GetPosition()	{return position();}
	}



}




#endif
