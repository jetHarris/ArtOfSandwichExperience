#include "cCheese.h"

cCheese::cCheese()
{
}

cCheese::~cCheese()
{
}

cGameObject * cCheese::MakeOtherHalf()
{
	return new cCheese();
}
