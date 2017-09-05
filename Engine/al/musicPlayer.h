#pragma once

#include "decoder.h"
#include <general/thread_system.h>
#include "openal.h"
#include <general/timer.h>

namespace Engine
{
	namespace OpenAL
	{
		class MusicPlayer : public Sys::ThreadObject
		{
		public:
			typedef Sys::ThreadObject	Super;

			/**/		MusicPlayer() {};
			/**/		MusicPlayer(const MusicPlayer&)=delete;
			virtual		~MusicPlayer();
			void		operator=(const MusicPlayer&)=delete;

			void		Begin(const PathString&source, bool loop);

			/**
			Updates playback volume.
			*/
			void		SetVolume(float volume);

			float		GetVolume() const	{return volume;}

			void		ThreadMain() override;
			void		Stop();
			void		FadeOut(float fadeSeconds);
		private:
			bool			GetData(ALuint target, ALenum format, count_t samplesPerSecond);

			Timer::Time		fadeStarted;
			float			fadeSeconds;
			String			exception;
			Array<BYTE>		buffer;
			count_t			bufferFrameSize;
			float			volume = 1;
			AudioDecoder	decoder;
			PathString		source;
			bool			loop;
			volatile bool	quit=false,doFade=false;;
		};






	}







}
