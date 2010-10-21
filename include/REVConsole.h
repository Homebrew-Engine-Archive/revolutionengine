///////////////////////////////////
//Technik 2009
//Revolution Engine 0.5
///////////////////////////////////
//Console
#ifndef REV_CONSOLE_H
#define REV_CONSOLE_H

extern u8 keyState[256];

void keyboardInit(void);
void keyboardProcess(void);

class CONSOLE
{
public:
	void 	open			();
	void 	open			(GXColor usrClr, GXColor sysClr);
	void 	open			(u8 fontSize, u8 nLines, u8 lineSize);
	void 	write			(const char * message);
	void 	nextLine		();
	void 	insert			(const char * message);
	void 	clear			();
	void 	addChar			(char a);
	void 	deleteChar		();
	void 	clearLine		();
	char *	getLastLine		() 										{	return lines[nLines-1]->text;	}
	void 	close			();
	u8		getLineSize		()										{	return lineSize;				}
	u8 isActive				()										{	return active;					}
	GXColor usrClr, sysClr;
private:
	TEXT ** lines;
	u8 nLines, lineSize, fontSize;
	u8 writePos, active;
	CONSOLE					();
	~CONSOLE				();
friend void REV_init(void);
friend void REV_preProcess(void);
friend void REV_exit(void);
friend class ROOT;
};

//Key definitions
//Alphabet
#define KEY_A	4
#define KEY_B	5
#define KEY_C	6
#define KEY_D	7
#define KEY_E	8
#define KEY_F	9
#define KEY_G	10
#define KEY_H	11
#define KEY_I	12
#define KEY_J	13
#define KEY_K	14
#define KEY_L	15
#define KEY_M	16
#define KEY_N	17
#define KEY_O	18
#define KEY_P	19
#define KEY_Q	20
#define KEY_R	21
#define KEY_S	22
#define KEY_T	23
#define KEY_U	24
#define KEY_V	25
#define KEY_W	26
#define KEY_X	27
#define KEY_Y	28
#define KEY_Z	29

//Numeric
#define KEY_1	30
#define KEY_2	31
#define KEY_3	32
#define KEY_4	33
#define KEY_5	34
#define KEY_6	35
#define KEY_7	36
#define KEY_8	37
#define KEY_9	38
#define KEY_0	39

//Control keys
#define KEY_ENTER	40
#define KEY_ESC		41
#define KEY_BACK	42
#define KEY_TAB		43
#define KEY_SPACE	44

#define KEY_FLEX	47
#define KEY_PLUS	48
#define KEY_CBRACES_OPEN		52
#define KEY_CBRACES_CLOSE	54

//Other keys
#define KEY_COLON	54
#define KEY_DOT		55
#define KEY_MINUS	56

//Arrow keys
#define KEY_RIGHT	79
#define KEY_LEFT	80
#define KEY_DOWN	81
#define KEY_UP		82

#define KEY_LCTRL	224
#define KEY_LSHIFT	225
#define KEY_ALT		226
#define KEY_RCTRL	228
#define KEY_RSHIFT	229
#define KEY_ALTGR	230

#endif
