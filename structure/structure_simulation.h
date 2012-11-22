#ifndef structure_simulationH
#define structure_simulationH

/******************************************************************

Experimental structure-simulator.

This file is part of Delta-Works
Copyright (C) 2006-2008 Stefan Elsen, University of Trier, Germany.
http://www.delta-works.org/forge/
http://informatik.uni-trier.de/

******************************************************************/


#include "../math/vector.h"
#include "../math/matrix.h"
#include "../math/object.h"
#include "../math/object_def.h"
#include "../math/vclasses.h"
#include "../math/mclasses.h"
#include "../container/lvector.h"
#include "../geometry/cgs.h"



#define SIMULATION_SCALAR               1000
#define AERODYNAMIC_FORCE_SCALAR        1.0
#define WHEEL_FORCE                     1

#define G_CONST                         0.00000000006673
#define C2EXP1_3                        1.2599210498948731647672106072779

#define COLLISION_BUFFER_SIZE           0x10000
#define ELEMENT_COLLISION_BUFFER_SIZE   0x400

#define COLLISION_MAX_LOOP              0x100

/** "Modulate" physical simulation **/




namespace PhysicalSimulation
{

    template <class> class EnvTree;
    template <class> class Accelerator;
    template <class> class Wheel;
    template <class> class StructureSection;
    template <class> class Structure;
    template <class> class PhysicalSpace;

    template <class> class Animator;
    template <class, class, unsigned> struct AnimationTrace;
    template <class, unsigned> struct TAnimationStep;
    template <class> class CV2;

    template <class> struct TEnvVertex;
    template <class> struct TEnvSpace;
    template <class> struct TAtmosphere;

    template <class> struct TGlobalStructState;
    template <class> struct TInertia;
    template <class> struct TQuaternionState;
    template <class> struct TGlobalState;
    template <class> struct TLocalState;

    template <class> struct TContactSequence;
    template <class> struct TContact;
    
    template <class> struct TPhSphere;

    template <class> class TTreeData;




    enum eAnimatorState {AS_INACTIVE, AS_HOLD, AS_ACTIVE, AS_BOUNCE, AS_REVERSE};




    template <class C> struct TContact
    {
            Vec3<C>                root_point,relative_position,velocity;
            C                       impression,grip,mass;

            void                    define(const Vec3<C>&root,const Vec3<C>&relative,const Vec3<C>&velocity,const C&impression,const C&grip,const C&mass);
    };

    template <class Def> struct TContactSequence
    {
            typedef typename Def::PhFloatType   Float;
            Float           radial_weight,planar_weight;
            Structure<Def> *entity;
            unsigned        count;
            TContact<Float> contact[ELEMENT_COLLISION_BUFFER_SIZE];
    };

    template <class C> struct TEnvVertex
    {
            Vec3<C>                position,last_position,normal,movement;
    };

    template <class C> struct TEnvSpace
    {
            Vec3<C>                lower,upper;
    };
    
    template <class C> struct TPhSphere
    {
            Vec3<C>                center;
            C                       radius;
    };



    template <class C, unsigned Band> struct TAnimationStep
    {
            C                       time0,time1,length;
            C                       state0[Band],delta[Band];
    };

    template <class C, class TC, unsigned Band> struct AnimationTrace
    {
    public:
            TC                     *target;
            unsigned                steps;
            TAnimationStep<C,Band> *step;

                                    AnimationTrace();
                                   ~AnimationTrace();
            void                    seek(const C&progress, C out[Band]);
    };

    template <class C> struct TAtmosphere
    {
            C                       density,
                                    range0,half_range;
            bool                    enabled;
    };

    template <class Def> class Animator
    {
    public:
            typedef typename Def::PhFloatType   Float;
    private:
        template <class SourceObject, class DestinationObject, unsigned Band>
            Float                   copyTraces(CGS::TraceA<SourceObject,Band>*source, AnimationTrace<Float,DestinationObject,Band>*destination, unsigned len);

    public:
            CGS::AnimatorA<Def>        *origin;

            __int64                 name;
            Float                   progress,total_length;
            eAnimatorState          state;
            unsigned                otraces,
                                    atraces,
                                    wtraces;
            AnimationTrace<Float,StructureSection<Def>,4> *otrace;
            AnimationTrace<Float,Accelerator<Def>,1>      *atrace;
            AnimationTrace<Float,Wheel<Def>,2>            *wtrace;

                                    Animator();
            virtual                ~Animator();
            void                    apply();
            void                    seek(const Float&now);
            void                    cast(const Float&delta);
            
            void                    readFromOrigin();
            void                    link();
    };


