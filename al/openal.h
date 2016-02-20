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
	namespace Listener
	{
		void					CheckSources();	//!< Called by SetPosition automatically
	}

	class WaveBuffer
	{
	protected:
		ALuint				bufferHandle;
		friend WaveBuffer	Load(const String&filename, String*);
		friend void			Discard(WaveBuffer&wbo);
	public:
		/**/				WaveBuffer():bufferHandle(0) {}
		inline bool			IsEmpty()	const	{return bufferHandle == 0;}
		inline bool			IsNotEmpty()	const	{return bufferHandle != 0;}
		inline ALuint		getHandle()	const	{return bufferHandle;}
		inline ALuint		GetHandle()	const	{return bufferHandle;}
	};
	
	typedef WaveBuffer	Wave;
	
	struct TSourceData
	{
		ALuint			handle=0;
		TVec3<>			position;
		bool			isRelative=false,
						wantsToPlay=false,
						isPaused=false,
						hasWave=false;
		float			priority = 1.f,
						gain=1.f,
						refDistance=1.f,
						rollOffFactor=1.f;
	};
	
	class Source : private TSourceData
	{
	private:
						Source(const Source&other)
						{}
		void			operator=(const Source&other)
						{}

		bool			ALIsPlaying()	const;
		bool			ALIsPaused()	const;
		float			ALGetGain()		const;
		float			ALGetReferenceDistance()	const;
		/**
		Stops the OpenAL playback
		@return true, if playback was playing and is now stopped, false otherwise
		*/
		bool			ALStop();
		bool			ALStart();


		friend void		Listener::CheckSources();
	public:
		/**/			Source();
		/**/			Source(Source&&fleeting);
		virtual			~Source();
	
		bool			Create();
		bool			IsCreated()	const;
		void			Destroy();
			
		void			SetWaveBuffer(const WaveBuffer&wbo);
		void			ReplaceWaveBuffer(const WaveBuffer&wbo);
		void			EnqueueWaveBuffer(const WaveBuffer&wbo);
		void			ClearQueue();
		void			Rewind();
		void			Play();
		void			Stop();
		bool			IsPlaying()	const	{return wantsToPlay;}
		void			Pause();
		bool			IsPaused()	const;
		void			SetLooping(bool do_loop);
		bool			IsLooping()	const;
		void			SetVelocity(const TVec3<>&velocity);
		void			SetVelocity(float x, float y, float z);
		void			GetVelocity(TVec3<>&velocity_out)	const;
		void			SetPosition(float x, float y, float z, bool relative=false);
		void			SetPosition(const TVec3<>&position, bool relative=false);
		void			GetLocation(TVec3<>&positionOut);
		inline void		GetPosition(TVec3<>&positionOut)	{GetLocation(positionOut);}
		bool			IsRelativeLocation()	const;
		void			SetPitch(float pitch);
		float			GetPitch()	const;
		void			SetGain(float gain);
		float			GetGain()		const	{return gain;}
		inline void		SetVolume(float volume)	{SetGain(volume);}
		inline float	GetVolume()		const	{return gain;}
		void			SetReferenceDistance(float distance, float rolloffFactor = 1.f);
		float			GetReferenceDistance()	const	{return refDistance;}
		float			GetRollOffFactor() const		{return rollOffFactor;}

		bool			CheckQueueIsDone()	const;
		void			UpdatePriority();
		inline float	GetPriority() const {return priority;}

		void			swap(Source&other);
	};
	
	typedef Source	Player;

	/**
	Initializes the global device handle and audio context.
	This function is called automatically when the listener is updated the first time, or sources created
	*/
	bool				Init();
	bool				IsInitialized();
	/**
	Destroys the global OpenAL context and device handle.
	Does not destroy audio sources but playback will probably stop on its own.
	*/
	void				Shutdown();
	
	WaveBuffer			Load(const String&filename, String*error_out = NULL);
	void				Discard(WaveBuffer&wbo);
	
	void				SetSpeedOfSound(float speed);
	float				GetSpeedOfSound();
	
	
	bool				PlayOnce(float x, float y, float z, const WaveBuffer&buffer);
	bool				PlayOnce(const TVec3<>&position, const WaveBuffer&buffer);
	
	
	namespace Listener
	{
		void					CheckSources();	//!< Called by SetPosition automatically

		void					SetOrientation(const TVec3<>&direction, const TVec3<>&up, bool negate_direction=false, bool negate_up=false);
		const TVec3<>&			GetDirectionVector();
		const TVec3<>&			GetUpVector();
		void					SetVelocity(float x, float y, float z);
		void					SetVelocity(const TVec3<>&velocity);
		const TVec3<>&			GetVelocity();
		/**
		Changes the listener position in the world.
		This function doubles as a priority cleanup. It is assumed you call this function around once per frame.
		*/
		void					SetPosition(const TVec3<>&position);
		const TVec3<>&			GetLocation();
		inline const TVec3<>&	GetPosition()	{return GetLocation();}
		void					SetGain(float volume);

		void					SetMaxPlayingSources(count_t);
		count_t					GetMaxPlayingSources();

	}



}




#endif
