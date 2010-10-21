//Revolution engine specifics header files
#include "REV.h"

RSCMGR::RSCMGR()
{
	m_pSmodels = NULL;
}

RSCMGR::~RSCMGR()
{
	while(m_pSmodels != NULL) m_pSmodels->release();
}

RSC<SMODEL> * RSCMGR::getSmodel(const char * name)
{
	return m_pSmodels->getChildRsc(name);
}

template<class T>RSC::RSC(const char * _name, T* _pParent)
{
	m_ready = 0;
	m_refCount = 1;//someone must have created this resource:
	m_pHigher = NULL;
	m_pLower = NULL;
	m_pParent = _pParent;
	strcpy(m_name, _name);
	m_content = new T(_name);
}