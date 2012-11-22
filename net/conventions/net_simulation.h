#ifndef conv_simulationH
#define conv_simulationH

/*************************************************************************************

This file is part of the DeltaWorks-Foundation or an immediate derivative core module.
Original Author: Stefan Elsen
Copyright (C) 2006 Computer Science Department of the Univerity of Trier, Germany.

This file is part of Delta-Works
Copyright (C) 2006-2008 Stefan Elsen, University of Trier, Germany.
http://www.delta-works.org/forge/
http://informatik.uni-trier.de/

*************************************************************************************/

#include "../structure/fractal_map.h"

#include "net_connection.h"


#define CH_POSITION             CTS (CH_MODULE+0x0)
#define CH_SET_POSITION         STC (CH_MODULE+0x0)
#define CH_POSITION_FRAME       STC (CH_MODULE+0x1)
#define CH_NEW_PLANET           STC (CH_MODULE+0x2)
#define CH_UPDATE_PLANET_STATE  STC (CH_MODULE+0x3)
#define CH_ERASE_PLANET         STC (CH_MODULE+0x4)
#define CH_ARMAGEDDON           STC (CH_MODULE+0x5)
#define CH_CREATE_CHILDREN      STC (CH_MODULE+0x6)
#define CH_SYNC                     (CH_MODULE+0x7)
#define CH_NOTIFY               STC (CH_MODULE+0x8)


// landscape-flags

#define LF_WATER_SURFACE        0x1
#define LF_CLOUDS               0x2
#define LF_TREE_LAYER           0x4
#define LF_TREE_OBJECTS         0x8


#define LF_NATIVE               (LF_WATER_SURFACE|LF_CLOUDS|LF_TREE_LAYER|LF_TREE_OBJECTS)


enum eNetNotification{NET_OUT_OF_SYNC};

#define     NUM_PLANET_TYPES    2
enum ePlanetType{PT_GREEN,PT_MOON,PT_BARREN,PT_GAS,PT_ICE};

char*planet_type_name[]={"native","moon","barren","gas","ice"};
float planet_atmosphere[] = {0.7,0.75,1.0,1.0,  0,0,0,0,  1,0.8,0.4,0.5,  0.8,0.8,1.0,0.8, 0,0,1.0,0.6};
bool planet_has_water[] = {true, false, false, false, false};

struct TGlobalAddress;      //used to specify the exact location of a fractal face in global space
struct TFocusCarrier;       //used to notify a client about new children of a specific face
struct TObjectOrientation;  //orientation of an object in R3. Also contains a momentum to specify the rotation.


/* ALIGN1 is used throughout the definition-section but in the end it'll be ALIGN4 in all cases (only 4-byte-blocks used).
ALIGN1 is merely to ensure that gcc doesn't mess up.
*/

typedef float   NETFLOAT ALIGN1;
typedef __int64 NETTIME ALIGN1;
typedef UINT32  NETUINT ALIGN1;
typedef char                        NETCHAR ALIGN1;







struct TObjectPosition
{
        NETFLOAT    location[3],inertia[3];
} ALIGN1;

struct TObjectOrientation
{
        NETFLOAT        x_axis[3],y_axis[3],momentum[3];
};

struct TTimedPosition:public TObjectPosition
{
        NETTIME     time;
} ALIGN1;

struct TTimedOrientation:public TObjectPosition, public TObjectOrientation
{
        NETTIME     time;
} ALIGN1;

struct TPlanetBoundary
{
        NETFLOAT    height,appearance,fertility;
} ALIGN1;

struct TCloudConfiguration
{
        NETFLOAT    min_height,max_height,density;
} ALIGN1;

struct TLandscapeConfiguration
{
        typedef TTimedOrientation   Orientation ALIGN1;
        typedef TPlanetBoundary     Boundary ALIGN1;
        typedef TCloudConfiguration CloudConfig ALIGN1;

        NETCHAR     name[256];
        Orientation status;
        Boundary    lower,upper;
        CloudConfig clouds;
        NETUINT     planet_type,planet_depth,flags,reserved;
} ALIGN1;


#endif
