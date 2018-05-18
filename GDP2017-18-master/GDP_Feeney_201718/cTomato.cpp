#include "cTomato.h"

cTomato::cTomato()
{
}

cTomato::~cTomato()
{
}

cGameObject * cTomato::MakeOtherHalf()
{
	return new cTomato();
}
