#ifndef structure_simulationTplH
#define structure_simulationTplH

/******************************************************************

Experimental structure-simulator.

This file is part of Delta-Works
Copyright (C) 2006-2008 Stefan Elsen, University of Trier, Germany.
http://www.delta-works.org/forge/
http://informatik.uni-trier.de/

******************************************************************/





namespace PhysicalSimulation
{

    template <class C> void TContact<C>::define(const Vec3<C>&root,const Vec3<C>&relative,const Vec3<C>&velocity,const C&impression,const C&grip,const C&mass)
    {
        root_point = root;
        relative_position = relative;
        this->velocity = velocity;
        this->impression = impression;
        this->grip = grip;
        this->mass = mass;
    }


    template <class C, class TC, unsigned Band> AnimationTrace<C,TC,Band>::AnimationTrace():target(NULL),step(NULL),steps(0)
    {}

    template <class C, class TC, unsigned Band> AnimationTrace<C,TC,Band>::~AnimationTrace()
    {
        dealloc(step);
    }


    template <class C, class TC, unsigned Band> void AnimationTrace<C,TC,Band>::seek(const C&progress, C out[Band])
    {
            if (progress > step[steps-1].time1)
            {
                _addV<C,C,C,Band>(step[steps-1].state0,step[steps-1].delta,out);
                return;
            }
            unsigned lower = 0,
                     upper = steps-1;
            while (lower<=upper && upper < steps)
            {
                unsigned el = (lower+upper)/2;
                if (step[el].time0 > progress)
                    upper = el;
                else
                    if (step[el].time1 < progress)
                        lower = el+1;
                    else
                    {
                        C scalar = (progress - step[el].time0) / (step[el].length);
                        _madV<C,C,C,C,Band>(step[el].state0,step[el].delta,scalar,out);
                        return;
                    }
            }
            FATAL__("step not found!");
            //*smoooth~*
    }

    template <class Def> void Animator<Def>::apply()
    {
        Float   res[4];
        for (unsigned i = 0; i < otraces; i++)
        {
            otrace[i].seek(progress,res);
            _add4(otrace[i].target->animation_state,res);
        }
        for (unsigned i = 0; i < atraces; i++)
        {
            atrace[i].seek(progress,res);
            atrace[i].target->present_power += res[0];
        }
        for (unsigned i = 0; i < wtraces; i++)
        {
            wtrace[i].seek(progress,res);
            wtrace[i].target->present_power += res[0];
            wtrace[i].target->present_brake_state += res[1];
        }
    }

    template <class Def> Animator<Def>::Animator():otraces(0),atraces(0),wtraces(0),name(0),origin(NULL),
                                                   otrace(NULL),atrace(NULL),wtrace(NULL),state(AS_INACTIVE),progress(0),total_length(1)
    {}

    template <class Def> Animator<Def>::~Animator()
    {
        dealloc(otrace);
        dealloc(atrace);
        dealloc(wtrace);
    }


    template <class Def>  void Animator<Def>::seek(const Float&now)
    {
        progress = now;
        apply();
    }

    template <class Def>  void Animator<Def>::cast(const Float&delta)
    {
        switch (state)
        {
            case AS_INACTIVE:
                progress = 0;
            break;
            case AS_ACTIVE:
                progress += delta/total_length;
                if (progress > 1)
                {
                    progress = 1;
                    state = AS_HOLD;
                }
            case AS_HOLD:
                apply();
            break;
            case AS_REVERSE:
                progress -= delta/total_length;
                if (progress < 0)
                {
                    progress = 0;
                    state = AS_INACTIVE;
                    return;
                }
                apply();
            break;
            case AS_BOUNCE:
                progress += delta/total_length;
                if (progress > 1)
                {
                    progress = 2-progress;
                    state = AS_REVERSE;
                }
                apply();
            break;
        }
    }
    
    
    template <class Def>
    template <class SourceObject, class DestinationObject, unsigned Band>
    typename Def::PhFloatType Animator<Def>::copyTraces(CGS::TraceA<SourceObject,Band>*source, AnimationTrace<Float,DestinationObject,Band>*destination, unsigned len)
    {
        Float vec[Band];
        _clearV<Float,Band>(vec);
        for (unsigned i = 0; i < len; i++)
        {
            destination[i].target = (DestinationObject*)source[i].target->reference;
            reloc(destination[i].step,destination[i].steps,source[i].steps);
            for (unsigned j = 0; j < destination[i].steps; j++)
            {
                destination[i].step[j].time0 = source[i].step[j].start;
                destination[i].step[j].time1 = source[i].step[j].end;
                destination[i].step[j].length = destination[i].step[j].time1 - destination[i].step[j].time0;
                _copyV<Float,Float,Band>(vec,destination[i].step[j].state0);
                _addV<Float,Float,Band>(vec,source[i].step[j].range);
                _copyV<Float,Float,Band>(source[i].step[j].range,destination[i].step[j].delta);
            }
        }
    }
    
    template <class Def> void Animator<Def>::readFromOrigin()
    {
        if (!origin)
            return;
        reloc(otrace,otraces,origin->obj_traces);
        reloc(atrace,atraces,origin->acc_traces);
        reloc(wtrace,wtraces,origin->whl_traces);
        total_length =                   copyTraces(origin->obj_trace,otrace,otraces);
        total_length = vmax(total_length,copyTraces(origin->whl_trace,wtrace,wtraces));
        total_length = vmax(total_length,copyTraces(origin->acc_trace,atrace,atraces));
        progress = 0;
        state = AS_INACTIVE;
        name = origin->name;
    }

    

    template <class Def> Wheel<Def>::Wheel():parent(NULL),world(NULL),super(NULL),sliding(false),present_suspension_contraction(0),suspension_inertia(0),present_power(0),
                                             present_brake_state(0),wheel_inertia(0),wheel_state(0),wheel_mass(0)
    {}


    template <class Def> void Wheel<Def>::setup(const Float&density)
    {
        edot = extension_direction * extension_direction;
        elen = extension_direction.length();
        wheel_mass = M_PI*radius*radius*width*density;


        sliding = false;
        present_suspension_contraction = 0;
        suspension_inertia = 0;
        present_power = 0;
        present_brake_state = 0;
        wheel_inertia = 0;
        wheel_state = 0;
    }

    template <class Def> void Wheel<Def>::link(StructureSection<Def>*new_parent)
    {
        parent = new_parent;
        super = parent->super;
    }

    template <class Def> void Wheel<Def>::link(PhysicalSpace<Def>*new_world)
    {
        world = new_world;
    }


    template <class Def> void Wheel<Def>::resetState()
    {
        present_power = 0;
        present_brake_state = 0;
    }

    template <class Def> void Wheel<Def>::applyState()
    {
        wheel_state -= (int)wheel_state;
        if (wheel_state < 0)
            wheel_state ++;
        suspension.seek(1-present_suspension_contraction);
        rotation.seek(wheel_state);
    }

    template <class Def> void  Wheel<Def>::beginContactSequence(Structure<Def>*entity)
    {
        sequence.radial_weight = 0;
        sequence.planar_weight = 0;
        sequence.entity = entity;
        sequence.count = 0;
    }

