#ifndef COOKING_H

#define COOKING_H

#define NOMINMAX
#include "../global_root.h"

#if SYSTEM==WINDOWS
	#define WIN32
#endif

#include <NxCooking.h>

#ifndef _M_X64
	#pragma comment(lib,"PhysXCooking.lib")
	#pragma comment(lib,"PhysXCore.lib")
#else
	#pragma comment(lib,"PhysXCooking64.lib")
	#pragma comment(lib,"PhysXCore64.lib")
#endif


class NxPMap;
class NxTriangleMesh;
class NxUserOutputStream;

bool hasCookingLibrary(); // check to see if the cooking library is available or not!
bool InitCooking(NxUserAllocator* allocator = NULL, NxUserOutputStream* outputStream = NULL);
void CloseCooking();
bool CookConvexMesh(const NxConvexMeshDesc& desc, NxStream& stream);
bool CookClothMesh(const NxClothMeshDesc& desc, NxStream& stream);
bool CookTriangleMesh(const NxTriangleMeshDesc& desc, NxStream& stream);
bool CookSoftBodyMesh(const NxSoftBodyMeshDesc& desc, NxStream& stream);
bool CreatePMap(NxPMap& pmap, const NxTriangleMesh& mesh, NxU32 density, NxUserOutputStream* outputStream = NULL);
bool ReleasePMap(NxPMap& pmap);


#endif
