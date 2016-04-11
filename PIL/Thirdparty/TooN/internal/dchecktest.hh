
namespace TooN {

struct Argument_Needed_For_Dynamic_Parameter{};

namespace Internal {
	template<int N>
	struct DCheck {
		static int val() {return N;}
	};
	
	template<>
	struct DCheck<-1>{
		static Argument_Needed_For_Dynamic_Parameter val(); // {return ArgumentNeededForDynamicParameter();}
	};
}

}

