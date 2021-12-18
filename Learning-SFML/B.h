#pragma once
#include "A.h"

class A;

class B
{
	B();
	friend B A::doStuff();

	// B has private constructor
	// I want B to only be constructed in A::doStuff()
};

/*
// Alternative implementation which works,
but has to make the whole class A a friend

class B
{
	B();
	friend class A;
}
*/
