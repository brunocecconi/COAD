
#ifndef META_CLASS_INFO_H
#define META_CLASS_INFO_H

#include "Meta/TypeInfo.h"

#include <EASTL/any.h>

namespace Meta
{

class ParamInfo
{
public:
	enum Flags : Uint32
	{
		eNone=0,
		eIn=1,
		eOut=2,
		eOptional=4
	};

public:
	

private:
	const TypeInfo& type_info_;
	Uint32 flags_;
	eastl::any default_value;
};

}

#endif
