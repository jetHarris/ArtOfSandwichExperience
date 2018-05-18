#ifndef _cCheese_HG_
#define _cCheese_HG_
#include "cSandwichItem.h"

class cCheese : public cSandwichItem
{
public:
	cCheese();
	~cCheese();
	virtual cGameObject* MakeOtherHalf();
};
#endif // !_cCheese_HG_

