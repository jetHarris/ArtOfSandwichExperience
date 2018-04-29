#pragma once
#include "eConstraintType.h"

namespace nPhysics
{
	class iConstraint
	{
	public:
		virtual ~iConstraint() {}
		inline eConstraintType GetConstraintType() const { return constraintType; }
		iConstraint(eConstraintType constraintTypeIn) : constraintType(constraintTypeIn) {}
		iConstraint(const iConstraint& otherCon) {}
		iConstraint& operator=(const iConstraint& other) { return *this; }
		eConstraintType constraintType;

	};
}