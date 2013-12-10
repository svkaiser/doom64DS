#include <math.h>

#include "ds.h"
#include "r_local.h"
#include "d_main.h"
#include "z_zone.h"
#include "p_local.h"

sector_t* frontsector;

subsector_t *ssectlist[MAXSUBSECTORS];
subsector_t **nextssect = NULL;
mobj_t *visspritelist[MAXSPRITES];
mobj_t **vissprite = NULL;

int	checkcoord[12][4] =
{
    {3,0,2,1},
    {3,0,2,0},
    {3,1,2,0},
    {0},
    {2,0,2,1},
    {0,0,0,0},
    {3,1,3,0},
    {0},
    {2,0,3,1},
    {2,1,3,1},
    {2,1,3,0}
};

//
// R_CheckBBox
//

static dboolean NOINLINE R_CheckBBox(fixed_t* bspcoord)
{
    int         boxpos;
    fixed_t     x1;
    fixed_t     y1;
    fixed_t     x2;
    fixed_t     y2;
    fixed_t     left;
    fixed_t     right;
    fixed_t     span_x1;
    fixed_t     span_y1;
    fixed_t     span_x2;
    fixed_t     span_y2;
    fixed_t     cos;
    fixed_t     sin;
    byte        bit;
    byte        *occlude;
    
    cos = dcos(viewangle);
    sin = dsin(viewangle);
    
    // Find the corners of the box
    // that define the edges from current viewpoint.
    boxpos = (viewx <= bspcoord[BOXLEFT] ? 0 : viewx < bspcoord[BOXRIGHT] ? 1 : 2) +
        (viewy >= bspcoord[BOXTOP] ? 0 : viewy > bspcoord[BOXBOTTOM] ? 4 : 8);

    if(boxpos == 5)
        return true;
    
    // rotate box to relative viewpoint
    span_x1 = bspcoord[checkcoord[boxpos][BOXTOP]] - viewx;
    span_y1 = bspcoord[checkcoord[boxpos][BOXBOTTOM]] - viewy;
    span_x2 = bspcoord[checkcoord[boxpos][BOXLEFT]] - viewx;
    span_y2 = bspcoord[checkcoord[boxpos][BOXRIGHT]] - viewy;
    
    x1 = FixedMul(sin, span_x1) - FixedMul(cos, span_y1);
    y1 = FixedMul(sin, span_y1) + FixedMul(cos, span_x1);
    x2 = FixedMul(sin, span_x2) - FixedMul(cos, span_y2);
    y2 = FixedMul(sin, span_y2) + FixedMul(cos, span_x2);
    
    // check if viewing backside
    if(x1 < -y1 && x2 < -y2)
        return false;

    // check if viewing backside
    if(y1 < x1 && y2 < x2)
        return false;
    
    // check for length
    if(((F2INT(x2) * F2INT(y1)) -
        (F2INT(x1) * F2INT(y2))) < 2)
        return true;
    
    if(y1 <= 0 && y2 <= 0)
        return false; // near plane clipped
    
    // drag left side over if needed
    if(x1 < -y1)
    {
        fixed_t div = FixedDiv((x1 + y1), ((x1 + y1) - x2) - y2);
        y1 += FixedMul(div, (y2 - y1));
        x1 = -y1;
    }
    
    // drag right side over if needed
    if(y2 < x2)
    {
        fixed_t div = FixedDiv(x1 - y1, ((x1 - y1) - x2) + y2);
        x2 = y2 = (FixedMul(div, (y2 - y1)) + y1);
    }
    
    // project left side to screen space
    left = F2INT(FixedDiv(x1, y1) * CLIPSPACE) + CLIPSPACE;
    
    // project right side to screen space
    right = F2INT(FixedDiv(x2, y2) * CLIPSPACE) + CLIPSPACE;
    
    // clamp left
    if(left < 0)
        left = 0;
    
    // clamp right
    if(right > SCREENWIDTH)
        right = SCREENWIDTH;
    
    // check if viewing backside
    if(!(left < right))
        return false;
    
    // set occlusion table
    occlude = occludeBuffer + left;
    
    // test against occlusion table
    do
    {
        bit = *occlude;
        occlude++;
        left++;
        
        if(!(bit < 1))
            continue;
        
        return true; // part of it is visible
        
    } while(left < right);
    
    return false;
}

//
// R_AddClipLine
// Clips the given segment
// and adds any visible pieces to the line list.
//

