
#include <iostream>
#include <tuple>
#include <vector>
#include <functional>

//#include "../speed_core/trusted/Deduplicable.h"
//#include "../speed_core/trusted/siftpp_sgx/sift-driver.h"
//#include "../speed_core/trusted/zlib_sgx/zlib.h"
//#include "../speed_core/trusted/pcre_sgx/pcre.h"
//
using namespace std;

//rc = sift(img_pt, width, height, sigman, sigma0, O, S, omin, smin, smax);
//
//Deduplicable<float*, int, int, float, float, int, int, int, int, int> 
//	dedup_sift("libsiftpp", "0.8.1", "sift");
//rc = dedup_sift(img_pt, width, height, sigman,
//	sigma0, O, S, omin, smin, smax);


template<typename _Res, typename... _ArgTypes>
class FunctionDB
{
public:
	virtual std::function<_Res(_ArgTypes...)> getSumFunction() = 0;
};

template<typename _Res, typename... _ArgTypes>
class Test : public FunctionDB<_Res, _ArgTypes...>
{
public:
	int add(int a, double b)
	{
		cout << "in add:";
		cout << a << "  ";
		cout << b << endl << endl;;
		return (int)(a + b);
	}

	virtual std::function<_Res ( _ArgTypes...)> getSumFunction()
	{
		//std::function<void()> f_display_31337 = std::bind(print_num, 31337);
		return std::bind(&Test::add, this, std::placeholders::_1, std::placeholders::_2);
	}
};

//
//int main()
//{
//	//siftpp
//
//
//	//zlib
//
//
//	//pcre 
//
//	FunctionDB<int, int, double>* pBase = new Test<int,int,double>();
//	auto func = pBase->getSumFunction();
//	cout<<func(5, 10.0)<<endl;
//	//cout<<<<endl;
//
//
//	
//
//
//	//Deduplicable<int, int, double> sample("a","b","c");
//	//cout << sample(3, 10.0)<<endl;
//
//	char n;
//	cin >> n;
//	return 0;
//}