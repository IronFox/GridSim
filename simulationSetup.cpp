#include "simulationSetup.h"
#include <engine/interface/input.h>
#include "simulation.h"
#include "statistics.h"
#include "types.h"
#include "experiment.h"
//#include 

count_t		accumulation = 0;
Simulation	testSimulation;
GUID		avatarID;
bool		hasAvatar=false;
TExperiment	currentSetup;
count_t		connectionSamples=0,connectedSamples=0;

Random		random;

count_t		numResets = 0;	//!< Number of times SetupScene() has been called
bool		simulate = false;
bool		stopSimulation = false;

void LogMessage(const String&msg);

namespace EntityFlags
{
	enum flag_t
	{
		None=0,
		IsFish=1,
		IsFocus=2,
		IsWall=4,
	};

}



namespace Logic
{
	struct TBroadcastData
	{
		UINT32 seed;
		EntityID sender;
	};


	void RandomMotion(Array<BYTE>&serialState, count_t generation, const Entity&e, EntityShape&inOutShape,Random&random, const MessageReceiver&, MessageDispatcher&)
	{
		float r = random.NextFloat(0.f,Entity::MaxMotionDistance);
		Metric::Direction(random).ToVector(inOutShape.velocity);
		inOutShape.velocity *= r;
	}

	void AggressiveRandomMotion(Array<BYTE>&serialState, count_t generation, const Entity&e, EntityShape&inOutShape,Random&, const MessageReceiver&recv, MessageDispatcher&disp)
	{
		CRC32::Sequence seq;

		foreach (recv,m)
		{
			seq.AppendPOD(m->sender);
			seq.AppendPOD(m->isBroadcast);
			#ifndef ONE_LOGIC_PER_ENTITY
				seq.AppendPOD(m->senderProcess);
			#endif
			auto ref = ToArrayRef(m->data);
			seq.Append(ref.GetPointer(),ref.Length());
		}

		#ifndef NO_SENSORY
			foreach (e.neighborhood,n)
			{
				seq.AppendPOD(n->guid);
				seq.AppendPOD(n->coordinates);
			}
		#endif

		seq.AppendPOD(e.guid);
		seq.AppendPOD(e.coordinates);
		if (serialState.GetLength()	== 4)
		{
			seq.AppendPOD(*(const UINT32*)serialState.GetPointer());
		}
		const auto seed = seq.Finish();
		Random random(seed);

		float r = Entity::MaxMotionDistance;
	//	random.getFloat(0.f,Entity::MaxOperationalRadius);
		TEntityCoords motion;
		Metric::Direction(random).ToVector(motion);
		motion *= r;

		serialState.SetSize(4);
		(*(UINT32*)serialState.GetPointer()) = random.NextSeed();

		inOutShape.velocity += motion;

		#ifdef NO_SENSORY
			#ifdef INT_MESSAGES
				UINT64 broadcast;
				UINT32*parts = (UINT32*)&broadcast;
				parts[0] = seed;
				parts[1] = e.guid.Data1;
				disp.BroadcastCopy(AggressiveRandomMotion,broadcast);
			#else
				TBroadcastData broadcast;
				broadcast.seed = seed;
				broadcast.sender = e;
				disp.BroadcastPOD(AggressiveRandomMotion,broadcast);
			#endif
		#endif
	}

	void AggressiveRandomMotionX(Array<BYTE>&serialState, count_t generation, const Entity&e, EntityShape&inOutShape, Random&, const MessageReceiver&recv, MessageDispatcher&disp)
	{
		CRC32::Sequence seq;
		foreach (recv,m)
		{
			seq.AppendPOD(m->sender);
			seq.AppendPOD(m->isBroadcast);
			#ifndef ONE_LOGIC_PER_ENTITY
				seq.AppendPOD(m->senderProcess);
			#endif
			auto ref = ToArrayRef(m->data);
			seq.Append(ref.GetPointer(),ref.Length());
		}

		#ifndef NO_SENSORY
			foreach (e.neighborhood,n)
			{
				seq.AppendPOD(n->guid);
				seq.AppendPOD(n->coordinates);
			}
		#endif

		seq.AppendPOD(e.guid);
		seq.AppendPOD(e.coordinates);
		if (serialState.GetLength() == 4)
		{
			seq.AppendPOD(*(const UINT32*)serialState.GetPointer());
		}
		const auto seed = seq.Finish();
		Random random(seed);

		float r = Entity::MaxMotionDistance;
		//	random.getFloat(0.f,Entity::MaxOperationalRadius);
		TEntityCoords motion;
		motion.x = random.NextBool() ? -r : r;
		//Metric::Direction(random).ToVector(motion);
		//motion *= r;

		serialState.SetSize(4);
		(*(UINT32*)serialState.GetPointer()) = random.NextSeed();

		inOutShape.velocity += motion;


		#ifdef INT_MESSAGES
			UINT64 broadcast;
			UINT32*parts = (UINT32*)&broadcast;
			parts[0] = seed;
			parts[1] = e.guid.Data1;
			disp.BroadcastCopy(AggressiveRandomMotionX,broadcast);
		#else
			TBroadcastData broadcast;
			broadcast.seed = seed;
			broadcast.sender = e;
			disp.BroadcastPOD(AggressiveRandomMotionX,broadcast);

		#endif

	}

