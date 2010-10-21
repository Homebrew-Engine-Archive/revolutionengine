///////////////////////////////////
//Technik 2009
//Revolution Engine 0.5
///////////////////////////////////
//Script system
#ifndef REV_SCRIPT_H
#define REV_SCRIPT_H

#include "REV.h"

extern "C"
{
#include "lua/lua.h"
#include "lua/lualib.h"
#include "lua/lauxlib.h"
}

class SCRIPTMGR
{
public:
	SCRIPTMGR();
	~SCRIPTMGR();

	void	registerFunction	( const char * name, int(* _function)(lua_State*) );

	//Run commands and strings
	void	runString			( const char * command);
private:
	lua_State * m_pLuaVM;
};

//Default registered functions
int l_print(lua_State* _luaVM);

#endif
