///////////////////////////////////
//Technik 2009
//Revolution Engine 0.5
///////////////////////////////////
//Resources management system
#ifndef REV_CONTENT_MGR_H
#define REV_CONTENT_MGR_H

#include "REVConfig.h"

//Basic resource class
template<class T>
class RSC
{
public:
	u16		getRefCount		()		{	return m_refcount;					}
	T*		get				()		{	return m_ready?m_pContent:NULL;		}
	u16		retain			()		{	return m_refcount++;				}
	void	release			();
private:
	RSC(const char * name, T* parent);
	~RSC();

	RSC<T>*			getChildRsc		(const char * name);

	u8				m_ready;		//Is this resource ready to be used? (access semaphore)
	u16				m_refcount;		//reference count
	T*				m_pContent;		//pointer to the actual resource
	RSC<T>*			m_pParent;
	RSC<T>*			m_pHigher;
	RSC<T>*			m_pLower;
	char			m_name[MAX_PATH];//name of this resource
friend class RSCMGR;
};

class RSCMGR
{
public:
	//Resource manager constructor and destructor
	RSCMGR();
	~RSCMGR();

	//Accessing resources
	RSC<SMODEL> * getSmodel(const char * name);
private:
	RSC<SMODEL> * m_pSmodels;
};

#endif