	#ifndef NO_SENSORY
		void MinDistance(Array<BYTE>&serialState, count_t generation, const Entity&e, EntityShape&inOutShape,Random&random, const MessageReceiver&, MessageDispatcher&)
		{
			TEntityCoords delta;
			foreach (e.neighborhood,n)
			{
				TEntityCoords dn = e.coordinates - n->coordinates;
				Metric::Distance d2(dn);
				static const float minDist = 0.1f;
				if (d2 < minDist)
				{
					if (M::nearingZero(*d2))
					{
						float moveBy = (minDist) / 2;
						TEntityCoords pdelta;
						Metric::Direction(random).ToVector(pdelta);
						pdelta *= moveBy;
						delta += pdelta;
					}
					else
					{
						float d = *d2;
						float moveBy = (minDist - d) / 2;
						delta += dn / d * moveBy;
					}
				}
			}
			inOutShape.velocity +=  delta;
		}

		void HorizontalMinDistance(Array<BYTE>&serialState, count_t generation, const Entity&e, EntityShape&inOutShape, Random&random, const MessageReceiver&, MessageDispatcher&)
		{
			TEntityCoords delta;
			foreach(e.neighborhood, n)
			{
				TEntityCoords dn = e.coordinates - n->coordinates;
				Metric::Distance d2(dn);
				static const float minDist = 0.1f;
				if (d2 < minDist)
				{
					if (M::nearingZero(*d2))
					{
						float moveBy = (minDist) / 2;
						float pdelta = random.NextBool() ? -moveBy : moveBy;
						delta.x += pdelta;
					}
					else
					{
						float d = *d2;
						float moveBy = (minDist - d) / 2;
						delta.x += dn.x / d * moveBy;
					}
				}
			}
			inOutShape.velocity += delta;
		}

		namespace Flock
		{
			namespace Helper
			{
				float AngularDistance (const Metric::Direction2D& a0, const Metric::Direction2D& a1)
				{
					return a0.MinimalDifferenceTo(a1);
				}

				float AngularDistance (const Metric::Direction3D& a0, const Metric::Direction3D& a1)
				{
					M::TVec3<> v0,v1;
					a0.ToVector(v0);
					a1.ToVector(v1);
					return Vec::quadraticDistance(v0,v1);
				}
			}
			static const float MinSpeed = Entity::MaxMotionDistance * 0.2f * 0.05f;
			static const float MaxSpeed = Entity::MaxMotionDistance * 0.4f * 0.05f;

			void MaintainVelocity(Array<BYTE>&serialState, count_t generation, const Entity&e, EntityShape&inOutShape,Random&random, const MessageReceiver&, MessageDispatcher&)
			{
				inOutShape.velocity = e.velocity;
				float speed = e.velocity.GetLength();
				float wantSpeed = random.NextFloat(MinSpeed,MaxSpeed);
				TEntityCoords wantDirection;
				e.GetDirection(wantDirection);
				wantDirection *= wantSpeed;
				Vec::interpolate(inOutShape.velocity,wantDirection,0.5f,inOutShape.velocity);
			}

