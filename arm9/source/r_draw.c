#include "m_fixed.h"
#include "tables.h"
#include "r_local.h"
#include "z_zone.h"
#include "p_local.h"
#include "w_wad.h"
#include "s_sound.h"
#include "sounds.h"

#define FIRESKYSIZE 64

dboolean        skyvisible = false;
int             skypicnum = -1;
int             skybackdropnum = -1;
int             skyflatnum = -1;
skydef_t*       sky;
int             thunderCounter = 0;
int             lightningCounter = 0;
int             thundertic = 1;
dboolean        skyfadeback = false;
fixed_t         scrollfrac;

static int      cloud_offsetx = 0;
static int      cloud_offsety = 0;

static int spr_polyid = 0;
static subsector_t* spr_prevssect = NULL;

//
// R_DrawSwitch
//

static void R_DrawSwitch(seg_t* seg, dtexture texture, fixed_t top, fixed_t bottom)
{
    fixed_t cenx;
    fixed_t ceny;
    fixed_t f1;
    fixed_t f2;
    fixed_t s1;
    fixed_t s2;
    light_t* light;
    int x1;
    int x2;
    int y1;
    int y2;
    int z1;
    int z2;
    int r;
    int g;
    int b;

    if(texture <= 0)
        return;

    I_CheckGFX();

    light = &lights[frontsector->colors[LIGHT_THING]];

    r = light->active_r;
    g = light->active_g;
    b = light->active_b;

    //
    // sequenced lighting is too expensive on the DS
    // instead of creating a glowing plane, just amplify the
    // RGB values
    //
    if(frontsector->lightlevel)
    {
        r = MIN(r + frontsector->lightlevel, 255);
        g = MIN(g + frontsector->lightlevel, 255);
        b = MIN(b + frontsector->lightlevel, 255);
    }

    if(nolights)
        GFX_COLOR = 0x1F7FFF;
    else
        GFX_COLOR = RGB8(r, g, b);

    cenx    = (seg->linedef->v1->x + seg->linedef->v2->x) >> 1;
    ceny    = (seg->linedef->v1->y + seg->linedef->v2->y) >> 1;
    f1      = FixedMul(2*FRACUNIT, dcos(seg->angle + ANG90));
    f2      = FixedMul(2*FRACUNIT, dsin(seg->angle + ANG90));
    s1      = FixedMul(16*FRACUNIT, dcos(seg->angle));
    s2      = FixedMul(16*FRACUNIT, dsin(seg->angle));
    x1      = F2DSFIXED((cenx - s1) - f1);
    x2      = F2DSFIXED((cenx + s1) - f1);
    y1      = F2DSFIXED((ceny - s2) - f2);
    y2      = F2DSFIXED((ceny + s2) - f2);
    z1      = F2DSFIXED(top);
    z2      = F2DSFIXED(bottom);

    R_LoadTexture(texture, false, false, false);

    GFX_POLY_FORMAT = POLY_ALPHA(31) | POLY_ID(0) | POLY_CULL_BACK | POLY_MODULATION | POLY_FOG;
    GFX_BEGIN       = GL_TRIANGLE_STRIP;
    GFX_TEX_COORD   = COORD_PACK(0, 0);
    GFX_VERTEX16    = VERTEX_PACK(x2, z1);
    GFX_VERTEX16    = VERTEX_PACK(y2, 0);
    GFX_TEX_COORD   = COORD_PACK(32, 0);
    GFX_VERTEX16    = VERTEX_PACK(x1, z1);
    GFX_VERTEX16    = VERTEX_PACK(y1, 0);
    GFX_TEX_COORD   = COORD_PACK(0, 32);
    GFX_VERTEX16    = VERTEX_PACK(x2, z2);
    GFX_VERTEX16    = VERTEX_PACK(y2, 0);
    GFX_TEX_COORD   = COORD_PACK(32, 32);
    GFX_VERTEX16    = VERTEX_PACK(x1, z2);
    GFX_VERTEX16    = VERTEX_PACK(y1, 0);
}

//
// R_DrawLine
//

