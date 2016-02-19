#include "../global_root.h"
#include "openal.h"

#include <cstdlib>
#include <iostream>
 
//using namespace std;

namespace OpenAL
{
	static Source	source_field[0x100];
	
	
	bool				playOnce(float x, float y, float z, const WaveBuffer&buffer)
	{
		TVec3<>	field = {x,y,z};
		return playOnce(field,buffer);
	}
	
	bool				playOnce(const TVec3<>&position, const WaveBuffer&buffer)
	{
		for (index_t i = 0; i < ARRAYSIZE(source_field); i++)
		{
			if (source_field[i].isPlaying())
				continue;
			if (!source_field[i].isCreated())
				source_field[i].create();
			source_field[i].locate(position);
			source_field[i].setWaveBuffer(buffer);
			return source_field[i].play();
		}
		return false;
	}

	
	
	
	
	
	
	
	Source::Source():handle(0)
	{}

	Source::Source(Source&&fleeting)
	{
		handle = fleeting.handle;
		fleeting.handle = 0;
	}

	
	Source::~Source()
	{
		destroy();
	}
	
	
	bool	Source::create()
	{
		if (handle)
			return true;
		if (!init())
			return false;
	    alGenSources( 1, &handle );
		return handle != 0;
	}
	
	bool	Source::isCreated()	const
	{
		return handle != 0;
	}
	
	void	Source::destroy()
	{
		if (!handle)
			return;
		alDeleteSources(1,&handle);
		handle = 0;
	}
	
			
	void	Source::replaceWaveBuffer(const WaveBuffer&wbo)
	{
		alSourcei( handle, AL_BUFFER, wbo.getHandle() );		
	}
	
	void	Source::setWaveBuffer(const WaveBuffer&wbo)
	{
		alSourcei( handle, AL_BUFFER, wbo.getHandle() );		
	}
	
	void	Source::enqueueWaveBuffer(const WaveBuffer&wbo)
	{
		ALuint whandle = wbo.getHandle();
		alSourceQueueBuffers(handle,1,&whandle);
	}
	
	void	Source::clearQueue()
	{
		alSourceStop(handle);
		ALint	processed = 0;
		alGetSourcei(handle, AL_BUFFERS_PROCESSED, &processed);
		while (processed > 0)
		{
			ALuint uiBuffer = 0;
			alSourceUnqueueBuffers(handle, 1, &uiBuffer);
			processed--;
		}
	}
	
	void	Source::rewind()
	{
		alSourceRewind(handle);
	}
	
	bool	Source::play()
	{
		if (!handle)
			return false;
		alSourcePlay(handle);
		return alGetError()==AL_NO_ERROR;
	}
	
	void	Source::stop()
	{
		alSourceStop(handle);
	}
	
	bool	Source::isPlaying()	const
	{
		if (!handle)
			return false;
		ALint	iState;
		alGetSourcei(handle, AL_SOURCE_STATE, &iState);
		return iState == AL_PLAYING;
	}
	
	void	Source::pause()
	{
		alSourcePause(handle);
	}
	
	bool	Source::isPaused()	const
	{
		ALint	iState;
		alGetSourcei(handle, AL_SOURCE_STATE, &iState);
		return iState == AL_PAUSED;
	}
	
	void	Source::setLooping(bool do_loop)
	{
		alSourcei(handle, AL_LOOPING, do_loop?AL_TRUE:AL_FALSE);
	}
	
	bool	Source::isLooping()	const
	{
		ALint	iLoop;
		alGetSourcei(handle, AL_LOOPING, &iLoop);
		return iLoop == AL_TRUE;
	}
	
	void	Source::setVelocity(float x, float y, float z)
	{
		TVec3<> v = {x,y,z};
		setVelocity(v);
	}
	
	void	Source::setVelocity(const TVec3<>&velocity)
	{
		alSourcefv(handle, AL_VELOCITY, velocity.v);
	}
	
	void	Source::getVelocity(TVec3<>&velocity_out)	const
	{
		alGetSourcefv(handle, AL_VELOCITY, velocity_out.v);
	}
	
