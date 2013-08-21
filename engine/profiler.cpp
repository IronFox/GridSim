#include "../global_root.h"
#include "profiler.h"

namespace Engine
{
	namespace Profiler
	{
	
		static unsigned channel_resolution=64;
		static float	record_interval=0.5,
						current_time=0;
		static Timer::Time	last_change;
		static Channel*active_channel=NULL;
		static bool		record = false;
		static OrthographicAspect<>		orthographic_aspect;
		static Camera<>					projected_aspect;

		List::ReferenceVector<Graph>	orthographic_graphs, 
										perspective_graphs;
	
		Data		time_data,
					fps_data;
		
		/*virtual*/ Graph::~Graph()
		{
			if (!application_shutting_down)
				uninstall();
		}

		/*virtual*/ void			OrthographicGraph::install()
		{
			if (!orthographic_graphs(this))
				orthographic_graphs.append(this);
		}
		
		/*virtual*/ void			OrthographicGraph::uninstall()
		{
			orthographic_graphs.drop(this);
		}
		
		TVec3<>			C3dGraph::light = {1,1,1},
						C3dGraph::background = {0.5,0.5,0.5};
		
		
		/*virtual*/ void			C3dGraph::install()
		{
			if (!perspective_graphs(this))
				perspective_graphs.append(this);
		}
		/*virtual*/ void			C3dGraph::uninstall()
		{
			perspective_graphs.drop(this);
		}


		
		const Timer::Time&	lastChange()
		{
			return last_change;
		}
		
		static void pickColor(TVec3<>&color)
		{
			static const TVec3<>	colors[] = {
				{0,1,0},
				{1,1,0},
				{0,0,1},
				{0,1,1},
				{1,0,0},
				{0,1,0.5},
				{0.5,1,0},
				{0,0.5,1},
				{1,0.5,0},
				{1,1,1},
				{0,0,0}
			};
			static unsigned at=0;
			color = colors[at];
			at = (at+1)%ARRAYSIZE(colors);
		}
		
		
		
		
		void		Channel::setResolution(unsigned res)
		{
			if (res == history.length())
				return;
			count_t old = history.length();
			history.resizePreserveContent(res);
			history.fill(0,old);
			//weighted_history.resizeCopy(res);
			//weighted_history.fill(0,old);
			minimum_history.resizePreserveContent(res);
			minimum_history.fill(0,old);
			maximum_history.resizePreserveContent(res);
			maximum_history.fill(0,old);
		}
		
		void		Channel::advance()
		{
			if (!history.length())
				return;
			history.shiftDown();
			minimum_history.shiftDown();
			maximum_history.shiftDown();
			if (!counter)
				counter++;
			if (first_advance)
			{
				faccumulated = 0;
				accumulated = 0;
				//frame_accumulated = 0;
				//weighted_accumulated = 0;
				maximum = minimum = 0;
			}
			history.last() = (faccumulated + timer.toSecondsf(accumulated))/counter;
			if (faccumulated > 0)
			{
				if (accumulated > 0)
				{
					minimum_history.last() = vmin(fminimum,timer.toSecondsf(minimum));
					maximum_history.last() = vmax(fmaximum,timer.toSecondsf(maximum));
				}
				else
				{
					minimum_history.last() = fminimum;
					maximum_history.last() = fmaximum;
				}
			}
			else
			{
				minimum_history.last() = timer.toSecondsf(minimum);
				maximum_history.last() = timer.toSecondsf(maximum);
			}
			
			
			/*
			weighted_history.shiftLeft();
			if (!weighted_counter)
				weighted_counter = 1;
			weighted_history.last() = (timer.toSecondsf(weighted_accumulated))/weighted_counter;
			*/
			
		
			faccumulated = 0;
			accumulated = 0;
			fframe_accumulated = 0;
			frame_accumulated = 0;
			//weighted_accumulated = 0;
			fmaximum = 0;
			maximum = 0;
			fminimum = 0;
			minimum = 0;
			counter = 0;
			//weighted_counter = 0;
			first_advance = false;
		}
		
		
		void		Channel::nextFrame(float weight)
		{
			if (!counter)
			{
				fmaximum = fminimum = fframe_accumulated;
				maximum = minimum = frame_accumulated;
			}
			else
			{
				fmaximum = vmax(fmaximum,fframe_accumulated);
				fminimum = vmin(fminimum,fframe_accumulated);
				maximum = vmax(maximum,frame_accumulated);
				minimum = vmin(minimum,frame_accumulated);
			}
			
			frame_accumulated = 0;
			fframe_accumulated = 0;
			counter++;
			/*
			weighted_accumulated += (Timer::Time)(frame_accumulated*weight);
			weighted_counter += weight;
			*/
		}

		
		Channel::Channel():history(Config::channelResolution()),minimum_history(Config::channelResolution()),maximum_history(Config::channelResolution()),accumulated(0),minimum(0),maximum(0),frame_accumulated(0),faccumulated(0),fminimum(0),fmaximum(0),counter(0),total(0),name("[unnamed]")
		{
			history.fill(0);
			//weighted_history.fill(0);
			maximum_history.fill(0);
			minimum_history.fill(0);
			first_advance = true;
		}
		