static void R_DrawLine(seg_t* seg, fixed_t top, fixed_t bottom,
                       dtexture texture, light_t* l1, light_t* l2,
                       fixed_t u1, fixed_t u2, fixed_t v1, fixed_t v2)
{
    int x1, x2;
    int y1, y2;
    int z1, z2;
    int r1, r2;
    int g1, g2;
    int b1, b2;
    line_t* line;
    side_t* side;

    I_CheckGFX();

    r1 = l1->active_r;
    g1 = l1->active_g;
    b1 = l1->active_b;
    r2 = l2->active_r;
    g2 = l2->active_g;
    b2 = l2->active_b;

    line = seg->linedef;
    side = seg->sidedef;

    //
    // sequenced lighting is too expensive on the DS
    // instead of creating a glowing plane, just amplify the
    // RGB values
    //
    if(frontsector->lightlevel)
    {
        r1 = MIN(r1 + frontsector->lightlevel, 255);
        g1 = MIN(g1 + frontsector->lightlevel, 255);
        b1 = MIN(b1 + frontsector->lightlevel, 255);
        r2 = MIN(r2 + frontsector->lightlevel, 255);
        g2 = MIN(g2 + frontsector->lightlevel, 255);
        b2 = MIN(b2 + frontsector->lightlevel, 255);
    }

    if(line->flags & ML_TWOSIDED)
    {
        int height = 0;
        int sideheight1 = 0;
        int sideheight2 = 0;

        height = frontsector->ceilingheight - frontsector->floorheight;

        if(bottom != frontsector->floorheight)
        {
            if(!(line->flags & ML_BLENDFULLTOP))
            {
                sideheight1 = seg->backsector->ceilingheight - seg->frontsector->floorheight;
                sideheight2 = seg->frontsector->ceilingheight - seg->backsector->ceilingheight;

                //
                // math for RGB values is done in fixed point first
                //
                if(!nolights)
                {
                    r1 = F2INT(FixedMul(FixedDiv(INT2F(l1->active_r), height), sideheight1));
                    g1 = F2INT(FixedMul(FixedDiv(INT2F(l1->active_g), height), sideheight1));
                    b1 = F2INT(FixedMul(FixedDiv(INT2F(l1->active_b), height), sideheight1));
                    r2 = F2INT(FixedMul(FixedDiv(INT2F(l2->active_r), height), sideheight2));
                    g2 = F2INT(FixedMul(FixedDiv(INT2F(l2->active_g), height), sideheight2));
                    b2 = F2INT(FixedMul(FixedDiv(INT2F(l2->active_b), height), sideheight2));

                    r2 = MIN(r1+r2, 255);
                    g2 = MIN(g1+g2, 255);
                    b2 = MIN(b1+b2, 255);
                    r1 = l1->active_r;
                    g1 = l1->active_g;
                    b1 = l1->active_b;
                }
            }

            if(line->flags & ML_INVERSEBLEND)
            {
                r1 = l2->active_r;
                g1 = l2->active_g;
                b1 = l2->active_b;
                r2 = l1->active_r;
                g2 = l1->active_g;
                b2 = l1->active_b;
            }
        }
        else if(top != frontsector->ceilingheight && !(line->flags & ML_BLENDFULLBOTTOM))
        {
            sideheight1 = seg->backsector->floorheight - seg->frontsector->floorheight;
            sideheight2 = seg->frontsector->ceilingheight - seg->backsector->floorheight;

            //
            // math for RGB values is done in fixed point first
            //
            if(!nolights)
            {
                r1 = F2INT(FixedMul(FixedDiv(INT2F(l1->active_r), height), sideheight1));
                g1 = F2INT(FixedMul(FixedDiv(INT2F(l1->active_g), height), sideheight1));
                b1 = F2INT(FixedMul(FixedDiv(INT2F(l1->active_b), height), sideheight1));
                r2 = F2INT(FixedMul(FixedDiv(INT2F(l2->active_r), height), sideheight2));
                g2 = F2INT(FixedMul(FixedDiv(INT2F(l2->active_g), height), sideheight2));
                b2 = F2INT(FixedMul(FixedDiv(INT2F(l2->active_b), height), sideheight2));

                r1 = MIN(r1+r2, 255);
                g1 = MIN(g1+g2, 255);
                b1 = MIN(b1+b2, 255);
                r2 = l2->active_r;
                g2 = l2->active_g;
                b2 = l2->active_b;
            }
        }
    }

    x1 = F2DSFIXED(side->v1->x);
    x2 = F2DSFIXED(side->v2->x);
    y1 = F2DSFIXED(side->v1->y);
    y2 = F2DSFIXED(side->v2->y);
    z1 = F2DSFIXED(top);
    z2 = F2DSFIXED(bottom);

    R_LoadTexture(texture,
        (line->flags & ML_HMIRROR),
        (line->flags & ML_VMIRROR),
        (line->flags & ML_DRAWMIDTEXTURE));

    if(nolights)
        r1 = r2 = g1 = g2 = b1 = b2 = 255;

    GFX_POLY_FORMAT = POLY_ALPHA(31) | POLY_ID(0) | POLY_CULL_BACK | POLY_MODULATION | POLY_FOG;
    GFX_BEGIN       = GL_TRIANGLE_STRIP;
    GFX_COLOR       = RGB8(r1, g1, b1);
    GFX_TEX_COORD   = COORD_PACK(F2INT(u1), F2INT(v1));
    GFX_VERTEX16    = VERTEX_PACK(x2, z1);
    GFX_VERTEX16    = VERTEX_PACK(y2, 0);
    GFX_TEX_COORD   = COORD_PACK(F2INT(u2), F2INT(v1));
    GFX_VERTEX16    = VERTEX_PACK(x1, z1);
    GFX_VERTEX16    = VERTEX_PACK(y1, 0);
    GFX_COLOR       = RGB8(r2, g2, b2);
    GFX_TEX_COORD   = COORD_PACK(F2INT(u1), F2INT(v2));
    GFX_VERTEX16    = VERTEX_PACK(x2, z2);
    GFX_VERTEX16    = VERTEX_PACK(y2, 0);
    GFX_TEX_COORD   = COORD_PACK(F2INT(u2), F2INT(v2));
    GFX_VERTEX16    = VERTEX_PACK(x1, z2);
    GFX_VERTEX16    = VERTEX_PACK(y1, 0);
}

//
// R_DrawSeg
//

