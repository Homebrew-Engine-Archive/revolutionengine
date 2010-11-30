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
OBJECT::OBJECT(TModel * _model, Vector _pos, u8 _flags):NODE(_flags)
{
	setPos(_pos);
	m_clr = DEFAULT_OBJECT_COLOR;
	type = NT_OBJECT;
	this->m_model = _model;
	m_vMaterials.resize(1);
}

//--------------------------------------------------------------
bool OBJECT::usesAlpha()
{
	if(m_clr.a != 255) return true;
	//Do we use materials?
	return false;//!m_vMaterials.empty();
	//TODO: materials should have a usesAlpha function,
	//and this function should iterate through materials to see if any uses alpha
}

//--------------------------------------------------------------
void OBJECT::setMaterial(IMaterial *_material, u8 _slot)
{
	m_vMaterials[_slot] = _material;
}

//--------------------------------------------------------------
IMaterial * OBJECT::getMaterial(u8 _slot)
{
	if(m_vMaterials.empty()) return NULL;
	if(m_vMaterials.capacity() <= _slot) return m_vMaterials[0];
	return m_vMaterials[_slot];
}

//--------------------------------------------------------------
bool OBJECT::isRenderable()
{
	if(!m_model)
		return false;
	return true;
}

//--------------------------------------------------------------
void OBJECT::render(u32 _lightMask)
{
	//Render my model using this material
	m_model->render(m_vMaterials, m_clr, _lightMask);
}