    template <class Def> class Wheel: public CGS::WheelCompatible
    {
    private:
            TContactSequence<Def>   sequence;
            typedef typename Def::PhFloatType   Float;

            void                    beginContactSequence(Structure<Def>*entity);
            void                    handleContact(const Vec3<Float>&position, const Vec3<Float>&root_point, const Vec3<Float>&relative_position, const Vec3<Float>&velocity, const Float&impression, const Float&grip, Structure<Def>*entity);
            void                    finishContactSequence(const Float&time_delta);
    public:

            Vec3<Float>            root,extension_direction,axis_direction; //defined axis_direction to be of length width/2
            Float                   width,radius,suspension_strength,wheel_mass,power,grip,zero_efficiency,suspension_friction,rotation_friction,
                                    elen,edot;
            //that covers all preset values
            bool                    sliding;
            Float                   present_suspension_contraction ,suspension_inertia,present_power,present_brake_state,wheel_inertia,
                                    wheel_state;
            StructureSection<Def> *parent;
            PhysicalSpace<Def>    *world;
            Structure<Def>        *super;
            Animator<Def>          suspension,rotation;


                                    Wheel();
            void                    link(StructureSection<Def>*new_parent);
            void                    link(PhysicalSpace<Def>*new_world);
            void                    setup(const Float&density);
            void                    resetState();
            void                    applyState(); //rotation
            void                    process(const Float&air_density, const Float&time_delta);
    };

    template <class Def> class Accelerator: public CGS::AcceleratorCompatible
    {
    public:
            typedef typename Def::PhFloatType   Float;
            
            Vec3<Float>            root,direction; //non normalized (length does matter)
            Float                   power,zero_efficiency;

            Float                   present_power;
            StructureSection<Def> *parent;
            Structure<Def>         *super;

                                    Accelerator();
            void                    link(StructureSection<Def>*new_parent);
            void                    resetState();
            void                    process(const Float&air_density, const Float&time_delta);
    };

    template <class C> struct TLocalState
    {
            SystemMatrix<C>        out,in,out_path,in_path;
    };

    template <class C> struct TGlobalState
    {
            Vec3<C>                closure_center;
            SystemMatrix<C>        all_out, all_in;
    };
    
    template <class Def> class PhSystemPath
    {
    public:
            typedef typename Def::PhFloatType   Float;
    private:
            SystemMatrix<Float>    current,previous;
    public:
                                    PhSystemPath();
                                    PhSystemPath(StructureSection<Def>*from, StructureSection<Def>*to);
            void                    set(StructureSection<Def>*from, StructureSection<Def>*to);
            void                    transform(const MeshVertex<TPhDef<Float> >*vtx, TEnvVertex<Float>&out);
            void                    transform(MeshVertex<TPhDef<Float> >*inout);
            void                    transformWithoutNormal(const MeshVertex<TPhDef<Float> >*vtx, TEnvVertex<Float>&out);
    };

    template <class Def> class StructureSection:public CGS::SubGeometryCompatible
    {
    public:
            typedef typename Def::PhFloatType   Float;

            typedef TPhDef<Float>   PhDef;
            
            TGlobalState<Float>     global,last_global;                     //update call from 'super->move' and 'super->unmove'  -> 'update' & 'backdate'
            TLocalState<Float>      local,last_local;                       //update call from 'super->processItems' -> 'recursiveProcess'

            __int64                 name;
            StructureSection<Def> *parent,*child;                          //static
            unsigned                children;

            Structure<Def>        *super;                                  //static
            PhysicalSpace<Def>    *world;                                  //static
            bool                    empty;                                  //static

            SystemMatrix<Float>    base_system;                            //static
            Float                   animation_state[4];                     //update call from 'super->processItems'

            Mesh<PhDef>				*hull;                                   //static
            ObjMap<PhDef>			*map;                                    //static
            __int64                 visual_hull;

            TPhSphere<Float>        closure, mass,                          //static
                                    recursive_mass;                         //update call from 'super->processItems' -> 'recursiveProcess'
            Float                   weight,                                 //static
                                    recursive_weight;                       //update call from 'super->processItems' -> 'recursiveProcess'

            TEnvSpace<Float>        dimension;                              //update call from 'super->move' -> 'update'

            unsigned                accelerators,wheels;
            Wheel<Def>            *wheel;
            Accelerator<Def>      *accelerator;

            bool                    added;                                  //used by StructTree to check wether the section was previously added or not

                                    StructureSection();
    virtual                        ~StructureSection();
            void                    link(PhysicalSpace<Def>*world);
            void                    buildSphere();
            
            unsigned                mapChildrenTo(StructureSection<Def>**list);
            unsigned                createFrom(CGS::SubGeometryA<Def>&origin);
            
            void                    resetState(); //reloads zero-vector to animation_state
            void                    recursiveProcess(const Float&time_delta);
            bool                    handleCollision(StructureSection<Def>*other, const Float&time_delta);
            void                    update();
            void                    backdate();

            void                    recursiveFinalize();
    };

    template <class C> struct TGlobalStructState //elements to be overwritten by move (which can thereby be reset by unmove and overwritten many times per frame)
    {
            SystemMatrix<C>        out,in;
    };

    template <class C> struct TInertia
    {
            Vec3<C>                momentum,linear;
    };