static void R_DrawSeg(seg_t* seg)
{
    line_t*     linedef;
    side_t*     sidedef;
    fixed_t     top;
    fixed_t     bottom;
    fixed_t     btop;
    fixed_t     bbottom;
    light_t*    l1;
    light_t*    l2;
    fixed_t     col;
    fixed_t     row;
    fixed_t     offset;
    
    linedef = seg->linedef;
    sidedef = seg->sidedef;

    if(!linedef)
        return;

    col = sidedef->length;
    row = sidedef->rowoffset;

    if(linedef->flags & ML_BLENDING)
    {
        l1 = &lights[frontsector->colors[LIGHT_UPRWALL]];
        l2 = &lights[frontsector->colors[LIGHT_LWRWALL]];
    }
    else
    {
        l1 = &lights[frontsector->colors[LIGHT_THING]];
        l2 = &lights[frontsector->colors[LIGHT_THING]];
    }
    
    top = frontsector->ceilingheight;
    bottom = frontsector->floorheight;
    
    if(seg->backsector)
    {
        btop = seg->backsector->ceilingheight;
        bbottom = seg->backsector->floorheight;
        
        if((frontsector->ceilingpic == skyflatnum) && (seg->backsector->ceilingpic == skyflatnum))
            btop = top;
        
        //
        // botom side seg
        //
        if(bottom < bbottom)
        {
            if(seg->sidedef[0].bottomtexture != 1)
            {
                fixed_t v1;
                fixed_t v2;

                if(linedef->flags & ML_DONTPEGBOTTOM)
                {
                    v1 = row + (top - bbottom);
                    v2 = row + (top - bottom);
                }
                else
                {
                    v1 = row;
                    v2 = row + (bbottom - bottom);
                }

                R_DrawLine(
                    seg,
                    bbottom,
                    bottom,
                    sidedef->bottomtexture,
                    l1,
                    l2,
                    col + sidedef->textureoffset,
                    sidedef->textureoffset,
                    v1,
                    v2
                    );
            }
            
            bottom = bbottom;
        }
        
        //
        // upper side seg
        //
        if(top > btop)
        {
            if(seg->sidedef[0].toptexture != 1)
            {
                fixed_t v1;
                fixed_t v2;

                if(linedef->flags & ML_DONTPEGTOP)
                {
                    v1 = row;
                    v2 = row + (top - btop);
                }
                else
                {
                    v2 = row;
                    v1 = row - (top - btop);
                }

                R_DrawLine(
                    seg,
                    top,
                    btop,
                    sidedef->toptexture,
                    l1,
                    l2,
                    col + sidedef->textureoffset,
                    sidedef->textureoffset,
                    v1,
                    v2
                    );
            }
            
            top = btop;
        }

        if(SWITCHMASK(linedef->flags))
        {
            if(SWITCHMASK(linedef->flags) == ML_SWITCHX02)
            {
                offset = seg->backsector->floorheight - (16*FRACUNIT - (seg->sidedef->rowoffset));
                R_DrawSwitch(seg, seg->sidedef->toptexture, offset, offset - (32*FRACUNIT));
            }
            else if(SWITCHMASK(linedef->flags) == ML_SWITCHX04)
            {
                offset = seg->backsector->ceilingheight + (16*FRACUNIT + (seg->sidedef->rowoffset));
                R_DrawSwitch(seg, seg->sidedef->bottomtexture, offset + (32*FRACUNIT), offset);
            }
            else
            {
                if(seg->backsector->floorheight > seg->frontsector->floorheight)
                {
                    offset = seg->backsector->floorheight - (16*FRACUNIT - (seg->sidedef->rowoffset));
                    R_DrawSwitch(seg, seg->sidedef->midtexture, offset, offset - (32*FRACUNIT));
                }
                else if(seg->backsector->ceilingheight < seg->frontsector->ceilingheight)
                {
                    offset = seg->backsector->ceilingheight + (16*FRACUNIT + (seg->sidedef->rowoffset));
                    R_DrawSwitch(seg, seg->sidedef->midtexture, offset + (32*FRACUNIT), offset);
                }
            }
        }
    }

    //
    // middle side seg
    //
    if(sidedef->midtexture != 1)
    {
        fixed_t v1;
        fixed_t v2;

        v1 = row;
        v2 = row + (top - bottom);

        if(seg->backsector)
        {
            if(!(linedef->flags & ML_DRAWMIDTEXTURE))
                return;

            btop = seg->backsector->ceilingheight;
            bbottom = seg->backsector->floorheight;

            if((frontsector->ceilingpic == skyflatnum) && (seg->backsector->ceilingpic == skyflatnum))
                btop = top;
        
            if(bottom < bbottom)
                bottom = bbottom;
        
            if(top > btop)
                top = btop;

            if(linedef->flags & ML_DONTPEGMID)
            {
                v1 = row - (top - btop);
                v2 = row + (((top + btop) - (bottom + bbottom)) >> 1);
            }
        }
        else
        {
            if(linedef->flags & ML_DONTPEGTOP)
            {
                v1 = ((row - bottom) - (top - bottom));
                v2 = ((row - bottom) - (top - bottom)) + (top - bottom);
            }
            else if(linedef->flags & ML_DONTPEGBOTTOM)
            {
                v1 = row - (top - bottom);
                v2 = row;
            }

            if(SWITCHMASK(linedef->flags))
            {
                if(SWITCHMASK(linedef->flags) == ML_SWITCHX02)
                {
                    offset = seg->frontsector->floorheight + (16*FRACUNIT + (seg->sidedef->rowoffset));
                    R_DrawSwitch(seg, seg->sidedef->toptexture, offset + (32*FRACUNIT), offset);
                }
                else if(SWITCHMASK(linedef->flags) == ML_SWITCHX04)
                {
                    offset = seg->frontsector->floorheight + (16*FRACUNIT + (seg->sidedef->rowoffset));
                    R_DrawSwitch(seg, seg->sidedef->bottomtexture, offset + (32*FRACUNIT), offset);
                }
            }
        }
        
        if(!(linedef->flags & ML_SWITCHX02 && linedef->flags & ML_SWITCHX04))
        {
            R_DrawLine(
            seg,
            top,
            bottom,
            sidedef->midtexture,
            l1,
            l2,
            col + sidedef->textureoffset,
            sidedef->textureoffset,
            v1,
            v2
            );
        }
    }
}

//
// R_DrawSubsector
//

