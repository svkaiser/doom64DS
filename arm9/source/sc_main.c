#include <ctype.h>

#include "ds.h"
#include "doomdef.h"
#include "doomtype.h"
#include "z_zone.h"
#include "w_wad.h"
#include "sc_main.h"

scparser_t sc_parser;

//
// SC_Open
//

void SC_Open(char* name)
{
    int lump;

    lump = W_CheckNumForName(name);

    if(lump <= -1)
    {
        sc_parser.buffsize   = I_ReadFile(name, (byte**)&sc_parser.buffer);

        if(sc_parser.buffsize == -1)
            I_Error("SC_Open: %s not found", name);
    }
    else
    {
        sc_parser.buffer     = W_CacheLumpNum(lump, PU_STATIC);
        sc_parser.buffsize   = W_LumpLength(lump);
    }

    sc_parser.pointer_start  = sc_parser.buffer;
    sc_parser.pointer_end    = sc_parser.buffer + sc_parser.buffsize;
    sc_parser.linepos        = 1;
    sc_parser.rowpos         = 1;
    sc_parser.buffpos        = 0;
}

//
// SC_Close
//

void SC_Close(void)
{
    Z_Free(sc_parser.buffer);

    sc_parser.buffer         = NULL;
    sc_parser.buffsize       = 0;
    sc_parser.pointer_start  = NULL;
    sc_parser.pointer_end    = NULL;
    sc_parser.linepos        = 0;
    sc_parser.rowpos         = 0;
    sc_parser.buffpos        = 0;
}

//
// SC_Compare
//

void SC_Compare(char* token)
{
    SC_Find(false);
    if(stricmp(sc_parser.token, token))
    {
        I_Error("SC_Compare: Expected '%s', found '%s' (line = %i, pos = %i)",
        token, sc_parser.token, sc_parser.linepos, sc_parser.rowpos);
    }
}

//
// SC_ReadTokens
//

int SC_ReadTokens(void)
{
    return (sc_parser.buffpos < sc_parser.buffsize);
}

//
// SC_GetString
//

char* SC_GetString(void)
{
    SC_Compare("="); // expect a '='
    SC_Find(false);  // get next token which should be a string

    return sc_parser.token;
}

//
// SC_GetInteger
//

int SC_GetInteger(void)
{
    SC_Compare("="); // expect a '='
    SC_Find(false);  // get next token which should be an integer

    return atoi(sc_parser.token);
}

//
// SC_SetData
//

int SC_SetData(byte* data, const scdatatable_t* table)
{
    int i;
    dboolean ok = false;

    for(i = 0; table[i].token; i++)
    {
        if(!stricmp(table[i].token, sc_parser.token))
        {
            byte* pointer = ((byte*)data + table[i].ptroffset);
            char* name;
            byte rgb[3];

            ok = true;

            switch(table[i].type)
            {
            case 's':
                name = SC_GetString();
                strncpy((char*)pointer, name, strlen(name));
                break;
            case 'S':
                name = SC_GetString();
                strupr(name);
                strncpy((char*)pointer, name, strlen(name));
                break;
            case 'i':
                *(int*)pointer = SC_GetInteger();
                break;
            case 'b':
                *(int*)pointer = true;
                break;
            case 'c':
                SC_Compare("="); // expect a '='
                SC_Find(false);
                rgb[0] = dhtoi(sc_parser.token) >> 3;
                SC_Find(false);
                rgb[1] = dhtoi(sc_parser.token) >> 3;
                SC_Find(false);
                rgb[2] = dhtoi(sc_parser.token) >> 3;
                *(rcolor*)pointer = ARGB16(0xff, rgb[0], rgb[1], rgb[2]);
                break;
            }

            break;
        }
    }

    return ok;
}

//
// SC_Find
//

int SC_Find(dboolean forceupper)
{
    char c = 0;
    int i = 0;
    dboolean comment = false;
    dboolean havetoken = false;
    dboolean string = false;

    memset(sc_parser.token, 0, 256);

    while(SC_ReadTokens())
    {
        c = SC_GetChar();

        if(c == '/')
            comment = true;

        if(comment == false)
        {
            if(c == '"')
            {
                if(!string)
                {
                    string = true;
                    continue;
                }
                else if(havetoken)
                {
                    c = SC_GetChar();

                    if(c != ',')
                        return true;
                    else
                    {
                        havetoken = false;
                        continue;
                    }
                }
                else
                {
                    if(SC_GetChar() == '"')
                    {
                        if(SC_GetChar() == ',')
                            continue;
                        else
                        {
                            SC_Rewind();
                            SC_Rewind();
                        }
                    }
                    else
                        SC_Rewind();
                }
            }

            if(!string)
            {
                if(c > ' ')
                {
                    havetoken = true;
                    sc_parser.token[i++] =
                        forceupper ? toupper((int)c) : c;
                }
                else if(havetoken)
                    return true;
            }
            else
            {
                if(c >= ' ' && c != '"')
                {
                    havetoken = true;
                    sc_parser.token[i++] =
                        forceupper ? toupper((int)c) : c;
                }
            }
        }

        if(c == '\n')
        {
            sc_parser.linepos++;
            sc_parser.rowpos = 1;
            comment = false;
            if(string)
                sc_parser.token[i++] = c;
        }
    }

    return false;
}

//
// SC_GetChar
//

char SC_GetChar(void)
{
    sc_parser.rowpos++;
    return sc_parser.buffer[sc_parser.buffpos++];
}

//
// SC_Rewind
//

void SC_Rewind(void)
{
    sc_parser.rowpos--;
    sc_parser.buffpos--;
}

//
// SC_Error
//

void SC_Error(char* function)
{
    if(sc_parser.token[0] < ' ')
        return;

    I_Error("%s: Unknown token: '%s' (line = %i, pos = %i)",
        function, sc_parser.token, sc_parser.linepos, sc_parser.rowpos);
}

//
// SC_Init
//

void SC_Init(void)
{
    //
    // clear variables
    //
    memset(&sc_parser, 0, sizeof(scparser_t));
}