    template <class Def> void Wheel<Def>::finishContactSequence(const Float&time_delta)
    {
        bool    new_slide = false;
        Float   rot_mod(0);
        for (unsigned i = 0; i < sequence.count; i++)
        {
            Float   radial_weight = sequence.contact[i].impression / sequence.radial_weight;
            Vec3<Float> rb = sequence.contact[i].relative_position.normalized() * radius,
                     b = rb + sequence.contact[i].root_point;
            Line<Float> force(b, (sequence.contact[i].relative_position - rb) * WHEEL_FORCE*radius*radial_weight),
                       global_force = parent->global.all_out * force;
            sequence.entity->postForce(-global_force);
            Float   alpha = - (force.direction * extension_direction) / edot;
            Vec3<Float> pf = force.direction + extension_direction*alpha;
            Float        down_force = alpha * elen;
            global_force.direction = parent->global.all_out * pf;
            super->postForce(global_force);
            Float       down_accel = down_force / wheel_mass;
            suspension_inertia += down_accel * time_delta; // / wheel_mass;


            Float   planar_weight = sequence.contact[i].grip * sequence.contact[i].mass / sequence.planar_weight;
            Vec3<Float> v = sequence.contact[i].velocity * radius / sequence.contact[i].relative_position.length();

            Float   matrix[9],tmatrix[9],scalar[3],sv[3];
            _divide(rb.v,radius,matrix);
            _divide(axis_direction.v,elen,&matrix[3]);
            _cross(matrix,&matrix[3],&matrix[6]);
            __mult331(tmatrix,v.v,scalar);

            _multiply(&matrix[3],scalar[1],sv);
            Line<Float>  side_force;
            __transform(parent->global.all_out.system,sv,side_force.direction.v);
            side_force.position = global_force.position;
            Float   max_force = force.direction.length() * sequence.contact[i].grip;
            if (sliding)
                max_force *= 0.75;
            if (!sequence.entity->fixed)
            {
                Float   m0 = super->getAffectedMass(side_force),
                        m1 = sequence.entity->getAffectedMass(side_force);
                side_force.direction *= m0*m1/(m0+m1) * planar_weight;
            }
            else
                side_force.direction *= super->getAffectedMass(side_force) * planar_weight;
            Float   flen = side_force.direction.length();
            if (flen > max_force)
            {
                side_force *= max_force / flen;
                new_slide = true;
            }
            super->postForce(side_force);
            sequence.entity->postForce(-side_force);

            Float   acceleration = (scalar[2] - wheel_inertia) * planar_weight,
                    faccel = acceleration * 0.5 * wheel_mass * radius;
            if (faccel > max_force)
            {
                faccel = max_force;
                new_slide = true;
            }
            rot_mod += faccel * 2 / (wheel_mass*radius);
            Vec3<Float>    v_accel;
            _mult(&matrix[6],faccel,v_accel.v);
            Line<Float>  radial_force(global_force.position,parent->global.all_out*v_accel);
            super->postForce(radial_force);
            sequence.entity->postForce(-radial_force);
        }
        sliding = new_slide;
        wheel_inertia += rot_mod;
    }

    template <class Def> void    Wheel<Def>::handleContact(const Vec3<Float>&position, const Vec3<Float>&root_point, const Vec3<Float>&relative_position, const Vec3<Float>&velocity, const Float&impression, const Float&grip, Structure<Def>*entity)
    {
        sequence.radial_weight += impression;
        Float   mass = entity->getAffectedMass(parent->global.all_out*position,parent->global.all_out*velocity);
        sequence.planar_weight += grip*mass;
        sequence.contact[sequence.count++].define(root_point,relative_position,velocity,impression,grip,mass);
        if (sequence.count >= ELEMENT_COLLISION_BUFFER_SIZE)
            FATAL__("contact-sequence boundaries exceeded");
    }

    template <class Def> void   Wheel<Def>::process(const Float&air_density, const Float&time_delta)
    {
        /*  specification *

            given:

                root:               position of wheel-bumper-root in local parent-system
                extension_direction:    direction, the bumper does extract to
                axis_direction:     defined axis_direction to be of length half_width/2


                width:              wheel's thickness
                radius:             wheel-radius
                bumper_strength:    strength of bumper-spring (the higher, the stronger), bumper_strength is multiplied with bumper_contraction and bumper_length
                wheel_weight:       generated value
                power:              indicates maximum acceleration
                grip:               grip value defines the wheel's grip on other structures
                air_consumption:    0 .. 1: the higher the less efficient the wheel will operate in zero-space
                bumper_friction:    friction value indicates ease of bumper-extension/contraction. lesser values may cause the bumper to oscillate
                rotation_friction:  friction value indicates the overall slowdown of the wheel (other than airfriction rotation will slowdown even in zerospace)

            indoor = within local system
            outdoor = in global world system

            wheel::process checks the global environment-tree for possible contacts and
            creates an item of type Contact for each one it finds.
            ContactList only adds one contact per touching structure and
            alters already existing contacts of the given structure.
            This alteration is processed by replacing the existing contact's relative position
            by the new if its distance is smaller while its movement is generalized (arithmetic average using grip-weight).

            each contact now provides:
                scalar distance
                distance vector (indoor) (from wheel-center-axis directing towards contact)
                movement (indoor)

            grip:
                applied from faces to edges down to vertices.
        */

        /* initialization */

        TEnvSpace<Float> space; //space for information-retrieval
        Vec3<Float> center,abs_center,abs_direction,outer_center[2];
            //center is the local wheel-center (depending on present bumper_contraction)
            //abs_center will be center transfered to global space
            //outer_center will represent the two points where the axis leaves the wheel (at +/- width) in global space
        center = root + extension_direction * (1-present_suspension_contraction); //get the final position of the wheel in inner space

        abs_center = parent->global.all_out * center;
        abs_direction = parent->global.all_out * axis_direction;

        outer_center[0] = center + axis_direction; //these are outer center #0
        outer_center[1] = center - axis_direction; //and outer center #1

        space.lower = abs_center - (radius + width/2);
        space.upper = abs_center + (radius + width/2);
        unsigned sections   = world->dynamic_tree.lookup(space,world->collision_buffer,NULL,world->static_tree.lookup(space,world->collision_buffer));

        Float   bdot = axis_direction * axis_direction, cdot = axis_direction * center,
                odot[2] = {
                        outer_center[0] * axis_direction,
                        outer_center[1] * axis_direction
                      }; //same here
        unsigned index = 0;
        MeshEdge<TPhDef<Float> >*e_buffer[ELEMENT_COLLISION_BUFFER_SIZE];
        MeshTriangle<TPhDef<Float> >*t_buffer = (MeshTriangle<TPhDef<Float> >*)e_buffer;
        MeshQuad<TPhDef<Float> >*q_buffer = (MeshQuad<TPhDef<Float> >*)e_buffer;
        MeshVertex<TPhDef<Float> >*v_buffer = (MeshVertex<TPhDef<Float> >*)e_buffer;

        Float   efficiency = air_density*(1-zero_efficiency)+zero_efficiency;
        wheel_inertia += 2 * efficiency * present_power * power * time_delta / (wheel_mass * radius);
        wheel_inertia *= pow(1-present_brake_state,time_delta);

        for (unsigned j = 0; j < sections; j++)
            if (world->collision_buffer[j]->super != super) //no self-collision allowed
            {
                unsigned v_count,e_count,t_count,q_count;
                StructureSection<Def>*entity = world->collision_buffer[j];


                Vec3<Float>    local_center = entity->global.all_in * abs_center,
                                local_direction = entity->global.all_in * abs_direction;

                TEnvSpace<Float> lspace;
                lspace.lower = local_center - (radius + width/2);
                lspace.upper = lspace.lower + 2*(radius + width/2);
                entity->map->lookup(lspace.lower.v,lspace.upper.v);
                v_count = entity->map->resolveVertices(v_buffer,ELEMENT_COLLISION_BUFFER_SIZE);
                Structure<Def>*e_super = entity->super;

                beginContactSequence(e_super);
                
                PhSystemPath<Def>  path(entity,parent);

                for (unsigned i = 0; i < v_count; i++) //all vertices
                {
                    TEnvVertex<Float>  local;
                    path.transform(v_buffer[i],local);
                    Float   alpha = (local.position * axis_direction - cdot)/bdot;
                    if (alpha < -1 ||alpha > 1)
                        continue;
                    root = (center + axis_direction * alpha);
                    Vec3<Float> p = local.position - root; // where p is the relative contact-position pointing away from the wheel's axis
                    Float dist = p.length();
                    if (dist <= radius && p * local.normal < 0) // out-axis and normal must oppose each other for the contact to be valid
                        handleContact(local.position,root,p,local.movement/time_delta,radius-dist,v_buffer[i]->grip*grip,e_super);
                }
                e_count = entity->map->resolveEdges(e_buffer,ELEMENT_COLLISION_BUFFER_SIZE);
				
                for (unsigned i = 0; i < e_count; i++) //edges
                {
                    TEnvVertex<Float>  local[2];
                    for (BYTE k = 0; k < 2; k++)
                        path.transform(e_buffer[i]->vertex[k],local[k]);

                    Vec3<Float>    e = local[1].position - local[0].position,
                                    normal = e | (outer_center[1] - outer_center[0]),
                                    n0 = normal | (outer_center[1] - outer_center[0]),
                                    n1 = normal | e;
                    Float   alpha = ((local[0].position * n1) - (outer_center[0] * n1)) / ((axis_direction * 2) * n1),
                            beta  = ((outer_center[0] * n0) - (local[0].position * n0)) / (e * n1);
                    if (alpha >= 0 && alpha <= 1 && beta >= 0 && beta <= 1)
                    {
                        Vec3<Float>    p0 = outer_center[0] + axis_direction * 2*alpha,
                                        p1 = local[0].position + e * beta,
                                        normal,m,
                                        p = p1-p0;
                        _interpolate(local[0].normal.v,local[1].normal.v,beta,normal.v);
                        _interpolate(local[0].movement.v,local[1].movement.v,beta,m.v);
                        if (_distance(p0.v,p1.v) < radius && p * normal < 0)
                            handleContact(p1,p0,p,m/time_delta,radius - p.length(),e_buffer[i]->grip*grip,e_super);
                    }
                    for (BYTE k = 0; k < 2; k++)
                    {
                        alpha = (odot[k] - local[0].position * axis_direction) / (e * axis_direction);
                        if (alpha > 1 || alpha < 0)
                            continue;
                        Vec3<Float>    local_position = (local[0].position + e * alpha),
                                        p = local_position - outer_center[k],
                                        m,normal;
                        _interpolate(local[0].movement.v,local[1].movement.v,alpha,m.v);
                        _interpolate(local[0].normal.v,local[1].normal.v,alpha,normal.v);
                        Float   dist = p.length();
                        if (dist <= radius && p * normal < 0)
                            handleContact(local_position,outer_center[k],p,m/time_delta,radius-dist,e_buffer[i]->grip*grip,e_super);
                    }
                }
                t_count = entity->map->resolveTriangles(t_buffer,ELEMENT_COLLISION_BUFFER_SIZE);
				
                for (unsigned i = 0; i < t_count; i++) //and faces
                {
                    TEnvVertex<Float>  local[3];
                    for (BYTE k = 0; k < 3; k++)
                        path.transformWithoutNormal(t_buffer[i]->vertex[k],local[k]);

                    Vec3<Float>    e0 = local[1].position - local[0].position,
                                    e1 = local[2].position - local[0].position,

                                    normal = e0 | e1,
                                    temp = normal | axis_direction,
                                    vector = temp | axis_direction,

                                    n0 = normal | e0,
                                    n1 = normal | e1;

                    Float           d0 = local[0].position * normal,
                                    d1 = vector * normal;

                    for (BYTE k = 0; k < 2; k++)
                    {
                        Float   alpha = (d0 - outer_center[k] * normal) / d1;
                        Vec3<Float> p = outer_center[k] + vector * alpha;
                        Float   dist = _distance(p.v,outer_center[k].v);
                        if (dist > radius)
                            continue;

                        Vec3<Float> dif = p - local[0].position;
                        Float   beta  = (n0 * dif) / (n0 * e1),
                                gamma = (n1 * dif) / (n1 * e0);
                        if (beta < 0 || gamma < 0 || beta+gamma > 1)
                            continue;

                        Vec3<Float>    m0 = local[1].movement - local[0].movement,
                                        m1 = local[2].movement - local[0].movement,
                                        m =  local[0].movement + m0 * beta + m1 * gamma,
                                        relative_position = p - outer_center[k];
                        p[3] = 1;
                        if (p * normal < 0)
                            handleContact(p,outer_center[k],relative_position,m/time_delta,radius-dist,t_buffer[i]->grip*grip,e_super);
                    }
                }
				
				/*q_count = entity->map->resolveQuads(q_buffer,ELEMENT_COLLISION_BUFFER_SIZE);
				
                for (unsigned i = 0; i < q_count; i++) //and faces
                {
                    TEnvVertex<Float>  local[4];
                    for (BYTE k = 0; k < 4; k++)
                        path.transformWithoutNormal(q_buffer[i]->vertex[k],local[k]);

                    Vec3<Float>    e0 = local[1].position - local[0].position,
                                    e1 = local[2].position - local[0].position,

                                    normal = e0 | e1,
                                    temp = normal | axis_direction,
                                    vector = temp | axis_direction,

                                    n0 = normal | e0,
                                    n1 = normal | e1;

                    Float           d0 = local[0].position * normal,
                                    d1 = vector * normal;

                    for (BYTE k = 0; k < 2; k++)
                    {
                        Float   alpha = (d0 - outer_center[k] * normal) / d1;
                        Vec3<Float> p = outer_center[k] + vector * alpha;
                        Float   dist = _distance(p.v,outer_center[k].v);
                        if (dist > radius)
                            continue;

                        Vec3<Float> dif = p - local[0].position;
                        Float   beta  = (n0 * dif) / (n0 * e1),
                                gamma = (n1 * dif) / (n1 * e0);
                        if (beta < 0 || gamma < 0 || beta+gamma > 1)
                            continue;

                        Vec3<Float>    m0 = local[1].movement - local[0].movement,
                                        m1 = local[2].movement - local[0].movement,
                                        m =  local[0].movement + m0 * beta + m1 * gamma,
                                        relative_position = p - outer_center[k];
                        p[3] = 1;
                        if (p * normal < 0)
                            handleContact(p,outer_center[k],relative_position,m/time_delta,radius-dist,t_buffer[i]->grip*grip,e_super);
                    }
                }*/
                finishContactSequence(time_delta);
            }
        suspension_inertia += present_suspension_contraction * elen * suspension_strength * time_delta/ wheel_mass;
        wheel_inertia *= pow(1-rotation_friction,time_delta);
        suspension_inertia *= pow(1-suspension_friction,time_delta);
        wheel_state += wheel_inertia * time_delta;
        present_suspension_contraction += suspension_inertia * time_delta / elen;
        if (present_suspension_contraction > 1)
        {
            present_suspension_contraction = 1;
            suspension_inertia = 0;
        }
        if (present_suspension_contraction < 0)
        {
            present_suspension_contraction = 0;
            suspension_inertia = 0;
        }
        Line<Float>  force(root,extension_direction * -present_suspension_contraction * elen * suspension_strength);
        super->postForce(parent->global.all_out*force);
    }