static void R_DrawSubsector(subsector_t* ss, fixed_t height,
                            dtexture texture, light_t* light,
                            int xoffset, int yoffset)
{
    int i;
    int x;
    int y;
    int z;
    int tx;
    int ty;
    fixed_t tsx;
    fixed_t tsy;
    int mapx;
    int mapy;
    int length;
    int r;
    int g;
    int b;

    I_CheckGFX();

    r = light->active_r;
    g = light->active_g;
    b = light->active_b;

    //
    // sequenced lighting is too expensive on the DS
    // instead of creating a glowing plane, just amplify the
    // RGB values
    //
    if(frontsector->lightlevel)
    {
        r = MIN(r + frontsector->lightlevel, 255);
        g = MIN(g + frontsector->lightlevel, 255);
        b = MIN(b + frontsector->lightlevel, 255);
    }

    if(nolights)
        GFX_COLOR = 0x1F7FFF;
    else
        GFX_COLOR = RGB8(r, g, b);

    tx      = F2INT(leafs[ss->leaf].vertex->x) & 0x3F;
    ty      = F2INT(leafs[ss->leaf].vertex->y) & 0x3F;
    tsx     = leafs[ss->leaf].vertex->x;
    tsy     = leafs[ss->leaf].vertex->y;
    mapx    = 0;
    mapy    = 0;
    z       = F2DSFIXED(height);

    if(texture >= 0)
        R_LoadTexture(texture, false, false, true ^ (ss->sector->flags & MS_LIQUIDFLOOR));
    else
    {
        GFX_TEX_FORMAT = 0;
        GFX_PAL_FORMAT = 0;
    }

#define SS_UV_PACK(u, v)                            \
    (((((u) << 4) - (xoffset & 0x3FF)) & 0xFFFF) |  \
    ( (((v) << 4) + (yoffset & 0x3FF)) << 16))

#define DRAWSSECT(index)                    \
    v = leafs[index].vertex;                \
    length = F2INT(tsx - v->x) + mapx;      \
    tu = length - tx;                       \
    mapx = length;                          \
    tsx = v->x;                             \
    length = F2INT(tsy - v->y) + mapy;      \
    tv = length - ty;                       \
    mapy = length;                          \
    tsy = v->y;                             \
    x = F2DSFIXED(v->x);                    \
    y = F2DSFIXED(v->y);                    \
    GFX_TEX_COORD   = SS_UV_PACK(tu, tv);   \
    GFX_VERTEX16    = VERTEX_PACK(x, z);    \
    GFX_VERTEX16    = VERTEX_PACK(y, 0)

    GFX_BEGIN = GL_TRIANGLES;

    for(i = 0; i < ss->numleafs - 2; i++)
    {
        fixed_t tu;
        fixed_t tv;
        vertex_t* v;

        DRAWSSECT(ss->leaf + 1 + i);
        DRAWSSECT(ss->leaf + 2 + i);
        DRAWSSECT(ss->leaf + 0);
    }

#undef DRAWSSECT
#undef SS_UV_PACK
}

//
// R_DrawLeafs
//

static void R_DrawLeafs(subsector_t* subsector)
{
    int i;
    light_t* l;
    fixed_t x, y;

    for(i = 0; i < subsector->numleafs; i++)
    {
        seg_t* seg = leafs[subsector->leaf + i].seg;

        if(seg->sidedef)
        {
            if(seg->sidedef->draw)
            {
                R_DrawSeg(seg);
                seg->sidedef->draw = false;
            }
        }
    }

    if(!skyvisible && (frontsector->ceilingpic == skyflatnum || frontsector->floorpic == skyflatnum))
        skyvisible = true;

    if(viewz <= frontsector->ceilingheight && frontsector->ceilingpic != skyflatnum)
    {
        if(frontsector->flags & MS_SCROLLCEILING)
        {
            x = frontsector->xoffset >> 12;
            y = frontsector->yoffset >> 12;
        }
        else
            x = y = 0;

        l = &lights[frontsector->colors[LIGHT_CEILING]];

        GFX_POLY_FORMAT =
            POLY_ALPHA(31)  |
            POLY_ID(0)      |
            POLY_CULL_BACK  |
            POLY_MODULATION |
            POLY_FOG;

        R_DrawSubsector(subsector,
            frontsector->ceilingheight,
            frontsector->ceilingpic, l, x, y);
    }

    if(viewz >= frontsector->floorheight && frontsector->floorpic != skyflatnum)
    {
        if(frontsector->flags & MS_SCROLLFLOOR)
        {
            x = frontsector->xoffset >> 12;
            y = frontsector->yoffset >> 12;
        }
        else
            x = y = 0;

        l = &lights[frontsector->colors[LIGHT_FLOOR]];

        if(frontsector->flags & MS_LIQUIDFLOOR)
        {
            GFX_POLY_FORMAT =
                POLY_ALPHA(20)  |
                POLY_ID(0x3F)   |
                POLY_CULL_FRONT |
                POLY_MODULATION |
                POLY_FOG        |
                POLY_DEPTHTEST_EQUAL;

            R_DrawSubsector(subsector,
            frontsector->floorheight,
            frontsector->floorpic, l, x + scrollfrac, y);
            
            GFX_POLY_FORMAT =
                POLY_ALPHA(31)  |
                POLY_ID(3)      |
                POLY_CULL_FRONT |
                POLY_MODULATION |
                POLY_FOG;

            R_DrawSubsector(subsector,
                frontsector->floorheight,
                frontsector->floorpic + 1, l, x, y - scrollfrac);
        }
        else
        {
            GFX_POLY_FORMAT =
                POLY_ALPHA(31)  |
                POLY_ID(0)      |
                POLY_CULL_FRONT |
                POLY_MODULATION |
                POLY_FOG;

            R_DrawSubsector(subsector,
                frontsector->floorheight,
                frontsector->floorpic, l, x, y);
        }
    }
}

//
// R_DrawSprite
//

