#include "cSalami.h"

cSalami::cSalami()
{
}

cSalami::~cSalami()
{
}

cGameObject * cSalami::MakeOtherHalf()
{
	return new cSalami();
}
