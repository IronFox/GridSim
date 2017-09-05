#pragma once

#include <global_string.h>
#include <container/array.h>
#include <container/buffer.h>
#include <container/queue.h>
#include <iostream>
#include <string.h>
#include <windows.h>


#pragma comment(lib,"Mfplat.lib")
#pragma comment(lib,"Mfreadwrite.lib")
#pragma comment(lib,"mfuuid.lib")


struct IMFSourceReader;
struct IMFMediaType;

namespace DeltaWorks
{

	class BaseHandle
	{
	protected:
		IUnknown*	_handle;
	public:
		/**/		BaseHandle():_handle(NULL)	{}
		/**/		BaseHandle(const BaseHandle&other):_handle(other._handle)	{if (_handle) _handle->AddRef();}
		/**/		~BaseHandle()	{if (!application_shutting_down) Clear();}
		BaseHandle&	operator=(const BaseHandle&other)	{Clear(); _handle = other._handle; if (_handle) _handle->AddRef(); return *this;}
		bool		IsSet()	const	{return _handle != NULL;}
		bool		IsNotEmpty() const	{return _handle != NULL;}
		bool		IsEmpty() const	{return _handle == NULL;}
		IUnknown**	Init()	{Clear(); return &_handle;}
		IUnknown*const*	Reference() const {return &_handle;}
		void		Clear()		{if (_handle) _handle->Release(); _handle = NULL;}
		void		Release()	{Clear();}
		void		swap(BaseHandle&other)	{std::swap(_handle,other._handle);}
		bool		operator==(const BaseHandle&other) const {return _handle == other._handle;}
		bool		operator!=(const BaseHandle&other) const	{return _handle != other._handle;}
		bool		operator!()	{return _handle == NULL;}

	};

	template <typename T>
		class Handle : public BaseHandle
		{
		public:
			typedef BaseHandle Super;
			Handle<T>&	operator=(const Handle<T>&other)	{Super::operator=(other); return *this;}
			T*			operator->() const	{return (T*)_handle;}
			operator	T*() const {return (T*)_handle;}
			T**			Init()	{Clear(); return (T**)&_handle;}
			T*const*	Reference() const {return (T*const*)&_handle;}
			void		swap(Handle<T>&other)	{std::swap(_handle,other._handle);}
		};


	/**
	Audio codec decoder.
	Uses Windows Media Foundation.
	Heavily modified version of libAudioDecoder by Albert Santoni, Bill Good, RJ Ryan (2012).
	https://github.com/asantoni/libaudiodecoder
	*/
	class AudioDecoder
	{
	public:
		typedef short SHORT_SAMPLE;
		typedef float SAMPLE;

		virtual ~AudioDecoder();

		/**
		Clears the local decoder and opens the specified file.
		Throws exceptions in case of errors.
		*/
		void	Open(const PathString&);
		/**
		Clears and deactivates the local decoder
		*/
		void	Clear();
		/**
		Attempts to seek the local audio document to the specified frame.
		The local decoder must be active.
		May throw exceptions
		*/
		void	SeekFrame(index_t frameIdx);
		/**
		Attempts to read the specified number of frames from the local document.
		The local decoder must be active.
		May throw exceptions
		@param numFrames Number of frames to read. Less may be read if the stream end has been reached
		@param destination Buffer to read samples to. Must be at least numFrames * GetFrameByteSize() in size
		@return Number of frames actually read. May be less or equal to numFrames
		*/
		count_t	ReadFrames(const count_t numFrames, void *destination);
		/**
		Queries whether or not the local decoder is currently active
		*/
		bool	IsActive() const {return data.reader.IsNotEmpty();}
		count_t CountSamples() const;
		count_t	GetChannels() const {return data.numChannels;}
		count_t	GetFramesPerSecond() const {return data.frameRate;}
		void	Rewind()	{SeekFrame(0); data.eof=false;};
		/**
		Checks whether or not the local stream has reached the end.
		*/
		bool	IsAtEnd() const {return data.eof;}
		count_t	GetBitsPerSample() const {return data.m_iBitsPerSample;}
		count_t	GetFrameByteSize() const {return data.numChannels * data.m_iBitsPerSample/8;}
	private:
		void	ConfigureAudioStream();
		void	ReadProperties();
		double	secondsFromMF(__int64 mf) const;
		__int64 mfFromSeconds(double sec) const;
		__int64 frameFromMF(__int64 mf) const;
		__int64 mfFromFrame(__int64 frame) const;
		void	GetUINT32(const GUID&id, UINT32&v) const;

		struct Data
		{
			PathString filename;

			count_t	numChannels=0,frameRate=0;

			Handle<IMFSourceReader>	 reader;
			Handle<IMFMediaType> audioType;
			__int64 m_nextFrame = 0;

			__int64 m_mfDuration = 0;
			long m_iCurrentPosition = 0;
			bool m_seeking = false;
			double m_fDuration = 0;
			unsigned int m_iBitsPerSample = 0;
			bool	eof = false;
		}		data;

		Queue<BYTE>	extra;
		//Buffer<short> leftoverBuffer;
		//index_t leftoverFramePosition = 0;
		//SHORT_SAMPLE rawBuffer[8192];
	};
	
	
}