static void R_DrawSprite(mobj_t* thing)
{
    spriteframe_t*  sprframe;
    angle_t ang;
    int rot;
    int flipoffs;
    int tu1;
    int tu2;
    int tx1;
    int tx2;
    short alpha;
    rcolor color;
    int offx;
    int offy;
    int width;
    int height;
    fixed_t x1, x2;
    fixed_t y1, y2;
    fixed_t z1, z2;

    alpha = thing->alpha >> 3;

    if(alpha <= 0)
        return;

    I_CheckGFX();

    sprframe = &spriteinfo[thing->sprite].spriteframes[thing->frame & FF_FRAMEMASK];
    
    if(sprframe->rotate)
    {
        // choose a different rotation based on player view
        ang = R_PointToAngle2(viewx, viewy, thing->x, thing->y);
        rot = (ang-thing->angle + (unsigned)(ANG45 / 2) * 9) >> 29;
    }
    else
        // use single rotation for all views
        rot = 0;

    if(!R_LoadSprite(thing->sprite, thing->frame & FF_FRAMEMASK, rot, thing->info->palette,
        &offx, &offy, &width, &height))
        return;

    if(thing->frame & FF_FULLBRIGHT || nolights)
        color = RGB15(31, 31, 31);
    else
    {
        light_t *light;
        int r;
        int g;
        int b;

        light = &lights[thing->subsector->sector->colors[LIGHT_THING]];

        r = light->active_r;
        g = light->active_g;
        b = light->active_b;

        if(thing->subsector->sector->lightlevel)
        {
            r = MIN(r + frontsector->lightlevel, 255);
            g = MIN(g + frontsector->lightlevel, 255);
            b = MIN(b + frontsector->lightlevel, 255);
        }

        color = RGB8(r, g, b);
    }

    if(sprframe->flip[rot])
    {
        flipoffs = width - offx;
        tu1 = 0;
        tu2 = width;
        tx1 = width - flipoffs;
        tx2 = flipoffs;
    }
    else
    {
        flipoffs = width - offx;
        tu1 = width;
        tu2 = 0;
        tx1 = flipoffs;
        tx2 = width - flipoffs;
    }

    x1 = INT2DSFIXED(F2INT(thing->x) - FixedMul(viewcos[0], tx1));
    y1 = INT2DSFIXED(F2INT(thing->y) - FixedMul(viewsin[0], tx1));
    x2 = INT2DSFIXED(F2INT(thing->x) + FixedMul(viewcos[0], tx2));
    y2 = INT2DSFIXED(F2INT(thing->y) + FixedMul(viewsin[0], tx2));
    z1 = INT2DSFIXED(F2INT(thing->z) + offy - height);
    z2 = INT2DSFIXED(F2INT(thing->z) + offy);

    GFX_POLY_FORMAT =
        POLY_ALPHA(alpha)   |
        POLY_ID(spr_polyid) |
        POLY_CULL_BACK      |
        POLY_MODULATION     |
        POLY_FOG            |
        POLY_NEW_DEPTH;

    GFX_COLOR       = color;
    GFX_BEGIN       = GL_QUADS;
    GFX_TEX_COORD   = COORD_PACK(tu1, 0);
    GFX_VERTEX16    = VERTEX_PACK(x1, z2);
    GFX_VERTEX16    = VERTEX_PACK(y1, 0);
    GFX_TEX_COORD   = COORD_PACK(tu2, 0);
    GFX_VERTEX16    = VERTEX_PACK(x2, z2);
    GFX_VERTEX16    = VERTEX_PACK(y2, 0);
    GFX_TEX_COORD   = COORD_PACK(tu2, height);
    GFX_VERTEX16    = VERTEX_PACK(x2, z1);
    GFX_VERTEX16    = VERTEX_PACK(y2, 0);
    GFX_TEX_COORD   = COORD_PACK(tu1, height);
    GFX_VERTEX16    = VERTEX_PACK(x1, z1);
    GFX_VERTEX16    = VERTEX_PACK(y1, 0);

    if(thing->subsector != spr_prevssect)
    {
        spr_prevssect = thing->subsector;
        spr_polyid++;

        if(spr_polyid > 0x3F)
            spr_polyid = 0x3F;
    }
}

//
// R_DrawLaser
//

static void R_DrawLaser(mobj_t* thing)
{
    laser_t* laser;
    int spritenum;
    fixed_t c;
    fixed_t s;
    fixed_t dx1;
    fixed_t dx2;

    // must have data present
    if(!thing->extradata)
        return;

    laser = (laser_t*)thing->extradata;
    spritenum = spriteinfo[SPR_BOLT].spriteframes[0].lump[0];

    if(!R_LoadSprite(SPR_BOLT, thing->frame & FF_FRAMEMASK, 0, 0, NULL, NULL, NULL, NULL))
        return;

    I_CheckGFX();

    // get angles
    s = dsin(laser->angle + ANG90);
    c = dcos(laser->angle + ANG90);

    dx1 = -INT2F(spritetopoffset[spritenum]);
    dx2 = dx1 + INT2F(spriteheight[spritenum] << 2);

    GFX_POLY_FORMAT =
        POLY_ALPHA((thing->alpha >> 3)) |
        POLY_ID(0)                      |
        POLY_CULL_NONE                  |
        POLY_MODULATION;

#define LASER_VERTEX(x, a, d) F2DSFIXED(laser->x + FixedMul(a, d))

    GFX_COLOR       = RGB15(31, 0, 0);
    GFX_BEGIN       = GL_TRIANGLE_STRIP;
    GFX_TEX_COORD   = COORD_PACK(0, 0);
    GFX_VERTEX16    = VERTEX_PACK(LASER_VERTEX(x1, c, dx1), F2DSFIXED(laser->z1));
    GFX_VERTEX16    = VERTEX_PACK(LASER_VERTEX(y1, s, dx1), 0);
    GFX_TEX_COORD   = COORD_PACK(spritewidth[spritenum], 0);
    GFX_VERTEX16    = VERTEX_PACK(LASER_VERTEX(x2, c, dx1), F2DSFIXED(laser->z2));
    GFX_VERTEX16    = VERTEX_PACK(LASER_VERTEX(y2, s, dx1), 0);
    GFX_TEX_COORD   = COORD_PACK(0, spriteheight[spritenum]);
    GFX_VERTEX16    = VERTEX_PACK(LASER_VERTEX(x1, c, dx2), F2DSFIXED(laser->z1));
    GFX_VERTEX16    = VERTEX_PACK(LASER_VERTEX(y1, s, dx2), 0);
    GFX_TEX_COORD   = COORD_PACK(spritewidth[spritenum], spriteheight[spritenum]);
    GFX_VERTEX16    = VERTEX_PACK(LASER_VERTEX(x2, c, dx2), F2DSFIXED(laser->z2));
    GFX_VERTEX16    = VERTEX_PACK(LASER_VERTEX(y2, s, dx2), 0);

#undef LASER_VERTEX
}

