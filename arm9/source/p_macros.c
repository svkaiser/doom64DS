#include "z_zone.h"
#include "doomdef.h"
#include "p_local.h"
#include "p_spec.h"
#include "r_local.h"
#include "d_main.h"

thinker_t       *macrothinker    = NULL;
macrodef_t      *macro           = NULL;
macrodata_t     *nextmacro       = NULL;
mobj_t          *mobjmacro       = NULL;
short           macrocounter    = -1;
short           macroid         = -1;

int             taglist[MAXQUEUELIST];
int             taglistidx      = 0;

static dboolean bTriggerOnce = false;

//
// P_QueueSpecial
//

void P_QueueSpecial(mobj_t* mobj)
{
    if(!P_ActivateLineByTag(mobj->tid, mobj))
    {
        taglist[taglistidx] = mobj->tid;
        taglistidx = (taglistidx + 1) & (MAXQUEUELIST - 1);
    }
}

//
// P_RestartMacro
//

void P_RestartMacro(int id)
{
    macrodata_t *m;
    
    m = macro->data;
    
    for(; m != &macro->data[macro->count]; m++)
    {
        if(m->id == id)
        {
            nextmacro = m;
            return;
        }
    }
}

//
// P_InitMacroCounter
//

int P_InitMacroCounter(int counts)
{
    if(macrocounter == -1)
        macrocounter = counts;
    else
    {
        if(macrocounter-- > 0)
            P_RestartMacro(globalint);
        else
            macrocounter = 0;
    }
    
    return -1;
}

//
// P_MacroDetachThinker
//

void P_MacroDetachThinker(thinker_t* thinker)
{
    if(thinker == macrothinker)
        macrothinker = NULL;
}

//
// P_ToggleMacros
//
// Enable/disable a macro by setting its first ID to a negative value
// The purpose for this function remains a mystery but it has
// something to do with the elusive easter egg found in Breakdown.
//

void P_ToggleMacros(int tag, dboolean toggleon)
{
    macrodef_t* macro = &macros.def[MACROMASK(tag)];
    
    if(toggleon)
    {
        if(macro->data[0].id >= 0)
            return;
    }
    else
    {
        if(macro->data[0].id <= 0)
            return;
    }
    
    macro->data[0].id = !macro->data[0].id;
}

//
// P_InitMacroVars
//

void P_InitMacroVars(void)
{
    macro = NULL;
    macrothinker = NULL;
    nextmacro = NULL;
    mobjmacro = NULL;
    macroid = -1;
    macrocounter = -1;
    
    bTriggerOnce = false;
}

//
// P_StartMacro
//

int P_StartMacro(mobj_t* thing, line_t* line)
{
    if(macro)
        return 0;
    
    macro = &macros.def[MACROMASK(line->special)];
    
    if(macro->data[0].id <= 0)
    {
        if(macro->data[0].id == 0)
            line->special = 0;
        
        P_InitMacroVars();
        
        return 0;
    }
    
    // true if line can only be triggered once
    if(!(line->special & MLU_REPEAT))
        bTriggerOnce = true;
    else
        bTriggerOnce = false;
    
    macroid = MACROMASK(line->special);
    nextmacro = NULL;
    if(thing->flags & MF_SPECIAL)
        mobjmacro = players[0].mo;
    else
        mobjmacro = thing;
    macrocounter = -1;
    
    return 1;
}

//
// P_SuspendMacro
//

int P_SuspendMacro(int tag)
{
    int id = MACROMASK(tag);	// not used?
    
    if(id < 0)
        return 0;

    if(!macro)
        return 0;

    if(!macrothinker)
        return 0;
    
    P_RemoveThinker(macrothinker);
    P_InitMacroVars();
    
    return 1;
}

//
// P_RunMacros
//
// Process behind this is to trigger all specials in a batch and then
// wait until all thinkers are done before starting the next batch.
//
// This is runned at every P_Tick
//

void P_RunMacros(void)
{
    int currentID = 0;
    macrodata_t* m;
    thinker_t* thinker;
    line_t l;
    
    if(!macro)
    {
        for(currentID = 0; currentID < MAXQUEUELIST; currentID++)
        {
            if(taglist[currentID])
            {
                P_ActivateLineByTag(taglist[currentID], players[0].mo);
                taglist[currentID] = 0;
                return;
            }
        }
        return;
    }
    
    if(macrothinker)
        return;
    
    if(!nextmacro)
        m = macro->data;
    else
        m = nextmacro;

    thinker = NULL;
    
    for(currentID = m->id; m != &macro->data[macro->count]; m++)
    {
        if(m->id != currentID)
            break; // end of batch

        if(m->special == 0)
            continue;
        
        l.special = m->special;
        l.tag = m->tag;

        thinker = thinkercap.prev;
        
        if(P_DoSpecialLine(mobjmacro, &l, 0) == -1 && macrocounter)
            return;
        
        // Look for any new thinkers that need to be watched
        if(thinker != thinkercap.prev)
            thinker = thinkercap.prev;
        else
            thinker = NULL;
    }
    
    nextmacro = m;
    macrothinker = thinker;
    
    if(m == &macro->data[macro->count])
    {
        if(bTriggerOnce)
            macro->data[0].id = 0;
        
        P_InitMacroVars(); // End of macro. Reset vars
    }
}

