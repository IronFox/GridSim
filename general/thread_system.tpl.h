

namespace System
{


#if 0
	template <typename Parameter, void (*Callback)(unsigned,const Parameter&), unsigned IterationsPerJob, bool Reduced>
		void ParallelLoop<Parameter,Callback,IterationsPerJob,Reduced>::Worker::init()
		{
			if (instance.set_affinity)
			{
				instance.mutex.lock();
					#if SYSTEM==WINDOWS
						SetThreadAffinityMask(GetCurrentThread(), DWORD_PTR(1)<<DWORD_PTR(instance.thread_counter++));	//this will work for up to 64 threads. hmm...
					#elif SYSTEM_VARIANCE==LINUX
						int index = (instance.thread_counter++);
						cpu_set_t	cpu_set;
						CPU_ZERO(&cpu_set);
						CPU_SET(index,&cpu_set);
						if (sched_setaffinity(0,sizeof(cpu_set),&cpu_set))
							cout << "Warning: CPU affinity set failed for processor #"<<index<<"\n";
					#endif
				instance.mutex.release();
			}
			if (instance.init_object)
			{

				if (instance.init_method)
					(instance.init_object->*instance.init_method)();
				else
					instance.init_object->ThreadMain();
			}
		}

	template <typename Parameter, void (*Callback)(unsigned,const Parameter&), unsigned IterationsPerJob, bool Reduced>
		void ParallelLoop<Parameter,Callback,IterationsPerJob,Reduced>::Worker::ThreadMain()
		{
			init();


			while (!application_shutting_down)
			{
				THREAD_REPORT("waiting for jobs");
				instance.job_semaphore.request();
				/*parent->mutex.lock();
					const unsigned offset = parent->offset++;
				parent->mutex.release();*/
			
				ParallelLoopBase::offset_t offset = InterlockedIncrement(instance.aligned_offset);

			
				if ((unsigned)offset > instance.num_jobs)
				{
					instance.destruct_semaphore.release();
					THREAD_REPORT("Job offset exceeds maximum. Exiting thread");
					return;
				}
				bool final = (offset == instance.num_jobs);
				offset--;
			
				//const unsigned	iterations = parent->iterations_per_job;
				unsigned		at = unsigned(offset) * IterationsPerJob;
				const unsigned	end = at + (final?instance.last_job_iterations:IterationsPerJob);
			
				THREAD_REPORT("processing range "<<at<<" .. "<<end);
			
				if (final)
					while (at < end)
						Callback(at++,*instance.parameter);
				else
					for (unsigned i = 0; i < IterationsPerJob; i++)
						Callback(at+i,*instance.parameter);

				bool terminal = !instance.terminal_semaphore.tryEnter();
				/*parent->mutex.lock();
					bool terminal = !--parent->jobs_left;
				parent->mutex.release();*/
			
				if (terminal)
				{
					THREAD_REPORT("sending terminal symbol");
					int token;
					instance.wait_pipe<<token;
				}
				THREAD_REPORT("loop completed");
			
			}

		}

	template <typename Parameter, void (*Callback)(unsigned,const Parameter&), unsigned IterationsPerJob, bool Reduced>
		ParallelLoop<Parameter,Callback,IterationsPerJob,Reduced>::Instance::Instance():ParallelLoopBase(Reduced)
		{
			volatile BYTE*address = offset_field;
			#ifdef __GNUC__
				while (((long)address)&0x3)
					address++;
			#else
				while (((__int64)address)&0x3)
					address++;
			#endif
		
			aligned_offset = (offset_t*)address;
			(*aligned_offset) = 0;
	
			unsigned num_threads = getProcessorCount();
			if (Reduced)
				if (num_threads>1)
					num_threads--;
			workers.setSize(num_threads);
		
		

			for (unsigned i = 0; i < num_threads; i++)
			{
				THREAD_REPORT("starting thread "<<i)
			   // workers[i].operation_counter = 0;
				workers[i].start();
			}
		}


	template <typename Parameter, void (*Callback)(unsigned,const Parameter&), unsigned IterationsPerJob, bool Reduced>
		ParallelLoop<Parameter,Callback,IterationsPerJob,Reduced>::Instance::~Instance()
		{
			num_jobs = 0;
			(*aligned_offset) = 1;
			job_semaphore.release(unsigned(workers.count()));
			for (index_t i = 0; i < workers.count(); i++)
				destruct_semaphore.enter();
		}