	void	Source::locate(const TVec3<>&position, bool relative)
	{
		alSourcefv(handle, AL_POSITION, position.v);
		alSourcei(handle,AL_SOURCE_RELATIVE,relative?AL_TRUE:AL_FALSE);
	}
	
	void	Source::locate(float x, float y, float z, bool relative)
	{
		TVec3<> v = {x,y,z};
		locate(v,relative);
	}
	
	void	Source::GetLocation(TVec3<>&position_out)
	{
		alGetSourcefv(handle, AL_POSITION,position_out.v);
	}
	
	void	Source::getPosition(TVec3<>&position_out)
	{
		alGetSourcefv(handle, AL_POSITION,position_out.v);
	}
	
	bool	Source::relativeLocation()	const
	{
		ALint	val;
		alGetSourcei(handle,AL_SOURCE_RELATIVE,&val);
		return val == AL_TRUE;
	}
	
	void	Source::setPitch(float pitch)
	{
		alSourcef(handle,AL_PITCH,pitch);
	}
	
	float	Source::pitch()		const
	{
		ALfloat val;
		alGetSourcef(handle,AL_PITCH,&val);
		return val;
	}
		
	void	Source::setGain(float gain)
	{
		alSourcef(handle,AL_GAIN,gain);
	}
	
	float	Source::gain()		const
	{
		ALfloat val;
		alGetSourcef(handle,AL_GAIN,&val);
		return val;
	}
			
	void	Source::setVolume(float volume)
	{
		alSourcef(handle,AL_GAIN,volume);
	}
	
	float	Source::volume()		const
	{
		ALfloat val;
		alGetSourcef(handle,AL_GAIN,&val);
		return val;
	}
	
	void	Source::setReferenceDistance(float distance, float rolloffFactor)
	{
		alSourcef(handle,AL_REFERENCE_DISTANCE,distance);
		alSourcef(handle,AL_ROLLOFF_FACTOR,rolloffFactor);
	}
	
	float	Source::referenceDistance()	const
	{
		ALfloat val;
		alGetSourcef(handle,AL_REFERENCE_DISTANCE,&val);
		return val;
	}
	
	namespace
	{
		CWaves		wave_loader;
		bool		initialized = false;
	}

	bool	init()
	{
		if (initialized)
			return true;
		ALCcontext *pContext = NULL;
		ALCdevice *pDevice = NULL;
	//	OPENALFNTABLE	ALFunction;
		
//		cout << "loading openal\n";
		
	/*	if (!LoadOAL10Library(NULL, &ALFunction) == TRUE)
		{
			cout << "openal loading failed\n";
			return false;
		}*/
		
		const char*defaultDeviceName = (const char *)alcGetString(NULL, ALC_DEFAULT_DEVICE_SPECIFIER);
		if (!defaultDeviceName || !strlen(defaultDeviceName))
		{
			std::cout << "no default device available\n";
			return false;
		}
		std::cout << "default device is '"<<defaultDeviceName<<"'"<<std::endl;
			
		
		pDevice = alcOpenDevice(defaultDeviceName);
		if (pDevice)
		{
			std::cout << "device opened\n";
			pContext = alcCreateContext(pDevice, NULL);
			if (pContext)
			{
				std::cout << "context created\n";
				alcMakeContextCurrent(pContext);
				initialized = true;
				std::cout << "all done\n";
				return true;
			}
			else
			{
				std::cout << "context creation failed. closing device...\n";
				alcCloseDevice(pDevice);
			}
		}
		std::cout << "all done. initialization failed. returning\n";
		return false;
	}

	
	bool				isInitialized()
	{
		return initialized;
	}
	

	
	WaveBuffer			load(const String&filename, String*error_out)
	{
		WaveBuffer	result;
		if (!init())
			return result;
		
		WAVEID			WaveID;
		ALint			iDataSize, iFrequency;
		ALenum			eBufferFormat;
		ALchar			*pData;

		if (error_out)
			(*error_out) = "No error";

		WAVERESULT rs = wave_loader.LoadWaveFile(filename.c_str(), &WaveID);
		if (WV_SUCCEEDED(rs))
		{
			if ((WV_SUCCEEDED(wave_loader.GetWaveSize(WaveID, (unsigned long*)&iDataSize))) &&
				(WV_SUCCEEDED(wave_loader.GetWaveData(WaveID, (void**)&pData))) &&
				(WV_SUCCEEDED(wave_loader.GetWaveFrequency(WaveID, (unsigned long*)&iFrequency))) &&
				(WV_SUCCEEDED(wave_loader.GetWaveALBufferFormat(WaveID, &alGetEnumValue, (unsigned long*)&eBufferFormat))))
			{
				alGetError();
				alGenBuffers(1,&result.bufferHandle);
				alGetError();
				alBufferData(result.bufferHandle, eBufferFormat, pData, iDataSize, iFrequency);
				if (alGetError() != AL_NO_ERROR)
				{
					if (error_out)
						(*error_out) = "OpenAL internal error";
					
					alDeleteBuffers(1,&result.bufferHandle);
					result.bufferHandle = 0;
				}
			}
			wave_loader.DeleteWaveFile(WaveID);
		}
		else
			if (error_out)
			{
				(*error_out) = "Failed to load file '"+filename+"'";
				#undef ECASE
				#define ECASE(_CASE_)	case _CASE_: (*error_out) += " ("#_CASE_")"; break;
				switch (rs)
				{
					ECASE(WR_OK)
					ECASE(WR_OUTOFMEMORY)
					ECASE(WR_BADWAVEFILE)
					default:
						(*error_out) += " (unknown error)";
				}	
			}

		return result;
	}
	
