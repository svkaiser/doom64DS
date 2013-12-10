#include <math.h>

#include "m_fixed.h"
#include "tables.h"
#include "r_local.h"
#include "z_zone.h"
#include "w_wad.h"
#include "p_local.h"
#include "d_main.h"

// render view globals
fixed_t         viewx;
fixed_t         viewy;
fixed_t         viewz;
angle_t         viewangle;
angle_t         viewpitch;
fixed_t         quakeviewx;
fixed_t         quakeviewy;
angle_t         viewangleoffset;
fixed_t         viewsin[2];
fixed_t         viewcos[2];
int             frametic = 0;
byte            occludeBuffer[SCREENWIDTH];

// sprite info globals
spritedef_t     *spriteinfo;

// gfx texture globals
int             t_start;
int             t_end;
int             numtextures;
int             swx_start;

// gfx sprite globals
int             s_start;
int             s_end;
int             numgfxsprites;
short           *spriteoffset;
short           *spritetopoffset;
short           *spritewidth;
short           *spriteheight;
byte            *spritetiles;
short           **spritetilelist;

// gfx img globals
int             g_start;
int             g_end;
int             numgfximgs;

extern int bsptic;
extern int rendertic;

//
// R_PointToAngle2
//
angle_t R_PointToAngle2(fixed_t x1, fixed_t y1, fixed_t x2, fixed_t y2)
{
    return _R_PointToAngle(x2 - x1, y2 - y1);
}

//
// R_PointToPitch
//

angle_t R_PointToPitch(fixed_t z1, fixed_t z2, fixed_t dist)
{
    return R_PointToAngle2(0, z1, dist, z2);
}

//
// R_PointOnSide
// Traverse BSP (sub) tree,
// check point against partition plane.
// Returns side 0 (front) or 1 (back).
//

int PUREFUNC R_PointOnSide(fixed_t x, fixed_t y, node_t* node)
{
    fixed_t	dx;
    fixed_t	dy;
    fixed_t	left;
    fixed_t	right;
    
    if(!node->dx)
    {
        if(x <= node->x)
            return node->dy > 0;
        
        return node->dy < 0;
    }
    if(!node->dy)
    {
        if(y <= node->y)
            return node->dx < 0;
        
        return node->dx > 0;
    }
    
    dx = (x - node->x);
    dy = (y - node->y);
    
    left = F2INT(node->dy) * F2INT(dx);
    right = F2INT(dy) * F2INT(node->dx);
    
    if(right < left)
    {
        // front side
        return 0;
    }

    // back side
    return 1;
}

//
// R_PointInSubsector
//

subsector_t* R_PointInSubsector(fixed_t x, fixed_t y)
{
    node_t*	node;
    int		side;
    int		nodenum;
    
    // single subsector is a special case
    if (!numnodes)
        return subsectors;
    
    nodenum = numnodes-1;
    
    while (!(nodenum & NF_SUBSECTOR))
    {
        node = &nodes[nodenum];
        side = R_PointOnSide (x, y, node);
        nodenum = node->children[side];
    }
    
    return &subsectors[nodenum & ~NF_SUBSECTOR];
}

//
// R_RenderView
//

static void R_RenderView(player_t* player)
{
    nextssect = ssectlist;
    vissprite = visspritelist;

    memset(occludeBuffer, 0, SCREENWIDTH);
    skyvisible = false;

    if(devparm)
        cpuStartTiming(2);

    if(player->cameratarget != player->mo && gamemap == 33)
        R_RenderBSPNodeNoClip(numnodes - 1);
    else
        R_RenderBSPNode(numnodes - 1);

    if(devparm)
        bsptic = timerTicks2msec(cpuEndTiming());
}

//
// R_Init
//

void R_Init(void)
{
    int i = 0;
    int a = 0;
    double an;

    //
    // [d64] build finesine table
    //
    for(i = 0; i < (5 * FINEANGLES / 4); i++)
    {
        an = a * M_PI / (double)FINEANGLES;
        finesine[i] = (fixed_t)(sin(an) * (double)FRACUNIT);
        a += 2;
    }

    I_SendDataToArm7(FIFO_MSG_FINESINEDATA, (void*)finesine, 0);
    I_SendDataToArm7(FIFO_MSG_FINECOSINEDATA, (void*)finecosine, 0);
    R_InitData();
}