static dboolean NOINLINE R_AddClipLine(seg_t *seg)
{
    vertex_t *v1;
    vertex_t *v2;
    fixed_t x;
    fixed_t y;
    fixed_t x1;
    fixed_t x2;
    fixed_t y1;
    fixed_t y2;
    fixed_t left;
    fixed_t right;
    int i;
    byte bit;
    dboolean occluded;
    byte *occlude;
    fixed_t cos;
    fixed_t sin;
    
    occluded = true;
    v1 = seg->v1;
    v2 = seg->v2;

    cos = dcos(viewangle);
    sin = dsin(viewangle);
    
    // rotate v1 to relative viewpoint
    if(v1->validcount != validcount)
    {
        x = v1->x - viewx;
        y = v1->y - viewy;
        
        v1->dx = FixedMul(sin, x) - FixedMul(cos, y);
        v1->dy = FixedMul(sin, y) + FixedMul(cos, x);
        
        v1->validcount = validcount;
    }
    
    x1 = v1->dx;
    y1 = v1->dy;
    
    // rotate v2 to relative viewpoint
    if(v2->validcount != validcount)
    {
        x = v2->x - viewx;
        y = v2->y - viewy;
        
        v2->dx = FixedMul(sin, x) - FixedMul(cos, y);
        v2->dy = FixedMul(sin, y) + FixedMul(cos, x);
        
        v2->validcount = validcount;
    }
    
    x2 = v2->dx;
    y2 = v2->dy;
    
    // check if viewing backside of seg
    if(x1 < -y1 && x2 < -y2)
        return false;
    
    // check if viewing backside of seg
    if(y1 < x1 && y2 < x2)
        return false;

    // check if clipping the near plane
    if(y1 <= CLIP_NEAR_Z && y2 <= CLIP_NEAR_Z)
        return false;
    
    // check if 0 length
    if(((F2INT(x2) * F2INT(y1)) -
        (F2INT(x1) * F2INT(y2))) <= 0)
        return false;
    
    
    if(y1 < CLIP_NEAR_Z)	// drag left side over if needed
    {
        fixed_t sdiv = FixedDiv(CLIP_NEAR_Z - y1, y2 - y1);
        fixed_t smul = FixedMul(sdiv, x2 - x1);
        x1 += smul;
        y1 = CLIP_NEAR_Z;
    }
    else if(y2 < CLIP_NEAR_Z) // drag right side over if needed
    {
        fixed_t sdiv = FixedDiv(CLIP_NEAR_Z - y2, y1 - y2);
        fixed_t smul = FixedMul(sdiv, x1 - x2);
        x2 += smul;
        y2 = CLIP_NEAR_Z;
    }
    
    // project v1 to screen space
    left = F2INT(FixedDiv(x1, y1) * CLIPSPACE) + CLIPSPACE;
    
    // project v2 to screen space
    right = F2INT(FixedDiv(x2, y2) * CLIPSPACE) + CLIPSPACE;
    
    // clamp left
    if(left < 0)
        left = 0;
    
    // clamp right
    if(right > SCREENWIDTH)
        right = SCREENWIDTH;
    
    if(left == right)
        return false; // everything is off-screen
    
    // set occlusion table
    occlude = occludeBuffer + left;
    
    i = left;
    if(left < right)
    {
        do
        {
            i++;
            bit = *occlude;
            occlude++;
            if(!(bit < 1))
                continue; // touching occluded point

            occluded = false; // part of seg is visible

            if(seg->linedef->validcount == validcount)
                continue;

            seg->linedef->validcount = validcount;
            seg->linedef->flags |= ML_MAPPED; // flag as visible in automap
            break;
            
        } while(i != right);
        
        if(occluded == true)
            return false; // completly behind something
    }
    
    // don't add to occlusion table if line is flagged as masking or non-occluding
    if(seg->linedef->flags & (ML_DRAWMIDTEXTURE|ML_DONTOCCLUDE))
        return true;
    
    // check sector heights
    if(seg->backsector)
    {
        dboolean heightclipped = false;

        if(seg->backsector->floorheight > seg->frontsector->floorheight &&
            seg->backsector->floorheight > viewz)
        {
            fixed_t xpitch;
            fixed_t ypitch;
            int screeny;
            fixed_t eye_y = 0;

            if(y1 > y2)
                eye_y = y1;
            else if(y2 > y1)
                eye_y = y2;

            xpitch = FixedMul(FRACUNIT, eye_y);
            ypitch = FixedMul(FRACUNIT, (seg->backsector->floorheight - viewz));

            screeny = (F2INT(FixedDiv(ypitch, xpitch) * -CLIPSPACE) + CLIPSPACE) - 40;
            heightclipped = !(screeny > 0 && screeny < SCREENHEIGHT);
        }

        if(!heightclipped)
        {
            if(frontsector->floorheight < seg->backsector->ceilingheight &&
                seg->backsector->floorheight < frontsector->ceilingheight &&
                seg->backsector->floorheight != seg->backsector->ceilingheight)
                return true;
        }
    }
    
    // sanity check
    if(!(left < right))
        return false;
    
    // set occlusion table
    occlude = occludeBuffer + left;
    i = left;
    
    // mark seg into occlusion table
    do
    {
        i++;
        *occlude = 1; // anything falling on this point will be occluded
        occlude++;
    } while(i < right);
    
    return true;
}