			void Focus(Array<BYTE>&serialState, count_t generation, const Entity&e, EntityShape&inOutShape,Random&random, const MessageReceiver&recv, MessageDispatcher&disp)
			{
				struct TFocusLocation
				{
					TEntityCoords loc,velocity;
					count_t gen;
				};
				if (serialState.size() != sizeof(TFocusLocation))
				{
					serialState.SetSize(sizeof(TFocusLocation));
					TFocusLocation init = {TEntityCoords(),TEntityCoords(),0};
					(*(TFocusLocation*)serialState.pointer()) = init;
				}

				TFocusLocation&best = (*(TFocusLocation*)serialState.pointer());

				foreach (e.neighborhood,n)
					if (n->userFlags & EntityFlags::IsFocus)
					{
						best.loc = n->coordinates;
						best.velocity = n->velocity;
						best.gen = generation;
						break;
					}
				if (best.gen != generation)
				{
					foreach (recv,msg)
					{
						if (msg->senderProcess == &Focus && msg->data.size() == sizeof(TFocusLocation))
						{
							const TFocusLocation&f = *(const TFocusLocation*)msg->data.pointer();
							if (f.gen > best.gen)
							{
								best = f;
							}
						}
						else
						{
							bool brk = true;
						}
					}
				}

				if (best.gen > 0)
				{
					if (e.neighborhood.IsNotEmpty())
					{
						index_t a = random.NextIndex(e.neighborhood.Count()-1);
						{
							MessageData dat(sizeof(TFocusLocation));
							memcpy(dat.pointer(),&best,sizeof(best));
							disp.DispatchFleeting(e.neighborhood[a],&Focus,std::move(dat));
						}
						//if (e.neighborhood.Count() > 1)
						//{
						//	index_t b = a;
						//	while (b == a)
						//		b = random.getIndex(e.neighborhood.Count()-1);
						//	{
						//		MessageData dat(sizeof(TFocusLocation));
						//		memcpy(dat.pointer(),&best,sizeof(best));
						//		disp.DispatchFleeting(e.neighborhood[b],&Focus,std::move(dat));
						//	}
						//}
					

					}
		/*			foreach (e.neighborhood,n)
					{
						MessageData dat(sizeof(TFocusLocation));
						memcpy(dat.pointer(),&best,sizeof(best));
						disp.DispatchFleeting(*n,&Focus,std::move(dat));
					}
	*/
					TEntityCoords delta = best.loc + best.velocity * (generation - best.gen) - e.coordinates;
					float pull = *Metric::Distance(delta);
					float finalPull = M::Min(pull,MinSpeed/10.f);
					delta *= finalPull / pull;
					inOutShape.velocity += delta;
					inOutShape.orientation = Metric::Direction(inOutShape.velocity);
				}
			}


			void AvoidCollision(Array<BYTE>&serialState, count_t generation, const Entity&e, EntityShape&inOutShape, Random&random, const MessageReceiver&, MessageDispatcher&)
			{
				TEntityCoords motion = inOutShape.velocity;
				const float orginalSpeed = Vec::length(motion);
				float desiredSpeed = orginalSpeed;
				const float lookahead = 20.f;
				if (orginalSpeed > 0)
				{
	//				NeighborhoodCollider collider( e.neighborhood);
					NeighborhoodCollider wallCollider( e.neighborhood,[](const EntityAppearance&app){return (app.userFlags & EntityFlags::IsWall) != 0;} );
					if (wallCollider.TestEdge(e.coordinates,e.coordinates+motion*lookahead,true))
					{
						float bestDistance = 0;
						float bestAngleDifference = std::numeric_limits<float>::max();
						Metric::Direction bestAngle;
						bool any = false;
						NeighborhoodCollider wallCollider( e.neighborhood,[](const EntityAppearance&app){return (app.userFlags & EntityFlags::IsWall) != 0;} );

						for (int i = 0; i < 128; i++)
						{
							Metric::Direction a (random);
							TEntityCoords dir;
							a.ToVector(dir);
							float d;
							//if (wallCollider.TestEdge(e.coordinates,e.coordinates+dir*orginalSpeed*2.f,false))
							//	continue;
							wallCollider.TestEdge(e.coordinates,e.coordinates+dir*orginalSpeed*lookahead,true,&d);

							float angleDifference = Helper::AngularDistance(a,e.orientation);
							if (d > bestDistance || (d == bestDistance && angleDifference < bestAngleDifference))
							{
								bestDistance = d;
								bestAngleDifference = angleDifference;
								any = true;
								bestAngle = a;
							}
							else
							{
								if (!any)
								bool b= true;
							}
						}
						ASSERT__ (any);

						if (bestDistance*0.5f < desiredSpeed)
						{
							desiredSpeed = bestDistance *0.5f;
						}
						inOutShape.orientation = bestAngle;
						TEntityCoords newDirection;
						inOutShape.orientation.ToVector(newDirection);
						motion = desiredSpeed * newDirection;
					
					}
				}
				inOutShape.velocity = motion;
			}

