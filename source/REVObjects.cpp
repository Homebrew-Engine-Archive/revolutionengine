///////////////////////////////////
//Technik 2009
//Revolution Engine 0.5
///////////////////////////////////
//Objects source file

//header files
#include <vector>

#include "REV.h"

using namespace std;

//--------------------------------------------------------------
OBJECT::OBJECT(MODEL * _model, Vector _pos, u8 _flags):NODE(_flags)
{
	setPos(_pos);
	m_clr = DEFAULT_OBJECT_COLOR;
	type = NT_OBJECT;
	this->model = _model;
}

//--------------------------------------------------------------
bool OBJECT::usesAlpha()
{
	if(m_clr.a != 255) return true;
	//Do we use materials?
	return !m_vMaterials.empty();
	//TODO: materials should have a usesAlpha function,
	//and this function should iterate through materials to see if any uses alpha
}

//--------------------------------------------------------------
void OBJECT::setMaterial(MATERIAL *_material, u8 _slot)
{
	m_vMaterials[_slot] = _material;
}

//--------------------------------------------------------------
MATERIAL * OBJECT::getMaterial(u8 _slot)
{
	if(m_vMaterials.capacity() <= _slot) return NULL;
	return m_vMaterials[_slot];
}