	template <typename Parameter, void (*Callback)(unsigned,const Parameter&), unsigned IterationsPerJob, bool Reduced>
		typename ParallelLoop<Parameter,Callback,IterationsPerJob,Reduced>::Instance				ParallelLoop<Parameter,Callback,IterationsPerJob,Reduced>::instance;

	template <typename Parameter, void (*Callback)(unsigned,const Parameter&), unsigned IterationsPerJob, bool Reduced>
		void	ParallelLoop<Parameter,Callback,IterationsPerJob,Reduced>::execute(unsigned iterations, const Parameter&parameter)
		{
			(*instance.aligned_offset) = 0;
			instance.num_jobs = iterations/IterationsPerJob;
			instance.last_job_iterations = iterations%IterationsPerJob;

			if (instance.last_job_iterations)
				instance.num_jobs++;
			else
				instance.last_job_iterations = IterationsPerJob;
			if (!instance.num_jobs)
				return;
			if (instance.num_jobs > 1)
			{
				instance.parameter = &parameter;
				//THREAD_REPORT("configured. releasing "<<instance.num_jobs<<" jobs");
				instance.terminal_semaphore.release(instance.num_jobs-1);
				instance.job_semaphore.release(instance.num_jobs);
			
				int token;
				instance.wait_pipe >> token;
				//THREAD_REPORT("token received");
			}
			else
			{
				for (unsigned i = 0; i < iterations; i++)
					Callback(i,parameter);
			}
		}


#endif /*0*/
	

	

	template <class Entry>
		BlockingQueue<Entry>::BlockingQueue():semaphore(0)//,waiting(false)
		{}

	template <class Entry>
		BlockingQueue<Entry>::BlockingQueue(size_t size):Queue<Entry>(size),semaphore(0)//,waiting(false)
		{}


	template <class Entry>
		void	BlockingQueue<Entry>::read(Entry&out)
		{
			while (true)
			{
				THREAD_REPORT("attempting to read element");
				semaphore.enter();
				THREAD_REPORT("semaphore not negative");
				mutex.lock();
					THREAD_REPORT("retrieving element");
					bool success = Queue<Entry>::Pop(out);
				mutex.release();
				if (success)
				{
					THREAD_REPORT("returning");
					return;
				}
				THREAD_REPORT("semaphore fault. retrying in 10.");
				sleep(10);
			}

		}
		
	template <class Entry>
		void	BlockingQueue<Entry>::operator>>(Entry&out)
		{
			read(out);
		}
		
	template <class Entry>
		BlockingQueue<Entry>&	BlockingQueue<Entry>::operator<<(const Entry&in)
		{
			write(in);
			return *this;
		}



	template <class Entry>
		void	BlockingQueue<Entry>::write(const Entry&data)
		{
			THREAD_REPORT("attempting to write element");
			mutex.lock();
				//bool was_empty = begin == end;
				//cout << "writing to "<<end<<endl;
				THREAD_REPORT("writing element");
				Queue<Entry>::Push(data);

			mutex.release();
			THREAD_REPORT("releasing semaphore by one");
			semaphore.release(1);
			THREAD_REPORT("returning");
			
		}

	template <class Entry>
		void	BlockingQueue<Entry>::beginSequence()
		{
			sequence_counter = 0;
			mutex.lock();
		}
	
	template <class Entry>
		void	BlockingQueue<Entry>::writeElement(const Entry&data)
		{
			Queue<Entry>::Push(data);
			sequence_counter++;
		}
		
	template <class Entry>
		void	BlockingQueue<Entry>::endSequence()
		{
			mutex.release();
			if (sequence_counter)
				semaphore.release((unsigned)sequence_counter);
		}


		
	template <class Entry>
		void	BlockingQueue<Entry>::insertPrimary(const Entry&data, size_t count)
		{
			if (!count)
				return;
			THREAD_REPORT("replacing queue by element");
			mutex.lock();
				//bool was_empty = begin == end;
				//cout << "writing to "<<end<<endl;
				THREAD_REPORT("writing element");
				for (size_t i = 0; i < count; i++)
					Queue<Entry>::Push(data);

			mutex.release();
			THREAD_REPORT("releasing semaphore by one");
			semaphore.release((unsigned)count);
			THREAD_REPORT("returning");
		}
	
	
	template <class Entry>
		BlockingPriorityQueue<Entry>::BlockingPriorityQueue(size_t size):PriorityQueue<Entry,int>(size),semaphore(0)//,waiting(false)
		{}


