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
	void	release			()		{}//TODO
private:
	RSC						(const char * _name, T* _pParent) 
	{
		m_ready = false;
		m_refcount = 0;
		m_pContent =  new T(_name);
		m_pHigher = NULL;
		m_pLower = NULL;
		m_pParent = _pParent;
		strcpy(m_name, _name);
	}
	~RSC();

	RSC<T>*			getChildRsc		(const char * name);

	bool			m_ready;		//Is this resource ready to be used? (access semaphore)
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
	RSC<TModel> * getSmodel(const char * name);
private:
	RSC<TModel> * m_pSmodels;
};

#endif
