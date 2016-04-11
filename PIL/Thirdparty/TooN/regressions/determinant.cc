#include "regressions/regression.h"
int main(){
	{
		Matrix<2> m = Data(1.36954561915420e-01, 5.07440445575612e-01, 4.40579637085006e-01, 2.69269087931921e-01);
		cout << setprecision(14) << determinant(m) << " " << determinant_gaussian_elimination(m) << " " << determinant_LU(m) << endl;
		Matrix<> n = m;
		cout << setprecision(14) << determinant(n) << " " << determinant_gaussian_elimination(n) << " " << determinant_LU(n) << endl;
	}

	{
		Matrix<2> m = Data(6.33333470514712e-01, 4.93941807913729e-01, 6.92126254554125e-01, 7.34433297395312e-01);
		cout << setprecision(14) << determinant(m) << " " << determinant_gaussian_elimination(m) << " " << determinant_LU(m) << endl;
		Matrix<> n = m;
		cout << setprecision(14) << determinant(n) << " " << determinant_gaussian_elimination(n) << " " << determinant_LU(n) << endl;
	}

	{
		Matrix<2> m = Data(1.76413099536679e-01, 2.60216176256871e-01, 3.08850368383576e-01, 1.52272949704579e-02);
		cout << setprecision(14) << determinant(m) << " " << determinant_gaussian_elimination(m) << " " << determinant_LU(m) << endl;
		Matrix<> n = m;
		cout << setprecision(14) << determinant(n) << " " << determinant_gaussian_elimination(n) << " " << determinant_LU(n) << endl;
	}

	{
		Matrix<2> m = Data(3.88523551845252e-01, 1.25338828369562e-01, 8.13964004172765e-01, 2.19819623220283e-01);
		cout << setprecision(14) << determinant(m) << " " << determinant_gaussian_elimination(m) << " " << determinant_LU(m) << endl;
		Matrix<> n = m;
		cout << setprecision(14) << determinant(n) << " " << determinant_gaussian_elimination(n) << " " << determinant_LU(n) << endl;
	}

	{
		Matrix<2> m = Data(5.55188910865601e-01, 6.57965205670083e-01, 4.15793049697370e-01, 6.75896220658060e-01);
		cout << setprecision(14) << determinant(m) << " " << determinant_gaussian_elimination(m) << " " << determinant_LU(m) << endl;
		Matrix<> n = m;
		cout << setprecision(14) << determinant(n) << " " << determinant_gaussian_elimination(n) << " " << determinant_LU(n) << endl;
	}

	{
		Matrix<2> m = Data(4.08447197123214e-01, 2.63040277790550e-01, 7.56602015669260e-02, 5.83795216528427e-01);
		cout << setprecision(14) << determinant(m) << " " << determinant_gaussian_elimination(m) << " " << determinant_LU(m) << endl;
		Matrix<> n = m;
		cout << setprecision(14) << determinant(n) << " " << determinant_gaussian_elimination(n) << " " << determinant_LU(n) << endl;
	}

	{
		Matrix<2> m = Data(6.60452866329957e-01, 4.17386245416842e-01, 4.37274269538722e-01, 7.99343646936289e-01);
		cout << setprecision(14) << determinant(m) << " " << determinant_gaussian_elimination(m) << " " << determinant_LU(m) << endl;
		Matrix<> n = m;
		cout << setprecision(14) << determinant(n) << " " << determinant_gaussian_elimination(n) << " " << determinant_LU(n) << endl;
	}

	{
		Matrix<2> m = Data(2.44005935455320e-01, 4.91469211584358e-02, 2.70848173957733e-02, 3.64328544937310e-01);
		cout << setprecision(14) << determinant(m) << " " << determinant_gaussian_elimination(m) << " " << determinant_LU(m) << endl;
		Matrix<> n = m;
		cout << setprecision(14) << determinant(n) << " " << determinant_gaussian_elimination(n) << " " << determinant_LU(n) << endl;
	}

	{
		Matrix<2> m = Data(2.78487643313992e-01, 2.02171532521378e-01, 6.16894848985417e-01, 8.75779770124611e-01);
		cout << setprecision(14) << determinant(m) << " " << determinant_gaussian_elimination(m) << " " << determinant_LU(m) << endl;
		Matrix<> n = m;
		cout << setprecision(14) << determinant(n) << " " << determinant_gaussian_elimination(n) << " " << determinant_LU(n) << endl;
	}

	{
		Matrix<2> m = Data(1.55690488795012e-01, 8.99338728730216e-01, 2.60344574436150e-01, 1.69080554093970e-01);
		cout << setprecision(14) << determinant(m) << " " << determinant_gaussian_elimination(m) << " " << determinant_LU(m) << endl;
		Matrix<> n = m;
		cout << setprecision(14) << determinant(n) << " " << determinant_gaussian_elimination(n) << " " << determinant_LU(n) << endl;
	}

	{
		Matrix<3> m = Data(6.90021150920451e-01, 8.31931522264502e-01, 7.97758982120577e-01, 6.77038637453940e-02, 1.11675504817599e-01, 5.54139426976072e-01, 2.90556101356847e-01, 1.06217460868265e-01, 9.67272693613824e-01);
		cout << setprecision(14) << determinant(m) << " " << determinant_gaussian_elimination(m) << " " << determinant_LU(m) << endl;
		Matrix<> n = m;
		cout << setprecision(14) << determinant(n) << " " << determinant_gaussian_elimination(n) << " " << determinant_LU(n) << endl;
	}

	{
		Matrix<3> m = Data(3.95578211514861e-01, 5.38439072491004e-01, 2.48583336989898e-01, 2.35136576943800e-01, 5.12747960597765e-02, 5.15931258219743e-01, 2.99053128597866e-01, 7.55952692058674e-01, 3.21254484137218e-02);
		cout << setprecision(14) << determinant(m) << " " << determinant_gaussian_elimination(m) << " " << determinant_LU(m) << endl;
		Matrix<> n = m;
		cout << setprecision(14) << determinant(n) << " " << determinant_gaussian_elimination(n) << " " << determinant_LU(n) << endl;
	}

	{
		Matrix<3> m = Data(5.25179194428188e-01, 8.30298421516969e-01, 4.79026869389020e-01, 5.25006522404920e-01, 3.76188158583889e-01, 6.12617838270040e-01, 8.48734029924822e-01, 4.99178260400046e-01, 1.14161624199738e-01);
		cout << setprecision(14) << determinant(m) << " " << determinant_gaussian_elimination(m) << " " << determinant_LU(m) << endl;
		Matrix<> n = m;
		cout << setprecision(14) << determinant(n) << " " << determinant_gaussian_elimination(n) << " " << determinant_LU(n) << endl;
	}

	{
		Matrix<3> m = Data(1.25896928575379e-01, 2.76654657280153e-01, 4.97744484508350e-01, 7.21766354640397e-01, 2.18315038612563e-02, 3.24133435582330e-01, 6.99898354510819e-01, 8.77350909043173e-01, 4.61347806920723e-01);
		cout << setprecision(14) << determinant(m) << " " << determinant_gaussian_elimination(m) << " " << determinant_LU(m) << endl;
		Matrix<> n = m;
		cout << setprecision(14) << determinant(n) << " " << determinant_gaussian_elimination(n) << " " << determinant_LU(n) << endl;
	}

	{
		Matrix<3> m = Data(6.19315008875198e-01, 3.16340238427387e-01, 8.68123433418303e-01, 4.21088622539474e-01, 2.51067192196979e-01, 2.97467897931370e-01, 5.74224653649563e-01, 8.35310663540258e-01, 6.86700114194084e-02);
		cout << setprecision(14) << determinant(m) << " " << determinant_gaussian_elimination(m) << " " << determinant_LU(m) << endl;
		Matrix<> n = m;
		cout << setprecision(14) << determinant(n) << " " << determinant_gaussian_elimination(n) << " " << determinant_LU(n) << endl;
	}

	{
		Matrix<3> m = Data(6.93895766634135e-01, 8.11449917296234e-01, 5.63520809740165e-01, 9.06347644339764e-01, 4.15416343251668e-01, 6.56277265358439e-01, 9.69685276853739e-01, 2.10970289210663e-02, 5.77207282521436e-02);
		cout << setprecision(14) << determinant(m) << " " << determinant_gaussian_elimination(m) << " " << determinant_LU(m) << endl;
		Matrix<> n = m;
		cout << setprecision(14) << determinant(n) << " " << determinant_gaussian_elimination(n) << " " << determinant_LU(n) << endl;
	}

	{
		Matrix<3> m = Data(2.37857910250608e-01, 5.57784312164023e-01, 4.92705818179744e-01, 2.94128593480721e-02, 6.19077492309405e-01, 6.79558062981238e-01, 8.21177413351666e-01, 5.82183747324961e-01, 9.38330581806202e-01);
		cout << setprecision(14) << determinant(m) << " " << determinant_gaussian_elimination(m) << " " << determinant_LU(m) << endl;
		Matrix<> n = m;
		cout << setprecision(14) << determinant(n) << " " << determinant_gaussian_elimination(n) << " " << determinant_LU(n) << endl;
	}

	{
		Matrix<3> m = Data(2.20550433261086e-01, 2.98754845934495e-02, 2.74747600337341e-01, 5.24307758078751e-01, 6.19855432287777e-01, 2.38619269842085e-01, 7.14483482497759e-01, 3.78399144258969e-01, 1.93442693072056e-01);
		cout << setprecision(14) << determinant(m) << " " << determinant_gaussian_elimination(m) << " " << determinant_LU(m) << endl;
		Matrix<> n = m;
		cout << setprecision(14) << determinant(n) << " " << determinant_gaussian_elimination(n) << " " << determinant_LU(n) << endl;
	}

	{
		Matrix<3> m = Data(5.17969051657360e-01, 1.59064269371209e-01, 4.89217203037848e-01, 7.20681040442171e-01, 3.38686026553331e-01, 1.70159499323172e-01, 1.02432934599155e-01, 7.52580685685526e-01, 8.86384897246990e-01);
		cout << setprecision(14) << determinant(m) << " " << determinant_gaussian_elimination(m) << " " << determinant_LU(m) << endl;
		Matrix<> n = m;
		cout << setprecision(14) << determinant(n) << " " << determinant_gaussian_elimination(n) << " " << determinant_LU(n) << endl;
	}

	{
		Matrix<3> m = Data(5.35364107383601e-01, 7.67825923766655e-01, 5.49823145365650e-01, 6.40856778882242e-01, 9.63920605061494e-01, 3.89540555593080e-01, 8.28487261593980e-01, 4.09307725195978e-01, 5.38904901216016e-01);
		cout << setprecision(14) << determinant(m) << " " << determinant_gaussian_elimination(m) << " " << determinant_LU(m) << endl;
		Matrix<> n = m;
		cout << setprecision(14) << determinant(n) << " " << determinant_gaussian_elimination(n) << " " << determinant_LU(n) << endl;
	}

	{
		Matrix<4> m = Data(6.31998788314413e-01, 9.73760212367266e-01, 3.08867522009234e-01, 5.04044604832999e-01, 8.00069206025706e-01, 9.99382355663476e-01, 7.58924495091854e-01, 6.99745358887332e-01, 7.58030507477543e-01, 7.31358173985778e-01, 8.83207868809474e-01, 6.55592443384357e-01, 4.80335196335811e-01, 6.41407083656371e-01, 4.07598318503830e-01, 8.90912566122183e-01);
		cout << setprecision(14) << determinant(m) << " " << determinant_gaussian_elimination(m) << " " << determinant_LU(m) << endl;
		Matrix<> n = m;
		cout << setprecision(14) << determinant(n) << " " << determinant_gaussian_elimination(n) << " " << determinant_LU(n) << endl;
	}

	{
		Matrix<4> m = Data(7.74334991123453e-01, 8.92792753083005e-01, 7.18463542228311e-01, 3.72428408758461e-01, 7.82808146980001e-01, 3.72813059194352e-01, 6.38726034933220e-01, 8.78171733588292e-01, 7.20040796180532e-01, 9.61127898698709e-01, 1.43114091710088e-01, 7.67366630435602e-01, 3.63520123203509e-01, 8.74396656248534e-01, 2.70689848252612e-01, 2.17037861040468e-01);
		cout << setprecision(14) << determinant(m) << " " << determinant_gaussian_elimination(m) << " " << determinant_LU(m) << endl;
		Matrix<> n = m;
		cout << setprecision(14) << determinant(n) << " " << determinant_gaussian_elimination(n) << " " << determinant_LU(n) << endl;
	}

	{
		Matrix<4> m = Data(2.93527129614395e-01, 1.45890026595788e-01, 3.82956306092162e-01, 5.93629750611561e-01, 3.54555898622876e-01, 3.63414638888524e-01, 7.40603178940656e-01, 7.14616161828011e-01, 8.53557687624533e-03, 6.48490354501316e-01, 9.20349626424737e-01, 1.59752483361758e-01, 6.84193046695865e-01, 7.81961572209509e-01, 5.50969845786340e-01, 6.79823911477558e-01);
		cout << setprecision(14) << determinant(m) << " " << determinant_gaussian_elimination(m) << " " << determinant_LU(m) << endl;
		Matrix<> n = m;
		cout << setprecision(14) << determinant(n) << " " << determinant_gaussian_elimination(n) << " " << determinant_LU(n) << endl;
	}

	{
		Matrix<4> m = Data(6.93107439415493e-01, 7.71250729887264e-01, 2.89436180433845e-01, 9.68734458337373e-02, 9.69162062760386e-02, 6.52255600415049e-01, 1.52635260944129e-01, 9.47806895809457e-01, 1.98217953448501e-01, 1.72943245096034e-01, 7.81728977919013e-01, 7.37759950962994e-01, 7.63842183179273e-01, 5.48921809610796e-01, 6.02454337326163e-01, 8.48286395785509e-01);
		cout << setprecision(14) << determinant(m) << " " << determinant_gaussian_elimination(m) << " " << determinant_LU(m) << endl;
		Matrix<> n = m;
		cout << setprecision(14) << determinant(n) << " " << determinant_gaussian_elimination(n) << " " << determinant_LU(n) << endl;
	}

	{
		Matrix<4> m = Data(2.09456708099802e-01, 7.95333085732939e-01, 6.76434153965128e-01, 6.62641402399578e-01, 5.21754965708820e-01, 2.53660930345825e-01, 4.16565870616613e-01, 4.96068902420179e-01, 5.54788311265565e-02, 8.86961897906333e-01, 8.39513431532884e-01, 1.99732879363206e-01, 6.67601987196954e-01, 6.34435223024121e-01, 1.61995072264232e-01, 3.48387002135752e-01);
		cout << setprecision(14) << determinant(m) << " " << determinant_gaussian_elimination(m) << " " << determinant_LU(m) << endl;
		Matrix<> n = m;
		cout << setprecision(14) << determinant(n) << " " << determinant_gaussian_elimination(n) << " " << determinant_LU(n) << endl;
	}

	{
		Matrix<4> m = Data(3.16597068508537e-01, 3.77516373491959e-01, 9.97607343596163e-01, 7.88863195306705e-02, 3.03283706925791e-01, 6.21524422403540e-01, 5.14514621807263e-01, 7.64309429202428e-01, 6.27443613729029e-01, 3.22814260560820e-01, 3.71185552522174e-01, 1.06536617390620e-01, 5.82213151683078e-01, 5.40132953759879e-01, 3.83308845215937e-01, 6.03948057966203e-01);
		cout << setprecision(14) << determinant(m) << " " << determinant_gaussian_elimination(m) << " " << determinant_LU(m) << endl;
		Matrix<> n = m;
		cout << setprecision(14) << determinant(n) << " " << determinant_gaussian_elimination(n) << " " << determinant_LU(n) << endl;
	}

	{
		Matrix<4> m = Data(2.56803014815598e-01, 6.15241124350821e-01, 1.51456466795590e-03, 7.09518449556452e-01, 4.25632653613925e-01, 1.25677950424214e-01, 9.30803791745417e-01, 4.17379412668159e-01, 5.02550044921844e-01, 3.46238226285804e-01, 2.41270917187225e-01, 9.78130977172992e-01, 6.76711090534688e-01, 7.82090883034153e-01, 8.66574202144397e-01, 9.98451079380616e-01);
		cout << setprecision(14) << determinant(m) << " " << determinant_gaussian_elimination(m) << " " << determinant_LU(m) << endl;
		Matrix<> n = m;
		cout << setprecision(14) << determinant(n) << " " << determinant_gaussian_elimination(n) << " " << determinant_LU(n) << endl;
	}

	{
		Matrix<4> m = Data(3.12634459621086e-04, 9.56412304025577e-01, 4.93352863347731e-01, 1.63408245010880e-01, 6.94062984488828e-01, 8.81994441860080e-01, 4.17494501530825e-01, 9.23991653192706e-01, 8.65148239470971e-01, 4.64503297387623e-02, 8.73043354903065e-01, 4.25939122258307e-01, 5.14171189406589e-01, 7.97394747093327e-02, 6.87467069832980e-01, 5.40641514317958e-01);
		cout << setprecision(14) << determinant(m) << " " << determinant_gaussian_elimination(m) << " " << determinant_LU(m) << endl;
		Matrix<> n = m;
		cout << setprecision(14) << determinant(n) << " " << determinant_gaussian_elimination(n) << " " << determinant_LU(n) << endl;
	}

	{
		Matrix<4> m = Data(3.39658473447714e-01, 6.26092761085018e-01, 1.53209748518362e-01, 3.20747393550340e-03, 7.96119553037662e-01, 8.32635654321941e-01, 7.80892057977699e-01, 5.52390129688029e-01, 5.45432146378685e-01, 3.25522435927600e-01, 6.28919956105013e-01, 9.55227528682191e-01, 3.23929438331184e-01, 3.37967232594177e-01, 5.53120665456127e-01, 9.47443637273142e-01);
		cout << setprecision(14) << determinant(m) << " " << determinant_gaussian_elimination(m) << " " << determinant_LU(m) << endl;
		Matrix<> n = m;
		cout << setprecision(14) << determinant(n) << " " << determinant_gaussian_elimination(n) << " " << determinant_LU(n) << endl;
	}

	{
		Matrix<4> m = Data(2.80659370718153e-01, 8.77845531530688e-01, 3.57113263015425e-01, 7.40476151215130e-02, 1.87560299861532e-01, 7.04728135724626e-01, 4.48745908569793e-01, 2.12063004698260e-01, 9.01283472635972e-01, 4.15800487949385e-01, 1.77784890381720e-01, 2.87762350351412e-02, 9.17484388422937e-01, 6.73842810867808e-01, 6.98413771440294e-01, 3.34442707662181e-01);
		cout << setprecision(14) << determinant(m) << " " << determinant_gaussian_elimination(m) << " " << determinant_LU(m) << endl;
		Matrix<> n = m;
		cout << setprecision(14) << determinant(n) << " " << determinant_gaussian_elimination(n) << " " << determinant_LU(n) << endl;
	}

	{
		Matrix<5> m = Data(6.61278472445204e-01, 7.44176305882686e-01, 1.97425772464288e-01, 6.08350291005180e-01, 9.01177085945009e-01, 9.65819092554194e-01, 9.72426419016103e-01, 9.27333163467757e-01, 3.64230852432365e-01, 5.81861406938673e-01, 5.55242650690258e-01, 2.31920595441028e-01, 7.63258217897842e-01, 7.33110012173132e-01, 2.93200270591155e-01, 4.41983668489113e-01, 6.35568658105070e-01, 6.96734355459148e-01, 6.80851026924895e-01, 6.19337548653006e-01, 5.22984960899778e-01, 6.53283440442012e-01, 8.29817563879325e-01, 6.23625493378364e-01, 1.99693883253539e-01);
		cout << setprecision(14) << determinant(m) << " " << determinant_gaussian_elimination(m) << " " << determinant_LU(m) << endl;
		Matrix<> n = m;
		cout << setprecision(14) << determinant(n) << " " << determinant_gaussian_elimination(n) << " " << determinant_LU(n) << endl;
	}

	{
		Matrix<5> m = Data(5.93257713948681e-01, 8.87975641932238e-01, 6.33572338420259e-01, 4.12408571166541e-01, 2.39942173700552e-01, 3.35428728089550e-01, 4.03485324638132e-01, 6.85381425534439e-01, 7.68195738721624e-01, 4.13658310211090e-01, 2.53571918734394e-02, 8.86715229354483e-01, 8.44886800746036e-01, 6.77463265097230e-01, 1.10368477778113e-01, 6.54107914762155e-01, 7.33834001525343e-01, 3.06028383220892e-01, 1.43645054046214e-01, 2.37172337913824e-01, 2.60613978586240e-01, 7.24127905681678e-01, 3.97953891569877e-01, 8.91807424384919e-02, 5.23209443301202e-01);
		cout << setprecision(14) << determinant(m) << " " << determinant_gaussian_elimination(m) << " " << determinant_LU(m) << endl;
		Matrix<> n = m;
		cout << setprecision(14) << determinant(n) << " " << determinant_gaussian_elimination(n) << " " << determinant_LU(n) << endl;
	}

	{
		Matrix<5> m = Data(2.39462796892182e-01, 9.37872936011089e-01, 2.22061393923331e-01, 6.52145567113405e-01, 4.92759820969214e-01, 8.52748960485213e-01, 3.41644626810745e-01, 4.29336083728000e-01, 4.76450680465151e-01, 1.15056290859390e-01, 7.65942075032089e-01, 7.66905656383551e-01, 3.76597759449075e-01, 7.81499388919150e-01, 8.70783613077809e-01, 3.32266593531209e-01, 6.79700718203678e-01, 7.10135010710740e-01, 4.85373879386974e-01, 6.19067455900779e-01, 1.74422954786218e-01, 3.39247892632025e-01, 2.26088551497986e-01, 2.84647212250385e-01, 1.37287780669858e-01);
		cout << setprecision(14) << determinant(m) << " " << determinant_gaussian_elimination(m) << " " << determinant_LU(m) << endl;
		Matrix<> n = m;
		cout << setprecision(14) << determinant(n) << " " << determinant_gaussian_elimination(n) << " " << determinant_LU(n) << endl;
	}

	{
		Matrix<5> m = Data(7.69596599449678e-01, 1.80960464007782e-02, 1.32548834555804e-01, 9.12040322702435e-01, 5.25354034546498e-01, 9.41775375418082e-01, 4.44906928277018e-01, 6.81080325582201e-01, 9.84192080252163e-01, 8.89100182340677e-01, 1.01817060134196e-01, 2.11431859037615e-02, 4.30789142275106e-01, 2.50882876564970e-02, 2.58400721356293e-01, 5.30635872839879e-02, 9.21937456830432e-02, 5.71864270236285e-01, 2.00009233901039e-01, 2.29352124936166e-02, 3.17470712584264e-01, 7.31123753711116e-01, 3.61427408718217e-01, 5.00398158846378e-01, 9.08777354809560e-01);
		cout << setprecision(14) << determinant(m) << " " << determinant_gaussian_elimination(m) << " " << determinant_LU(m) << endl;
		Matrix<> n = m;
		cout << setprecision(14) << determinant(n) << " " << determinant_gaussian_elimination(n) << " " << determinant_LU(n) << endl;
	}

	{
		Matrix<5> m = Data(2.41769005385622e-02, 7.06040711831501e-01, 4.01400269892127e-01, 6.22528318958789e-01, 9.70219808437457e-01, 1.40314478524010e-01, 9.23634202443502e-01, 7.24906866380750e-01, 4.21674540188453e-01, 3.85730953556790e-01, 4.69287958133036e-02, 5.08149987496600e-02, 9.22064795087949e-01, 1.40800302981703e-01, 3.71443829061928e-01, 1.62746662339684e-01, 4.44312412946320e-01, 8.85464398503279e-01, 1.93841241643644e-01, 4.20581312059001e-02, 4.55300500629184e-01, 9.03035272141837e-01, 4.57757783510980e-01, 4.18177151842367e-01, 9.69109793150232e-01);
		cout << setprecision(14) << determinant(m) << " " << determinant_gaussian_elimination(m) << " " << determinant_LU(m) << endl;
		Matrix<> n = m;
		cout << setprecision(14) << determinant(n) << " " << determinant_gaussian_elimination(n) << " " << determinant_LU(n) << endl;
	}

	{
		Matrix<5> m = Data(5.91632267859177e-01, 8.14332544199039e-01, 3.34507049243343e-01, 8.12577918178363e-03, 5.81157036224266e-01, 9.17488581142820e-01, 5.15247342390906e-01, 9.09538796663461e-02, 6.23354293440825e-01, 7.13813668942402e-01, 3.45904240605146e-01, 3.98875610500330e-01, 2.73602520675960e-01, 8.53862859684272e-01, 4.58863386411325e-01, 3.75370427488046e-03, 1.40301868730795e-02, 7.24481516280936e-01, 7.72000815595856e-01, 5.60233819140367e-01, 8.10417771288134e-02, 7.99591957774011e-02, 5.05434588665483e-01, 2.06613577812289e-01, 8.36788161781671e-01);
		cout << setprecision(14) << determinant(m) << " " << determinant_gaussian_elimination(m) << " " << determinant_LU(m) << endl;
		Matrix<> n = m;
		cout << setprecision(14) << determinant(n) << " " << determinant_gaussian_elimination(n) << " " << determinant_LU(n) << endl;
	}

	{
		Matrix<5> m = Data(9.71181725959251e-01, 2.98621517706498e-01, 1.47998634380959e-01, 8.24787072980069e-01, 1.83866661866007e-01, 2.24902437834599e-01, 2.40099860603221e-01, 5.07556367297291e-01, 4.10876021473013e-01, 4.97539254790682e-01, 1.56892070474953e-01, 8.27163318369144e-01, 3.63567904863797e-01, 4.03619694298915e-01, 2.58110387179272e-01, 5.79206038913936e-01, 4.04199171649225e-01, 1.02716918223691e-01, 9.36273777144197e-01, 2.31285520726267e-01, 8.92102900573901e-01, 8.94441354417486e-01, 5.69651042803142e-01, 9.51434043635223e-01, 5.24784081644284e-01);
		cout << setprecision(14) << determinant(m) << " " << determinant_gaussian_elimination(m) << " " << determinant_LU(m) << endl;
		Matrix<> n = m;
		cout << setprecision(14) << determinant(n) << " " << determinant_gaussian_elimination(n) << " " << determinant_LU(n) << endl;
	}

	{
		Matrix<5> m = Data(8.05944084380757e-01, 1.54494444735867e-01, 5.68352033258144e-01, 2.76844760724612e-01, 5.99571106446652e-01, 6.50155658495726e-01, 5.80321118181015e-01, 2.78427483124922e-01, 7.28960571445496e-03, 8.12111228367507e-01, 6.17381855248769e-01, 5.68244088125756e-01, 8.52652241614986e-01, 6.93038934510588e-01, 1.86444660490361e-01, 3.11467607646139e-01, 2.15998679569308e-01, 7.83206311483730e-01, 2.35282520551438e-01, 4.86344067089717e-01, 9.38518471624478e-01, 8.17929944875947e-01, 7.10710431845036e-01, 7.86305852002155e-02, 9.90703040300500e-01);
		cout << setprecision(14) << determinant(m) << " " << determinant_gaussian_elimination(m) << " " << determinant_LU(m) << endl;
		Matrix<> n = m;
		cout << setprecision(14) << determinant(n) << " " << determinant_gaussian_elimination(n) << " " << determinant_LU(n) << endl;
	}

	{
		Matrix<5> m = Data(2.96304354743256e-01, 3.58030729171153e-01, 6.92522698805887e-01, 1.79732936094113e-01, 4.46275774232821e-01, 5.90476309273118e-01, 9.95204598929733e-01, 8.25123844754980e-01, 7.74601889392295e-01, 8.34700769223100e-01, 2.65391268303669e-01, 1.23042509553764e-01, 6.90049624348518e-01, 5.73617712556864e-01, 2.28904873550037e-02, 3.54405369665876e-01, 6.27987425226178e-01, 9.45338262364467e-02, 8.60778233147086e-02, 5.46691443929291e-01, 1.79812776509577e-01, 8.58741350469442e-01, 9.80165499951307e-01, 6.10698345141943e-01, 9.31443003023478e-01);
		cout << setprecision(14) << determinant(m) << " " << determinant_gaussian_elimination(m) << " " << determinant_LU(m) << endl;
		Matrix<> n = m;
		cout << setprecision(14) << determinant(n) << " " << determinant_gaussian_elimination(n) << " " << determinant_LU(n) << endl;
	}

	{
		Matrix<5> m = Data(3.39964572159617e-01, 6.38241707359396e-01, 6.21412435206930e-01, 5.68735165662361e-01, 8.73088379948172e-01, 1.67465181137605e-01, 1.20888911216575e-01, 5.95419962556582e-01, 7.10617412658523e-01, 4.79210410591114e-01, 5.50461047851262e-01, 6.98171017421455e-01, 6.99286564954746e-01, 6.41871452264122e-01, 6.31038672421652e-01, 9.98169469793503e-01, 8.62015911697331e-01, 1.03569535941564e-01, 4.18230944367960e-01, 3.48689244090852e-01, 2.95738342328614e-01, 3.79228962582527e-01, 3.01127710986076e-02, 2.59596355422295e-02, 3.50082597722248e-01);
		cout << setprecision(14) << determinant(m) << " " << determinant_gaussian_elimination(m) << " " << determinant_LU(m) << endl;
		Matrix<> n = m;
		cout << setprecision(14) << determinant(n) << " " << determinant_gaussian_elimination(n) << " " << determinant_LU(n) << endl;
	}

	{
		Matrix<6> m = Data(8.93075598797506e-01, 6.61791213441977e-01, 3.70808394704666e-01, 1.46498265578876e-01, 2.78200058445582e-01, 5.54055849790748e-01, 3.05838033486911e-02, 4.44993680180680e-01, 7.88390456740249e-01, 1.16780461562958e-01, 8.94930054060706e-02, 7.20970082346876e-01, 4.96218302511453e-02, 9.26240006596247e-01, 8.88607870117642e-01, 9.56710878032350e-01, 3.34282792831214e-01, 5.06367876919566e-02, 2.94190762622914e-02, 3.29275811069937e-01, 1.02572965442664e-01, 5.74897939140586e-01, 6.04507277836211e-01, 3.16549966656048e-01, 2.54575094468752e-01, 2.03366058685225e-01, 8.57634594514238e-01, 2.42119201608056e-02, 8.19973432772943e-01, 3.66885067646032e-01, 2.75984628157832e-01, 3.72465990737422e-01, 3.46690847997890e-01, 5.74756598678313e-01, 5.20360401152951e-02, 3.20497439992830e-01);
		cout << setprecision(14) << determinant(m) << " " << determinant_gaussian_elimination(m) << " " << determinant_LU(m) << endl;
		Matrix<> n = m;
		cout << setprecision(14) << determinant(n) << " " << determinant_gaussian_elimination(n) << " " << determinant_LU(n) << endl;
	}

	{
		Matrix<6> m = Data(8.15392759141323e-01, 6.63158606166048e-01, 4.24915733225933e-01, 5.13840925856423e-02, 7.76096543424744e-01, 7.21737544336952e-01, 2.33859953705507e-01, 1.75532653637049e-01, 6.03225873234580e-01, 8.13022105284277e-02, 9.19352094631728e-01, 8.75922643086659e-01, 8.81936558574934e-01, 5.67844667500457e-01, 2.84497452709346e-01, 5.84201836419325e-01, 4.13275047049565e-01, 7.16932718916952e-01, 2.85560149281632e-02, 2.77700719095446e-01, 9.70552166249673e-01, 7.24330731235627e-01, 5.60320492487045e-02, 4.50849171009584e-01, 3.07755500267491e-01, 8.02695395900112e-01, 8.66603008241087e-01, 9.89697732040402e-01, 7.46460693466325e-01, 5.00393836423695e-01, 5.18167950536741e-01, 4.81086393432127e-01, 1.66258717031924e-01, 8.03124653041700e-01, 4.50467794321799e-01, 8.52012765668135e-01);
		cout << setprecision(14) << determinant(m) << " " << determinant_gaussian_elimination(m) << " " << determinant_LU(m) << endl;
		Matrix<> n = m;
		cout << setprecision(14) << determinant(n) << " " << determinant_gaussian_elimination(n) << " " << determinant_LU(n) << endl;
	}

	{
		Matrix<6> m = Data(1.73873830340882e-01, 8.15506501222735e-01, 7.99892345081102e-01, 3.02841618913877e-01, 9.95972163237521e-01, 3.51766410154212e-02, 9.27171748962429e-01, 8.42581163100880e-01, 3.38903338044945e-01, 4.74427705590175e-01, 5.08524050367466e-02, 7.36311857376742e-02, 3.15994578679089e-02, 5.15587619567004e-01, 9.02891075269430e-02, 4.78151724884952e-02, 1.71814928120137e-01, 2.63329788559906e-01, 4.58833436305044e-01, 7.69608099236108e-02, 6.64097930557233e-03, 1.16716255720669e-01, 3.53975275198141e-01, 4.45358789776681e-01, 5.52511592287953e-01, 2.20041620061399e-01, 6.92787271247266e-01, 5.01641456568400e-01, 6.65791707544045e-01, 2.20790421105144e-01, 5.81720318881466e-01, 2.20147517157036e-01, 5.78342794307046e-01, 5.96769597293235e-01, 8.69084512634995e-01, 7.26041564766618e-01);
		cout << setprecision(14) << determinant(m) << " " << determinant_gaussian_elimination(m) << " " << determinant_LU(m) << endl;
		Matrix<> n = m;
		cout << setprecision(14) << determinant(n) << " " << determinant_gaussian_elimination(n) << " " << determinant_LU(n) << endl;
	}

	{
		Matrix<6> m = Data(1.69012517494236e-01, 7.23361485720808e-01, 1.58648898560071e-01, 7.93902010679314e-01, 7.45699046746697e-01, 1.47948459995012e-01, 4.14485542350386e-01, 8.68453695840411e-01, 1.59666334468436e-01, 2.29205209386140e-01, 8.99934637128772e-01, 8.94825577210968e-01, 1.15893558310896e-01, 4.01736973894975e-01, 4.04462150112230e-01, 9.16387261868535e-01, 1.77560640355935e-01, 4.11447069445629e-01, 1.53615368394506e-01, 6.95952502117472e-01, 4.97767568523417e-02, 4.86264755554507e-01, 3.12486411340096e-01, 1.71515530957746e-01, 7.58688422862998e-01, 5.39024514700782e-01, 1.35498947757275e-01, 9.39456294393559e-01, 4.64986854845827e-01, 3.72800645666414e-01, 4.75043913233161e-01, 2.11914763778214e-02, 8.97784967463455e-01, 1.52714792749979e-01, 2.64372757954814e-01, 6.41838842117378e-01);
		cout << setprecision(14) << determinant(m) << " " << determinant_gaussian_elimination(m) << " " << determinant_LU(m) << endl;
		Matrix<> n = m;
		cout << setprecision(14) << determinant(n) << " " << determinant_gaussian_elimination(n) << " " << determinant_LU(n) << endl;
	}

	{
		Matrix<6> m = Data(7.75732897302276e-01, 1.59091329139989e-01, 2.68089113054277e-01, 4.38160860931799e-01, 6.70417379663448e-02, 7.96002091411798e-01, 4.54711877134461e-01, 9.47229237656936e-01, 9.51085062444666e-01, 4.80099340755984e-01, 7.42919532109935e-01, 2.85779156530161e-01, 4.57995860939676e-01, 6.67363890807460e-01, 6.32290548378104e-01, 4.40918001389516e-01, 4.62796357705982e-01, 7.77234940451805e-01, 4.75560920835477e-01, 4.15675682606075e-01, 8.61915798984557e-01, 3.64876523432098e-02, 5.60987447670667e-01, 1.29605895274223e-01, 3.94842826718269e-01, 5.93959686896221e-01, 2.47482674014164e-01, 4.02923611744962e-01, 3.72244997624730e-01, 7.29654535331805e-01, 7.40192418754549e-01, 4.22298466969688e-01, 9.77815406682506e-01, 7.91116978944657e-01, 5.67709113751070e-01, 6.19851782055395e-01);
		cout << setprecision(14) << determinant(m) << " " << determinant_gaussian_elimination(m) << " " << determinant_LU(m) << endl;
		Matrix<> n = m;
		cout << setprecision(14) << determinant(n) << " " << determinant_gaussian_elimination(n) << " " << determinant_LU(n) << endl;
	}

	{
		Matrix<6> m = Data(3.18226764790049e-01, 6.04990475428641e-01, 8.47856200257748e-01, 7.63433268204239e-01, 3.49048204614404e-02, 1.89798773730778e-01, 6.20816378149365e-01, 4.39430991976835e-01, 8.09371879113661e-01, 4.05909130927198e-01, 5.50970337684356e-01, 3.04670648124735e-01, 4.36602448768744e-02, 4.47556668966246e-01, 5.28189202675348e-01, 6.56653023910958e-01, 6.90368275675822e-01, 2.07129789189039e-01, 4.98652988515397e-01, 7.97605678364206e-01, 7.85268678833408e-01, 1.95493747072068e-01, 1.25720815064634e-01, 2.79682069016325e-01, 4.69092731851899e-01, 7.88690794057686e-01, 2.12826437774348e-01, 9.77254334775169e-01, 5.89212250079350e-01, 7.93888259759860e-01, 6.44656450071723e-01, 2.49255087293649e-02, 9.72785936638460e-01, 4.47475717732895e-01, 8.43749723576504e-02, 4.61707393051235e-01);
		cout << setprecision(14) << determinant(m) << " " << determinant_gaussian_elimination(m) << " " << determinant_LU(m) << endl;
		Matrix<> n = m;
		cout << setprecision(14) << determinant(n) << " " << determinant_gaussian_elimination(n) << " " << determinant_LU(n) << endl;
	}

	{
		Matrix<6> m = Data(6.18812741281392e-02, 3.74068943074471e-01, 6.63915750469443e-01, 3.54983604761906e-01, 9.42683167058176e-01, 6.36037694108753e-01, 9.39317919407114e-01, 8.00162275029759e-01, 8.88224087928930e-01, 5.07799945684691e-01, 6.56859417305437e-01, 2.29071728408510e-01, 8.96520263537621e-01, 8.32421286310729e-01, 6.85086208473243e-01, 7.96713080968666e-01, 6.95228178548774e-02, 3.21065842249768e-01, 8.13085436558866e-01, 9.96785990862678e-01, 9.99794419525261e-01, 5.06733384408189e-01, 2.24347081645378e-01, 7.70300205790558e-01, 1.42279378761482e-01, 7.13798271861761e-01, 7.59865488317435e-01, 7.14113103449323e-01, 8.72145288721125e-02, 6.60530754098642e-01, 5.12071597579688e-01, 4.84850932681686e-01, 3.44516751617267e-01, 8.05727667320606e-01, 5.93584163172660e-03, 3.31947921577863e-01);
		cout << setprecision(14) << determinant(m) << " " << determinant_gaussian_elimination(m) << " " << determinant_LU(m) << endl;
		Matrix<> n = m;
		cout << setprecision(14) << determinant(n) << " " << determinant_gaussian_elimination(n) << " " << determinant_LU(n) << endl;
	}

	{
		Matrix<6> m = Data(3.49705542827719e-01, 1.37154944797163e-01, 6.45239524870601e-01, 4.31276022990973e-02, 8.46517570966223e-01, 8.86036951068266e-01, 9.91510616858571e-01, 6.87024047653616e-02, 7.39402886537011e-01, 7.20916863357948e-01, 2.84475490456196e-01, 2.22671025009815e-01, 5.78440664421205e-01, 5.97855361442432e-01, 3.00653206604492e-01, 4.69252239511250e-01, 4.47009863416795e-01, 3.48234586037146e-01, 1.01840607107278e-01, 3.43101456172621e-01, 3.80111889860627e-01, 8.84945193485220e-02, 6.23633404737741e-01, 7.31812466790873e-01, 7.62998066335941e-01, 9.66205817990241e-01, 2.47845564976849e-01, 3.79902441061743e-01, 9.97577931303812e-01, 7.15377289547433e-01, 6.44923638104279e-01, 7.19239911502218e-01, 5.39820510091201e-01, 8.48885217361553e-01, 4.89454460716457e-01, 9.82603494825003e-01);
		cout << setprecision(14) << determinant(m) << " " << determinant_gaussian_elimination(m) << " " << determinant_LU(m) << endl;
		Matrix<> n = m;
		cout << setprecision(14) << determinant(n) << " " << determinant_gaussian_elimination(n) << " " << determinant_LU(n) << endl;
	}

	{
		Matrix<6> m = Data(9.83495551129402e-01, 8.96438923325791e-01, 7.65595118153860e-01, 6.70088435212700e-01, 1.28373104373757e-01, 7.95865935827994e-01, 3.80452441451673e-01, 7.76023968303416e-01, 5.97902888635629e-01, 8.81282332475203e-01, 4.51220508158266e-01, 3.94786616586638e-01, 7.86418138438833e-01, 6.44674583213448e-01, 8.84231744282032e-01, 6.65852931265202e-01, 3.62849453841196e-01, 5.82554758877527e-01, 7.51933660082863e-01, 9.89343406928702e-01, 3.30715965455562e-01, 7.80249339682639e-01, 7.71406127001749e-01, 6.42095402889450e-01, 5.69000996687613e-01, 6.69335637399247e-01, 5.18974190991716e-01, 7.46706468592545e-01, 4.85398019583926e-01, 9.48516570907321e-01, 4.62234245294165e-01, 9.62100841143866e-01, 5.04552101843300e-01, 5.54298923615301e-01, 7.67623550209119e-01, 7.85572711446036e-01);
		cout << setprecision(14) << determinant(m) << " " << determinant_gaussian_elimination(m) << " " << determinant_LU(m) << endl;
		Matrix<> n = m;
		cout << setprecision(14) << determinant(n) << " " << determinant_gaussian_elimination(n) << " " << determinant_LU(n) << endl;
	}

	{
		Matrix<6> m = Data(4.70901409159873e-01, 6.16718330229730e-01, 2.92510391583698e-01, 7.18900513043769e-01, 1.16092123525330e-01, 2.67417883907672e-01, 4.49033651959167e-01, 7.43373040563550e-01, 3.08512735313409e-01, 7.45404418147625e-01, 3.92298614689306e-01, 2.16621800393400e-01, 7.29906485798085e-01, 1.80455959397962e-01, 3.48883647220554e-01, 6.47630016718079e-01, 6.73020430653967e-01, 5.67042165392109e-01, 1.46353190901009e-01, 6.27891196103089e-01, 8.81123105536663e-01, 4.84702126218149e-01, 5.80946400504362e-01, 5.44173697172403e-01, 3.91523536586145e-02, 6.93003427652823e-01, 6.36770236648195e-01, 4.68642500236709e-01, 4.61680771916075e-01, 4.85512743888567e-01, 8.94114883838645e-01, 1.66978408137268e-03, 7.72267414921243e-01, 4.58920766982541e-02, 2.44879410805987e-01, 2.28132316064184e-01);
		cout << setprecision(14) << determinant(m) << " " << determinant_gaussian_elimination(m) << " " << determinant_LU(m) << endl;
		Matrix<> n = m;
		cout << setprecision(14) << determinant(n) << " " << determinant_gaussian_elimination(n) << " " << determinant_LU(n) << endl;
	}

}