//
// R_DrawFrame
//

void R_DrawFrame(void)
{
    angle_t pitch;
    angle_t angle;
    fixed_t cam_z;
    mobj_t* viewcamera;
    player_t* player;
    pspdef_t* psp;
    int i;

    for(i = 0; i < 32; i++)
        GFX_FOG_TABLE[i] = (i * 4);

    GFX_FOG_TABLE[31] = 0x7F;
    GFX_CONTROL = (GFX_CONTROL & 0xF0FF) | 0x700;

    if(automapactive)
    {
        GFX_CLEAR_COLOR = 0x1F0000;
        return;
    }
    else if(sky != NULL)
    {
        if(sky->flags & SKF_CLOUD)
            GFX_CLEAR_COLOR = sky->skycolor[1];
        else if(sky->flags & SKF_VOID)
            GFX_CLEAR_COLOR = sky->skycolor[2];
        else
            GFX_CLEAR_COLOR = 0x1F0000;

        GFX_FOG_COLOR = sky->fogcolor;
        GFX_FOG_OFFSET = GL_MAX_DEPTH - ((1000 - sky->fognear) * 10);
    }
    else
    {
        GFX_CLEAR_COLOR = 0x1F0000;
        GFX_FOG_COLOR = 0;
        GFX_FOG_OFFSET = GL_MAX_DEPTH - 192;
    }

    //
    // setup view rotation/position
    //
    player = &players[consoleplayer];
    viewcamera = player->cameratarget;
    angle = (viewcamera->angle + quakeviewx) + viewangleoffset;
    pitch = viewcamera->pitch + ANG90;
    cam_z = (viewcamera == player->mo ? player->viewz : viewcamera->z) + quakeviewy;

    if(viewcamera == player->mo)
        pitch += player->recoilpitch;

    viewangle = angle;
    viewpitch = pitch;
    viewx = viewcamera->x;
    viewy = viewcamera->y;
    viewz = cam_z;

    viewsin[0]  = dsin(viewangle + ANG90);
    viewsin[1]  = dsin(viewpitch - ANG90);
    viewcos[0]  = dcos(viewangle + ANG90);
    viewcos[1]  = dcos(viewpitch - ANG90);

    MATRIX_CONTROL      = GL_PROJECTION;
    MATRIX_IDENTITY     = 0;
    MATRIX_CONTROL      = GL_MODELVIEW;
    MATRIX_IDENTITY     = 0;

    if(skyvisible)
        R_DrawSky();

    MATRIX_PUSH = 0;

    MATRIX_TRANSLATE    = -(16 << 4);
    MATRIX_TRANSLATE    = -0;
    MATRIX_TRANSLATE    = -0;

    MATRIX_PUSH = 0;

    gluPerspective(74, 256.0f / 192.0f, 0.002f, 1000);

    glRotatef(-TRUEANGLES(viewpitch) + 90, 1.0f, 0.0f, 0.0f);
    glRotatef(-TRUEANGLES(viewangle) + 90, 0.0f, 1.0f, 0.0f);

    MATRIX_SCALE        =  0x1000;
    MATRIX_SCALE        =  0x1000;
    MATRIX_SCALE        = -0x1000;
    MATRIX_TRANSLATE    = -F2DSFIXED(viewx);
    MATRIX_TRANSLATE    = -F2DSFIXED(viewz);
    MATRIX_TRANSLATE    = -F2DSFIXED(viewy);

    D_IncValidCount();
    D_UpdateTiccmd();

    R_RenderView(player);

    if(devparm)
        cpuStartTiming(2);

    R_DrawScene();

    if(devparm)
        rendertic = timerTicks2msec(cpuEndTiming());

    MATRIX_POP = GFX_MTX_STACK_LEVEL;

    for(psp = player->psprites; psp < &player->psprites[NUMPSPRITES]; psp++)
    {
        if(psp->state && player->cameratarget == player->mo)
            R_DrawPSprite(psp, player->mo->subsector->sector, player);
    }
}

