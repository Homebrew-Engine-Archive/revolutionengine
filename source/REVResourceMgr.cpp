//Revolution engine specifics header files
#include "REV.h"

//------------------------------------------------------------
RSCMGR::RSCMGR()
{
	m_pSmodels = NULL;
}

//------------------------------------------------------------
RSCMGR::~RSCMGR()
{
	while(m_pSmodels != NULL) m_pSmodels->release();
}

//------------------------------------------------------------
RSC<TModel> * RSCMGR::getSmodel(const char * name)
{
	return m_pSmodels->getChildRsc(name);
}