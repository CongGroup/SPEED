#include "mathlib_dedup_v1.0.5.h"
#include "mathlib_dedup_v1.0.5.h"
#include "piclib_dedup_v1.0.2.h"
#include <iostream>

using namespace std;

#include "mathlib.h"
#include "piclib.h"

int main()
{
	int a = 5, b = 10, c;
	double d;
	
	c = add_dedup(a, b);
	d = log_dedup(log_dedup(c));

	cout << c << d << endl;

	
	float * pixs = new float[a*b];

	if (findPeople(pixs, a, (int)log_dedup(b)))
	{
		cout << "fine one guy." << endl;
	}

	delete pixs;

	return 0;
}
