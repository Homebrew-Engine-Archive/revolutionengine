///////////////////////////////////
//Technik 2009
//Revolution Engine 0.5
///////////////////////////////////
//Console
#include "REV.h"

u8 keyState[256];
u8 oldState[256];
u8 newState[256];

//Global internal data
u8 shifted = 0, Capital = 0;
u8 altGr = 0;

void keyboardInit(void)
{
	for(u16 i = 0; i < 256; i++)
	{
		keyState[i] = 0;
		oldState[i] = 0;
		newState[i] = 0;
	}
}

#define ALTGR (keyState[KEY_ALTGR])

void keyboardProcess(void)
{
	keyboard_event event;
	//char message[16];
	while(KEYBOARD_GetEvent(&event) > 0)
	{
		u8 value = event.keycode;
		switch(event.type)
		{
			case KEYBOARD_PRESSED:
			{
				keyState[value] = 1;
				//sprintf(message, "%u", (u32)value);
				//REVConsole->write(message);
				if((value == KEY_RSHIFT)||(value == KEY_LSHIFT))
				{
					shifted = 1;
				}
				//Process pressed key as input of the console
				else if(REVConsole->isActive())
				{
					switch(value)
					{
						case KEY_ENTER:
						{
							//System.runCommand(REVConsole->getLastLine());
							ScriptMgr->runString(REVConsole->getLastLine());
							REVConsole->nextLine();
							break;
						}
						case KEY_ESC:
						{
							REVConsole->close();
							break;
						}
						case KEY_BACK:
						{
							REVConsole->deleteChar();
							break;
						}
						case KEY_SPACE:
						{
							REVConsole->addChar(' ');
							break;
						}
						default:
						{
							if((value >= KEY_A)&&(value <= KEY_Z))
							{
								if(shifted || Capital)
									REVConsole->addChar(value - KEY_A + 'A');
								else
									REVConsole->addChar(value - KEY_A + 'a');
							}
							else if((value >= KEY_1)&&(value <= KEY_9))
							{
								if(value == KEY_3)
								{
									if(altGr)			REVConsole->addChar('#');
									else if(shifted)	REVConsole->addChar('·');
									else				REVConsole->addChar('3');
								}
								else if(shifted)
								{
									if(value == KEY_7)
										REVConsole->addChar('/');
									else
										REVConsole->addChar(value - KEY_1 + '!');
								}
								else
									REVConsole->addChar(value - KEY_1 + '1');
							}
							else if(value == KEY_0)
							{
								if(shifted)	REVConsole->addChar('=');
								else		REVConsole->addChar('0');
							}
							else if(value == KEY_DOT)
							{
								if(shifted) REVConsole->addChar(':');
								else		REVConsole->addChar('.');
							}
							else if(value == KEY_COLON)
							{
								if(shifted) REVConsole->addChar(';');
								else		REVConsole->addChar(',');
							}
							else if(value == KEY_MINUS)
							{
								if(shifted) REVConsole->addChar('_');
								else		REVConsole->addChar('-');
							}
							else if(value == KEY_PLUS)
							{
								if(ALTGR)	REVConsole->addChar(']');
								else if(shifted) REVConsole->addChar('*');
								else		REVConsole->addChar('+');
							}
							else if(value == KEY_FLEX)
							{
								if(ALTGR)	REVConsole->addChar('[');
								else		REVConsole->addChar('^');
							}
							else if(value == KEY_CBRACES_OPEN)
							{
								if(ALTGR)	REVConsole->addChar('{');
							}
							else if(value == KEY_CBRACES_CLOSE)
							{
								if(ALTGR)	REVConsole->addChar('}');
							}
						}
					}
				}
				break;
			}
			case KEYBOARD_RELEASED:
			{
				keyState[value] = 0;
				if((value == KEY_RSHIFT)||(value == KEY_LSHIFT))
				{
					shifted = 0;
				}
				break;
			}
			default:
			{
				break;
			}
		}
	}
	for(u16 i = 0; i < 256; i++)
	{
		//if(newState[i] > oldState[i]) keyState[i] = 1;
		//else keyState[i] = 0;
	}
}

CONSOLE::CONSOLE()
{
	nLines = 10;
	lineSize = 64;
	fontSize = 16;
	writePos = 0;
	active = 0;
	usrClr = SC_WHITE;
	sysClr = SC_WHITE;
	lines = (TEXT**)malloc(nLines*sizeof(TEXT*));
	for(u8 i = 0; i < nLines; i++)
	{
		lines[i] = new TEXT(fontSize, vector3(50, 50+(fontSize+4)*(1+i), 10), lineSize, SC_GREEN, 0);
		lines[i]->text[0]='\0';
	}
}

CONSOLE::~CONSOLE()
{
	for(u8 i=0; i < nLines; i++)
	{
		delete lines[i];
	}
	free(lines);
}

void CONSOLE::open()
{
	for(u8 i=0; i < nLines; i++)
	{
		lines[i]->flags = F_Visible;
	}
	active = 1;
}

void CONSOLE::close()
{
	for(u8 i=0; i < nLines; i++)
	{
		lines[i]->flags = 0;
	}
	active = 0;
}

void CONSOLE::write(const char * message)
{
	for(u8 i=0; i < nLines-1; i++)
	{
		strcpy(lines[i]->text, lines[i+1]->text);
	}
	strcpy(lines[nLines-1]->text, message);
	writePos = 0;
}

void CONSOLE::nextLine()
{
	for(u8 i=0; i < nLines-1; i++)
	{
		strcpy(lines[i]->text, lines[i+1]->text);
	}
	lines[nLines-1]->text[0] = '\0';
	writePos = 0;
}

void CONSOLE::insert(const char * message)
{
	strcpy(lines[nLines-1]->text, message);
}

void CONSOLE::addChar(char a)
{
	if(writePos < (lineSize-1))
	lines[nLines-1]->text[writePos] = a;
	writePos++;
	lines[nLines-1]->text[writePos] = '\0';
}

void CONSOLE::deleteChar()
{
	if(writePos)
	{
		writePos--;
		lines[nLines-1]->text[writePos] = '\0';
	}
}