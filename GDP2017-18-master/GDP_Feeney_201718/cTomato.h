#ifndef _cTomato_HG_
#define _cTomato_HG_
#include "cSandwichItem.h"

class cTomato : public cSandwichItem
{
public:
	cTomato();
	~cTomato();
	virtual cGameObject* MakeOtherHalf();
};
#endif // !_cTomato_HG_