    template <class Def> Accelerator<Def>::Accelerator():power(0),zero_efficiency(0),present_power(0),
                                        parent(NULL),super(NULL)
    {}

    template <class Def> void Accelerator<Def>::link(StructureSection<Def>*new_parent)
    {
        parent = new_parent;
        super = parent->super;
    }

    template <class Def> void  Accelerator<Def>::resetState()
    {
        present_power = 0;
    }

    template <class Def> void  Accelerator<Def>::process(const Float&air_density, const Float&time_delta)
    {
        Float efficiency = air_density*(1-zero_efficiency)+zero_efficiency;
        super->postForce(parent->global.all_out * root,parent->global.all_out * (direction * (efficiency*present_power*power)));
    }

    template <class Def> StructureSection<Def>::StructureSection():parent(NULL),child(NULL),children(0),super(NULL),accelerators(0),wheels(0),
                                                                   wheel(NULL),accelerator(NULL),weight(0),recursive_weight(0),hull(NULL),map(NULL),added(false)
    {
        _clear(recursive_mass.center.v);
        recursive_mass.radius = 0;
        _clear(closure.center.v);
        closure.radius = 0;
        _clear(mass.center.v);
        mass.radius = 0;
        dimension.upper = (Float)0;
        dimension.lower = (Float)0;
        _clear4(animation_state);
        base_system.loadIdentity();
        local.out.loadIdentity();
        local.in.loadIdentity();
        local.out_path.loadIdentity();
        local.in_path.loadIdentity();
        last_local = local;
        global.closure_center = (Float)0;
        global.all_out.loadIdentity();
        global.all_in.loadIdentity();
        last_global = global;
    }

    template <class Def> StructureSection<Def>::~StructureSection()
    {
        dealloc(wheel);
        dealloc(accelerator);
        dealloc(child);
    }

    template <class Def> unsigned StructureSection<Def>::mapChildrenTo(StructureSection<Def>**list)
    {
        StructureSection<Def>**inner = list;
        for (unsigned i = 0; i < children; i++)
        {
            (*inner++) = child+i;
            inner += child[i].mapChildrenTo(inner);
        }
        return inner-list;
    }


    template <class Def> PhSystemPath<Def>::PhSystemPath()
    {}
    
    template <class Def> PhSystemPath<Def>::PhSystemPath(StructureSection<Def>*from, StructureSection<Def>*to)
    {
        set(from,to);
    }
    
    template <class Def> void PhSystemPath<Def>::set(StructureSection<Def>*from, StructureSection<Def>*to)
    {
        __transformSystem(to->global.all_in.system,from->global.all_out.system,current.system);
        __transformSystem(to->last_global.all_in.system,from->last_global.all_out.system,previous.system);
    }
    

