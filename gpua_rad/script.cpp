#include "script.h"

void CScript::ParseFromMemory(char * buffer, int size)
{
	script = scriptstack;
	script++;
	if (script == &scriptstack[MAX_INCLUDES])
		pp::Error("Script file exceeded MAX_INCLUDES");
	strcpy(script->filename, "memory buffer");

	script->buffer = buffer;
	script->line = 1;
	script->script_p = script->buffer;
	script->end_p = script->buffer + size;

	endofscript = false;
	tokenready = false;
}

qboolean CScript::GetToken(qboolean crossline)
{
	char    *token_p;

	if (tokenready)                         // is a token allready waiting?
	{
		tokenready = false;
		return true;
	}

	// printf("script_p %x (%x)\n", script->script_p, script->end_p ); fflush( stdout );

	if (script->script_p >= script->end_p)
	{
		return EndOfScript(crossline);
	}

	tokenready = false;

	// skip space, ctrl chars
skipspace:
	while (*script->script_p <= 32)
	{
		if (script->script_p >= script->end_p)
		{
			return EndOfScript(crossline);
		}
		if (*(script->script_p++) == '\n')
		{
			if (!crossline)
			{
				pp::Error("Line %i is incomplete\n", scriptline);
			}
			scriptline = ++script->line;
		}
	}

	if (script->script_p >= script->end_p)
	{
		return EndOfScript(crossline);
	}

	// strip single line comments
	if (*script->script_p == ';' || *script->script_p == '#' ||		 // semicolon and # is comment field
		(*script->script_p == '/' && *((script->script_p) + 1) == '/')) // also make // a comment field
	{
		if (!crossline)
			pp::Error("Line %i is incomplete\n", scriptline);
		while (*script->script_p++ != '\n')
		{
			if (script->script_p >= script->end_p)
			{
				return EndOfScript(crossline);
			}
		}
		scriptline = ++script->line;
		goto skipspace;
	}

	//  strip out matching /* */ comments
	if (*script->script_p == '/' && *((script->script_p) + 1) == '*')
	{
		script->script_p += 2;
		while (*script->script_p != '*' || *((script->script_p) + 1) != '/')
		{
			if (*script->script_p++ != '\n')
			{
				if (script->script_p >= script->end_p)
				{
					return EndOfScript(crossline);
				}

				scriptline = ++script->line;
			}
		}
		script->script_p += 2;
		goto skipspace;
	}

	// copy token to buffer
	token_p = token;

	if (*script->script_p == '"')
	{
		// quoted token
		script->script_p++;
		while (*script->script_p != '"')
		{
			*token_p++ = *script->script_p++;
			if (script->script_p == script->end_p)
				break;
			if (token_p == &token[MAX_TOKEN])
				pp::Error("Token too large on line %i\n", scriptline);
		}
		script->script_p++;
	}
	else	// regular token
		while (*script->script_p > 32 && *script->script_p != ';')
		{
			if (!ExpandMacroToken(token_p))
			{
				if (!ExpandVariableToken(token_p))
				{
					*token_p++ = *script->script_p++;
					if (script->script_p == script->end_p)
						break;
					if (token_p == &token[MAX_TOKEN])
						pp::Error("Token too large on line %i\n", scriptline);

				}
			}
		}

	// add null to end of token
	*token_p = 0;

	// check for other commands
	if (!stricmp(token, "$include"))
	{
		pp::Error("Trying to use non implemented feature");
		/*GetToken(false);

		bool bFallbackToToken = true;

		CUtlVector< CUtlString > expandedPathList;

		if (CmdLib_ExpandWithBasePaths(expandedPathList, token) > 0)
		{
			for (int i = 0; i < expandedPathList.Count(); ++i)
			{
				CUtlVector< CUtlString > findFileList;
				FindFileAbsoluteList(findFileList, expandedPathList[i].String());

				if (findFileList.Count() > 0)
				{
					bFallbackToToken = false;

					// Only add the first set of glob matches from the first base path
					for (int j = 0; j < findFileList.Count(); ++j)
					{
						AddScriptToStack(const_cast< char * >(findFileList[j].String()));
					}

					break;
				}
			}
		}

		if (bFallbackToToken)
		{
			AddScriptToStack(token);
		}

		return GetToken(crossline);*/
	}
	else if (!stricmp(token, "$definemacro"))
	{
		GetToken(false);
		DefineMacro(token);
		return GetToken(crossline);
	}
	else if (!stricmp(token, "$definevariable"))
	{
		GetToken(false);
		DefineVariable(token);
		return GetToken(crossline);
	}
	else if (AddMacroToStack(token))
	{
		return GetToken(crossline);
	}
	
	return true;
}

