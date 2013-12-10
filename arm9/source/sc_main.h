#ifndef __SC_MAIN__
#define __SC_MAIN__

typedef struct
{
    char    token[512];
    char*   buffer;
    char*   pointer_start;
    char*   pointer_end;
    int     linepos;
    int     rowpos;
    int     buffpos;
    int     buffsize;
} scparser_t;

extern scparser_t sc_parser;

typedef struct
{
    char*   token;
    int32   ptroffset;
    char    type;
} scdatatable_t;

void SC_Init(void);
void SC_Open(char* name);
void SC_Close(void);
void SC_Compare(char* token);
int SC_ReadTokens(void);
char* SC_GetString(void);
int SC_GetInteger(void);
int SC_SetData(byte* data, const scdatatable_t* table);
int SC_Find(dboolean forceupper);
char SC_GetChar(void);
void SC_Rewind(void);
void SC_Error(char* function);

#endif // __SC_MAIN__