	template <class Entry>
		void	BlockingPriorityQueue<Entry>::read(Entry&out)
		{
			THREAD_REPORT("attempting to read element");
			semaphore.enter();
			THREAD_REPORT("semaphore not negative");
			mutex.lock();
				THREAD_REPORT("retrieving element");
				PriorityQueue<Entry,int>::pop(out);
			mutex.release();

			THREAD_REPORT("returning");

		}
		
	template <class Entry>
		void	BlockingPriorityQueue<Entry>::operator>>(Entry&out)
		{
			read(out);
		}
		

		
	template <class Entry>
		void	BlockingPriorityQueue<Entry>::beginSequence()
		{
			sequence_counter = 0;
			mutex.lock();
		}
	
	template <class Entry>
		void	BlockingPriorityQueue<Entry>::writeElement(const Entry&data, int priority)
		{
			PriorityQueue<Entry,int>::push(data,priority);
			sequence_counter++;
		}
		
	template <class Entry>
		void	BlockingPriorityQueue<Entry>::endSequence()
		{
			mutex.release();
			if (sequence_counter)
				semaphore.release(unsigned(sequence_counter));
		}

		
	template <class Entry>
		void	BlockingPriorityQueue<Entry>::write(const Entry&data, int priority)
		{
			THREAD_REPORT("attempting to write element");
			mutex.lock();
				//bool was_empty = begin == end;
				//cout << "writing to "<<end<<endl;
				THREAD_REPORT("writing element");
				PriorityQueue<Entry,int>::push(data,priority);

			mutex.release();
			THREAD_REPORT("releasing semaphore by one");
			semaphore.release(1);
			THREAD_REPORT("returning");
		}

		
	template <class Entry>
		void	BlockingPriorityQueue<Entry>::insertPrimary(const Entry&data, size_t count)
		{
			if (!count)
				return;
			THREAD_REPORT("replacing queue by element");
			mutex.lock();
				//bool was_empty = begin == end;
				//cout << "writing to "<<end<<endl;
				THREAD_REPORT("writing element");
				for (size_t i = 0; i < count; i++)
					PriorityQueue<Entry,int>::push(data,INT_MAX);
			mutex.release();
			THREAD_REPORT("releasing semaphore by one");
			semaphore.release((unsigned)count);
			THREAD_REPORT("returning");
		}

		
		
	template <class Queue>
		void             ThreadPoolWorker<Queue>::ThreadMain()
	    {
			ThreadPoolJob		job;

	        if (pool->init_object)
			{
				if (pool->init_method)
					(pool->init_object->*pool->init_method)();
				else
					pool->init_object->ThreadMain();
			}

	        while (true)
	        {
				THREAD_REPORT("waiting for jobs");
				pool->job_pipe >> job;
				
				if (!job.obj0)
				{
					THREAD_REPORT("received terminal symbol. returning");
					return;
				}
				THREAD_REPORT("executing");
				try
				{
					if (job.method)
					{
						(job.obj0->*job.method)();
						#if QUAD_JOBS
							if (job.obj1)
							{
								(job.obj1->*job.method)();
								if (job.obj2)
								{
									(job.obj2->*job.method)();
									if (job.obj3)
										(job.obj3->*job.method)();
								}
							}
						#endif
					}
					else
					{
						job.obj0->ThreadMain();
						#if QUAD_JOBS
							if (job.obj1)
							{
								job.obj1->ThreadMain();
								if (job.obj2)
								{
									job.obj2->ThreadMain();
									if (job.obj3)
										job.obj3->ThreadMain();
								}
							}
						#endif
					}
				}
				catch (const std::exception& exception)
				{
					pool->exception = exception;
					pool->exception_caught = true;
				}
				catch (...)
				{
					FATAL__("Unsupported exception type caught");
				};
//             cout << "-decounting...\n";


	            job.context->counter_mutex.lock();
	                (job.context->counter)--;
					//THREAD_REPORT("done. counter now "<<(job.context->counter));
					
					if (!job.context->counter)
					{
						for (unsigned i = 0; i < job.context->waiting; i++)
						{
							//THREAD_REPORT("releasing waiting thread "<<i<<"/"<<job.context->waiting);
							int token;
							job.context->wait_pipe<<token;
						}
						job.context->waiting = 0;
					}
	            job.context->counter_mutex.release();
	        }

	    }
	