//
// R_Subsector
//

static void NOINLINE R_Subsector(int num)
{
    subsector_t	*sub;
    mobj_t* thing;
    int i;

    if(num >= numsubsectors)
        I_Error("R_Subsector: ss %i with numss = %i", num, numsubsectors);

    if(nextssect - ssectlist >= MAXSUBSECTORS)
        return;
    
    sub = &subsectors[num];
    frontsector = sub->sector;

    *nextssect = sub;
    nextssect++;

    for(i = 0; i < sub->numlines; i++)
    {
        seg_t* seg = &segs[sub->firstline + i];

        if(R_AddClipLine(seg))
            seg->sidedef->draw = true;
    }

    // Handle all things in sector.
    for(thing = sub->sector->thinglist; thing; thing = thing->snext)
    {
        int x       = F2INT(thing->x - viewx);
        int y       = F2INT(thing->y - viewy);
        int dir1    = F2INT((x * viewsin[0]) - (y * viewcos[0]));
        int dir2    = F2INT((x * viewcos[0]) + (y * viewsin[0]));

        if((-dir2 - 16) > dir1 || (dir2 - 16) > dir1)
            continue;

        // don't add vissprite if it doesn't meet these requirements
        if(thing->subsector != sub || thing->flags & MF_NOSECTOR ||
            (thing->type == MT_PLAYER && thing == players[consoleplayer].cameratarget))
            continue;
        
        if(vissprite - visspritelist >= MAXSPRITES)
            return;

        *vissprite = thing;
        vissprite++;
    }
}

//
// R_RenderBSPNode
//

void R_RenderBSPNode(int bspnum)
{
    node_t  *bsp;
    int     side;
    
    while(!(bspnum & NF_SUBSECTOR))
    {
        bsp = &nodes[bspnum];
        
        // Decide which side the view point is on.
        side = R_PointOnSide(viewx, viewy, bsp);
        
        // check the front space
        if(R_CheckBBox(bsp->bbox[side]))
            R_RenderBSPNode(bsp->children[side]);
        
        // continue down the back space
        if(!R_CheckBBox(bsp->bbox[side^1]))
            return;
        
        bspnum = bsp->children[side^1];
    }
    
    // subsector with contents
    // add all the drawable elements in the subsector
    if(bspnum == -1)
        bspnum = 0;
    
    R_Subsector(bspnum & ~NF_SUBSECTOR);
}

//
// R_RenderBSPNodeNoClip
//
// Same as R_RenderBSPNode, except don't
// do bounding box/seg clipping. Just render
// everything as nodes are being traversed
//

void R_RenderBSPNodeNoClip(int bspnum)
{
    node_t* bsp;
    int side;
    int i;
    subsector_t* sub;
    mobj_t* thing;
    
    while(!(bspnum & NF_SUBSECTOR))
    {
        bsp = &nodes[bspnum];
        
        // Decide which side the view point is on.
        side = R_PointOnSide(viewx, viewy, bsp);
        
        R_RenderBSPNodeNoClip(bsp->children[side]);
        
        bspnum = bsp->children[side^1];
    }

    // subsector with contents
    // add all the drawable elements in the subsector
    if(bspnum == -1)
        bspnum = 0;
    
    if(nextssect - ssectlist >= MAXSUBSECTORS)
        return;

    sub = &subsectors[bspnum & ~NF_SUBSECTOR];
    *nextssect = sub;
    nextssect++;

    for(i = 0; i < sub->numlines; i++)
    {
        seg_t* seg = &segs[sub->firstline + i];
        seg->sidedef->draw = true;
    }

    for(thing = sub->sector->thinglist; thing; thing = thing->snext)
    {
        // don't add vissprite if it doesn't meet these requirements
        if(thing->subsector != sub || thing->flags & MF_NOSECTOR ||
            (thing->type == MT_PLAYER && thing == players[consoleplayer].cameratarget))
            continue;
        
        if(vissprite - visspritelist >= MAXSPRITES)
            return;

        *vissprite = thing;
        vissprite++;
    }
}

