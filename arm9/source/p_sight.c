#include "doomdef.h"
#include "m_fixed.h"
#include "p_local.h"
#include "r_local.h"
#include "d_main.h"

// the original Doom64 game uses BSP traversing for sight checking
// but due to the limiting DS CPU cache, traversal functions are
// extremly expensive. This version uses path traversing methods
// based off of eariler versions of Doom ~1.2 which is much cheaper
// to execute but can still result in high CPU cache overhead

//
// P_CheckSight
//
fixed_t     sightzstart;        // eye z of looker
fixed_t     topslope;
fixed_t     bottomslope;        // slopes to top and bottom of target

divline_t   strace;             // from t1 to t2
fixed_t     t2x;
fixed_t     t2y;

//
// PTR_SightTraverse
//

static dboolean PTR_SightTraverse(intercept_t *in)
{
    line_t	*li;
    fixed_t	slope;

    li = in->d.line;

    //
    // crosses a two sided line
    //

    P_LineOpening (li);

    if(openbottom >= opentop)   // quick test for totally closed doors
        return false;   // stop

    if(li->frontsector->floorheight != li->backsector->floorheight)
    {
        slope = FixedDiv(openbottom - sightzstart , in->frac);
        if(slope > bottomslope)
            bottomslope = slope;
    }

    if(li->frontsector->ceilingheight != li->backsector->ceilingheight)
    {
        slope = FixedDiv(opentop - sightzstart , in->frac);
        if(slope < topslope)
            topslope = slope;
    }

    if(topslope <= bottomslope)
        return false;   // stop

    return true;    // keep going
}

//
// P_SightBlockLinesIterator
//

static dboolean P_SightBlockLinesIterator(int x, int y)
{
    int         offset;
    short       *list;
    line_t      *ld;
    int         s1, s2;
    divline_t   dl;

    offset = y * bmapwidth + x;
    offset = *(blockmap + offset);

    for(list = blockmaplump + offset ; *list != -1 ; list++)
    {
        ld = &lines[*list];

        if(ld->validcount == validcount)
            continue;   // line has already been checked

        ld->validcount = validcount;

        s1 = P_PointOnDivlineSide(ld->v1->x, ld->v1->y, &trace);
        s2 = P_PointOnDivlineSide(ld->v2->x, ld->v2->y, &trace);

        if(s1 == s2)
            continue;   // line isn't crossed

        P_MakeDivline(ld, &dl);

        s1 = P_PointOnDivlineSide(trace.x, trace.y, &dl);
        s2 = P_PointOnDivlineSide(trace.x + trace.dx, trace.y + trace.dy, &dl);

        if(s1 == s2)
            continue;   // line isn't crossed

        // try to early out the check

        if(!ld->backsector)
            return false;   // stop checking

        // store the line for later intersection testing
        intercept_p->d.line = ld;
        intercept_p++;
    }

    return true;		// everything was checked
}

//
// P_SightTraverseIntercepts
//

static dboolean P_SightTraverseIntercepts(void)
{
    int             count;
    fixed_t         dist;
    intercept_t*    scan;
    intercept_t*    in;
    divline_t       dl;

    count = intercept_p - intercepts;

    //
    // calculate intercept distance
    //

    for(scan = intercepts; scan < intercept_p; scan++)
    {
        P_MakeDivline(scan->d.line, &dl);
        scan->frac = P_InterceptVector(&trace, &dl);		
    }

    //
    // go through in order
    //

    in = 0; // shut up compiler warning

    while(count--)
    {
        dist = MAXINT;
        for(scan = intercepts; scan < intercept_p; scan++)
            if (scan->frac < dist)
            {
                dist = scan->frac;
                in = scan;
            }

            if(!PTR_SightTraverse(in))
                return false;   // don't bother going farther

            in->frac = MAXINT;
    }
    
    return true;    // everything was traversed
}

//
// P_SightPathTraverse
//

