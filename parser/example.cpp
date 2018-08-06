#include <iostream>

using namespace std;

#include "mathlib.h"
#include "piclib.h"

int main()
{
	int a = 5, b = 10, c;
	double d;
	
	c = add(a, b);
	d = log(log(c));

	cout << c << d << endl;

	
	float * pixs = new float[a*b];

	if (findPeople(pixs, a, (int)log(b)))
	{
		cout << "fine one guy." << endl;
	}

	delete pixs;

	return 0;
}