			void Align(Array<BYTE>&serialState, count_t generation, const Entity&e, EntityShape&inOutShape,Random&random, const MessageReceiver&, MessageDispatcher&)
			{

				constexpr float DistanceCutOff = 0.05f;
				constexpr float DistanceCutOff2 = DistanceCutOff*DistanceCutOff;

				float load = 2;
				TEntityCoords speed = inOutShape.velocity * load;

				foreach (e.neighborhood,n)
				{
					{
						speed += n->velocity;
						load++;
					}
				}
				inOutShape.velocity = speed / load;
				inOutShape.orientation = Metric::Direction(inOutShape.velocity);
			}

			constexpr float DistanceCutOff = 0.025f;
			constexpr float DistanceCutOff2 = DistanceCutOff*DistanceCutOff;

			void Center(Array<BYTE>&serialState, count_t generation, const Entity&e, EntityShape&inOutShape,Random&random, const MessageReceiver&, MessageDispatcher&)
			{
				if (e.neighborhood.IsEmpty())
					return;
				TEntityCoords motion = inOutShape.velocity;
				const float orginalSpeed = Vec::length(motion);
				float desiredSpeed = orginalSpeed;

				TEntityCoords center = e.coordinates;
				count_t cnt = 1;
				foreach (e.neighborhood,n)
				{
					if (n->shape != Entity::Shape::None)
						continue;
					float w = Vec::intensity(n->coordinates - e.coordinates,e.velocity)*0.5f + 0.5f;
					if (w > 0.35f)
					{
						center += n->coordinates;// + n->velocity;
						cnt ++;
					}
				}
				if (!cnt)
					return;
				center /= cnt;
				TEntityCoords wantOutTo = center - e.coordinates;
				float total = wantOutTo.GetLength();
				//if (total < DistanceCutOff*0.5f)
					//return;
				static const float Max = MaxSpeed/10.f;
				if (total > Max)
					wantOutTo *= Max / total;
				inOutShape.velocity += wantOutTo;
				inOutShape.orientation = Metric::Direction(inOutShape.velocity);
			}

			void MaintainMinimumDistance(Array<BYTE>&serialState, count_t generation, const Entity&e, EntityShape&inOutShape,Random&random, const MessageReceiver&, MessageDispatcher&)
			{
				TEntityCoords motion = inOutShape.velocity;
				const float orginalSpeed = Vec::length(motion);
				float desiredSpeed = orginalSpeed;

				static const constexpr Metric::Distance CutOff = Metric::Distance(DistanceCutOff);
				TEntityCoords wantOutTo;

				foreach (e.neighborhood,n)
				{
					float w = Vec::intensity(n->coordinates - e.coordinates,e.velocity)*0.5f + 0.5f;
					if (w > 0.35f)
					{

						//float d2 = Vec::quadraticDistance(e.coordinates,n->coordinates);
						//if (d2 < DistanceCutOff2)
						Metric::Distance	d(e.coordinates,n->coordinates);
						if (d < CutOff)
						{
							float dist =*d;
							float wantOut =  (CutOff.Squared() / d.Squared()  - 1.f);
							TEntityCoords to = wantOut * (e.coordinates - n->coordinates) / dist;
							wantOutTo += to;
						}
					}
				}
				Metric::Distance total(wantOutTo);
				static const float Max = MaxSpeed/5.f;
				if (total > Max)
					wantOutTo *= Max / *total;
				inOutShape.velocity += wantOutTo;
				inOutShape.orientation = Metric::Direction(inOutShape.velocity);
			}
	#endif
	}

