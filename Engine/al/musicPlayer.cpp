#include "musicPlayer.h"

namespace Engine
{
	namespace OpenAL
	{

		static	void	CheckError(const TCodeLocation&loc)
		{
			ALenum error = alGetError();
			if (error != AL_NO_ERROR)
			{
				throw Except::Program::GeneralFault(loc,"OpenAL: "+String(alGetString(error)));
			}
		}


		void	MusicPlayer::Stop()
		{
			if (!Super::IsActive())
				return;
			quit = true;
			
		}

		void	MusicPlayer::FadeOut(float fadeSeconds)
		{
			this->fadeSeconds = fadeSeconds;
			this->fadeStarted = timer.Now();
			this->doFade = true;
		}


		bool MusicPlayer::GetData(ALuint target, ALenum format, count_t samplesPerSecond)
		{
			if (decoder.IsAtEnd())
			{
				if (loop)
					decoder.Rewind();
				else
					return false;
			}
			count_t framesRead = decoder.ReadFrames(bufferFrameSize,buffer.pointer());
			alBufferData(target, format, buffer.pointer(), framesRead * decoder.GetFrameByteSize(), samplesPerSecond);
			CheckError(CLOCATION);
			return true;
		}



		void	MusicPlayer::Begin(const PathString&source, bool loop)
		{
			Stop();

			Super::awaitCompletion();
			if (exception.IsNotEmpty())
			{
				String ex;
				ex.swap(exception);
				throw Except::Exception(CLOCATION,ex);
			}

			this->source = source;
			this->loop = loop;
			this->doFade = false;
			this->quit = false;
			Super::Start();
		}


			 
		MusicPlayer::~MusicPlayer()
		{
		}



		void	MusicPlayer::ThreadMain()
		{
			try
			{
				ALuint			output=0,buffer[4];

				decoder.Open(source);
				alGenSources( 1, &output );
				CheckError(CLOCATION);
				alGenBuffers(ARRAYSIZE(buffer),buffer);
				CheckError(CLOCATION);

				const count_t framesPerSecond = decoder.GetFramesPerSecond();
				const count_t numChannels = decoder.GetChannels();

				bufferFrameSize = framesPerSecond*5;
				this->buffer.SetSize(bufferFrameSize*numChannels*decoder.GetBitsPerSample()/8);

				const ALenum format = FindFormat(numChannels,decoder.GetBitsPerSample());

				for (index_t i = 0; i < ARRAYSIZE(buffer); i++)
					GetData(buffer[i],format,framesPerSecond*numChannels);

				alSourceQueueBuffers(output,ARRAYSIZE(buffer),buffer);
				CheckError(CLOCATION);

				alSourcePlay(output);
				CheckError(CLOCATION);
				alSourcef(output,AL_GAIN,0.5f);

				while (!quit)
				{
					if (!doFade)
						Sleep(100);
					else
						Sleep(1);
					ALint	processed = 0;
					alGetSourcei(output, AL_BUFFERS_PROCESSED, &processed);
					while (processed > 0)
					{
						ALuint uiBuffer = 0;
						alSourceUnqueueBuffers(output, 1, &uiBuffer);
						if (!GetData(uiBuffer,format,framesPerSecond*numChannels))
							quit = true;
						else
							alSourceQueueBuffers(output,1,&uiBuffer);
						processed--;
					}

					if (doFade)
					{
						float fadeAt = timer.GetSecondsSince(fadeStarted) / fadeSeconds;
						if (fadeAt > 1)
						{
							break;
						}

						alSourcef(output,AL_GAIN,1.f-fadeAt);
						
					}

				}
				alSourceStop(output);

				decoder.Clear();

				alDeleteSources(1,&output);
				alDeleteBuffers(ARRAYSIZE(buffer),buffer);

			}
			catch (const std::exception&ex)
			{
				exception = ex.what();
			}
			catch(...)
			{}
		}






	}







}
