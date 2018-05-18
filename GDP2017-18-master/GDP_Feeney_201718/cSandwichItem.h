#ifndef _cSandwichItem_HG_
#define _cSandwichItem_HG_

#include "cGameObject.h"

class cSandwichItem : public cGameObject
{
public:
	cSandwichItem();
	~cSandwichItem();
	virtual cGameObject* MakeOtherHalf() = 0;
};


#endif // !_cSandwichItem_HG_