    template <class Def> void PhSystemPath<Def>::transform(const MeshVertex<TPhDef<Float> >*vtx, TEnvVertex<Float>&out)
    {
        __transform(current.system,vtx->position,out.position.v);
        __transform(previous.system,vtx->position,out.last_position.v);
        _subtract(out.position.v,out.last_position.v,out.movement.v);
        __rotate(current.system,vtx->normal,out.normal.v);
    }

    template <class Def> void PhSystemPath<Def>::transform(MeshVertex<TPhDef<Float> >*inout)
    {
        __transform(current.system,inout->position,inout->p1);
        __transform(previous.system,inout->position,inout->p0);
    }

    template <class Def> void PhSystemPath<Def>::transformWithoutNormal(const MeshVertex<TPhDef<Float> >*vtx, TEnvVertex<Float>&out)
    {
        __transform(current.system,vtx->position,out.position.v);
        __transform(previous.system,vtx->position,out.position.v);
        _subtract(out.position.v,out.last_position.v,out.movement.v);
    }

    template <class Def> void StructureSection<Def>::link(PhysicalSpace<Def>*world)
    {
        this->world = world;
        for (unsigned i = 0; i < accelerators; i++)
            accelerator[i].link(this);
        for (unsigned i = 0; i < wheels; i++)
        {
            wheel[i].link(this);
            wheel[i].link(world);
        }
    }


    template <class Def> void StructureSection<Def>::resetState()
    {
        _clear4(animation_state);
        for (unsigned i = 0; i < accelerators; i++)
            accelerator[i].resetState();
        for (unsigned i = 0; i < wheels; i++)
            wheel[i].resetState();
    }

    template <class Def> void StructureSection<Def>::recursiveProcess(const Float&time_delta)
    {
        Vec3<Float>    local_wind,
                        force_root,force_direction;


        Vec3<Float>    air_movement;
        Float           density = world->getAirMovement(global.all_out * mass.center,air_movement,super);
        local_wind = global.all_in * air_movement;

        Float   global_move[3],direction[3];

        for (unsigned i = 0; i < accelerators; i++)
            accelerator[i].process(density,time_delta);

        for (unsigned i = 0; i < wheels; i++)
            wheel[i].process(density,time_delta);

        for (unsigned i = 0; i < hull->triangle_field.length(); i++)
        {
            MeshTriangle<PhDef>&face = hull->triangle_field[i];
            Float   now[3],local_move[3],wind[3],global_direction[3];
            __transform(global.all_out.system,face.center,now);
            _subtract(now,face.gcenter,global_move);
            __rotate(global.all_in.system,global_move,local_move);
            _c3(now,face.gcenter);
            _divide(local_move,time_delta);

            _subtract(local_wind.v,local_move,wind);

            Float intensity = _dot(face.normal,wind) * density * _length(wind) / 4; //this works, because normal is a x b and equals field_size * 2
                    //furthermore the intensity is proportional with |wind|^2 since we got it once in the dotProduct we multiply it once more to get the final result
            if (intensity > 0)
                continue;
            _subtract(face.normal,wind,direction);
            _setLen(direction,intensity);
            __transform(global.all_out.system,direction,global_direction);
            super->postForce(face.gcenter,global_direction);
        }
		
		#if 0
        for (unsigned i = 0; i < hull->quad_field.length(); i++)
        {
            MeshQuad<PhDef>&face = hull->quad_field[i];
            Float   now[3],local_move[3],wind[3],global_direction[3];
            __transform(global.all_out.system,face.center,now);
            _subtract(now,face.gcenter,global_move);
            __rotate(global.all_in.system,global_move,local_move);
            _c3(now,face.gcenter);
            _divide(local_move,time_delta);

            _subtract(local_wind.v,local_move,wind);

            Float intensity = _dot(face.normal,wind) * density * _length(wind) / 4; //this works, because normal is a x b and equals field_size * 2
                    //furthermore the intensity is proportional with |wind|^2 since we got it once in the dotProduct we multiply it once more to get the final result
            if (intensity > 0)
                continue;
            _subtract(face.normal,wind,direction);
            _setLen(direction,intensity);
            __transform(global.all_out.system,direction,global_direction);
            super->postForce(face.gcenter,global_direction);
        }
		#endif


        SystemMatrix<Float> temp;
        __rotationMatrix(animation_state[3], temp.system, 4);
        __swapCols<Float,4,4,1,2>(temp.system);
        __swapRows<Float,4,4,1,2>(temp.system);
        last_local = local;
        local.out = base_system * temp + Vec3<Float>(animation_state);
        local.in.loadInvert(local.out);
        if (parent)
        {
            local.out_path = parent->local.out_path * local.out;
            local.in_path.loadInvert(local.out_path);
        }
        else
        {
            local.out_path = local.out;
            local.in_path = local.in;
        }

        recursive_mass.center = mass.center*weight;
        recursive_mass.radius = mass.radius*weight;
        recursive_weight = weight;
        for (unsigned i = 0; i < children; i++)
        {
            child[i].recursiveProcess(time_delta);      //reseting matrices, applying inertias

            recursive_mass.center += child[i].local.out * child[i].recursive_mass.center * child[i].recursive_weight;
            recursive_weight += child[i].recursive_weight;
        }
        if (recursive_weight)
            recursive_mass.center /= recursive_weight;

        for (unsigned i = 0; i < children; i++)
        {
            Vec3<Float>distance = child[i].local.out * child[i].recursive_mass.center - recursive_mass.center;
            recursive_mass.radius += child[i].recursive_weight * sqrt(distance * distance + child[i].recursive_mass.radius * child[i].recursive_mass.radius);
        }
        if (recursive_weight)
            recursive_mass.radius /= recursive_weight;
    }

    template <class Def> void StructureSection<Def>::buildSphere()
    {
        Float dist = 0;
        _clear(closure.center.v);
        unsigned index0,index1;
        for (unsigned i = 0; i < hull->vertex_cnt; i++)
            for (unsigned j = i+1; j < hull->vertex_cnt; j++)
            {
                Float   dif[3];
                _subtract(hull->vertex[i].position,hull->vertex[j].position,dif);
                Float d = _dot(dif);
                if (d > dist)
                {
                    dist = d;
                    index0 = i;
                    index1 = j;
                }
            }
        if (dist)
        {
            dist /= 4;
            _center(hull->vertex[index0].position,hull->vertex[index1].position,closure.center.v);
            for (unsigned i = 0; i < hull->vertex_cnt; i++)
            {
                Float   dif[3];
                _subtract(hull->vertex[i].position,closure.center.v,dif);
                Float   d = _dot(dif);
                if (d > dist)
                    dist = d;
            }
        }
        closure.radius = sqrt(dist);
        empty = !hull->vertex_cnt && !hull->face_cnt && !hull->edge_cnt;
    }

