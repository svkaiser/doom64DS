#include "arm7.h"
#include <dswifi7.h>
#include <maxmod7.h>
#include "doomtype.h"
#include "d_player.h"
#include "t_bsp.h"

//
// IRQ_VBlank
//

void IRQ_VBlank(void)
{
    Wifi_Update();
}

//
// IRQ_VCount
//

void IRQ_VCount()
{
    inputGetAndSend();
}

//
// powerButtonCB
//

volatile bool exitflag = false;

void powerButtonCB()
{
    exitflag = true;
}

//
// I_PollArm9Messages
//

static void I_PollArm9Messages(int bytes, void *userdata)
{
    fifomsg_t msg;

    fifoGetDatamsg(FIFO_USER_01, bytes, (u8*)&msg);

    switch(msg.type)
    {
    case FIFO_MSG_PLAYERDATA:
        player = (player_t*)msg.arg[0].arg_p;
        break;
    case FIFO_MSG_LINEDEFDATA:
        lines = (line_t*)msg.arg[0].arg_p;
        numlines = msg.arg[1].arg_i;
        break;
    case FIFO_MSG_SSECTDATA:
        subsectors = (subsector_t*)msg.arg[0].arg_p;
        numsubsectors = msg.arg[1].arg_i;
        break;
    case FIFO_MSG_VERTEXDATA:
        vertexes = (vertex_t*)msg.arg[0].arg_p;
        numvertexes = msg.arg[1].arg_i;
        break;
    case FIFO_MSG_FINESINEDATA:
        finesine = (fixed_t*)msg.arg[0].arg_p;
        break;
    case FIFO_MSG_FINECOSINEDATA:
        finecosine = (fixed_t*)msg.arg[0].arg_p;
        break;
    default:
        break;
    }
}

//
// ARM7 MAIN
//

int ARM7_MAIN(void)
{
    readUserSettings();

    // init irq
    irqInit();
    initClockIRQ();

    // init irq callback
    irqSet(IRQ_VCOUNT, IRQ_VCount);
    irqSet(IRQ_VBLANK, IRQ_VBlank);
    irqEnable(IRQ_VBLANK | IRQ_VCOUNT | IRQ_NETWORK);

    // init fifo system
    fifoInit();

    // install fifo subsystems
    installWifiFIFO();
    installSoundFIFO();
    installSystemFIFO();
    mmInstall(FIFO_MAXMOD);

    // init fifo callback for arm9 communication
    fifoSetDatamsgHandler(FIFO_USER_01, I_PollArm9Messages, 0);

    SetYtrigger(80);
    setPowerButtonCB(powerButtonCB);   

    // Keep the ARM7 mostly idle
    while(!exitflag)
    {
        if(0 == (REG_KEYINPUT & (KEY_SELECT | KEY_START | KEY_L | KEY_R)))
            exitflag = true;

	    swiIntrWait(1, IRQ_FIFO_NOT_EMPTY | IRQ_VBLANK);
    }

	return 0;
}
