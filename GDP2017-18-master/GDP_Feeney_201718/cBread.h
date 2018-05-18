#ifndef _cBread_HG_
#define _cBread_HG_
#include "cSandwichItem.h"

class cBread : public cSandwichItem
{
public:
	cBread();
	~cBread();
	virtual cGameObject* MakeOtherHalf();
};
#endif // !_cBread_HG_