    template <class Def> bool StructureSection<Def>::handleCollision(StructureSection<Def>*other, const Float&time_delta)
    {
        Float   v0[4],v1[4],temp[4],collision_time(1),collision_normal[3],collision_speed[3],collision_grip,collision[4],other_collision[4];
        unsigned v_count,e_count,f_count;
        MeshEdge<PhDef>*e_buffer[ELEMENT_COLLISION_BUFFER_SIZE];
        MeshTriangle<PhDef>**t_buffer   = reinterpret_cast<MeshTriangle<PhDef>**>(e_buffer);    //using the same buffer here (I'll possibly change this later)
        MeshQuad<PhDef>**q_buffer   = reinterpret_cast<MeshQuad<PhDef>**>(e_buffer);    //using the same buffer here (I'll possibly change this later)
        MeshVertex<PhDef>**v_buffer = reinterpret_cast<MeshVertex<PhDef>**>(e_buffer);

        bool set(false);
        PhSystemPath<Def>  path(other,this);

        Mesh<PhDef>*ohull = other->hull;
        for (unsigned i = 0; i < ohull->vertex_field.length(); i++)
        {
            path.transform(ohull->vertex_field+i);
            Float   *v0 = ohull->vertex_field[i].p0,
                    *v1 = ohull->vertex_field[i].p1;
            map->lookupEdge(v0,v1);
            unsigned faces = map->resolveTriangles(t_buffer,ELEMENT_COLLISION_BUFFER_SIZE);
            Float   time;
            for (unsigned j = 0; j < faces; j++)
                if (_oDetFaceEdgeIntersection(t_buffer[j]->vertex[0]->position, t_buffer[j]->vertex[1]->position, t_buffer[j]->vertex[2]->position,v0,v1,collision_time,temp) && time < collision_time)
                {
                    _c3(temp,collision);
                    _c3(t_buffer[j]->normal,collision_normal);
                    _subtract(v1,v0,collision_speed);
                    collision_time = time;
                    collision_grip = t_buffer[j]->grip * ohull->vertex_field[i].grip;
                    _c3(ohull->vertex_field[i].position,other_collision);
                    set = true;
                }
				
			#if 0
			faces = map->resolveQuads(q_buffer,ELEMENT_COLLISION_BUFFER_SIZE);
            for (unsigned j = 0; j < faces; j++)
                if (_oDetFaceEdgeIntersection(q_buffer[j]->vertex[0]->position, f_buffer[j]->vertex[1]->position, f_buffer[j]->vertex[2]->position,v0,v1,collision_time,temp) && time < collision_time)
                {
                    _c3(temp,collision);
                    _c3(f_buffer[j]->normal,collision_normal);
                    _subtract(v1,v0,collision_speed);
                    collision_time = time;
                    collision_grip = f_buffer[j]->grip * ohull->vertex[i].grip;
                    _c3(ohull->vertex[i].position,other_collision);
                    set = true;
                }
			#endif
        }
        for (unsigned i = 0; i < ohull->edge_field.length(); i++)
        {
            Float   *edge[4] = {ohull->edge_field[i].vertex[0]->p0,
                                ohull->edge_field[i].vertex[1]->p0,
                                ohull->edge_field[i].vertex[0]->p1,
                                ohull->edge_field[i].vertex[1]->p1};

            Float   dim[6];
            _c3(edge[0],dim);
            _c3(edge[0],&dim[3]);
            for (BYTE k = 1; k < 4; k++)
                _oDetDimension(edge[k],dim);
            map->lookup(dim,&dim[3]);
            unsigned edges = map->resolveEdges(e_buffer,ELEMENT_COLLISION_BUFFER_SIZE);
            Float   time,intersection,e0[3],e1[3],delta0[3],delta1[3];
            for (unsigned j = 0; j < edges; j++)
                if (_oDetEdgeIntersection(e_buffer[j]->vertex[0]->position,e_buffer[j]->vertex[1]->position,edge[0],edge[1],edge[2],edge[3],time,intersection) && time < collision_time)
                {
                    _interpolate(edge[0],edge[2],time,e0); //edge at time = collision_time
                    _interpolate(edge[1],edge[3],time,e1);
                    _subtract(e1,e0,delta0);
                    _interpolate(e0,e1,intersection,collision); //local point of collision
                    _interpolate(edge[0],edge[1],intersection,e0); //trace-edge
                    _interpolate(edge[2],edge[3],intersection,e1);
                    _subtract(e1,e0,collision_speed); //get speed on specified edge
                    collision_time = time;
                    _subtract(e_buffer[j]->vertex[1]->position,e_buffer[j]->vertex[0]->position,delta1);
                    _cross(delta0,delta1,collision_normal);
                    collision_grip = e_buffer[j]->grip * ohull->edge[i].grip;
                    set = true;
                    _interpolate(ohull->edge[i].vertex[0]->position,ohull->edge[i].vertex[1]->position,intersection,other_collision);
                }
        }
        for (unsigned i = 0; i < ohull->face_field.length(); i++)
        {
            Float   *face[6] = {ohull->face[i].vertex[0]->p0,
                                ohull->face[i].vertex[1]->p0,
                                ohull->face[i].vertex[2]->p0,
                                ohull->face[i].vertex[0]->p1,
                                ohull->face[i].vertex[1]->p1,
                                ohull->face[i].vertex[2]->p1};
            Float   dim[6];
            _c3(face[0],dim);
            _c3(face[0],&dim[3]);
            for (BYTE k = 1; k < 6; k++)
                _oDetDimension(face[k],dim);
            map->lookup(dim,&dim[3]);
            unsigned vertices = map->resolveVertices(v_buffer,ELEMENT_COLLISION_BUFFER_SIZE);
            Float   time,x,y,normal[3];
            for (unsigned j = 0; j < vertices; j++)
                if (_oDetVolumeVertexIntersection(v_buffer[j]->position,face[0],face[1],face[2],face[3],face[4],face[5],time,x,y) && time < collision_time)
                {
                    Float   state[3][3],dx[3],dy[3],floor[3],ceiling[3];
                    _interpolate(face[0],face[3],time,state[0]);
                    _interpolate(face[1],face[4],time,state[1]);
                    _interpolate(face[2],face[5],time,state[2]);
                    _oTriangleCoord(state[0],state[1],state[2],x,y,collision);
                    _oTriangleNormal(state[0],state[2],state[2],collision_normal);
                    _oTriangleCoord(face[0],face[1],face[2],x,y,floor);
                    _oTriangleCoord(face[3],face[4],face[5],x,y,ceiling);
                    _subtract(ceiling,floor,collision_speed);
                    collision_time = time;
                    collision_grip = v_buffer[j]->grip * ohull->face[i].grip;
                    _oTriangleCoord(ohull->face[i].vertex[0]->position,ohull->face[i].vertex[1]->position,ohull->face[i].vertex[2]->position,x,y,other_collision);
                    set = true;
                }
        }
        if (!set)
            return false;
        _divide(collision_speed,time_delta);
        //so now we got collision-point, speed, normal and time of the first occuring collision (may have to change this later on)
        Float       fc = _dot(collision_speed,collision_normal)/_dot(collision_normal,collision_normal),
                    v_horizontal[3],v_vertical[3],
                    vforce[3],hforce[3];
        _multiply(collision_normal,fc,v_vertical);
        _subtract(collision_speed,v_vertical,v_horizontal);
        Float       m0 = other->super->getAffectedMass(collision,v_vertical),
                    m1 = super->getAffectedMass(collision,v_vertical);
        if (!other->super->fixed)
            _multiply(v_vertical,m0*m1 / (m0+m1),vforce);
        else
            _multiply(v_vertical,m1,vforce);
        m0 = other->super->getAffectedMass(collision,v_horizontal);
        m1 = super->getAffectedMass(collision,v_horizontal);
        if (!other->super->fixed)
            _multiply(v_horizontal,m0*m1 / (m0+m1),hforce);
        else
            _multiply(v_horizontal,m1,hforce);
        Float   vf = _length(vforce),
                hf = _length(hforce);
        if (hf > vf * collision_grip)
            _multiply(hforce,vf*collision_grip / hf);
        Vec3<Float>   global_root,global_force,other_global_root;
        __transform(global.all_out.system,collision,global_root.v);
        __transform(other->global.all_out.system,other_collision,other_global_root.v);
        __rotate(global.all_out.system,vforce,global_force.v);
        this ->super->postForce(global_root,global_force);
        other->super->postForce(other_global_root,-global_force);
        __rotate(global.all_out.system,hforce,global_force.v);
        this ->super->postForce(global_root,global_force);
        other->super->postForce(other_global_root,-global_force);
        return true;
    }



    template <class Def> void StructureSection<Def>::update()
    {
        last_global = global;

        global.all_out = super->global.out * (local.out_path - super->mass.center);
        global.all_in.loadInvert(global.all_out);

        global.closure_center = global.all_out * closure.center;
        dimension.lower = global.closure_center - closure.radius;
        dimension.upper = global.closure_center + closure.radius;
        TEnvSpace<Float> other;
        other.lower = last_global.closure_center - closure.radius;
        other.upper = last_global.closure_center + closure.radius;
        for (BYTE k = 0; k < 3; k++)
        {
            if (other.lower[k] < dimension.lower[k])
                dimension.lower[k] = other.lower[k];
            if (other.upper[k] > dimension.upper[k])
                dimension.upper[k] = other.upper[k];
        }
    }

    template <class Def> void StructureSection<Def>::backdate()
    {
        global = last_global;
    }

    template <class Def> void StructureSection<Def>::recursiveFinalize()
    {
        local.out = base_system;
        local.in.loadInvert(base_system);
        local.out_path = parent?parent->local.out_path * local.out:local.out;
        last_local = local;
        _clear4(animation_state);

        recursive_mass.center = mass.center*weight;
        recursive_mass.radius = mass.radius*weight;
        recursive_weight = weight;
        for (unsigned i = 0; i < children; i++)
        {
            child[i].recursiveFinalize();
            recursive_mass.center += child[i].local.out* child[i].recursive_mass.center * child[i].recursive_weight;
            recursive_weight += child[i].recursive_weight;
    //        ShowMessage(name2str(section->name)+" = "+FloatToStr(section->recursive_weight));
        }
        if (recursive_weight)
            recursive_mass.center /= recursive_weight;

        for (unsigned i = 0; i < children; i++)
        {
            Vec3<Float>distance =     child[i].local.out* child[i].recursive_mass.center - recursive_mass.center;
            recursive_mass.radius += child[i].recursive_weight * sqrt(distance * distance + child[i].recursive_mass.radius * child[i].recursive_mass.radius);
        }
        if (recursive_weight)
            recursive_mass.radius /= recursive_weight;
    }