	template <class Queue>
		bool        TmplThreadPool<Queue>::busy(Context*context)
	    {
	        context->counter_mutex.lock();
	            unsigned cnt = context->counter;
	        context->counter_mutex.release();
			if (exception_caught)
			{
				exception_caught = false;
				throw(exception);
			}
			
	        return cnt!=0;
	    }

    template <class Queue>
		void        TmplThreadPool<Queue>::waitUntilIdle(Context*context)
	    {
			THREAD_REPORT("waiting until idle");
		
	        context->counter_mutex.lock();
	            if (!context->counter)
	            {
	                context->counter_mutex.release();
					THREAD_REPORT("nothing to wait for");
					if (exception_caught)
					{
						exception_caught = false;
						throw(exception);
					}
	                return;
	            }
	            context->waiting++;
	        context->counter_mutex.release();

	        int token;
			THREAD_REPORT("waiting");
	        context->wait_pipe>>token;
			if (exception_caught)
			{
				exception_caught = false;
				throw(exception);
			}

	    }





    template <class Queue>
		TmplThreadPool<Queue>::TmplThreadPool(size_t num_threads,ThreadMainObject*init_object_, Thread::pMethod init_method_):/*terminated(true),*/exception_caught(false)
	    {
	        build(num_threads,init_object_,init_method_);
	    }

	
	
    template <class Queue>
		TmplThreadPool<Queue>::~TmplThreadPool()
	    {
			THREAD_REPORT("imploding due to destructor execution")
			this->TmplThreadPool<Queue>::implode();
	    }
	
	template <class Queue>
		void			TmplThreadPool<Queue>::implode()
		{
				//if (!terminated)
			{
				THREAD_REPORT("sending terminal symbols ("<<workers.count()<<")")
				
				job_pipe.insertPrimary(Job(),workers.count());
				THREAD_REPORT("waiting for threads to finish")
				for (unsigned i = 0; i < workers.count(); i++)
				{
					THREAD_REPORT("joining thread ("<<i<<")")
					workers[i].awaitCompletion();
				}
			}
			THREAD_REPORT("sleeping")
			sleep((DWORD)workers.count());
			THREAD_REPORT("delocating workers")
			workers.free();
			THREAD_REPORT("delocated")

		}	
	
	template <class Queue>
		void			TmplThreadPool<Queue>::terminate()
		{
			//terminated = true;
			//THREAD_REPORT("terminated")
			//implode();
		}
	
	template <class Queue>
		void			TmplThreadPool<Queue>::flush(ThreadPoolContext*context)
		{
			//terminated = true;
			signalIdle(context);
			terminate();
			if (exception_caught)
			{
				exception_caught = false;
				throw(exception);
			}
		}
	
	template <class Queue>
		void 		TmplThreadPool<Queue>::signalIdle(ThreadPoolContext*context)
		{
			context->counter_mutex.lock();
					
				for (unsigned i = 0; i < context->waiting; i++)
				{
					THREAD_REPORT("releasing waiting thread "<<i<<"/"<<context->waiting);
					int token;
					context->wait_pipe<<token;
				}
				context->waiting = 0;
			context->counter_mutex.release();
		}
		

	
	
	
	
	template <class Queue>
	    void            TmplThreadPool<Queue>::build(size_t num_threads,ThreadMainObject*init_object_, Thread::pMethod init_method_)
	    {
			THREAD_REPORT("implode due to build("<<num_threads<<", "<<init_object_<<", "<<init_method_<<") call")
			implode();
			//terminated = false;
			THREAD_REPORT("resizing")
			workers.setSize(num_threads);
	        init_object = init_object_;
			init_method = init_method_;

	        for (size_t i = 0; i < num_threads; i++)
	        {
				THREAD_REPORT("starting thread "<<i)
	           // workers[i].operation_counter = 0;
	            workers[i].pool = this;;
	            workers[i].start();
	        }
	    }