//
// R_DrawScene
//

void R_DrawScene(void)
{
    for(nextssect = nextssect - 1; nextssect >= ssectlist; nextssect--)
    {
        subsector_t* sub;

        sub = *nextssect;
        frontsector = sub->sector;

        R_DrawLeafs(sub);
    }

    spr_polyid = 0;

    for(vissprite = vissprite - 1; vissprite >= visspritelist; vissprite--)
    {
        mobj_t* mobj;

        mobj = *vissprite;

        if(mobj->flags & MF_RENDERLASER)
            R_DrawLaser(mobj);
        else
            R_DrawSprite(mobj);
    }
}

//
// R_DrawPSprite
//

void R_DrawPSprite(pspdef_t *psp, sector_t* sector, player_t *player)
{
    rcolor      color;
    short       alpha;
    fixed_t     x;
    fixed_t     y;
    int         width;
    int         height;
    uint32      polyflags;

    I_CheckGFX();

    alpha = (((player->mo->alpha * psp->alpha) / 0xff) >> 3);

    if(!R_LoadSprite(psp->state->sprite, psp->state->frame & FF_FRAMEMASK,
        0, 0, &x, &y, &width, &height))
        return;

    x = F2INT(psp->sx) - x - 32;
    y = F2INT(psp->sy) - y - 48;

    if(player->onground)
    {
        x += (quakeviewx >> 24);
        y += (quakeviewy >> 16);
    }
    
    if(psp->state->frame & FF_FULLBRIGHT || nolights)
        color = RGB15(31, 31, 31);
    else
    {
        light_t *light;
        int r;
        int g;
        int b;

        light = &lights[sector->colors[LIGHT_THING]];

        r = light->active_r;
        g = light->active_g;
        b = light->active_b;

        if(sector->lightlevel)
        {
            r = MIN(r + sector->lightlevel, 255);
            g = MIN(g + sector->lightlevel, 255);
            b = MIN(b + sector->lightlevel, 255);
        }

        color = RGB8(r, g, b);
    }

    GFX_ORTHO();

    polyflags = POLY_ALPHA(alpha) | POLY_ID(0x3F) | POLY_CULL_NONE | POLY_MODULATION;

    //
    // hack for plasma gun sprite;
    // set the depth test to equal when
    // drawing the animating plasma tube
    //
    if(psp == &players->psprites[ps_flash] && psp->state->sprite == SPR_PLAS)
        polyflags |= POLY_DEPTHTEST_EQUAL;

    GFX_POLY_FORMAT = polyflags;
    GFX_COLOR       = color;
    GFX_BEGIN       = GL_QUADS;
    GFX_TEX_COORD   = COORD_PACK(0, 0);
    GFX_VERTEX16    = VERTEX_PACK(x, y);
    GFX_VERTEX16    = VERTEX_PACK(-4, 0);
    GFX_TEX_COORD   = COORD_PACK(width, 0);
    GFX_VERTEX16    = VERTEX_PACK(width + x, y);
    GFX_VERTEX16    = VERTEX_PACK(-4, 0);
    GFX_TEX_COORD   = COORD_PACK(width, height);
    GFX_VERTEX16    = VERTEX_PACK(width + x, height + y);
    GFX_VERTEX16    = VERTEX_PACK(-4, 0);
    GFX_TEX_COORD   = COORD_PACK(0, height);
    GFX_VERTEX16    = VERTEX_PACK(x, height + y);
    GFX_VERTEX16    = VERTEX_PACK(-4, 0);
}

//
// R_DrawSkyPic
//

static void R_DrawSkyPic(int lump, int yoffset, dboolean backdrop)
{
    short* lumpdata;
    int width;
    int height;
    int pw;
    int ph;
    int z;
    byte* data;
    int offset;
    lumpinfo_t* l;
    int index = 0;

    if(lump == -1)
        return;

    I_CheckGFX();

    l = &lumpinfo[lump];

    if(l->cache == NULL)
        W_CacheLumpNum(lump, PU_CACHE);

    lumpdata = (short*)l->cache;
    width = lumpdata[0];
    height = lumpdata[1];
    data = (byte*)(lumpdata + 4);
    pw = R_PadTextureDims(width);
    ph = R_PadTextureDims(height);
    index = lump - g_start;

    if(!I_AllocVBlock(
        &gfx_images[index],
        data,
        pw * height,
        TEXGEN_OFF | GL_TEXTURE_COLOR0_TRANSPARENT | GL_TEXTURE_WRAP_S | GL_TEXTURE_WRAP_T,
        R_GetTextureSize(pw),
        R_GetTextureSize(ph),
        GL_RGB256))
        return;

    offset = -((viewangle >> 22) & 0xff);

    if(backdrop)
        z = 0xFFC;
    else
        z = 0x1000;

    height = (ph << 5);
    yoffset <<= 5;

    GFX_POLY_FORMAT = POLY_ALPHA(31) | POLY_ID(0) | POLY_CULL_NONE | POLY_MODULATION;
    GFX_TEX_FORMAT  = gfx_images[index].params;
    GFX_PAL_FORMAT  = gfx_imgpal_params[index];
    GFX_COLOR       = RGB15(31, 31, 31);
    GFX_BEGIN       = GL_QUADS;
    GFX_TEX_COORD   = COORD_PACK(offset, 0);
    GFX_VERTEX16    = VERTEX_PACK(-0x1000, height - yoffset);
    GFX_VERTEX16    = VERTEX_PACK(z, 0);
    GFX_TEX_COORD   = COORD_PACK(256 + offset, 0);
    GFX_VERTEX16    = VERTEX_PACK(0x1000, height - yoffset);
    GFX_VERTEX16    = VERTEX_PACK(z, 0);
    GFX_TEX_COORD   = COORD_PACK(256 + offset, ph);
    GFX_VERTEX16    = VERTEX_PACK(0x1000, -yoffset);
    GFX_VERTEX16    = VERTEX_PACK(z, 0);
    GFX_TEX_COORD   = COORD_PACK(offset, ph);
    GFX_VERTEX16    = VERTEX_PACK(-0x1000, -yoffset);
    GFX_VERTEX16    = VERTEX_PACK(z, 0);
}