    template <class Def> unsigned StructureSection<Def>::createFrom(CGS::SubGeometryA<Def>&origin)
    {
        origin.reference = this;
        origin.signature = super;
        hull = &origin.ph_hull;
        if (!hull->map)
            hull->map = hull->buildMap(O_ALL,4);
        map = hull->map;

        reloc(accelerator,accelerators,origin.accelerators);
        reloc(wheel,wheels,origin.wheels);
        reloc(child,children,origin.children);
        for (unsigned i = 0; i < accelerators; i++)
        {
            accelerator[i].root = origin.accelerator[i].position;
            accelerator[i].direction = origin.accelerator[i].direction;
            accelerator[i].power = origin.accelerator[i].power;
            accelerator[i].zero_efficiency = origin.accelerator[i].zero_efficiency;
            accelerator[i].present_power = 0;
            accelerator[i].parent = this;
            accelerator[i].super = super;
            origin.accelerator[i].reference = accelerator+i;
            origin.accelerator[i].signature = super;
        }
        for (unsigned i = 0; i < wheels; i++)
        {
            wheel[i].root = origin.wheel[i].position;
            wheel[i].extension_direction = origin.wheel[i].contraction;
            wheel[i].extension_direction *= -1;
            wheel[i].axis_direction = origin.wheel[i].axis;
            wheel[i].axis_direction *= origin.wheel[i].width/2/wheel[i].axis_direction.length();
            wheel[i].width = origin.wheel[i].width;
            wheel[i].radius = origin.wheel[i].radius;
            wheel[i].suspension_strength = origin.wheel[i].suspension_strength;
            wheel[i].power = origin.wheel[i].power;
            wheel[i].grip = origin.wheel[i].grip;
            wheel[i].zero_efficiency = origin.wheel[i].zero_efficiency;
            wheel[i].suspension_friction = origin.wheel[i].suspension_friction;
            wheel[i].rotation_friction = origin.wheel[i].rotation_friction;
            wheel[i].setup(origin.wheel[i].density);
            wheel[i].parent = this;
            wheel[i].world = world;
            wheel[i].super = super;
            wheel[i].suspension.origin = &origin.wheel[i].suspension;
            wheel[i].rotation.origin = &origin.wheel[i].rotation;

            origin.wheel[i].reference = wheel+i;
            origin.wheel[i].signature = super;
        }
        buildSphere();
        unsigned rs = 1;
        for (unsigned i = 0; i < children; i++)
            rs += child[i].createFrom(origin.child[i]);
        return rs;
    }


    template <class Def> Structure<Def>::Structure():section(NULL),sections(0),total_sections(0),linear_list(NULL),animator(NULL),animators(0)
    {
        clear();
        initialize();
    }

    template <class Def> void Structure<Def>::initialize()
    {
        global.out.loadIdentity();
        global.in.loadIdentity();
        last_global = global;
        inertia.momentum = (Float)0;
        inertia.linear = (Float)0;
        finalized_inertia = inertia;
        __eye3(finalized.rotation_per_second);
        _c9(finalized.rotation_per_second,finalized.rotation_per_frame);
        finalized.set = false;
        handled = false;
    }

    template <class Def> void Structure<Def>::clear()
    {
        dealloc(linear_list);
        total_sections = 0;
        dealloc(section);
        section = NULL;
        sections = 0;
        atmosphere.density = 0;
        atmosphere.range0 = 1;
        atmosphere.half_range = 1;
        mass.center.clear();
        mass.radius = 0;
        last_mass = mass;
        global_center.clear();
        total_weight = 0;
        dealloc(animator);
        animator = NULL;
        animators = 0;
        handled = false;
        fixed = false;
    }

    template <class Def> void Structure<Def>::finalize()
    {
        last_mass = mass;
        mass.radius = 0;
        mass.center = (Float)0;
        Float   weight(0);

        for (unsigned i = 0; i < sections; i++)
        {
            section[i].recursiveFinalize();
            mass.center += section[i].local.out * section[i].recursive_mass.center * section[i].recursive_weight;
            weight += section[i].recursive_weight;
        }
        if (weight)
            mass.center /= weight;

        for (unsigned i = 0; i < sections; i++)
        {
            Vec3<Float> distance = section[i].local.out * section[i].recursive_mass.center - mass.center;
            mass.radius += section[i].recursive_weight * sqrt(distance * distance + section[i].recursive_mass.radius * section[i].recursive_mass.radius);
        }
        if (weight)
            mass.radius /= weight;
        last_mass = mass;
        global_center = global.out*mass.center;
        for (unsigned i = 0; i < total_sections; i++)
        {
            linear_list[i]->update();
            linear_list[i]->update();   //hmm....???
        }
    }

    template <class Def> typename Def::PhFloatType Structure<Def>::getAffectedMass(const Line<Float>&line)
    {
        return getAffectedMass(line.position,line.direction);
    }
    
    template <class Def> typename Def::PhFloatType Structure<Def>::getAffectedMass(const Vec3<Float>&b, const Vec3<Float>&d)
    {
        Float   radius[3],momentum[3];
        _subtract(b.v,global_center.v,radius);
        _cross(radius,d.v,momentum);
        Float   force = d.length(), //?
                acceleration = _length(radius) * _length(momentum)/(2.0/5.0*total_weight*sqr(mass.radius*C2EXP1_3)) + force/total_weight;
        return force / acceleration;
    } //could work

    template <class Def> void Structure<Def>::postForce(const Line<Float>&f)
    {
        if (fixed)
            return;
        Float   dist[3],rotation[3];
        _subtract(f.position.v,global_center.v,dist);
        _cross(dist,f.direction.v,rotation);
        _add(inertia.momentum.v,rotation);
        inertia.linear += f.direction / total_weight;
    }

    template <class Def> void Structure<Def>::postForce(const Float b[3], const Float d[3])
    {
        if (fixed)
            return;
        Float   dist[3],rotation[3];
        _subtract(b,global_center.v,dist);
        _cross(dist,d,rotation);
        _add(inertia.momentum.v,rotation);
        _divide(d,total_weight,dist);
        _add(inertia.linear.v,dist);
    }

    template <class Def> void Structure<Def>::postForce(const Vec3<Float>&b, const Vec3<Float>&d) //b and d are _very_ global vectors
    {
        if (fixed)
            return;
        Float   dist[3],rotation[3];
        _subtract(b.v,global_center.v,dist);
        _cross(dist,d.v,rotation);
        _add(inertia.momentum.v,rotation);
        inertia.linear += d / total_weight;
    }

    template <class Def>void Structure<Def>::processItems(const Float&time_delta)
    {
        for (unsigned i = 0; i < total_sections; i++)
        {
            linear_list[i]->last_global = linear_list[i]->global;
            linear_list[i]->resetState();
            for (unsigned j = 0; j < linear_list[i]->wheels; j++)
                linear_list[i]->wheel[j].applyState();
        }

        for (unsigned i = 0; i < animators; i++)
            animator[i].cast(time_delta);          //cast forward

        //now all animation- and acceleration-states are set.

        last_mass = mass;
        mass.radius = 0;
        mass.center = 0;
        Float   weight(0);
        for (unsigned i = 0; i < sections; i++)
        {
            section[i].recursiveProcess(time_delta);      //reseting matrices, applying inertias

            mass.center += section[i].local.out * section[i].recursive_mass.center * section[i].recursive_weight;
            weight += section[i].recursive_weight;
        }
        if (weight)
            mass.center /= weight;

        for (unsigned i = 0; i < sections; i++)
        {
            Vec3<Float> distance = section[i].local.out * section[i].recursive_mass.center - mass.center;
            mass.radius += section[i].recursive_weight * sqrt(distance * distance + section[i].recursive_mass.radius * section[i].recursive_mass.radius);
        }
        if (weight)
            mass.radius /= weight;

        global_center = global.out * mass.center;
        inertia.linear += world->getGravity(global_center,total_weight)*time_delta;

        //should work
        //could work
    }


