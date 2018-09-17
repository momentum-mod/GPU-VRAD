#ifndef _GPUARAD_SCRIPT_H_
#define _GPUARAD_SCRIPT_H_

#define	MAX_INCLUDES	64
#define	MAX_TOKEN		1024

#include <vector>

#include "prettyprint.h"

#include "basetypes.h"

struct script_t
{
	char	filename[1024];
	char    *buffer, *script_p, *end_p;
	int     line;

	char	macrobuffer[4096];
	char	*macroparam[64];
	char	*macrovalue[64];
	int		nummacroparams;
};

typedef struct
{
	char *param;
	char *value;
} variable_t;

enum ScriptPathMode_t
{
	SCRIPT_USE_ABSOLUTE_PATH,
	SCRIPT_USE_RELATIVE_PATH
};

class CScript
{
public:
	void ParseFromMemory(char* buffer, int size);
	qboolean GetToken(qboolean crossline);
	qboolean EndOfScript(qboolean crossline);
	bool ExpandMacroToken(char *&token_p);
	bool ExpandVariableToken(char *&token_p);
	void DefineMacro(char *macroname);
	qboolean TokenAvailable();
	void DefineVariable(char *variablename);
	bool AddMacroToStack(char *macroname);

	script_t	scriptstack[MAX_INCLUDES];
	script_t	*script = nullptr;
	int			scriptline;

	char    token[MAX_TOKEN];
	qboolean endofscript;
	qboolean tokenready;                     // only true if UnGetToken was just called

	script_t	*macrolist[256];
	int nummacros;

	std::vector<variable_t> m_vDefineVariable;
};

#endif // _GPUARAD_SCRIPT_H_