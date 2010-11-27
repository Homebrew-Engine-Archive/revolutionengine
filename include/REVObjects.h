///////////////////////////////////
//Technik 2009
//Revolution Engine 0.5
///////////////////////////////////
//Objects header file

#ifndef REV_OBJECTS_H
#define REV_OBJECTS_H

#include <vector>

using namespace std;

class OBJECT:public NODE
{
public:
	OBJECT						(TModel * model = NULL, Vector pos = nullVector, u8 flags = F_Visible);
	//OBJECT						(MODEL * model, MATERIAL * material, Vector pos = nullVector, u8 flags = F_Visible);
	
	bool		usesAlpha		();//To decide how to render this object
	void		setMaterial		(IMaterial * material, u8 slot = 0);//No more than 255 material slots allowed
	IMaterial *	getMaterial		(u8 slot = 0);

	TModel	*	m_model;
	GXColor		m_clr;
	virtual bool	isRenderable();
protected:
	virtual void render();
private:
	vector<IMaterial*>	m_vMaterials;
	GXColor shadowClr;
friend class ROOT;
friend void render(NODE * node, Vector camPos);
};

#endif
