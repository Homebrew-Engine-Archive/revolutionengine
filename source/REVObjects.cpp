///////////////////////////////////
//Technik 2009
//Revolution Engine 0.5
///////////////////////////////////
//Objects source file

//header files
#include "REV.h"

OBJECT::OBJECT(MODEL * model, Vector pos, u8 flags):NODE(flags)
{
	setPos(pos);
	clr = DEFAULT_OBJECT_COLOR;
	type = NT_OBJECT;
	this->model = model;
	material = NULL;
}