	void				discard(WaveBuffer&wbo)
	{
		if (!wbo.bufferHandle)
			return;
		alDeleteBuffers(1,&wbo.bufferHandle);
		wbo.bufferHandle = 0;
	}
	
	
	void				setSpeedOfSound(float speed)
	{
		alSpeedOfSound(speed);
	}
	
	float				speedOfSound()
	{
		return alGetFloat(AL_SPEED_OF_SOUND);
	}
	
	namespace Listener
	{

		void					SetGain(float volume)
		{
			alListenerf(AL_GAIN,volume);
		}

			void			setOrientation(const TVec3<float>&direction, const TVec3<float>&up, bool negate_direction, bool negate_up)
			{
				ALfloat field[6];
				TVec3<float>	&d = Vec::ref3(field),
								&u = Vec::ref3(field+3);
				d = direction;
				u = up;
				Vec::normalize0(d);
				Vec::normalize0(u);
				if (negate_direction)
					Vec::mult(d,-1);
				if (negate_up)
					Vec::mult(u,-1);
				//field[2] *= -1;
				//field[5] *= -1;
				alListenerfv(AL_ORIENTATION,field);
			}
			
			static float field[6];
			
			const TVec3<>&	direction()
			{
				alGetListenerfv(AL_ORIENTATION,field);
				return Vec::ref3(field);
			}
			
			const TVec3<>&	up()
			{
				alGetListenerfv(AL_ORIENTATION,field);
				return Vec::ref3(field+3);
			}
			void			setVelocity(float x, float y, float z)
			{
				TVec3<> v = {x,y,z};
				setVelocity(v);
			}
			
			void			setVelocity(const TVec3<>& velocity)
			{
				alListenerfv(AL_VELOCITY,velocity.v);
			}
			
			namespace
			{
				TVec3<> velocity_,location_;
			}
			
			const TVec3<>&	velocity()
			{
				alGetListenerfv(AL_VELOCITY,velocity_.v);
				return velocity_;
			}
			
			void			locate(const TVec3<>&position)
			{
				//float p[3] = {position[0],position[1],-position[2]};
				alListenerfv(AL_POSITION,position.v);
			}
			
			const TVec3<>&	location()
			{
				alGetListenerfv(AL_POSITION,location_.v);
				return location_;
			}
			
			const TVec3<>&	position()
			{
				alGetListenerfv(AL_POSITION,location_.v);
				return location_;
			}
	}


}
