#include "cBread.h"

cBread::cBread()
{
}

cBread::~cBread()
{
}

cGameObject * cBread::MakeOtherHalf()
{
	return new cBread();
}
