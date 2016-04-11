namespace TooN {

namespace Internal
{

	

	#if defined  TOON_CHECK_BOUNDS  || defined TOON_TEST_INTERNALS
		static inline void check_index(int s, int i)
		{
			if(i<0 || i >= s)
			{
				#ifdef TOON_TEST_INTERNALS
					throw Internal::BadIndex();
				#else
					std::cerr << "Toon index out of range" << std::endl;
					std::abort();
				#endif
			}
		}
	#else
		///@internal
		///Function used to check bounds.
		///By default it does nothing. See \ref sDebug.
		static inline void check_index(int, int){}
	#endif

	#if defined TOON_INITIALIZE_SNAN
		template<class P> static void debug_initialize(P* data, int n)
		{	
			using std::numeric_limits;
			for(int i=0; i < n; i++)
				data[i] = numeric_limits<P>::signaling_NaN();
		}
	#elif defined TOON_INITIALIZE_QNAN || defined TOON_INITIALIZE_NAN
		template<class P> static void debug_initialize(P* data, int n)
		{	
			using std::numeric_limits;
			for(int i=0; i < n; i++)
				data[i] = numeric_limits<P>::quiet_NaN();
		}
	#elif defined TOON_INITIALIZE_VAL
		template<class P> static void debug_initialize(P* data, int n)
		{	
			for(int i=0; i < n; i++)
				data[i] = TOON_INITIALIZE_VAL;
		}
	#elif defined TOON_INITIALIZE_RANDOM
		union intbits
		{
			unsigned long i;
			char c[4];
		};

		template<class P> union datafail
		{
			int i;
			P p;
		};


		template<class P> static void debug_initialize(P* data, int n)
		{
			//Get a random seed. Precision limited to 1 second.
			static intbits random = { ((std::time(NULL) & 0xffffffff) *1664525L + 1013904223L)& 0xffffffff};
			unsigned char* cdata = reinterpret_cast<unsigned char*>(data);

			size_t bytes = sizeof(P)*n, i=0;
			
			//Do nothing except for noisy failure with non-POD types.
			datafail<P> d={0};
			bytes+=d.i;
			
			switch(bytes & 0x3 )
			{
				for(i=0; i < bytes;)
				{
					//Really evil random number generator from NR.
					//About 4x faster than Mersenne twister. Good quality
					//is not needed, since it's only used to shake up the program
					//state to test for uninitialized values. Also, it doesn't disturb
					//the standard library random number generator.
					
					case 0:
						cdata[i++] = random.c[0];	
					case 3:
						cdata[i++] = random.c[1];	
					case 2:
						cdata[i++] = random.c[2];	
					case 1:
						cdata[i++] = random.c[3];	
					random.i = (1664525L * random.i + 1013904223L) & 0xffffffff;
				}
			}
		}
	#else
		///@internal
		///@brief This function is called on any uninitialized data. By default, no action is taken. 
		///See \ref sDebug
		template<class P> static void debug_initialize(P*, int)
		{
		}
	#endif
	

}

}