	#if 0
	void UserAvatar(Array<BYTE>&serialState, count_t generation, const Entity&e, EntityShape&inOutShape,Random&random, const MessageReceiver&recv, MessageDispatcher&)
	{
		if (serialState.size() != sizeof(float))
		{
			serialState.SetSize(sizeof(float));
			(*(float*)serialState.pointer()) = 0;
		}
		float&speed = (*(float*)serialState.pointer());

		float speedGain = 0;
		Metric::Direction::Angle dirGain = Metric::Direction::Angle(0);
		static const float GainPerIteration = Flock::MaxSpeed/10.f;

		foreach (recv,msg)
			if (!msg->senderProcess && msg->data.size() == sizeof(TInputVector))
			{
				const TInputVector	&vec = (const TInputVector	&)*(msg->data.pointer());
				speedGain += GainPerIteration * vec.speedChange;
				dirGain += Metric::Direction::MaxValue/40.f * vec.directionChange;
			}


		speed = M::Clamp(speed + speedGain,0,Entity::MaxMotionDistance);
		inOutShape.orientation += dirGain;
		inOutShape.orientation.ToVector(inOutShape.velocity);
		inOutShape.velocity *= speed;

		#ifdef D3
			Aspect::scene.location = e.coordinates+inOutShape.velocity;
		#else
			Aspect::scene.location = float3(e.coordinates+inOutShape.velocity,0.f);
		#endif
		Aspect::scene.UpdateView();
	}
	#endif /*0*/

	#ifndef NO_SENSORY

	static const float WaveSpacing = Entity::MaxAdvertisementRadius * 0.9f;
	static const float WavePlaneHeight = 20.f;
	constexpr float g = 9.806f;
	constexpr float EPSILON = std::numeric_limits<float>::epsilon();

	float3 HorizontalPotential(float3 gp)
	{
		float h = M::Max(gp.x, 0.0f);
		float uh = gp.y;
		float vh = gp.z;

		float h4 = h * h * h * h;
		float u = sqrtf(2.0f) * h * uh / (sqrtf(h4 + M::Max(h4, EPSILON)));

		float3 F;
		F.x = u * h;
		F.y = uh * u + g * h * h;
		F.z = vh * u;
		return F;
	}

	float3 VerticalPotential(float3 gp)
	{
		float h = M::Max(gp.x, 0.0f);
		float uh = gp.y;
		float vh = gp.z;

		float h4 = h * h * h * h;
		float v = sqrtf(2.0f) * h * vh / (sqrtf(h4 + M::Max(h4, EPSILON)));

		float3 G;
		G.x = v * h;
		G.y = uh * v;
		G.z = vh * v + g * h * h;
		return G;
	}