    template <class Def>Vec3<typename Def::PhFloatType> Structure<Def>::getPointMovement(const Vec3<Float>&position)
    {
        Float               delta[4],temp[4];
        Vec3<Float>        out;
        delta[3] = 0;
        if (finalized.set)
        {
            _sub(position.v,global_center.v,delta);
            __mult331(finalized.rotation_per_second,delta,temp);
            _sub(temp,delta);
            _add(temp,finalized_inertia.linear.v,out.v);
            return out;
        }
        else
            return finalized_inertia.linear;
    }

    template <class Def>typename Def::PhFloatType    Structure<Def>::getDensity(const Vec3<Float>&position)
    {
        return atmosphere.density * pow(2, - (_distance(position.v,global_center.v) - atmosphere.range0) / atmosphere.half_range);
    }

    template <class Def>void Structure<Def>::move(const Float&time_delta)
    {
        last_global = global;
        Float   axis[3],
                l = inertia.momentum.length(),
                angle = l / (2.0/5.0*total_weight * sqr(mass.radius*C2EXP1_3));//mass.radius multiplied by 2^(1/3) to derive sphere-radius from mass-radius
        if (l)
        {
            _div(inertia.momentum.v,l,axis);
            __rotationMatrix(angle,axis,finalized.rotation_per_second);
            angle *= time_delta;
            __rotationMatrix(angle,axis,finalized.rotation_per_frame);
            Float temp[3];
            __mult331(finalized.rotation_per_frame,global.out.v0,temp);
            _c3(temp,global.out.v0);
            __mult331(finalized.rotation_per_frame,global.out.v1,temp);
            _c3(temp,global.out.v1);
            __mult331(finalized.rotation_per_frame,global.out.v2,temp);
            _c3(temp,global.out.v2);
/*            __mult331(finalized.rotation_per_frame,global.out.v+12,temp);
            _c3(temp,global.out.v+12);*/
            finalized.set = true;
        }
        else
            finalized.set = false;
        global.out += inertia.linear * time_delta;
        global_center = global.out * mass.center;
        global.in.loadInvert(global.out);
        for (unsigned i = 0; i < total_sections; i++)
            linear_list[i]->update();
        handled = false;
        finalized_inertia = inertia;
    }

    template <class Def>void Structure<Def>::unmove()
    {
        global = last_global;
        for (unsigned i = 0; i < total_sections; i++)
            linear_list[i]->backdate();
    }

    template <class Def>void Structure<Def>::setAnimator(__int64 name, eAnimatorState state)
    {
        unsigned lower = 0,
                 upper = animators;
        while (lower<upper && upper <= animators)
        {
            unsigned el = (lower+upper)/2;
            if (animator[el].name > name)
                upper = el-1;
            else
                lower = el+1;
            if (animator[el].name == name)
            {
                animator[el].state = state;
                return;
            }
        }
        if (lower < animators && animator[lower].name == name)
            animator[lower].state = state;
    }


    template <class Def> StructTree<Def>::StructTree(const TEnvSpace<Float>&space, unsigned l):dim(space),level(l)
    {
        for (BYTE k = 0; k < 8; k++)
            child[k] = NULL;
    }

    template <class Def> StructTree<Def>::StructTree(const StructTree<Def>&other):dim(other.dim),level(other.level)
    {
        for (unsigned i = 0; i < other.count(); i++)
            append(other.getConst(i));
        dominating = other.dominating;
        if (level && count())
            for (BYTE k = 0; k < 8; k++)
                child[k] = SHIELDED(new StructTree<Def>(*other.child[k]));
        else
            for (BYTE k = 0; k < 8; k++)
                child[k] = NULL; //we will see if we actually need this
    }

    template <class Def> StructTree<Def>::StructTree(unsigned depth):level(depth)
    {
        for (BYTE k = 0; k < 8; k++)
            child[k] = 0;
    }

    template <class Def> StructTree<Def>::~StructTree()
    {
        for (BYTE k = 0; k < 8; k++)
            if (child[k])
                DISCARD(child[k]);
    }

    template <class Def> void StructTree<Def>::recursiveRemap(TTreeData<Def>*source)
    {
        flush();
        dominating = source->first()->super;
        source->reset();
        while (StructureSection<Def>*section = source->each())
        {
            if (_oneLess(section->dimension.upper.v,dim.lower.v) || _oneGreater(section->dimension.lower.v, dim.upper.v))
                continue;
            append(section);
            if (dominating != section->super)
                dominating = NULL;
        }
        if (!level || !count())
            return;
        for (BYTE k = 0; k < 8; k++)
        {
            if (!child[k])
            {
                BYTE p[3] = {k / 4,
                             k % 4 / 2,
                             k % 4 % 2};
                TEnvSpace<Float> d;
                for (BYTE j = 0; j < 3; j++)
                {
                    d.lower[j] = dim.lower[j]  + (dim.upper[j] - dim.lower[j])/2*p[j];
                    d.upper[j] = d.lower[j] + (dim.upper[j] - dim.lower[j])/2;
                }
                child[k] = SHIELDED(new StructTree<Def>(d, level-1));
            }
            child[k]->recursiveRemap(this);
        }
    }

    template <class C> void StructTree<C>::unmap(StructureSection<C>*source)
    {
        if (!dropElement(source) || !level)
            return;
        for (BYTE k = 0; k < 8; k++)
            child->unmap(source);
    }

    template <class C> void StructTree<C>::unmap(Structure<C>*source)
    {
        bool dropped = false;
        dominating = source;
        for (unsigned i = 0; i < count() && !dropped; i++)
            if (get(i)->super == source)
            {
                drop(i);
                dropped = true;
            }
            else
                if (dominating == source)
                    dominating = get(i)->super;
                else
                    if (dominating != get(i)->super)
                        dominating = NULL;
        if (!dropped || !level)
            return;

        for (BYTE k = 0; k < 8; k++)
            child->unmap(source);
    }

    template <class Def> void StructTree<Def>::map(StructureSection<Def>*source)
    {
        if (source->empty || _oneLesser(source->dimension.upper.v,dim.lower.v) || _oneGreater(source->dimension.lower.v, dim.upper.v))
            return;
        if (!count())
            dominating = source->super;
        else
            if (dominating != source->super)
                dominating = NULL;
        append(source);


        for (BYTE k = 0; k < 8; k++)
        {
            if (!child[k])
            {
                BYTE p[3] = {k / 4,
                             k % 4 / 2,
                             k % 4 % 2};
                TEnvSpace<Float> d;
                for (BYTE j = 0; j < 3; j++)
                {
                    d.lower[j] = dim.lower[j]  + (dim.upper[j] - dim.lower[j])/2*p[j];
                    d.upper[j] = d.lower[j]  + (dim.upper[j] - dim.lower[j])/2;
                }
                child[k] = SHIELDED(new StructTree<Def>(d, level-1));
            }
            child[k]->inmap(source);
        }
    }


    template <class Def> void StructTree<Def>::remap(Vector<Structure<Def> >*source)
    {
        if (!source->count())
            return;
        dim = source->first()->section[0].dimension;

        source->reset();
        while (Structure<Def>*structure = source->each())
            for (unsigned i = 0; i < structure->sections; i++)
            {
                if (!structure->section[i].empty)
                {
                    append(structure->section+i);
                    for (BYTE k = 0; k < 3; k++)
                    {
                        if (structure->section[i].dimension.lower[k] < dim.lower[k])
                            dim.lower[k] = structure->section[i].dimension.lower[k];
                        if (structure->section[i].dimension.upper[k] > dim.upper[k])
                            dim.upper[k] = structure->section[i].dimension.upper[k];
                    }
                }
            }
        dominating = source->first();
        if (!count())
            return;
        for (BYTE k = 0; k < 8; k++)
        {
            if (!child[k])
            {
                BYTE p[3] = {k / 4,
                             k % 4 / 2,
                             k % 4 % 2};
                TEnvSpace<Float> d;
                for (BYTE j = 0; j < 3; j++)
                {
                    d.lower[j] = dim.lower[j]  + (dim.upper[j] - dim.lower[j])/2*p[j];
                    d.upper[j] = d.lower[j] + (dim.upper[j] - dim.lower[j])/2;
                }
                child[k] = SHIELDED(new StructTree<Def>(d, level-1));
            }
            child[k]->recursiveRemap(this);
        }
    }