    template <class C> struct TRotationState
    {
            C                       rotation_per_second[3*3],rotation_per_frame[3*3];
            bool                    set;
    };

    template <class Def> class Structure: public Signature
    {
    public:
            typedef typename Def::PhFloatType   Float;

            TGlobalStructState<Float>   global,last_global;         //update call from 'move' and 'unmove' - DONE

            TPhSphere<Float>        mass,last_mass;             //update call from 'processItems' - last mass important for last in and out
            Vec3<Float>            global_center;              //update call from 'processItems'

            TInertia<Float>         inertia, finalized_inertia; //update call from 'postForce'
            TRotationState<Float>   finalized;
            Float                   total_weight;               //we assume total weight does not change, thus: static
            TAtmosphere<Float>      atmosphere;                 //sort of static
            StructureSection<Def> **linear_list,               //sections in linear list (all client-sections) static
                                   *section;                    //static
            unsigned                sections,total_sections;    //static
            PhysicalSpace<Def>    *world;                      //static
            __int64                 name;

            unsigned                animators;
            Animator<Def>         *animator;                  //animators are supposed to be sorted by name (IMPORTANT)

            bool                    handled,fixed;              //handled is unset by 'move'. fixed defines wether the object can be moved or not

                                    Structure();
            void                    postForce(const Float b[3], const Float d[3]);
            void                    postForce(const Vec3<Float>&b, const Vec3<Float>&d); //from global space
            void                    postForce(const Line<Float>&f);
            Float                   getAffectedMass(const Vec3<Float>&b, const Vec3<Float>&d);
            Float                   getAffectedMass(const Line<Float>&line);
            Vec3<Float>            getPointMovement(const Vec3<Float>&position);
            Float                   getDensity(const Vec3<Float>&position);
            void                    processItems(const Float&time_delta);
            void                    move(const Float&time_delta);
            void                    unmove();

            void                    setAnimator(__int64 name, eAnimatorState state);
            void                    initialize();
            void                    clear();
            void                    finalize();
    };

    template <class Def> class TTreeData:public List::ReferenceVector<StructureSection<Def> > {};


    template <class Def> class StructTree:public TTreeData<Def>
    {
    public:
            typedef TTreeData<Def>  Super;
            USE_LIST
            typedef typename Def::PhFloatType   Float;
    private:

    static  StructureSection<Def>* sector_map[GLOBAL_SECTOR_MAP_SIZE];
    static  unsigned                sector_count;

            void                    recursiveRemap(TTreeData<Def>*source);
            unsigned                recursiveLookup(const TEnvSpace<Float>&space, const Structure<Def>*exclude) const;
    public:
            TEnvSpace<Float>        dim;
            StructTree<Def>			*child[8];
            Structure<Def>			*dominating;
            unsigned                level;

                                    StructTree(const TEnvSpace<Float>&space, unsigned level);
                                    StructTree(const StructTree<Def>&other);
                                    StructTree(unsigned depth);
    virtual                        ~StructTree();
            void                    remap(List::Vector<Structure<Def> >*source);
            void                    unmap(Structure<Def>*source);
            void                    unmap(StructureSection<Def>*source);
            void                    map(StructureSection<Def>*source);
            unsigned                lookup(const TEnvSpace<Float>&space, StructureSection<Def>**out, const Structure<Def>*exclude=NULL, unsigned output_offset=0) const;  //output_offset specifies the first entry in the buffer to write to
            void                    operator=(const StructTree<Def>&other);
    };

    template <class Def> class PhysicalSpace
    {
    public:
            typedef typename Def::PhFloatType   Float;

            StructureSection<Def>     *collision_buffer[COLLISION_BUFFER_SIZE];

            List::Vector<Structure<Def> >   static_list,
											dynamic_list;
            StructTree<Def>					static_tree,dynamic_tree;
            Vec3<Float>                global_wind;
            bool                        dynamic_effects;

                                        PhysicalSpace();
            void                        simulate(const Float&time_delta);
            Float                       getAirDensity(const Vec3<Float>&position, Structure<Def>*exclude = NULL);
            Vec3<Float>                getGravity(const Vec3<Float>&position, const Float&mass, Structure<Def>*exclude = NULL);
            Float                       getAirMovement(const Vec3<Float>&position, Vec3<Float>&out, Structure<Def>*exclude = NULL);   //returns density (includes wind-movement)

            Structure<Def>*            embed(CGS::Geometry<Def>*source, bool dynamic=true);
    };
}

//---------------------------------------------------------------------------
#include "structure_simulation.tpl.h"

using namespace PhysicalSimulation;

#endif



/*  *


basic environmental functions (air-density, air-movement (including density and global wind), gravity) up & running

changed basic system to only allow structures (however distinguished between dynamic (moving) and static structures)
they are mapped to different trees (which may be shit but got no fuggin other chance)

variable 'dynamic_effects' allows to trigger wether of not to include dynamic objects to the above functions.







*/