//
// R_CloudThunder
//

static void R_CloudThunder(void)
{
    if(!(frametic & ((thunderCounter & 1) ? 1 : 3)))
        return;
    
    if((thunderCounter - thundertic) > 0)
    {
        thunderCounter = (thunderCounter - thundertic);
        return;
    }
    
    if(lightningCounter == 0)
    {
        S_StartSound(NULL, sfx_thndrlow + (M_Random() & 1));
        thundertic = (1 + (M_Random() & 1));
    }
    
    if(!(lightningCounter < 6))	// Reset loop after 6 lightning flickers
    {
        int rand = (M_Random() & 7);
        thunderCounter = (((rand << 4) - rand) << 2) + 60;
        lightningCounter = 0;
        return;
    }
    
    if((lightningCounter & 1) == 0)
    {
        sky->skycolor[0] += RGB8(0x11, 0x11, 0);
        sky->skycolor[2] += RGB8(0x11, 0x11, 0);
    }
    else
    {
        sky->skycolor[0] -= RGB8(0x11, 0x11, 0);
        sky->skycolor[2] -= RGB8(0x11, 0x11, 0);
    }
    
    thunderCounter = (M_Random() & 7) + 1;	// Do short delay loops for lightning flickers
    lightningCounter++;
}

//
// R_DrawCloud
//

static void R_DrawCloud(int lump)
{
    short* lumpdata;
    lumpinfo_t* l;
    byte* data;
    int offset;
    int index;

    if(lump == -1)
        return;

    I_CheckGFX();

    MATRIX_PUSH = 0;

    gluPerspective(45, 256.0f / 192.0f, 0.002f, 1000);

    l = &lumpinfo[lump];

    if(l->cache == NULL)
        W_CacheLumpNum(lump, PU_CACHE);

    lumpdata = (short*)l->cache;
    data = (byte*)(lumpdata + 4);
    index = lump - g_start;

    if(!I_AllocVBlock(
        &gfx_images[index],
        data,
        4096,
        TEXGEN_OFF | GL_TEXTURE_WRAP_S | GL_TEXTURE_WRAP_T,
        TEXTURE_SIZE_64,
        TEXTURE_SIZE_64,
        GL_RGB256))
        return;

    cloud_offsetx = (cloud_offsetx - (dcos(viewangle) >> 15)) & 0x3FF;
    cloud_offsety = (cloud_offsety + (dsin(viewangle) >> 14)) & 0x3FF;
    offset = cloud_offsetx - ((viewangle >> ANGLETOFINESHIFT) >> 1);

#define CLOUD_UV_PACK(u, v) (((((u) << 4) + offset) & 0xFFFF) | ((((v) << 4) + cloud_offsety) << 16))

    GFX_POLY_FORMAT = POLY_ALPHA(18) | POLY_ID(0x3F) | POLY_CULL_NONE | POLY_MODULATION;
    GFX_TEX_FORMAT  = gfx_images[index].params;
    GFX_PAL_FORMAT  = gfx_imgpal_params[index];
    GFX_BEGIN       = GL_QUADS;
    GFX_COLOR       = sky->skycolor[0];
    GFX_TEX_COORD   = CLOUD_UV_PACK(0, 0);
    GFX_VERTEX16    = VERTEX_PACK(-0x6000, 0x2000);
    GFX_VERTEX16    = VERTEX_PACK(0, 0);
    GFX_TEX_COORD   = CLOUD_UV_PACK(256, 0);
    GFX_VERTEX16    = VERTEX_PACK(0x5000, 0x2000);
    GFX_VERTEX16    = VERTEX_PACK(0, 0);
    GFX_COLOR       = sky->skycolor[2];
    GFX_TEX_COORD   = CLOUD_UV_PACK(256, 256);
    GFX_VERTEX16    = VERTEX_PACK(0x5000, 0);
    GFX_VERTEX16    = VERTEX_PACK(-0x7FFF, 0);
    GFX_TEX_COORD   = CLOUD_UV_PACK(0, 256);
    GFX_VERTEX16    = VERTEX_PACK(-0x6000, 0);
    GFX_VERTEX16    = VERTEX_PACK(-0x7FFF, 0);

#undef CLOUD_UV_PACK

    MATRIX_POP = 1;
}

//
// R_SpreadFire
//

static void R_SpreadFire(byte* src1, byte* src2, int pixel, int counter, int* rand, int width)
{
    int randIdx = 0;
    byte *tmpSrc;
    
    if(pixel != 0)
    {
        randIdx = rndtable[*rand];
        *rand = ((*rand+2) & 0xff);
        
        tmpSrc = (src1 + (((counter - (randIdx & 3)) + 1) & (width-1)));
        *(byte*)(tmpSrc - width) = (pixel - ((randIdx & 1) << 4));
    }
    else
        *(byte*)(src2 - width) = 0;
}

//
// R_DrawFire
//

