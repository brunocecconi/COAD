
#ifndef META_CLASS_INFO_H
#define META_CLASS_INFO_H

#include "Meta/CtorInfo.h"

namespace Meta
{

/**
 * @brief Class info class.
 *
*/
class ClassInfo
{
public:
private:
	eastl::vector<CtorInfo> ctors_{NAME_VAL("Meta")};
};

}

#endif
