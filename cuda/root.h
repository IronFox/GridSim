#ifndef cuda_rootH
#define cuda_rootH

#define _CRT_SECURE_NO_DEPRECATE 1
#define _CRT_NONSTDC_NO_DEPRECATE 1

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <math.h>

#ifdef _WIN32
#  define WINDOWS_LEAN_AND_MEAN
#  include <windows.h>
#endif

#include <cuda_runtime_api.h>

#define CUDA_ASSERT(EXPRESSION)	{cudaError_t error = (EXPRESSION); if (error != cudaSuccess) FATAL__("CUDA operation '"#EXPRESSION"' failed with error code #"+CString(error)+" '"+CString(cudaGetErrorString(error)));}

#include "../global_string.h"
#include "../io/log_stream.h"

namespace CUDA
{
	namespace Preference
	{
		enum preference_t
		{
			MostMemory=0x1,
			MostProcessors=0x2,
			FastestDevice=0x4,
			
			
			NoPreference=0xFFFFFFFF
		};
	}


	namespace Device
	{
		extern	int				index;
		extern	cudaDeviceProp	current;
		extern	cudaError_t		error_code;
		extern	const char		*error;
		extern	bool			emulation;
		
		bool	initialize(int preference=Preference::NoPreference);
		
		CString	toString();
	}







}



#endif