static void R_DrawFire(void)
{
    lumpinfo_t* l;
    byte* data;
    short* lumpdata;
    int index;
    int offset;

    l = &lumpinfo[skypicnum];

    if(l->cache == NULL)
        W_CacheLumpNum(skypicnum, PU_CACHE);

    lumpdata = (short*)l->cache;
    data = (byte*)(lumpdata + 4);
    index = skypicnum - g_start;

    if(!I_AllocVBlock(
        &gfx_images[index],
        data,
        4096,
        TEXGEN_OFF | GL_TEXTURE_WRAP_S | GL_TEXTURE_WRAP_T,
        TEXTURE_SIZE_64,
        TEXTURE_SIZE_64,
        GL_RGB256))
        return;

    if(frametic & 1)
    {
        int counter = 0;
        int rand = 0;
        int step = 0;
        int pixel = 0;
        const int width = FIRESKYSIZE;
        byte *src;
        byte *srcoffset;

        gfx_images[index].vram->tag = PU_NEWBLOCK;
        
        rand = (M_Random() & 0xff);
        src = (byte*)gfx_images[index].vram->block;
        counter = 0;
        src += width;
        
        do	// height
        {
            srcoffset = (src + counter);
            pixel = *(byte*)srcoffset;
            
            step = 2;
            
            R_SpreadFire(src, srcoffset, pixel, counter, &rand, width);
            
            src += width;
            srcoffset += width;
            
            do	// width
            {
                pixel = *(byte*)srcoffset;
                step += 2;
                
                R_SpreadFire(src, srcoffset, pixel, counter, &rand, width);
                
                pixel = *(byte*)(srcoffset + width);
                src += width;
                srcoffset += width;
                
                R_SpreadFire(src, srcoffset, pixel, counter, &rand, width);
                
                src += width;
                srcoffset += width;
                
            } while(step != width);
            
            counter++;
            src -= ((width*width)-width);
            
        } while(counter != width);
    }

    offset = -((viewangle >> 22) & 0xff);

    GFX_POLY_FORMAT = POLY_ALPHA(31) | POLY_ID(0) | POLY_CULL_NONE | POLY_MODULATION;
    GFX_TEX_FORMAT  = gfx_images[index].params;
    GFX_PAL_FORMAT  = gfx_imgpal_params[index];
    GFX_COLOR       = sky->skycolor[0];
    GFX_BEGIN       = GL_QUADS;
    GFX_TEX_COORD   = COORD_PACK(offset, 0);
    GFX_VERTEX16    = VERTEX_PACK(-0x1000, 0x1000);
    GFX_VERTEX16    = VERTEX_PACK(0x1000, 0);
    GFX_TEX_COORD   = COORD_PACK(256 + offset, 0);
    GFX_VERTEX16    = VERTEX_PACK(0x1000, 0x1000);
    GFX_VERTEX16    = VERTEX_PACK(0x1000, 0);
    GFX_COLOR       = sky->skycolor[1];
    GFX_TEX_COORD   = COORD_PACK(256 + offset, 64);
    GFX_VERTEX16    = VERTEX_PACK(0x1000, 0);
    GFX_VERTEX16    = VERTEX_PACK(0x1000, 0);
    GFX_TEX_COORD   = COORD_PACK(offset, 64);
    GFX_VERTEX16    = VERTEX_PACK(-0x1000, 0);
    GFX_VERTEX16    = VERTEX_PACK(0x1000, 0);
}

//
// R_DrawSky
//

void R_DrawSky(void)
{
    if(sky->flags & SKF_CLOUD)
    {
        R_DrawCloud(skypicnum);

        if(sky->flags & SKF_THUNDER)
            R_CloudThunder();
    }
    else if(sky->flags & SKF_FIRE)
        R_DrawFire();
    else
        R_DrawSkyPic(skypicnum, 0, false);

    if(sky->flags & SKF_BACKGROUND && !(sky->flags & SKF_FADEBACK))
        R_DrawSkyPic(skybackdropnum, 80, true);
}

//
// R_SlamBackground
//

void R_SlamBackground(const char* name, int x, int y, rcolor color)
{
    int width;
    int height;
    int pw;
    int ph;
    int index;
    short* lumpdata;
    byte* data;
    int lump;
    int tu;
    int tv;

    I_CheckGFX();

    lump = W_GetNumForName(name);

    lumpdata = (short*)W_CacheLumpNum(lump, PU_CACHE);
    width = lumpdata[0];
    height = lumpdata[1];
    data = (byte*)(lumpdata + 4);
    pw = R_PadTextureDims(width);
    ph = R_PadTextureDims(height);
    index = lump - g_start;

    if(!I_AllocVBlock(
        &gfx_images[index],
        R_CopyPic(data, 0, 0, height, pw, width, width),
        pw * ph,
        TEXGEN_OFF,
        R_GetTextureSize(pw),
        R_GetTextureSize(ph),
        GL_RGB256))
        return;

    tu = width;
    tv = height;

    if(width > 256)
    {
        int frac = divf32(256, width);

        tu = divf32(width, frac);
        tv = divf32(height, frac);
    }

    GFX_POLY_FORMAT = POLY_ALPHA(color >> 15) | POLY_ID(0) | POLY_CULL_NONE | POLY_MODULATION;
    GFX_TEX_FORMAT  = gfx_images[index].params;
    GFX_PAL_FORMAT  = gfx_imgpal_params[index];
    GFX_COLOR       = (color & 0x7FFF);
    GFX_BEGIN       = GL_QUADS;
    GFX_TEX_COORD   = COORD_PACK(0, 0);
    GFX_VERTEX16    = VERTEX_PACK(x, y);
    GFX_VERTEX16    = VERTEX_PACK(-4, 0);
    GFX_TEX_COORD   = COORD_PACK(tu, 0);
    GFX_VERTEX16    = VERTEX_PACK(width + x, y);
    GFX_VERTEX16    = VERTEX_PACK(-4, 0);
    GFX_TEX_COORD   = COORD_PACK(tu, tv);
    GFX_VERTEX16    = VERTEX_PACK(width + x, height + y);
    GFX_VERTEX16    = VERTEX_PACK(-4, 0);
    GFX_TEX_COORD   = COORD_PACK(0, tv);
    GFX_VERTEX16    = VERTEX_PACK(x, height + y);
    GFX_VERTEX16    = VERTEX_PACK(-4, 0);
}


