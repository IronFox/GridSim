#pragma once

#include "decoder.h"
#include <general/thread_system.h>
#include "openal.h"
#include <general/timer.h>

namespace Engine
{
	namespace OpenAL
	{
		class ThreadedDecoder : public Sys::ThreadObject
		{
		public:
			typedef Sys::ThreadObject	Super;

			/**/		ThreadedDecoder() {};
			/**/		ThreadedDecoder(const ThreadedDecoder&)=delete;
			virtual		~ThreadedDecoder();
			void		operator=(const ThreadedDecoder&)=delete;

			void		Begin(const PathString&source, bool loop);

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
			
			AudioDecoder	decoder;
			PathString		source;
			bool			loop;
			volatile bool	quit=false,doFade=false;;
		};






	}







}