    template <class Def>unsigned StructTree<Def>::recursiveLookup(const TEnvSpace<Float>&space, const Structure<Def>*exclude) const
    {
        const unsigned cnt = count();
        if ((exclude && dominating == exclude) || _oneLess(space.upper.v,dim.lower.v) ||_oneGreater(space.lower.v,dim.upper.v) || !cnt)
            return 0;
        if (!level || 8*level > cnt)
        {
            unsigned c = 0;
            for (unsigned i = 0; i < cnt; i++)
            {
                StructureSection<Def>*str = getConst(i);
                if (_oneLess(space.upper.v,str->dimension.lower.v) ||_oneGreater(space.lower.v,str->dimension.upper.v))
                    continue;
                if (sector_count >= GLOBAL_SECTOR_MAP_SIZE)
                    FATAL__("global sector-map size ("+IntToStr(GLOBAL_SECTOR_MAP_SIZE)+") exceeded.");
                sector_map[sector_count++] = str;
                c++;
            }
            return c;
        }
        unsigned at = sector_count;
        for (BYTE k = 0; k < 8; k++)
            if (child[k]->recursiveLookup(space,exclude) == cnt)
            {
                if (sector_count-at > cnt)
                {
                    sector_count = at;
                    for (unsigned i = 0; i < cnt; i++)
                        sector_map[sector_count++] = getConst(i);
                }
                return cnt;
            }
        return 0;
    }

    template <class Def>unsigned StructTree<Def>::lookup(const TEnvSpace<Float>&space, StructureSection<Def>**out, const Structure<Def>*exclude, unsigned output_offset)  const
    {
        sector_count = output_offset;
        recursiveLookup(space,exclude);
        unsigned c = output_offset;
        for (unsigned i = 0; i < sector_count; i++)
        {
            StructureSection<Def>*section = sector_map[i];
            if (!section->added)
            {
                if (c >= COLLISION_BUFFER_SIZE)
                    FATAL__("collsion-buffer size ("+IntToStr(COLLISION_BUFFER_SIZE)+") exceeded.");
                out[c++] = section;
                section->added = true;
            }
        }
        for (unsigned i = 0; i < c; i++)
            out[i]->added = false;
        return c;
    }

    template <class Def>void StructTree<Def>::operator=(const StructTree<Def>&other)
    {
        for (BYTE k = 0; k < 8; k++)
            DISCARD(child[k]);
        flush();
        for (unsigned i = 0; i < other.count(); i++)
            append(other.getConst(i));
        dominating = other.dominating;
        if (level && count())
            for (BYTE k = 0; k < 8; k++)
                child[k] = SHIELDED(new StructTree<Def>(*other.child[k]));
        else
            for (BYTE k = 0; k < 8; k++)
                child[k] = NULL; //we will see if we actually need this
    }


    template <class Def>PhysicalSpace<Def>::PhysicalSpace():static_tree(4),dynamic_tree(4),dynamic_effects(false)
    {
        global_wind = (Float)0;
    }

    template <class Def>void PhysicalSpace<Def>::simulate(const Float&time_delta)
    {
        Structure<Def>*structure;
        bool collision;
        unsigned counter(0);
        do
        {
            collision = false;
            dynamic_list.reset();
            while (structure = dynamic_list.each())
                structure->move(time_delta); //excluding animations - including section-redimensioning
            dynamic_tree.remap(&dynamic_list);
            dynamic_list.reset();
            while (structure = dynamic_list.each())
            {
                structure->handled = true;
                for (unsigned i = 0; i < structure->total_sections; i++)
                {
                    StructureSection<Def>*section = structure->linear_list[i];
                    unsigned collisions = dynamic_tree.lookup(section->dimension,collision_buffer);
                    for (unsigned i = 0; i < collisions; i++)
                        if (!collision_buffer[i]->super->handled && section->handleCollision(collision_buffer[i],time_delta))
                            collision = true;

                    collisions = static_tree.lookup(section->dimension,collision_buffer);
                    for (unsigned i = 0; i < collisions; i++)
                        if (section->handleCollision(collision_buffer[i],time_delta))
                            collision = true;
                }
            }
            if (collision)
            {
                //collision creates forces which would otherwise not be created
                dynamic_list.reset();
                while (structure = dynamic_list.each())
                    structure->unmove(); //so we erase the movement but keep the new inertia
            }
            if (++counter >= COLLISION_MAX_LOOP)
                FATAL__("collision loop repeated "+IntToStr(counter)+" times.");
        }
        while (collision);

        /* now-state: *
            structures moved and updated their position-matrix. they are positioned correctly within the environment-tree.
            so far no wind was calculated and accelerators+wheels remain unupdated
        */
        dynamic_list.reset();
        while (structure = dynamic_list.each())
            structure->processItems(time_delta); //including friction (updates inertia only) and animations
    }

    template <class Def>typename Def::PhFloatType PhysicalSpace<Def>::getAirDensity(const Vec3<Float>&position, Structure<Def>*exclude)
    {
        Structure<Def>*structure;
        static_list.reset();
        Float result(0);
        while (structure = static_list.each())
            if (structure->atmosphere.enabled)
                result += structure->getDensity(position);
        if (dynamic_effects)
        {
            dynamic_list.reset();
            while (structure = dynamic_list.each())
                if (structure->atmosphere.enabled && structure != exclude)
                    result += structure->getDensity(position);
        }
        return result; //me ait rulin' aright
    }

    template <class Def> typename Def::PhFloatType PhysicalSpace<Def>::getAirMovement(const Vec3<Float>&position, Vec3<Float>&out, Structure<Def>*exclude)
    {
        out.clear();
        Structure<Def>*structure;
        static_list.reset();
        Float   density(0);
        while (structure = static_list.each())
            if (structure->atmosphere.enabled)
            {
                Float   d = structure->getDensity(position);
                density += d;
                out += structure->getPointMovement(position) * d;
            }
        if (dynamic_effects)
        {
            dynamic_list.reset();
            while (structure = dynamic_list.each())
                if (structure->atmosphere.enabled && structure != exclude)
                {
                    Float   d = structure->getDensity(position);
                    density += d;
                    out += structure->getPointMovement(position) * d;
                }
        }
        out/=density;
        out+=global_wind;
        return density;
    }


    template <class Def>Vec3<typename Def::PhFloatType> PhysicalSpace<Def>::getGravity(const Vec3<Float>&position, const Float&m, Structure<Def>*exclude)
    {
        Vec3<Float> result(0,0,0);
        Structure<Def>*structure;
        static_list.reset();
        while (structure = static_list.each())
        {
            Vec3<Float> distance = structure->global_center - position;
            result += distance.normalized0() * ((G_CONST * structure->total_weight * m) / (distance * distance));
        }
        if (dynamic_effects)
        {
            dynamic_list.reset();
            while (structure = dynamic_list.each())
                if (structure != exclude)
                {
                    Vec3<Float> distance = structure->global_center - position;
                    result += distance.normalized0() * ((G_CONST * structure->total_weight * m) / (distance * distance));
                }
        }
        return result;
    }


    template <class Def> Structure<Def>*  PhysicalSpace<Def>::embed(CGS::Geometry<Def>*source, bool dynamic)
    {
        if (!source || !source->objects)
            return NULL;
        Structure<Def>*result = dynamic?dynamic_list.append():static_list.append();

        source->resolveWeight(result->total_weight);
        result->atmosphere.density = 0;
        result->atmosphere.range0 = 1;
        result->atmosphere.half_range = 1;
        result->atmosphere.enabled = false;
        reloc(result->section,result->sections,source->objects);
        unsigned scount=0;
        for (unsigned i = 0; i < source->objects; i++)
            scount+=result->section[i].createFrom(source->object[i]);
        reloc(result->linear_list,result->total_sections,scount);
        StructureSection<Def>**list = result->linear_list;
        for (unsigned i = 0; i < source->objects; i++)
        {
            (*list++) = result->section+i;
            list+=result->section[i].mapChildrenTo(list);
        }
        reloc(result->animator,result->animators,source->animators);
        for (unsigned i = 0; i < source->animators; i++)
        {
            result->animator[i].origin = source->animator+i;
            result->animator[i].readFromOrigin();
        }
        for (unsigned i = 0; i < result->total_sections; i++)
            for (unsigned j = 0; j < result->linear_list[i]->wheels; j++)
            {
                result->linear_list[i]->wheel[j].rotation.readFromOrigin();
                result->linear_list[i]->wheel[j].suspension.readFromOrigin();
            }
        return result;
    }


    template <class Def> StructureSection<Def>* StructTree<Def>::sector_map[GLOBAL_SECTOR_MAP_SIZE];
    template <class Def> unsigned                StructTree<Def>::sector_count(0);


}


#endif