		void		Channel::generateColor(index_t channel_index)
		{
			/*color[0] = (float)(channel_index%3)/2.0f;
			color[1] = (float)((channel_index/3)%3)/2.0f;
			color[2] = (float)((channel_index/9)%3)/2.0f;
			_normalize0(color);*/
			if (channel_index%2)
				Vec::set(color,0.2);
			else
				Vec::set(color,1);
		}
		
		bool		Channel::isActive()	const
		{
			return active_channel == this;
		}
		
		void		Channel::setColor(float r, float g, float b)
		{
			Vec::def(color,r,g,b);
		}
		
		void		Channel::adoptData(Channel&other)
		{
			history.adoptData(other.history);
			maximum_history.adoptData(other.maximum_history);
			minimum_history.adoptData(other.minimum_history);
			//weighted_history.adoptData(other.weighted_history);
			started = other.started;
			accumulated = other.accumulated;
			faccumulated = other.faccumulated;
			frame_accumulated = other.frame_accumulated;
			fframe_accumulated = other.fframe_accumulated;
			fminimum = other.fminimum;
			fmaximum = other.fmaximum;
			minimum = other.minimum;
			maximum = other.maximum;
			counter = other.counter;
			//weighted_counter = other.weighted_counter;
			color = other.color;
			name.adoptData(other.name);
		}
		
		void		Channel::start()
		{
			if (active_channel)
				active_channel->stop();
			active_channel = this;
			started = timer.now();
		}
		
		Timer::Time		Channel::stop()
		{
			if (record)
			{
				Timer::Time delta = timer.now()-started;
				accumulated += delta;
				frame_accumulated += delta;
				total += delta;
				active_channel = NULL;
				return delta;
			}
			active_channel = NULL;
			return 0;
		}
		
		
		
		
		void		Channel::accumulatei(const Timer::Time&t)
		{
			accumulated += t;
			frame_accumulated += t;
			total += t;
		}

		
		void		Channel::accumulate(float value)
		{
			faccumulated += value;
			fframe_accumulated += value;
		}
		
		
		void		Group::adoptData(Group&other)
		{
			Array<Channel,Adopt>::adoptData(other);
			color = other.color;
			name.adoptData(other.name);
		}
		
		Group::Group():res(Config::channelResolution()),name("[unnamed]")
		{
			pickColor(color);
		}
		
		void Group::setResolution(unsigned res_)
		{
			if (res == res_)
				return;
			for (unsigned i = 0; i < count(); i++)
				operator[](i).setResolution(res_);
			res = res_;
		}

		
		
		namespace Config
		{
			unsigned	channelResolution()
						{
							return channel_resolution;
						}
			void		setChannelResolution(unsigned resolution)
						{
							channel_resolution = resolution;
							fps_data.setResolution(resolution);
							time_data.setResolution(resolution);
						}
			void		setRecordInterval(float interval)
						{
							record_interval = interval;
						}
						
			float		recordInterval()
						{
							return record_interval;
						}
		}
		
		Data::Data():res(Config::channelResolution())
		{}
		
		count_t		Data::countChannels()	const
		{
			count_t rs = 0;
			for (index_t i = 0; i < count(); i++)
				rs += operator[](i).channels();
			return rs;
		
		}
		
		Channel&		Data::openChannel(index_t major_channel, index_t minor_channel)
		{
			if (major_channel >= count())
			{
				count_t pre = count();
				resizePreserveContent(major_channel+1);
				for (index_t i = pre; i <= major_channel; i++)
					Array<Group,Adopt>::operator[](i).setResolution(res);
			}
			Group&group = Array<Group,Adopt>::operator[](major_channel);
			if (minor_channel >= group.channels())
			{
				count_t pre = group.channels();
				group.resizePreserveContent(minor_channel+1);
				for (index_t i = pre; i <= minor_channel; i++)
				{
					group.channel(i).setResolution(res);
					group.channel(i).generateColor(i);
				}
				
			}
			Channel&ch = group.channel(minor_channel);
			return ch;
		}
		
