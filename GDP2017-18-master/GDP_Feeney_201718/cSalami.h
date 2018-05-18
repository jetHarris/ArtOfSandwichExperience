#ifndef _cSalami_HG_
#define _cSalami_HG_
#include "cSandwichItem.h"

class cSalami : public cSandwichItem
{
public:
	cSalami();
	~cSalami();
	virtual cGameObject* MakeOtherHalf();
};
#endif // !_cSalami_HG_