static dboolean P_SightPathTraverse(fixed_t x1, fixed_t y1, fixed_t x2, fixed_t y2)
{
    fixed_t	xt1;
    fixed_t yt1;
    fixed_t xt2;
    fixed_t yt2;
    fixed_t	xstep;
    fixed_t ystep;
    fixed_t	partial;
    fixed_t	xintercept;
    fixed_t yintercept;
    int     mapx;
    int     mapy;
    int     mapxstep;
    int     mapystep;
    int     count;

    D_IncValidCount();

    intercept_p = intercepts;

    if(((x1 - bmaporgx) & (MAPBLOCKSIZE-1)) == 0)
        x1 += FRACUNIT;				// don't side exactly on a line
    if(((y1-bmaporgy)&(MAPBLOCKSIZE-1)) == 0)
        y1 += FRACUNIT;				// don't side exactly on a line

    trace.x = x1;
    trace.y = y1;
    trace.dx = x2 - x1;
    trace.dy = y2 - y1;

    x1 -= bmaporgx;
    y1 -= bmaporgy;
    xt1 = x1 >> MAPBLOCKSHIFT;
    yt1 = y1 >> MAPBLOCKSHIFT;

    x2 -= bmaporgx;
    y2 -= bmaporgy;
    xt2 = x2 >> MAPBLOCKSHIFT;
    yt2 = y2 >> MAPBLOCKSHIFT;

    // points should never be out of bounds, but check once instead of
    // each block
    if(xt1 < 0 || yt1 < 0 || xt1 >= bmapwidth || yt1 >= bmapheight
        ||  xt2 < 0 || yt2 < 0 || xt2 >= bmapwidth || yt2 >= bmapheight)
        return false;

    if(xt2 > xt1)
    {
        mapxstep = 1;
        partial = FRACUNIT - ((x1 >> MAPBTOFRAC) & (FRACUNIT - 1));
        ystep = FixedDiv(y2 - y1, D_abs(x2-x1));
    }
    else if(xt2 < xt1)
    {
        mapxstep = -1;
        partial = (x1 >> MAPBTOFRAC) & (FRACUNIT - 1);
        ystep = FixedDiv(y2 - y1, D_abs(x2 - x1));
    }
    else
    {
        mapxstep = 0;
        partial = FRACUNIT;
        ystep = 256*FRACUNIT;
    }

    yintercept = (y1 >> MAPBTOFRAC) + FixedMul(partial, ystep);

    if(yt2 > yt1)
    {
        mapystep = 1;
        partial = FRACUNIT - ((y1 >> MAPBTOFRAC) & (FRACUNIT - 1));
        xstep = FixedDiv(x2 - x1, D_abs(y2 - y1));
    }
    else if(yt2 < yt1)
    {
        mapystep = -1;
        partial = (y1 >> MAPBTOFRAC) & (FRACUNIT - 1);
        xstep = FixedDiv(x2 - x1, D_abs(y2 - y1));
    }
    else
    {
        mapystep = 0;
        partial = FRACUNIT;
        xstep = 256*FRACUNIT;
    }

    xintercept = (x1 >> MAPBTOFRAC) + FixedMul(partial, xstep);


    //
    // step through map blocks
    // Count is present to prevent a round off error from skipping the break
    mapx = xt1;
    mapy = yt1;


    for(count = 0; count < 64; count++)
    {
        if(!P_SightBlockLinesIterator(mapx, mapy))
            return false;   // early out

        if(mapx == xt2 && mapy == yt2)
            break;

        if((yintercept >> FRACBITS) == mapy)
        {
            yintercept += ystep;
            mapx += mapxstep;
        }
        else if((xintercept >> FRACBITS) == mapx)
        {
            xintercept += xstep;
            mapy += mapystep;
        }
    }


    //
    // couldn't early out, so go through the sorted list
    //

    return P_SightTraverseIntercepts();
}

//
// P_CheckSight
// Returns true if a straight line between t1 and t2 is unobstructed.
// Uses REJECT.
//

dboolean P_CheckSight(mobj_t *t1, mobj_t *t2)
{
    int s1;
    int s2;
    int pnum;

    //
    // check for trivial rejection
    //
    s1 = (t1->subsector->sector - sectors);
    s2 = (t2->subsector->sector - sectors);
    pnum = s1*numsectors + s2;

    if(rejectmatrix[pnum >> 3] & (1 << (pnum & 7)))
        return false;   // can't possibly be connected

    //
    // check precisely
    //		
    sightzstart = t1->z + t1->height - (t1->height >> 2);
    topslope = (t2->z + t2->height) - sightzstart;
    bottomslope = (t2->z) - sightzstart;

    return P_SightPathTraverse(t1->x, t1->y, t2->x, t2->y);
}

//
// P_ScanSights
// Optimal mobj sight checking that check sights
// in main tick loop rather from multiple
// mobj action routines
//

static int latetics = 0;

void P_ScanSights(void)
{
    // when drawing a scene, depending on the complexity
    // of BSP traversal can lead to serious performance
    // issues when doing sight checking... even when using
    // the old path traverse methods. to lighten the cpu
    // cache overhead, sight checking may need to be skipped
    // a tic or two. unfortuently this is problematic for
    // network games because this check can only be done locally
    // so we'll just have to live with that for wifi games

    if(latetics <= 0 || netgame)
    {
        mobj_t* mobj;

        // get current tic
        latetics = I_GetTimeTicks();

	    for(mobj = mobjhead.next; mobj != &mobjhead; mobj = mobj->next)
	    {
            // must be killable
            if(!(mobj->flags & MF_COUNTKILL))
                continue;

            // must have longer tics
            if(mobj->tics == 1)
                continue;

            mobj->flags &= ~MF_SEETARGET;

            // must have a target
            if(!mobj->target)
                continue;

            if(!P_CheckSight(mobj, mobj->target))
                continue;

            mobj->flags |= MF_SEETARGET;
        }

        // determine how long it took to do sight checking
        latetics = (I_GetTimeTicks() - latetics);
    }
    else
        latetics--; // skip tic until ready to check sights again
}