qboolean CScript::EndOfScript(qboolean crossline)
{
	if (!crossline)
		pp::Error("Line %i is incomplete\n", scriptline);

	if (!strcmp(script->filename, "memory buffer"))
	{
		endofscript = true;
		return false;
	}

	free(script->buffer);
	script->buffer = NULL;
	if (script == scriptstack + 1)
	{
		endofscript = true;
		return false;
	}
	script--;
	scriptline = script->line;
	// printf ("returning to %s\n", script->filename);
	return GetToken(crossline);
}

bool CScript::ExpandMacroToken(char *&token_p)
{
	if (script->nummacroparams && *script->script_p == '$')
	{
		char *cp = script->script_p + 1;

		while (*cp > 32 && *cp != '$')
		{
			cp++;
		}

		// found a word with $'s on either end?
		if (*cp != '$')
			return false;

		// get token pointer
		char *tp = script->script_p + 1;
		int len = (cp - tp);
		*(tp + len) = '\0';

		// lookup macro parameter
		int index = 0;
		for (index = 0; index < script->nummacroparams; index++)
		{
			if (stricmp(script->macroparam[index], tp) == 0)
				break;
		}
		if (index >= script->nummacroparams)
		{
			pp::Error("unknown macro token \"%s\" in %s\n", tp, script->filename);
		}

		// paste token into 
		len = strlen(script->macrovalue[index]);
		strcpy(token_p, script->macrovalue[index]);
		token_p += len;

		script->script_p = cp + 1;

		if (script->script_p >= script->end_p)
			pp::Error("Macro expand overflow\n");

		if (token_p >= &token[MAX_TOKEN])
			pp::Error("Token too large on line %i\n", scriptline);

		return true;
	}
	return false;
}

/*
==============
==============
*/
// FIXME: this should create a new script context so the individual tokens in the variable can be parsed
bool CScript::ExpandVariableToken(char *&token_p)
{
	if (*script->script_p == '$')
	{
		char *cp = script->script_p + 1;

		while (*cp > 32 && *cp != '$')
		{
			cp++;
		}

		// found a word with $'s on either end?
		if (*cp != '$')
			return false;

		// get token pointer
		char *tp = script->script_p + 1;
		int len = (cp - tp);
		*(tp + len) = '\0';

		// lookup macro parameter

		int index;
		for (index = 0; index < m_vDefineVariable.size(); index++)
		{
			if (strnicmp(m_vDefineVariable[index].param, tp, len) == 0)
				break;
		}

		if (index >= m_vDefineVariable.size())
		{
			pp::Error("unknown variable token \"%s\" in %s\n", tp, script->filename);
		}

		// paste token into 
		len = strlen(m_vDefineVariable[index].value);
		strcpy(token_p, m_vDefineVariable[index].value);
		token_p += len;

		script->script_p = cp + 1;

		if (script->script_p >= script->end_p)
			pp::Error("Macro expand overflow\n");

		if (token_p >= &token[MAX_TOKEN])
			pp::Error("Token too large on line %i\n", scriptline);

		return true;
	}
	return false;
}