	template <class ThreadPool>
	    void             TmplKernel<ThreadPool>::ThreadMain()
	    {
	        #if SYSTEM==WINDOWS
	            mutex.lock();
	                SetThreadAffinityMask(GetCurrentThread(), 1<<(counter++));
	            mutex.release();
	        #elif SYSTEM_VARIANCE==LINUX
				mutex.lock();
					int index = (counter++);
					cpu_set_t	cpu_set;
					CPU_ZERO(&cpu_set);
					CPU_SET(index,&cpu_set);
					if (sched_setaffinity(0,sizeof(cpu_set),&cpu_set))
						cout << "Warning: CPU affinity set failed for processor #"<<index<<"\n";
				mutex.release();
	        #endif
	        if (init_object)
			{
				if (init_method)
					(init_object->*init_method)();
				else
					init_object->ThreadMain();
			}
	    }


	template <class ThreadPool>
	    TmplKernel<ThreadPool>::TmplKernel(ThreadMainObject*init_object, Thread::pMethod init_method):ThreadPool(0),counter(0)  //!< Constructor
	    {
	        TmplKernel<ThreadPool>::build(init_object,init_method);
	    }

	template <class ThreadPool>
	    void            TmplKernel<ThreadPool>::build(ThreadMainObject*init_object_, Thread::pMethod init_method_)
	    {
			init_object = init_object_;
			init_method = init_method_;
			
	        counter = 0;
	        ThreadPool::build(getProcessorCount(),this);
	    }
	



	template <class ThreadPool>
	    void             TmplHyperKernel<ThreadPool>::ThreadMain()
	    {
			static unsigned processors = getProcessorCount();
	        #if SYSTEM==WINDOWS
	            mutex.lock();
					unsigned index = (counter++)%processors;
					//cout << "setting processor affinity to "<<index<<endl;
	                SetThreadAffinityMask(GetCurrentThread(), 1<<index);

	            mutex.release();
	        #elif SYSTEM_VARIANCE==LINUX
				mutex.lock();
					int index = (counter++)%processors;
					cpu_set_t	cpu_set;
					CPU_ZERO(&cpu_set);
					CPU_SET(index,&cpu_set);
					if (sched_setaffinity(0,sizeof(cpu_set),&cpu_set))
						cout << "Warning: CPU affinity set failed for processor #"<<index<<"\n";
				mutex.release();
	        #endif
	        if (init_object)
			{
				if (init_method)
					(init_object->*init_method)();
				else
					init_object->ThreadMain();
			}
	    }

	template <class ThreadPool>
	    TmplHyperKernel<ThreadPool>::TmplHyperKernel(ThreadMainObject*init_object, Thread::pMethod init_method):ThreadPool(0),counter(0)  //!< Constructor
	    {
	        TmplHyperKernel<ThreadPool>::build(init_object,init_method);
	    }

	template <class ThreadPool>
	    void            TmplHyperKernel<ThreadPool>::build(ThreadMainObject*init_object_, Thread::pMethod init_method_)
	    {
			init_object = init_object_;
			init_method = init_method_;
			
	        counter = 0;
	        ThreadPool::build(getProcessorCount()*2,this);
	    }
	
    template <class ThreadPool>
		TmplHyperKernel<ThreadPool>::TmplHyperKernel(unsigned factor, ThreadMainObject*init_object, Thread::pMethod init_method):ThreadPool(0),counter(0)  //!< Constructor
	    {
	        TmplHyperKernel<ThreadPool>::build(factor, init_object,init_method);
	    }

	template <class ThreadPool>
	    void            TmplHyperKernel<ThreadPool>::build(unsigned factor, ThreadMainObject*init_object_, Thread::pMethod init_method_)
	    {
			init_object = init_object_;
			init_method = init_method_;
			
	        counter = 0;
	        ThreadPool::build(getProcessorCount()*factor,this);
	    }
	

	template <class Queue>
		typename TmplThreadPool<Queue>::Context			TmplThreadPool<Queue>::global_context;






}

