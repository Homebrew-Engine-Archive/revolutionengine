///////////////////////////////////
//Technik 2009
//Revolution Engine 0.5
///////////////////////////////////
//Script
//This is basically a lua wrapper
#include "REV.h"


//-------------------------------------------------------
SCRIPTMGR::SCRIPTMGR()
{
	m_pLuaVM = lua_open();

	//init lua
	luaL_openlibs(m_pLuaVM);

	//Register default functions
	registerFunction("print", &l_print);
}


//-------------------------------------------------------
SCRIPTMGR::~SCRIPTMGR()
{
	lua_close(m_pLuaVM);
}

//-------------------------------------------------------
void SCRIPTMGR::registerFunction(const char * _name, int(* _function)(lua_State*))
{
	lua_register(m_pLuaVM, _name, _function);
}

//-------------------------------------------------------
void SCRIPTMGR::runString(const char * _command)
{
	luaL_dostring( m_pLuaVM, _command);
}

//Lua registered functions
//-------------------------------------------------------
int l_camSpeed(lua_State* _luaVM)
{
	defaultCam->move(vector3(0, 500, 500), RT_SELF);
	lua_pushnumber( _luaVM, 0 );
	return 1;
}

//-------------------------------------------------------
int l_print(lua_State* _luaVM)
{
	int n = lua_gettop(_luaVM);  /* number of arguments */
	int i;
	lua_getglobal(_luaVM, "tostring");
	for (i=1; i<=n; i++)
	{
		const char *s;
		lua_pushvalue(_luaVM, -1);  /* function to be called */
		lua_pushvalue(_luaVM, i);   /* value to print */
		lua_call(_luaVM, 1, 1);
		s = lua_tostring(_luaVM, -1);  /* get result */
		if (s == NULL)
			return luaL_error(_luaVM, LUA_QL("tostring") " must return a string to "
								 LUA_QL("print"));
		if (i>1) REVConsole->addChar('\t');
		REVConsole->write(s);
		lua_pop(_luaVM, 1);  /* pop result */
	}
	return 1;
}