		Channel&		Data::activate(index_t major_channel, index_t minor_channel)
		{
			Channel&ch = openChannel(major_channel,minor_channel);
			Profiler::start(&ch);
			return ch;
		}
		
		void			Data::setResolution(unsigned res_)
		{
			if (res == res_)
				return;
			for (unsigned i = 0; i < count(); i++)
				Array<Group,Adopt>::operator[](i).setResolution(res_);
			res = res_;
		}
				
		/*void			Data::update()
		{
			update(timing.delta);
		}*/
		
		void			Data::update(float weight)
		{
			for (unsigned i = 0; i < count(); i++)
				for (unsigned j = 0; j < Array<Group,Adopt>::operator[](i).channels(); j++)
					Array<Group,Adopt>::operator[](i).channel(j).nextFrame(weight);
		}
		
		void			Data::advance()
		{
			for (unsigned i = 0; i < count(); i++)
				for (unsigned j = 0; j < Array<Group,Adopt>::operator[](i).channels(); j++)
					Array<Group,Adopt>::operator[](i).channel(j).advance();
		}

		
	
		Channel*		activeChannel()
						{
							return active_channel;
						}
		Channel*		openChannel(unsigned major_channel, unsigned minor_channel)
						{
							if (active_channel)
								active_channel->stop();
							return &time_data.openChannel(major_channel,minor_channel);
						}
		Channel*		start(unsigned major_channel, unsigned minor_channel)
						{
							if (active_channel)
								active_channel->stop();
							Channel*ch = openChannel(major_channel,minor_channel);
							ch->start();
							return ch;
						}
		Timer::Time	stop()
						{
							if (active_channel)
								return active_channel->stop();
							return 0;
						}
		Channel*		start(Channel*channel)
						{
							Channel*old_channel = active_channel;
							active_channel = channel;
							if (old_channel != active_channel)
							{
								if (old_channel)
									old_channel->stop();
								if (active_channel)
									active_channel->start();
							}
							return old_channel;
						}
		
		static Buffer<Channel*>	channel_stack;
		
		Channel*		replace(unsigned major_channel, unsigned minor_channel)
						{
							channel_stack << active_channel;
							return start(major_channel,minor_channel);
						}
						
		Timer::Time	restore()
						{
							Timer::Time	result;
							if (active_channel)
								result = active_channel->stop();
							else
								result = 0;
							start(channel_stack.pop());
							return result;
						}
					
		
		bool			update()
						{
							return update(timing.delta);
						}
						
		bool			update(float delta)
						{
			
							bool was_unset = !fps_data.groups();
							
							Channel&fps_channel = fps_data.openChannel(0,0);
							if (was_unset)
							{
								fps_data.group(0).set("FPS",0.8,0.85,1);
								fps_channel.set("FPS",0.8,0.85,1);
							}
							fps_channel.accumulate(timing.delta>0?1.0f/timing.delta:0);
							
							
							static Timer::Time t=timer.now();
							Timer::Time now = timer.now();
							
							static float times[5] = {0,0,0,0,0};
							float d = timer.toSecondsf(now-t);
							for (index_t i = 0; i < ARRAYSIZE(times)-1; i++)
								times[i] = times[i+1];
							times[ARRAYSIZE(times)-1] = d;
							float base = vmin(vmin(vmin(times[0],times[1]),vmin(times[2],times[3])),times[4]);
							float weight = d > base*2;;

							
							time_data.update(weight);
							fps_data.update(weight);
							t = now;
														
							
							current_time += delta;
							bool has_changed = false;
							if (current_time > record_interval)
								if (active_channel)
									active_channel->stop();
							while (current_time > record_interval)
							{
								has_changed = true;
								record = true;
								current_time -= record_interval;
								time_data.advance();
								fps_data.advance();
							}
							if (has_changed)
							{
								last_change = timer.now();
								if (active_channel)
									active_channel->start();
								orthographic_graphs.reset();
								while (Graph*graph = orthographic_graphs.each())
									graph->update();
								perspective_graphs.reset();
								while (Graph*graph = perspective_graphs.each())
									graph->update();
							}
							return has_changed;

						}
		void			renderOrthographic()
						{
							orthographic_graphs.reset();
							while (Graph*graph = orthographic_graphs.each())
								graph->draw();
						}
		void			renderPerspective()
						{
							perspective_graphs.reset();
							while (Graph*graph = perspective_graphs.each())
								graph->draw();
						}
	}
}