	float3 SlopeForce(float3 c, float3 n, float3 e, float3 s, float3 w)
	{
		float h = M::Max(c.x, 0.0f);

		float3 H;
		H.x = 0.0f;
		H.y = 0;//-g * h * (e.w - w.w);
		H.z = 0;//-g * h * (s.w - n.w);
		return H;
	}


	
	void Wave(Array<BYTE>&serialState, count_t generation, const Entity&e, EntityShape&inOutShape, Random&random, const MessageReceiver&recv, MessageDispatcher&send)
	{
		if (serialState.size() != sizeof(float3))
		{
			serialState.SetSize(sizeof(float3));
			(*(float3*)serialState.pointer()) = float3(WavePlaneHeight,0,0);
		}

		float3		&mine = (*(float3*)serialState.pointer());
		float3		left,right,top,bottom;
		bool		haveLeft=false,haveRight=false,haveTop=false,haveBottom=false;

		foreach (recv,msg)
		{
			if (msg->senderProcess != &Wave)
				continue;
			ASSERT_EQUAL__(msg->data.size(),sizeof(float3));
			const float3&src = *(const float3*)msg->data.pointer();
			if (msg->sender.coordinates.y == e.coordinates.y)
			{
				//horizontal
				if (msg->sender.coordinates.x < e.coordinates.x)
				{
					haveLeft = true;
					left = src;
				}
				else
				{
					haveRight = true;
					right = src;
				}
			}
			else
			{
				//vertical
				if (msg->sender.coordinates.y < e.coordinates.y)
				{
					haveBottom = true;
					bottom = src;
				}
				else
				{
					haveTop = true;
					top = src;
				}
			}
		}
		if (!haveTop)
			top = mine;
		if (!haveBottom)
			bottom = mine;
		if (!haveRight)
			right = mine;
		if (!haveLeft)
			left = mine;


		constexpr float dt = 0.1f;
		static const float dx = WaveSpacing*100.f;
		static const float dy = WaveSpacing*100.f;







        //int gridx = x + 1;
        //int gridy = y + 1;


        //fixShore(west, center, east);
        //fixShore(north, center, south);
		//if (haveLeft)
		{

			float3 u_south = 0.5f * ( bottom + mine ) - dt/(2*dy) * ( VerticalPotential(mine) - VerticalPotential(bottom));
			float3 u_north = 0.5f * ( top + mine ) - dt/(2*dy) * ( VerticalPotential(top) - VerticalPotential(mine));
			float3 u_west = 0.5f * ( left + mine ) - dt/(2*dx) * ( HorizontalPotential(mine) - HorizontalPotential(left) );
			float3 u_east = 0.5f * ( right + mine ) - dt/(2*dx) * ( HorizontalPotential(right) - HorizontalPotential(mine) );

			float3 u_center = mine /*+ dt * SlopeForce(mine, top, right, bottom, left)*/ - dt/dx * ( HorizontalPotential(u_east) - HorizontalPotential(u_west) ) - dt/dy * ( VerticalPotential(u_north) - VerticalPotential(u_south) );
			u_center.x = M::Max(0.0f, u_center.x);

			mine = u_center;


		}

		foreach (e.neighborhood,n)
			if (n->userFlags == EntityFlags::IsFocus)
			{
				float x = 1.f - Metric::Distance(n->coordinates,e.coordinates).Squared() / M::Sqr(Entity::MaxAdvertisementRadius);
//				Vec::quadraticDistance(n->coordinates,e.coordinates)/M::Sqr(Entity::MaxAdvertisementRadius);
				//mine.x = x * (WavePlaneHeight +sin(generation*0.1f)* 0.1f) + (1.f - x) * mine.x;
				#ifdef D3
					mine.yz += n->velocity.xy * x *  WavePlaneHeight * 10.f;
				#else
					mine.yz += n->velocity * x *  WavePlaneHeight * 10.f;
				#endif
			}

		//if (!haveLeft)
		//	mine.x = WavePlaneHeight +sin(generation*dt)* 0.1f;


		if (e.coordinates.x >= 0.45f && e.coordinates.x < 0.6f
			&&	e.coordinates.y >= 0.45f && e.coordinates.y < 0.6f
			)
			{
				mine.x = WavePlaneHeight +sin(generation*dt*1.5f)* 0.2f;
				mine.yz = M::Vector2<>::zero;
			}
		if (isnan(mine.x))
			mine = float3(WavePlaneHeight,0,0);


		foreach (e.neighborhood,n)
		{
			send.DispatchPOD(*n,&Wave, mine);
		}
		float scale = (mine.x-WavePlaneHeight)/10.f;
		inOutShape.zOffset = scale;
		inOutShape.color = float3(scale*20.f+0.5f);
	}


}
#endif

void Evolve()
{
	TInputVector vec;
	vec.directionChange = -(int)Engine::input.pressed[Key::RightArrow];
	if (Engine::input.pressed[Key::LeftArrow])
		vec.directionChange++;
	vec.speedChange = Engine::input.pressed[Key::UpArrow];
	if (Engine::input.pressed[Key::DownArrow])
		vec.speedChange--;
	EvolveV(vec);
}

void EvolveV(const TInputVector&vec)
{
	#if 0
	if (hasAvatar && !testSimulation.IsAsync())
		testSimulation.DispatchUserMessage(avatarID, Logic::UserAvatar, &vec, sizeof(vec));
	#endif

	const size_t physical = System::getPhysicalMemory();
	static const size_t window = 3000000000;
	const size_t allowance = physical > window ? physical - window : window;


	auto success = testSimulation.RunIterationAsync(currentSetup,true);
	if (success == Simulation::Result::Delayed)
		return;

	bool forceReset = false;

	if (System::getMemoryConsumption() >= allowance)
	{
		LogMessage("Max RAM consumption ("+String(allowance)+") reached. Forcing reset.");
		forceReset = true;
	}

	#if defined RECOVERY_TEST && defined DBG_SHARD_HISTORY
		if (testSimulation.GetTopGeneration() >= 50)
			currentSetup.reliabilityLevel = INT_MAX;
	#endif

	
	std::cout << "Simulation generation "<< testSimulation.GetTopGeneration()-1 << " completed" << std::endl;

	if (success == Simulation::Result::Failed)
	{
		std::cout << "\t... failed" << std::endl;

		#ifndef RECOVERY_TEST
			StartNext(true);
		#endif
	}
	else
		if (testSimulation.GetTopGeneration() > 10000 || forceReset)
			StartNext(false);

	if (stopSimulation)
	{
		simulate = false;
		SetThreadExecutionState(ES_CONTINUOUS);
		stopSimulation = false;
		return;
	}

}