void CScript::DefineMacro(char *macroname)
{
	script_t	*pmacro = (script_t *)malloc(sizeof(script_t));

	strcpy(pmacro->filename, macroname);
	pmacro->line = script->line;
	pmacro->nummacroparams = 0;

	char *mp = pmacro->macrobuffer;
	char *cp = script->script_p;

	while (TokenAvailable())
	{
		GetToken(false);

		if (token[0] == '\\' && token[1] == '\\')
		{
			break;
		}
		cp = script->script_p;

		pmacro->macroparam[pmacro->nummacroparams++] = mp;

		strcpy(mp, token);
		mp += strlen(token) + 1;

		if (mp >= pmacro->macrobuffer + sizeof(pmacro->macrobuffer))
			pp::Error("Macro buffer overflow\n");
	}
	// roll back script_p to previous valid location
	script->script_p = cp;

	// find end of macro def
	while (*cp && *cp != '\n')
	{
		//Msg("%d ", *cp );
		if (*cp == '\\' && *(cp + 1) == '\\')
		{
			// skip till end of line
			while (*cp && *cp != '\n')
			{
				*cp = ' '; // replace with spaces
				cp++;
			}

			if (*cp)
			{
				cp++;
			}
		}
		else
		{
			cp++;
		}
	}

	int size = (cp - script->script_p);

	pmacro->buffer = (char *)malloc(size + 1);
	memcpy(pmacro->buffer, script->script_p, size);
	pmacro->buffer[size] = '\0';
	pmacro->end_p = &pmacro->buffer[size];

	macrolist[nummacros++] = pmacro;

	script->script_p = cp;
}

qboolean CScript::TokenAvailable()
{
	char    *search_p;

	if (tokenready)                         // is a token allready waiting?
	{
		return true;
	}

	search_p = script->script_p;

	if (search_p >= script->end_p)
		return false;

	while (*search_p <= 32)
	{
		if (*search_p == '\n')
			return false;
		search_p++;
		if (search_p == script->end_p)
			return false;

	}

	if (*search_p == ';' || *search_p == '#' ||		 // semicolon and # is comment field
		(*search_p == '/' && *((search_p)+1) == '/')) // also make // a comment field
		return false;

	return true;
}

void CScript::DefineVariable(char *variablename)
{
	variable_t v;

	v.param = strdup(variablename);

	GetToken(false);

	v.value = strdup(token);

	m_vDefineVariable.push_back(v);
}

/*
==============
==============
*/
bool CScript::AddMacroToStack(char *macroname)
{
	// lookup macro
	if (macroname[0] != '$')
		return false;

	int i;
	for (i = 0; i < nummacros; i++)
	{
		if (strcmpi(macrolist[i]->filename, &macroname[1]) == 0)
		{
			break;
		}
	}
	if (i == nummacros)
		return false;

	script_t *pmacro = macrolist[i];

	// get tokens
	script_t	*pnext = script + 1;

	pnext++;
	if (pnext == &scriptstack[MAX_INCLUDES])
		pp::Error("script file exceeded MAX_INCLUDES");

	// get tokens
	char *cp = pnext->macrobuffer;

	pnext->nummacroparams = pmacro->nummacroparams;

	for (i = 0; i < pnext->nummacroparams; i++)
	{
		GetToken(false);

		strcpy(cp, token);
		pnext->macroparam[i] = pmacro->macroparam[i];
		pnext->macrovalue[i] = cp;

		cp += strlen(token) + 1;

		if (cp >= pnext->macrobuffer + sizeof(pnext->macrobuffer))
			pp::Error("Macro buffer overflow\n");
	}

	script = pnext;
	strcpy(script->filename, pmacro->filename);

	int size = pmacro->end_p - pmacro->buffer;
	script->buffer = (char *)malloc(size + 1);
	memcpy(script->buffer, pmacro->buffer, size);
	pmacro->buffer[size] = '\0';
	script->script_p = script->buffer;
	script->end_p = script->buffer + size;
	script->line = pmacro->line;

	return true;
}
