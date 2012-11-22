#include "../global_root.h"
#include "root.h"

#include <iostream>
using namespace std;

namespace CUDA
{
	namespace Device
	{
		cudaDeviceProp	current;
		
		int				index=-1;
		cudaError_t		error_code=cudaSuccess;
		const char		*error="";
		
		bool			emulation=true;
		
		
		
		
		bool			initialize(int preference)
		{
			index = -1;
			float		score = 0;
		
			int			devices = 0;
			error_code = cudaGetDeviceCount(&devices);
			if (error_code != cudaSuccess)
			{
				error = cudaGetErrorString(error_code);
				lout << error << nl;
				emulation = true;
				return false;
			}
			if (!devices)
			{
				lout << "No CUDA enabled devices available."<<nl;
				error = "No devices available.";
				emulation = true;
				return false;
			}
			lout << "Attempting to find suitable device"<<nl;
			lout <<" for preference "<<preference<<nl;
			lout <<" among "<<devices<<" device(s)."<<nl;
			for (int i = 0; i < devices; i++)
			{
				cudaDeviceProp	dev;
				if (cudaGetDeviceProperties(&dev,i) != cudaSuccess)
					continue;
				
				float this_score = 1;
				if (preference&Preference::MostMemory)
					this_score *= dev.totalGlobalMem;
				/*if (preference&Preference::MostProcessors)...???
					this_score *= dev.maxThreadsPerBlock;*/
				if (preference&Preference::FastestDevice)
					this_score *= dev.clockRate;
				
				if (this_score > score)
				{
					score = this_score;
					index = i;
				}
			}
			if (index == -1)
			{
				lout << "Unable to find a suitable device."<<nl;
				error = "Unable to find a suitable device.";
				emulation = true;
				return false;
			}
			lout << "setting device "<<index<<nl;
			
			cudaSetDevice(index);
			cudaGetDeviceProperties(&current,index);
			
			lout << "device is:"<<nl;
			
			lout << " name: "<<current.name<<nl;
			lout << " totalGlobalMem: "<<current.totalGlobalMem<<nl;
			lout << " sharedMemPerBlock: "<<current.sharedMemPerBlock<<nl;
			lout << " regsPerBlock: "<<current.regsPerBlock<<nl;
			lout << " warpSize: "<<current.warpSize<<nl;
			lout << " memPitch: "<<current.memPitch<<nl;
			lout << " maxThreadsPerBlock: "<<current.maxThreadsPerBlock<<nl;
			lout << " totalConstMem: "<<current.totalConstMem<<nl;
			lout << " major: "<<current.major<<nl;
			lout << " minor: "<<current.minor<<nl;
			lout << " clockRate: "<<current.clockRate<<nl;
			lout << " textureAlignment: "<<current.textureAlignment<<nl;
			lout << nl;
			emulation = false;
			
			return true;
		}
		
		CString	toString()
		{
			return	" name: "+CString(current.name)+"\n"
					" totalGlobalMem: "+CString(current.totalGlobalMem)+"\n"
					" sharedMemPerBlock: "+CString(current.sharedMemPerBlock)+"\n"
					" regsPerBlock: "+CString(current.regsPerBlock)+"\n"
					" warpSize: "+CString(current.warpSize)+"\n"
					" memPitch: "+CString(current.memPitch)+"\n"
					" maxThreadsPerBlock: "+CString(current.maxThreadsPerBlock)+"\n"
					" totalConstMem: "+CString(current.totalConstMem)+"\n"
					" major: "+CString(current.major)+"\n"
					" minor: "+CString(current.minor)+"\n"
					" clockRate: "+CString(current.clockRate)+"\n"
					" textureAlignment: "+CString(current.textureAlignment);
		}
	}
}