void StartSimulation()
{
	simulate = true;
	if (SetThreadExecutionState(ES_CONTINUOUS | ES_SYSTEM_REQUIRED | ES_AWAYMODE_REQUIRED) == NULL)
	   SetThreadExecutionState(ES_CONTINUOUS | ES_SYSTEM_REQUIRED);
}

bool	SimulationActive()
{
	return simulate;
}

void StopSimulation()
{
	if (!testSimulation.IsAsync())
	{
		simulate = false;
		SetThreadExecutionState(ES_CONTINUOUS);
	}
	else
		stopSimulation = true;
}


void Pause()
{
	if (simulate)
		StopSimulation();
	else
		StartSimulation();
}

void VerifyIntegrity()
{
	testSimulation.AssertIsRecovering();
}


void MakeErrorFree()
{
	currentSetup.reliabilityLevel = INT_MAX;

}

void RecoverOnly()
{
	testSimulation.RecoverOne(currentSetup,false);
	Database::ProcessDistribution();

}


void	SetupScene()
{
	std::cout << "Setting up scene "<< numResets << std::endl;
	numResets++;

	{

		const GridSize size(16);

		Array<Entity> entities(currentSetup.numEntities);
		{

			for (int i = 0; i < currentSetup.numEntities; i++)
			{
				Entity&e = entities[i];
				#ifdef D3
					e.coordinates = float3(random.NextFloat(0, size.width), random.NextFloat(0, size.height), random.NextFloat(0, size.depth));
				#else
					e.coordinates = float2(random.NextFloat(0, size.width), random.NextFloat(0, size.height));
				#endif
				//e.coordinates = float2(random.NextFloat(0,testSimulation.GetGridWidth()),0.5f);//don't care about rest
				e.shape = Entity::Shape::Sphere;
				e.scale.x = 0.05f *random.NextFloat(0.25f,1);
				e.scale.y = 0.05f *random.NextFloat(0.25f,1);
				e.orientation = Metric::Direction(random);
				//e.GenerateID();
				memset(&e.guid,0,sizeof(e.guid));
				e.guid.Data1 = (unsigned long)i;	//sufficient, more meaningful

				e.AddLogic(Logic::AggressiveRandomMotion);
				//e.AddLogic(Logic::RandomMotion);
				//e.AddLogic(Logic::HorizontalMinDistance);
			}
		}


		UINT32 seed = random.NextSeed();
		#ifdef RECOVERY_TEST
			seed = 0;
			#ifdef _DEBUG
				size = GridSize(5);
			#endif
		#endif
		testSimulation.Reset(size,Statistics::DoCompareEntityConsistency(),seed,currentSetup.numLayers,entities);
	}
	//testSimulation.Reset(2,2,true,2);

//	Random random;//(1024);

//	#ifndef _DEBUG //random hostile:



	#if 0
		//flock:
		for (index_t i = 0; i < 16; i++)
		{
			Entity e;
			{
				e.coordinates = float2(0.125f + 0.05f*i,0.9f);
				e.shape = Entity::Shape::LineCube;
				e.userFlags = EntityFlags::IsWall;
				e.scale.x = 0.025f;
				e.scale.y = 0.0125f;
				e.orientation = 0;
				e.GenerateID();

				//e.AddLogic(Logic::AggressiveRandomMotion);
				//e.AddLogic(Logic::MinDistance);
	
				testSimulation.CreateEntity(e);
			}
			{
				e.coordinates = float2(0.125f + 0.05f*i,0.1f);
				e.shape = Entity::Shape::LineCube;
				e.userFlags = EntityFlags::IsWall;
				e.scale.x = 0.025f;
				e.scale.y = 0.0125f;
				e.orientation = 0;
				e.GenerateID();

				//e.AddLogic(Logic::AggressiveRandomMotion);
				//e.AddLogic(Logic::MinDistance);
	
				testSimulation.CreateEntity(e);
			}
			{
				e.coordinates = float2(0.1f,0.125f + 0.05f*i);
				e.shape = Entity::Shape::LineCube;
				e.userFlags = EntityFlags::IsWall;
				e.scale.x = 0.025f;
				e.scale.y = 0.0125f;
				e.orientation = M_PIF/2.f;
				e.GenerateID();

				//e.AddLogic(Logic::AggressiveRandomMotion);
				//e.AddLogic(Logic::MinDistance);
	
				testSimulation.CreateEntity(e);
			}
			{
				e.coordinates = float2(0.9f,0.125f + 0.05f*i);
				e.shape = Entity::Shape::LineCube;
				e.userFlags = EntityFlags::IsWall;

				e.scale.x = 0.025f;
				e.scale.y = 0.0125f;
				e.orientation = M_PIF/2.f;
				e.GenerateID();

				//e.AddLogic(Logic::AggressiveRandomMotion);
				//e.AddLogic(Logic::MinDistance);
	
				testSimulation.CreateEntity(e);
			}



		}
		{
			Entity e;
			e.coordinates = float2(0.6f,0.6f);
			e.shape = Entity::Shape::LineCube;
			e.userFlags = EntityFlags::IsWall;

			e.scale.x = 0.025f;
			e.scale.y = 0.0125f;
			e.orientation = M_PIF/2.f;
			e.GenerateID();

			//e.AddLogic(Logic::AggressiveRandomMotion);
			//e.AddLogic(Logic::MinDistance);
	
			testSimulation.CreateEntity(e);
		}
		{
			Entity e;
			e.coordinates = float2(0.6f,0.65f);
			e.shape = Entity::Shape::LineCube;
			e.userFlags = EntityFlags::IsWall;

			e.scale.x = 0.025f;
			e.scale.y = 0.0125f;
			e.orientation = M_PIF/2.f;
			e.GenerateID();

			//e.AddLogic(Logic::AggressiveRandomMotion);
			//e.AddLogic(Logic::MinDistance);
	
			testSimulation.CreateEntity(e);
		}
		for (int i = 0; i < 30; i++)
		{
			Entity e;
			e.coordinates = float2(random.getFloat(0.2f,0.8f),random.getFloat(0.2f,0.8f));
			e.orientation = random.getFloat(M_PIF*2);
			e.userFlags = EntityFlags::IsFish;
			//M_PIF*0.25f;
			e.GenerateID();

			e.AddLogic(Logic::Flock::Align);
			e.AddLogic(Logic::Flock::MaintainVelocity);
			e.AddLogic(Logic::Flock::Focus);
			//e.AddLogic(Logic::Flock::Center);
			e.AddLogic(Logic::Flock::MaintainMinimumDistance);
			e.AddLogic(Logic::Flock::AvoidCollision);
		
			testSimulation.CreateEntity(e);
		}
	#endif /*0*/


	#if 0
		//waves:
		for (float x = 0; x < 1.f; x+= Logic::WaveSpacing)
		//for (float x = 0; x < Logic::WaveSpacing*0.5f; x+= Logic::WaveSpacing)
		for (float y = 0; y < 1.f; y+= Logic::WaveSpacing)
		//for (float y = 0; y < Logic::WaveSpacing*2.5f; y+= Logic::WaveSpacing)
		//for (float y = 0; y < Logic::WaveSpacing*0.5f; y+= Logic::WaveSpacing)
		{
			Entity e;
			e.coordinates = float2(x,y);
			e.orientation = 0;
			e.shape = Entity::Shape::SolidCube;
			e.scale = float2(Logic::WaveSpacing/4);
			//e.userFlags = EntityFlags::IsWave;
			//M_PIF*0.25f;
			e.GenerateID();

			e.AddLogic(Logic::Wave);
		
			testSimulation.CreateEntity(e);
			
		}
	#endif /*0*/


	//{
	//	Entity e;
	//	e.coordinates = float2(0.5f,0.5f);
	//	e.shape = Entity::Shape::None;
	//	e.userFlags = EntityFlags::IsFocus;
	//	e.scale.x = 0.025f;
	//	e.scale.y = 0.0125f;
	//	e.orientation = M_PIF/2.f;
	//	e.GenerateID();
	//	avatarID = e.guid;
	//	hasAvatar = true;

	//	e.AddLogic(Logic::UserAvatar);

	//	testSimulation.CreateEntity(e);
	//}
}




void	StartNext(bool failed)
{
	ASSERT__(!testSimulation.IsAsync());
	if (testSimulation.GetTopGeneration() > 0)
	{
		#ifndef RECOVERY_TEST
			if (failed)
				currentSetup = Statistics::SignalRecoveryFailed( testSimulation);
			else
				currentSetup = Statistics::SignalSimulationEnd( testSimulation);
		#endif
	}
	SetupScene();
	//dataRanges.Append().name = String(s.ToString())+"["+String(overload)+"]";

}

