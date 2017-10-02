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

			void		Begin(const PathString&source, bool loop, float fadeInSeconds = 0.5f);

			/**
			Updates playback volume.
			*/
			void		SetVolume(float volume);

			float		GetVolume() const	{return volume;}
			/**
			Checks if the local player is playing and not fading out
			*/
			bool		IsPlaying() const	{return isPlaying && !doFade;}
			bool		IsFadingOut() const {return doFade && isPlaying;}

			void		ThreadMain() override;
			void		Stop();
			void		FadeOut(float fadeOutSeconds);
		private:
			bool			GetData(ALuint target, ALenum format, count_t samplesPerSecond);

			Timer::Time		fadeOutStarted,fadeInStarted;
			float			fadeOutSeconds,fadeInSeconds;
			String			exception;
			Array<BYTE>		buffer;
			count_t			bufferFrameSize;
			float			volume = 1;
			AudioDecoder	decoder;
			PathString		source;
			bool			loop,doFadeIn=false,isPlaying=false;
			volatile bool	quit=false,doFade=false;
		};






	}







}
