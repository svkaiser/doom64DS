#include <math.h>

#include "ds.h"
#include "doomdef.h"
#include "m_fixed.h"
#include "w_wad.h"
#include "t_bsp.h"
#include "info.h"
#include "tables.h"
#include "z_zone.h"
#include "sc_main.h"
#include "d_main.h"
#include "p_local.h"
#include "r_local.h"
#include "p_spec.h"

void P_SpawnMapThing(mapthing_t *mthing);

//
// MAP related Lookup tables.
// Store VERTEXES, LINEDEFS, SIDEDEFS, etc.
//
int                 numvertexes;
vertex_t*           vertexes;
int                 numsegs;
seg_t*              segs;
int                 numsectors;
sector_t*           sectors;
int                 numsubsectors;
subsector_t*        subsectors;
int                 numnodes;
node_t*             nodes;
int                 numleafs;
leaf_t*             leafs;
int                 numlines;
line_t*             lines;
int                 numsides;
side_t*             sides;
light_t*            lights;
int                 numlights;
macroinfo_t         macros;


//
// [kex] mapinfo stuff
//

int                 nummapdef;
mapdef_t*           mapdefs;
int                 numclusterdef;
clusterdef_t*       clusterdefs;

//
// [kex] sky definition stuff
//

static int          numskydef;
static skydef_t*    skydefs;

// BLOCKMAP
// Created from axis aligned bounding box
// of the map, a rectangular array of
// blocks of size ...
// Used to speed up collision detection
// by spatial subdivision in 2D.
//
// Blockmap size.
int                 bmapwidth;
int                 bmapheight;     // size in mapblocks
short*              blockmap;       // int for larger maps
// offsets in blockmap are from here
short*              blockmaplump;
// origin of block map
fixed_t             bmaporgx;
fixed_t             bmaporgy;
// for thing chains
mobj_t**            blocklinks;


// REJECT
// For fast sight rejection.
// Speeds up enemy AI by skipping detailed
//  LineOf Sight calculation.
// Without special effect, this could be
//  used as a PVS lookup as well.
//
byte*               rejectmatrix;


// Maintain single player starting spots.
mapthing_t          playerstarts[MAXPLAYERS];

//
// P_InitTextureHashTable
//

static word* texturehashlist[2];

static void P_InitTextureHashTable(void)
{
    int i;
    int t = t_start;

    texturehashlist[0]  = Z_Alloca(numtextures * sizeof(word));
    texturehashlist[1]  = Z_Alloca(numtextures * sizeof(word));

    for(i = 0; i < numtextures; i++)
    {
        texturehashlist[0][i] = W_HashLumpName(lumpinfo[t + i].name) % 65536;
        texturehashlist[1][i] = i;
    }

}

//
// P_GetTextureHashKey
//

static word P_GetTextureHashKey(int hash)
{
    int i;

    for(i = 0; i < numtextures; i++)
    {
        if(texturehashlist[0][i] == hash)
            return texturehashlist[1][i];
    }

    return 0;
}


//
// P_LoadVertexes
//
void P_LoadVertexes(int lump)
{
    int             i;
    mapvertex_t*    ml;
    vertex_t*       li;
    
    numvertexes = W_MapLumpLength(lump) / sizeof(mapvertex_t);
    
    // Allocate zone memory for buffer.
    vertexes = Z_Malloc(numvertexes * sizeof(vertex_t),PU_LEVEL,0);
    
    ml = (mapvertex_t *)W_GetMapLump(lump);
    li = vertexes;
    
    // Copy and convert vertex coordinates,
    // internal representation as fixed.
    for(i = 0; i < numvertexes; i++, li++, ml++)
    {
        li->x = LONG(ml->x);
        li->y = LONG(ml->y);
        li->dx = 0;
        li->dy = 0;
        li->validcount = -1;
        li->clipspan = ANGLE_MAX;
    }
}

//
// P_LoadSegs
//

void P_LoadSegs(int lump)
{
    int         i;
    mapseg_t*   ml;
    seg_t*      li;
    line_t*     ldef;
    int         linedef;
    int         side;
    float       x;
    float       y;
    
    numsegs = W_MapLumpLength (lump) / sizeof(mapseg_t);
    segs = Z_Malloc (numsegs*sizeof(seg_t),PU_LEVEL,0);
    memset (segs, 0, numsegs*sizeof(seg_t));
    
    ml = (mapseg_t *)W_GetMapLump(lump);
    li = segs;

    for(i = 0; i < numsegs; i++, li++, ml++)
    {
        li->v1 = &vertexes[(word)SHORT(ml->v1)];
        li->v2 = &vertexes[(word)SHORT(ml->v2)];
        li->angle = INT2F(SHORT(ml->angle));
        li->offset = INT2F(SHORT(ml->offset));
        linedef = (word)SHORT(ml->linedef);
        ldef = &lines[linedef];
        li->linedef = ldef;
        side = SHORT(ml->side);
        li->sidedef = &sides[ldef->sidenum[side]];
        li->frontsector = sides[ldef->sidenum[side]].sector;
        if (ldef->flags & ML_TWOSIDED)
            li->backsector = sides[ldef->sidenum[side^1]].sector;
        else
            li->backsector = 0;

        ldef->angle = (li->angle >> ANGLETOFINESHIFT);

        x = F2D3D(li->v1->x - li->v2->x);
        y = F2D3D(li->v1->y - li->v2->y);

        li->length = (fixed_t)(sqrt(x * x + y * y) * FRACUNIT);

        if(li->frontsector == li->linedef->frontsector &&
            li->sidedef != &sides[li->linedef->sidenum[1]])
        {
            li->sidedef->v1 = li->linedef->v1;
            li->sidedef->v2 = li->linedef->v2;
		}
        else
        {
            li->sidedef->v1 = li->linedef->v2;
            li->sidedef->v2 = li->linedef->v1;
        }

        x = F2D3D(li->sidedef->v2->x - li->sidedef->v1->x);
        y = F2D3D(li->sidedef->v2->y - li->sidedef->v1->y);
        li->sidedef->length = (fixed_t)(sqrt(x * x + y * y) * FRACUNIT);
    }

    for(i = 0; i < numsides; i++)
    {
        if(sides[i].v1 == NULL || sides[i].v2 == NULL)
            I_Error("P_LoadSegs: missing vertex pointer for sidedef (%i)", i);
    }
}


//
// P_LoadSubsectors
//

void P_LoadSubsectors(int lump)
{
    int                 i;
    mapsubsector_t*     ms;
    subsector_t*        ss;
    
    numsubsectors = W_MapLumpLength (lump) / sizeof(mapsubsector_t);
    subsectors = Z_Malloc (numsubsectors*sizeof(subsector_t),PU_LEVEL,0);
    
    ms = (mapsubsector_t *)W_GetMapLump(lump);
    memset (subsectors,0, numsubsectors*sizeof(subsector_t));
    ss = subsectors;
    
    for (i=0 ; i<numsubsectors ; i++, ss++, ms++)
    {
        ss->numlines = (word)SHORT(ms->numsegs);
        ss->firstline = (word)SHORT(ms->firstseg);
        ss->leaf = 0;
        ss->numleafs = 0;
    }
}



//
// P_LoadSectors
//

void P_LoadSectors(int lump)
{
    int                 i, j;
    mapsector_t*        ms;
    sector_t*           ss;
    
    numsectors = W_MapLumpLength(lump) / sizeof(mapsector_t);
    sectors = Z_Malloc (numsectors*sizeof(sector_t),PU_LEVEL,0);
    memset (sectors, 0, numsectors*sizeof(sector_t));
    
    ms = (mapsector_t *)W_GetMapLump(lump);
    ss = sectors;
    for(i = 0; i < numsectors; i++, ss++, ms++)
    {
        ss->floorheight = INT2F(SHORT(ms->floorheight));
        ss->ceilingheight = INT2F(SHORT(ms->ceilingheight));
        ss->floorpic = P_GetTextureHashKey(ms->floorpic);
        ss->ceilingpic = P_GetTextureHashKey(ms->ceilingpic);
        ss->special = SHORT(ms->special);
        ss->flags = SHORT(ms->flags);

        for(j = 0; j < 5; j++)
            ss->colors[j] = SHORT(ms->colors[j]);

        ss->tag = SHORT(ms->tag);
        ss->thinglist = NULL;
        ss->frame_z1[0] = ss->floorheight;
        ss->frame_z1[1] = ss->floorheight;
        ss->frame_z2[0] = ss->ceilingheight;
        ss->frame_z2[1] = ss->ceilingheight;

        for(j = 0; j < numskydef; j++)
        {
            if(ss->ceilingpic == (W_GetNumForName(skydefs[j].flat) - t_start))
            {
                skyflatnum = j;
                break;
            }
        }
    }
}

//
// P_LoadLights
//

void P_LoadLights(int lump)
{
    maplights_t* ml;
    light_t* l;
    int i;
    
    numlights = (W_MapLumpLength(lump) / sizeof(maplights_t)) + 256;
    lights = Z_Malloc(numlights * sizeof(light_t), PU_LEVEL, NULL);
    memset(lights, 0, numlights * sizeof(light_t));
    
    ml = (maplights_t*)W_GetMapLump(lump);
    
    l = lights;
    
    for(i = 0; i < numlights; i++, l++)
    {
        l->base_r = l->base_g = l->base_b = 0;
        l->active_r = l->active_g = l->active_b = 0;
        l->tag = 0;
        
        if(i < 256)
            l->r = l->g = l->b = i;
        else
        {
            l->r = ml->r;
            l->g = ml->g;
            l->b = ml->b;
            l->tag = ml->tag;
            
            ml++;
        }
    }

    R_SetLightFactor(200);
}

//
// P_LoadMacros
//

void P_LoadMacros(int lump)
{
    short* data;
    short count;
    int size = 0;
    int i = 0;
    int j = 0;
    
    size = W_MapLumpLength(lump);
    
    // TODO - fixme
    if(size < 8)
    {
        macros.macrocount = 0;
        macros.specialcount = 0;
        macros.def = NULL;
        return;
    }
    
    data = (short*)W_GetMapLump(lump);

    macros.macrocount = SHORT(*data++);
    macros.specialcount = SHORT(*data++);
    macros.def = Z_Calloc(macros.macrocount * sizeof(macrodef_t), PU_LEVEL, NULL);
    
    for(i = 0; i < macros.macrocount; i++)
    {
        macrodata_t* mdata;

        macros.def[i].count = SHORT(*data++);
        count = macros.def[i].count + 1;

        macros.def[i].data = Z_Calloc(sizeof(macrodata_t) * count, PU_LEVEL, NULL);
        mdata = macros.def[i].data;
        
        for(j = 0; j < count; j++)
        {
            mdata[j].id = SHORT(*data++);
            mdata[j].tag = SHORT(*data++);
            mdata[j].special = SHORT(*data++);
        }
    }
    
    P_InitMacroVars();
}


//
// P_LoadNodes
//

void P_LoadNodes(int lump)
{
    int         i;
    int         j;
    int         k;
    mapnode_t*  mn;
    node_t*     no;
    
    numnodes = W_MapLumpLength (lump) / sizeof(mapnode_t);
    nodes = Z_Malloc (numnodes*sizeof(node_t),PU_LEVEL,0);
    
    mn = (mapnode_t *)W_GetMapLump(lump);
    no = nodes;
    
    for (i=0 ; i<numnodes ; i++, no++, mn++)
    {
        no->x = INT2F(SHORT(mn->x));
        no->y = INT2F(SHORT(mn->y));
        no->dx = INT2F(SHORT(mn->dx));
        no->dy = INT2F(SHORT(mn->dy));
        for (j=0 ; j<2 ; j++)
        {
            no->children[j] = SHORT(mn->children[j]);
            for (k=0 ; k<4 ; k++)
                no->bbox[j][k] = INT2F(SHORT(mn->bbox[j][k]));
        }
    }
}

//
// P_LoadLeafs
//

void P_LoadLeafs(int lump)
{
    int         i;
    int         j;
    short       *mlf;
    leaf_t      *lf;
    int         length;
    int         size;
    int         count;
    subsector_t *ss;
    
    length = W_MapLumpLength(lump);
    mlf = W_GetMapLump(lump);
    
    count = 0;
    size = 0;
    
    if(length)
    {
        short   *src = mlf;
        int     next;

        while(((byte*)src - (byte*)mlf) < length)
        {
            count++;
            size += (word)SHORT(*src);
            next = (*src << 2) + 2;
            src += (next >> 1);
        }
    }
    
    if(count != numsubsectors)
        I_Error("P_LoadLeafs: leaf/subsector inconsistancy %d/%d\n", count, numsubsectors);
    
    leafs = Z_Malloc((size * 2) * sizeof(leaf_t), PU_LEVEL, 0);
    numleafs = numsubsectors;
    
    if(count <= 0)	// this is probably not a good thing..
        return;
    
    lf = leafs;
    ss = subsectors;
    count = 0;
    
    for(i = 0; i < numleafs; i++, ss++)
    {
        ss->numleafs = (word)SHORT(*mlf++);
        ss->leaf = (lf - leafs);
        
        if(ss->numleafs)
        {
            int vertex;
            int seg;
            
            for(j = 0; j < ss->numleafs; j++, lf++)
            {
                vertex = (word)SHORT(*mlf++);
                if(vertex > numvertexes)
                    I_Error("P_LoadLeafs: vertex out of range: %i - %i\n", vertex, numvertexes);
                
                lf->vertex = &vertexes[vertex];
                
                seg = SHORT(*mlf++);
                if(seg == -1)
                    lf->seg = NULL;
                else
                {
                    if(seg > numsegs)
                        I_Error("P_LoadLeafs: seg out of range: %i - %i\n", seg, numsegs);
                    
                    lf->seg = &segs[(word)seg];
                }
            }
        }
    }
}

//
// P_LoadThings
//

void P_LoadThings(int lump)
{
    int             i;
    int             j;
    mapthing_t*     mt;
    int             numthings;
    dboolean        p2start = false;
    dboolean        p3start = false;
    dboolean        p4start = false;
    
    memset(playerstarts,0,sizeof(playerstarts));
    
    numthings = W_MapLumpLength (lump) / sizeof(mapthing_t);
    mt = (mapthing_t *)W_GetMapLump(lump);

    for(i = 0, j = 1; i < numthings; i++)
    {
        if(SHORT(mt[i].options) & MTF_SPAWN)
            j++;

        // 20120122 villsa - check if co-op starts exist
        if(SHORT(mt[i].type) == 2)
            p2start = true;

        if(SHORT(mt[i].type) == 3)
            p3start = true;

        if(SHORT(mt[i].type) == 4)
            p4start = true;
    }

    spawnlist = Z_Malloc(sizeof(mapthing_t) * j, PU_LEVEL, 0);

    for(i = 0; i < numthings; i++, mt++)
    {
        mt->x = SHORT(mt->x);
        mt->y = SHORT(mt->y);
        mt->z = SHORT(mt->z);
        mt->angle = SHORT(mt->angle);
        mt->type = SHORT(mt->type);
        mt->options = SHORT(mt->options);
        mt->tid = SHORT(mt->tid);
        
        P_SpawnMapThing(mt);
        
        // [kex] Hack to force-spawn co-op player starts on top of player 1
        // 20120122 villsa - updated to spawn co-op players away from
        // player 1 by radius
        if(netgame && mt->type == 1)
        {
            short x = mt->x;
            short y = mt->y;

            if(!p2start)
            {
                mt->type = 2;
                mt->x = x;
                mt->y = y;
                P_SpawnMapThing(mt);
            }

            if(!p3start)
            {
                mt->type = 3;
                mt->x = x;
                mt->y = y;
                P_SpawnMapThing(mt);
            }

            if(!p4start)
            {
                mt->type = 4;
                mt->x = x;
                mt->y = y;
                P_SpawnMapThing(mt);
            }
        }
    }
}


//
// P_LoadLineDefs
// Also counts secret lines for intermissions.
//

void P_LoadLineDefs(int lump)
{
    int             i;
    maplinedef_t*   mld;
    line_t*         ld;
    vertex_t*       v1;
    vertex_t*       v2;
    
    numlines = W_MapLumpLength (lump) / sizeof(maplinedef_t);
    lines = Z_Malloc (numlines*sizeof(line_t),PU_LEVEL,0);
    memset (lines, 0, numlines*sizeof(line_t));
    
    mld = (maplinedef_t *)W_GetMapLump(lump);
    ld = lines;
    for (i=0 ; i<numlines ; i++, mld++, ld++)
    {
        ld->flags = mld->flags;
        ld->special = mld->special;
        ld->tag = SHORT(mld->tag);
        v1 = ld->v1 = &vertexes[(word)SHORT(mld->v1)];
        v2 = ld->v2 = &vertexes[(word)SHORT(mld->v2)];
        ld->dx = v2->x - v1->x;
        ld->dy = v2->y - v1->y;
        
        if(ld->special & MLU_MACRO)
        {
            if(MACROMASK(ld->special) >= macros.macrocount)
                I_Error("P_LoadLineDefs: linedef %i has unknown macro", i);
        }
        
        if (!ld->dx)
            ld->slopetype = ST_VERTICAL;
        else if (!ld->dy)
            ld->slopetype = ST_HORIZONTAL;
        else
        {
            if (FixedDiv (ld->dy , ld->dx) > 0)
                ld->slopetype = ST_POSITIVE;
            else
                ld->slopetype = ST_NEGATIVE;
        }
        
        if (v1->x < v2->x)
        {
            ld->bbox[BOXLEFT] = v1->x;
            ld->bbox[BOXRIGHT] = v2->x;
        }
        else
        {
            ld->bbox[BOXLEFT] = v2->x;
            ld->bbox[BOXRIGHT] = v1->x;
        }
        
        if (v1->y < v2->y)
        {
            ld->bbox[BOXBOTTOM] = v1->y;
            ld->bbox[BOXTOP] = v2->y;
        }
        else
        {
            ld->bbox[BOXBOTTOM] = v2->y;
            ld->bbox[BOXTOP] = v1->y;
        }
        
        ld->sidenum[0] = (word)SHORT(mld->sidenum[0]);
        ld->sidenum[1] = (word)SHORT(mld->sidenum[1]);
        
        if (ld->sidenum[0] != NO_SIDE_INDEX)
            ld->frontsector = sides[ld->sidenum[0]].sector;
        else
            ld->frontsector = 0;
        
        if (ld->sidenum[1] != NO_SIDE_INDEX)
            ld->backsector = sides[ld->sidenum[1]].sector;
        else
            ld->backsector = 0;
    }
}


//
// P_LoadSideDefs
//

void P_LoadSideDefs(int lump)
{
    int                 i;
    mapsidedef_t*       msd;
    side_t*             sd;
    
    numsides = W_MapLumpLength(lump) / sizeof(mapsidedef_t);
    sides = Z_Malloc (numsides*sizeof(side_t),PU_LEVEL,0);
    memset (sides, 0, numsides*sizeof(side_t));
    
    msd = (mapsidedef_t *)W_GetMapLump(lump);
    sd = sides;
    for (i=0 ; i<numsides ; i++, msd++, sd++)
    {
        sd->textureoffset = INT2F(SHORT(msd->textureoffset));
        sd->rowoffset = INT2F(SHORT(msd->rowoffset));
        sd->toptexture = P_GetTextureHashKey(msd->toptexture);
        sd->bottomtexture = P_GetTextureHashKey(msd->bottomtexture);
        sd->midtexture = P_GetTextureHashKey(msd->midtexture);
        sd->sector = &sectors[SHORT(msd->sector)];
        sd->v1 = NULL;
        sd->v2 = NULL;
        sd->length = 0;
        sd->draw = false;
    }
}

//
// P_LoadReject
//

void P_LoadReject (int lump)
{
    int size = 0;
    
    size = W_MapLumpLength(lump);
    rejectmatrix = (byte*)Z_Malloc(size, PU_LEVEL, 0);
    memset(rejectmatrix, 0, size);
    memcpy(rejectmatrix, (byte*)W_GetMapLump(lump), size);
}

static const char *bmaperrormsg;

//
// P_VerifyBlockMap
//
// haleyjd 03/04/10: do verification on validity of blockmap.
//
static dboolean P_VerifyBlockMap(int count)
{
   dboolean isvalid = true;
   int x, y;
   short *maxoffs = blockmaplump + count;

   bmaperrormsg = NULL;

   for(y = 0; y < bmapheight; ++y)
   {
      for(x = 0; x < bmapwidth; ++x)
      {
         int offset;
         short *list, *tmplist;
         short *blockoffset;

         offset = y * bmapwidth + x;
         blockoffset = blockmaplump + offset + 4;
         
         // check that block offset is in bounds
         if(blockoffset >= maxoffs)
         {
            isvalid = false;
            bmaperrormsg = "offset overflow";
            break;
         }
         
         offset = *blockoffset;         
         list   = blockmaplump + offset;

         // scan forward for a -1 terminator before maxoffs
         for(tmplist = list; ; ++tmplist)
         {
            // we have overflowed the lump?
            if(tmplist >= maxoffs)
            {
               isvalid = false;
               bmaperrormsg = "open blocklist";
               break;
            }
            if(*tmplist == -1) // found -1
               break;
         }
         if(!isvalid) // if the list is not terminated, break now
            break;

         // scan the list for out-of-range linedef indicies in list
         for(tmplist = list; *tmplist != -1; ++tmplist)
         {
            if(*tmplist < 0 || *tmplist >= numlines)
            {
               isvalid = false;
               bmaperrormsg = "index >= numlines";
               break;
            }
         }
         if(!isvalid) // if a list has a bad linedef index, break now
            break;
      }

      // break out early on any error
      if(!isvalid)
         break;
   }

   return isvalid;
}


//
// P_LoadBlockMap
//

void P_LoadBlockMap(int lump)
{
    int         i;
    int         count;
    void*       mapdata;
    size_t      len;
    
    mapdata = W_GetMapLump(lump);
    len = W_MapLumpLength(lump);
    
    //
    // GhostlyDeath <10/3/11> -- Reallocate and copy since
    // W_GetMapLump() doesn't quite work like we want it to on 64-bit
    // it works, just the way it is laid out
    //
    blockmaplump = Z_Malloc(len, PU_LEVEL, NULL);
    memmove(blockmaplump, mapdata, len);
    blockmap = blockmaplump + 4;
    count = len / 2;
    
    for(i = 0; i < count; i++)
        blockmaplump[i] = SHORT(blockmaplump[i]);
    
    bmaporgx = INT2F(blockmaplump[0]);
    bmaporgy = INT2F(blockmaplump[1]);
    bmapwidth = blockmaplump[2];
    bmapheight = blockmaplump[3];

    if(!P_VerifyBlockMap(count))
        I_Error("P_LoadBlockMap: Bad blockmap - %s", bmaperrormsg);
    
    // clear out mobj chains
    count = sizeof(*blocklinks)* bmapwidth*bmapheight;
    blocklinks = Z_Malloc (count,PU_LEVEL, 0);
    memset (blocklinks, 0, count);
}



//
// P_GroupLines
// Builds sector line lists and subsector sector numbers.
// Finds block bounding boxes for sectors.
//

void P_GroupLines (void)
{
    line_t**            linebuffer;
    int                 i;
    int                 j;
    int                 total;
    line_t*             li;
    sector_t*           sector;
    subsector_t*        ss;
    seg_t*              seg;
    fixed_t             bbox[4];
    int                 block;
    
    // look up sector number for each subsector
    ss = subsectors;
    for (i=0 ; i<numsubsectors ; i++, ss++)
    {
        ss->sector=NULL;
        for (j=0;j<ss->numlines;j++)
        {
            seg = &segs[ss->firstline+j];
            if (seg->sidedef)//could be a miniseg...
            {
                ss->sector = seg->sidedef->sector;
                break;
            }
        }
        if (!ss->sector)
            I_Error("P_GroupLines: Blank subsector");
    }
    
    // count number of lines in each sector
    li = lines;
    total = 0;
    for (i=0 ; i<numlines ; i++, li++)
    {
        total++;
        li->frontsector->linecount++;
        
        if (li->backsector && li->backsector != li->frontsector)
        {
            li->backsector->linecount++;
            total++;
        }
    }
    
    // build line tables for each sector
    linebuffer =  Z_Malloc(total * sizeof(*linebuffer), PU_LEVEL, 0);
    sector = sectors;
    for (i=0 ; i<numsectors ; i++, sector++)
    {
        M_ClearBox (bbox);
        sector->lines = linebuffer;
        li = lines;
        for (j=0 ; j<numlines ; j++, li++)
        {
            if (li->frontsector == sector || li->backsector == sector)
            {
                *linebuffer++ = li;
                M_AddToBox (bbox, li->v1->x, li->v1->y);
                M_AddToBox (bbox, li->v2->x, li->v2->y);
            }
        }
        if (linebuffer - sector->lines != sector->linecount)
            I_Error ("P_GroupLines: miscounted");
        
        // set the degenmobj_t to the middle of the bounding box
        sector->soundorg.x = (bbox[BOXRIGHT]+bbox[BOXLEFT])/2;
        sector->soundorg.y = (bbox[BOXTOP]+bbox[BOXBOTTOM])/2;
        
        // adjust bounding box to map blocks
        block = (bbox[BOXTOP]-bmaporgy+MAXRADIUS)>>MAPBLOCKSHIFT;
        block = block >= bmapheight ? bmapheight-1 : block;
        sector->blockbox[BOXTOP]=block;
        
        block = (bbox[BOXBOTTOM]-bmaporgy-MAXRADIUS)>>MAPBLOCKSHIFT;
        block = block < 0 ? 0 : block;
        sector->blockbox[BOXBOTTOM]=block;
        
        block = (bbox[BOXRIGHT]-bmaporgx+MAXRADIUS)>>MAPBLOCKSHIFT;
        block = block >= bmapwidth ? bmapwidth-1 : block;
        sector->blockbox[BOXRIGHT]=block;
        
        block = (bbox[BOXLEFT]-bmaporgx-MAXRADIUS)>>MAPBLOCKSHIFT;
        block = block < 0 ? 0 : block;
        sector->blockbox[BOXLEFT]=block;
    }
    
}

//
// P_LoadingPlaque
//

#define LOADING_W   66
#define LOADING_H   21
#define LOADING_X   96
#define LOADING_Y   92

void P_LoadingPlaque(void)
{
    int i;
    int size;
    int lump;
    uint16* pal;
    byte* data;
    byte* bg = I_GetBackground();

    if(bg == NULL)
        return;

    lump = W_GetNumForName("LOADING");
    data = (byte*)W_CacheLumpNum(lump, PU_STATIC);
    size = LOADING_W * LOADING_H;
    pal = (uint16*)(data + size + ((4 - (size & 3)) & 3));

    for(i = 0; i < LOADING_H; i++)
    {
        byte* src;
        byte* dst;

        src = &data[i * LOADING_W];
        dst = &bg[((i + LOADING_Y) * 256) + LOADING_X];

        swiCopy(src, dst, (LOADING_W >> 1) | COPY_MODE_HWORD);
    }

    swiCopy(pal, BG_PALETTE, (64 >> 2) | COPY_MODE_WORD);

    Z_Free(data);
}

//
// P_ClearLoadingPlaque
//

void P_ClearLoadingPlaque(void)
{
    byte* bg;
    int fill;

    bg = I_GetBackground();

    if(bg == NULL)
        return;

    fill = 0;

    swiCopy(&fill, bg, (256 * 256) | COPY_MODE_FILL);
}


//
// P_SetupSky
//

void P_SetupSky(void)
{
    int skyindex;
    
    thunderCounter      = 180;
    skypicnum           = -1;
    skybackdropnum      = -1;
    skyfadeback         = false;
    sky                 = NULL;
    
    if(skyflatnum == -1)
        return;
    
    skyindex    = skyflatnum;
    sky         = &skydefs[skyindex];
    skyflatnum  = (W_GetNumForName(sky->flat) - t_start);

    if(sky->pic[0])
        skypicnum = W_GetNumForName(sky->pic);
    else if(sky->flags & SKF_FIRE)
        skypicnum = W_GetNumForName("FIRE");

    if(sky->backdrop[0])
        skybackdropnum = W_GetNumForName(sky->backdrop);
}

/*
//
// P_SetupPlanes
//

void P_SetupPlanes(void)
{
    int i;
    sector_t* sector;

    for(i = 0; i < numsectors; i++)
    {
        sector = &sectors[i];

        if(sector->linecount <= 2)
            continue;

        sector->floorplane.a = 0;
        sector->floorplane.b = 0;
        sector->floorplane.c = FRACUNIT;
        sector->floorplane.d = -sector->floorheight;

        sector->ceilingplane.a = 0;
        sector->ceilingplane.b = 0;
        sector->ceilingplane.c = -FRACUNIT;
        sector->ceilingplane.d = sector->ceilingheight;
    }
}
*/

//
// P_SetupLevel
//

void P_SetupLevel(int map, int playermask, skill_t skill)
{
    int i;
    int free;
    
    // [kex] 12/26/11 - don't reset total stats when loading a savegame
    if(gameaction != ga_loadgame)
    {
        totalkills = totalitems = totalsecret = 0;

        for(i = 0; i < MAXPLAYERS; i++)
        {
            players[i].killcount = players[i].secretcount
                = players[i].itemcount = 0;
        }
    }
    
    // Initial height of PointOfView
    // will be set by player think.
    players[consoleplayer].viewz = 1;

    P_InitThinkers();
    
    // [kex] 12/26/11 - don't reset leveltime when loading a savegame
    if(gameaction != ga_loadgame)
        leveltime = 0;

    skyflatnum = -1;
    numspawnlist = 0;

    M_ClearRandom();

    P_InitTextureHashTable();
    
    W_CacheMapLump(map);
    P_LoadMacros(ML_MACROS);
    P_LoadVertexes(ML_VERTEXES);
    P_LoadSectors(ML_SECTORS);
    P_LoadSideDefs(ML_SIDEDEFS);
    P_LoadLineDefs(ML_LINEDEFS);
    P_LoadSubsectors(ML_SSECTORS);
    P_LoadBlockMap(ML_BLOCKMAP);
    P_LoadNodes(ML_NODES);
    P_LoadSegs(ML_SEGS);
    P_LoadLeafs(ML_LEAFS);
    P_LoadReject(ML_REJECT);
    P_LoadLights(ML_LIGHTS);
    P_GroupLines();
    P_LoadThings(ML_THINGS);
    W_FreeMapLump();

    // [d64] player starts are spawned here instead of in P_SpawnMapThing
    for(i = 0; i < MAXPLAYERS; i++)
    {
        if(playeringame[i])
            P_SpawnPlayer(&playerstarts[i]);
    }

    I_SendDataToArm7(FIFO_MSG_PLAYERDATA, (void*)&players[consoleplayer], 0);
    I_SendDataToArm7(FIFO_MSG_LINEDEFDATA, (void*)lines, numlines);
    I_SendDataToArm7(FIFO_MSG_VERTEXDATA, (void*)vertexes, numvertexes);
    I_SendDataToArm7(FIFO_MSG_SSECTDATA, (void*)subsectors, numsubsectors);
    
    memset(taglist, 0, sizeof(int) * MAXQUEUELIST);
    taglistidx = 0;
    
    // set up world state
    // TODO
    P_SpawnSpecials();
    P_SetupSky();
    //P_SetupPlanes();
    
    // preload graphics
    R_PrecacheLevel();

    Z_CheckHeap();
    free = Z_FreeMemory();

    if(free < 4096)
        I_Error("P_SetupLevel: not enough free memory %d", free);

    Z_SetVAllocList(vramzone);
}


//
// P_InitMapInfo
//

static scdatatable_t mapdatatable[] =
{
    {   "CLASSTYPE",            (int32)&((mapdef_t*)0)->type,                 'i' },
    {   "LEVELNUM",             (int32)&((mapdef_t*)0)->mapid,                'i' },
    {   "CLUSTER",              (int32)&((mapdef_t*)0)->cluster,              'i' },
    {   "EXITDELAY",            (int32)&((mapdef_t*)0)->exitdelay,            'i' },
    {   "NOINTERMISSION",       (int32)&((mapdef_t*)0)->nointermission,       'b' },
    {   "CLEARCHEATS",          (int32)&((mapdef_t*)0)->clearchts,            'b' },
    {   "CONTINUEMUSICONEXIT",  (int32)&((mapdef_t*)0)->contmusexit,          'b' },
    {   "FORCEGODMODE",         (int32)&((mapdef_t*)0)->forcegodmode,         'b' },
    {   NULL,                   0,                                          0   }
};

static scdatatable_t clusterdatatable[] =
{
    {   "PIC",                  (int32)&((clusterdef_t*)0)->pic,              'S' },
    {   "NOINTERMISSION",       (int32)&((clusterdef_t*)0)->nointermission,   'b' },
    {   "SCROLLTEXTEND",        (int32)&((clusterdef_t*)0)->scrolltextend,    'b' },
    {   "PIC_X",                (int32)&((clusterdef_t*)0)->pic_x,            'i' },
    {   "PIC_Y",                (int32)&((clusterdef_t*)0)->pic_y,            'i' },
    {   NULL,                   0,                                          0   }
};

static void P_InitMapInfo(void)
{
    mapdef_t mapdef;
    clusterdef_t cluster;

    mapdefs = NULL;
    clusterdefs = NULL;
    nummapdef = 0;
    numclusterdef = 0;

    SC_Open("MAPINFO");

    while(SC_ReadTokens())
    {
        SC_Find(false);

        //
        // find map block
        //
        if(!stricmp(sc_parser.token, "MAP"))
        {
            memset(&mapdef, 0, sizeof(mapdef_t));

            //
            // set default values
            //
            mapdef.mapid = 1;
            mapdef.exitdelay = 15;
            mapdef.music = -1;

            SC_Find(false);  // skip map lump name

            // read level name
            SC_Find(false);
            strncpy(mapdef.mapname, sc_parser.token, strlen(sc_parser.token));

            SC_Compare("{");  // must expect open bracket

            //
            // read into block
            //
            while(SC_ReadTokens())
            {
                SC_Find(false);

                if(sc_parser.token[0] == '}') // exit block if closed bracket is found
                    break;

                if(!SC_SetData((byte*)&mapdef, mapdatatable))
                {
                    dboolean ok = false;

                    //
                    // get music track ID
                    //
                    if(!stricmp(sc_parser.token, "MUSIC"))
                    {
                        char* text;
                        int ds_start;
                        int ds_end;
                        int lump;

                        ok = true;

                        text = SC_GetString();
                        ds_start = W_GetNumForName("DS_START") + 1;
                        ds_end = W_GetNumForName("DS_END") - 1;

                        lump = W_GetNumForName(text);

                        if(lump > ds_end && lump < ds_start)
                        {
                            I_Printf("P_InitMapInfo: Invalid music name: %s\n", text);
                            mapdef.music = -1;
                        }
                        else
                            mapdef.music = (lump - ds_start);
                    }
                    else if(!stricmp(sc_parser.token, "COMPAT_COLLISION"))
                    {
                        if(datoi(SC_GetString()) == 1)
                            mapdef.oldcollision = 1;
                        else if(datoi(SC_GetString()) == 0)
                            mapdef.oldcollision = 2;

                        ok = true;
                    }
                    else if(!stricmp(sc_parser.token, "ALLOWJUMP"))
                    {
                        if(datoi(SC_GetString()) == 1)
                            mapdef.allowjump = 1;
                        else if(datoi(SC_GetString()) == 0)
                            mapdef.allowjump = 2;

                        ok = true;
                    }

                    if(!ok)
                        SC_Error("P_InitMapInfo");
                }
            }

            mapdefs = Z_Realloc(mapdefs,
                sizeof(mapdef_t) * ++nummapdef, PU_STATIC, 0);
            memcpy(&mapdefs[nummapdef - 1], &mapdef, sizeof(mapdef_t));
        }

        //
        // find cluster block
        //
        else if(!stricmp(sc_parser.token, "CLUSTER"))
        {
            memset(&cluster, 0, sizeof(clusterdef_t));

            SC_Find(false);
            cluster.id = datoi(sc_parser.token);

            SC_Compare("{");  // must expect open bracket

            //
            // read into block
            //
            while(SC_ReadTokens())
            {
                SC_Find(false);

                if(sc_parser.token[0] == '}') // exit block if closed bracket is found
                    break;

                if(!SC_SetData((byte*)&cluster, clusterdatatable))
                {
                    char* text;

                    //
                    // get music track ID
                    //
                    if(!stricmp(sc_parser.token, "MUSIC"))
                    {
                        int ds_start;
                        int ds_end;
                        int lump;

                        text = SC_GetString();
                        ds_start = W_GetNumForName("DS_START") + 1;
                        ds_end = W_GetNumForName("DS_END") - 1;

                        lump = W_GetNumForName(text);

                        if(lump > ds_end && lump < ds_start)
                        {
                            I_Printf("P_InitMapInfo: Invalid music name: %s\n", text);
                            cluster.music = -1;
                        }
                        else
                            cluster.music = (lump - ds_start);
                    }
                    //
                    // check for text
                    //
                    else if(!stricmp(sc_parser.token, "ENTERTEXT"))
                    {
                        cluster.enteronly = true;
                        text = SC_GetString();
                        strncpy(cluster.text, text, strlen(text));
                    }
                    else if(!stricmp(sc_parser.token, "EXITTEXT"))
                    {
                        text = SC_GetString();
                        strncpy(cluster.text, text, strlen(text));
                    }
                    else
                        SC_Error("P_InitMapInfo");
                }
            }

            clusterdefs = Z_Realloc(clusterdefs,
                sizeof(clusterdef_t) * ++numclusterdef, PU_STATIC, 0);
            memcpy(&clusterdefs[numclusterdef - 1], &cluster, sizeof(clusterdef_t));
        }
        else
            SC_Error("P_InitMapInfo");
    }

    SC_Close();
}

//
// P_GetMapInfo
//

mapdef_t* P_GetMapInfo(int map)
{
    int i;

    for(i = 0; i < nummapdef; i++)
    {
        if(mapdefs[i].mapid == map)
            return &mapdefs[i];
    }

    return NULL;
}

//
// P_GetCluster
//

clusterdef_t* P_GetCluster(int map)
{
    int i;
    mapdef_t* mapdef;

    mapdef = P_GetMapInfo(map);

    if(!mapdef)
        return NULL;

    for(i = 0; i < numclusterdef; i++)
    {
        if(mapdef->cluster == clusterdefs[i].id)
            return &clusterdefs[i];
    }

    return NULL;
}

//
// P_InitSkyDef
//

static scdatatable_t skydatatable[] =
{
    {   "PIC",          (int32)&((skydef_t*)0)->pic,          'S' },
    {   "BACKPIC",      (int32)&((skydef_t*)0)->backdrop,     'S' },
    {   "FOGFACTOR",    (int32)&((skydef_t*)0)->fognear,      'i' },
    {   "FOGCOLOR",     (int32)&((skydef_t*)0)->fogcolor,     'c' },
    {   "BASECOLOR",    (int32)&((skydef_t*)0)->skycolor[2],  'c' },
    {   "HIGHCOLOR",    (int32)&((skydef_t*)0)->skycolor[0],  'c' },
    {   "LOWCOLOR",     (int32)&((skydef_t*)0)->skycolor[1],  'c' },
    {   NULL,           0,                                  0   }
};

static void P_InitSkyDef(void)
{
    skydef_t sky;

    numskydef = 0;
    skydefs = NULL;

    SC_Open("SKYDEFS");

    while(SC_ReadTokens())
    {
        SC_Find(false);

        //
        // find sky block
        //
        if(!stricmp(sc_parser.token, "SKY"))
        {
            memset(&sky, 0, sizeof(skydef_t));

            SC_Find(false);
            strncpy(sky.flat, sc_parser.token, strlen(sc_parser.token));

            SC_Compare("{");  // must expect open bracket

            //
            // read into block
            //
            while(SC_ReadTokens())
            {
                SC_Find(false);

                if(sc_parser.token[0] == '}') // exit block if closed bracket is found
                    break;

                if(!SC_SetData((byte*)&sky, skydatatable))
                {
                    //
                    // check for sky flags
                    //
                    if(!stricmp(sc_parser.token, "CLOUD"))
                        sky.flags |= SKF_CLOUD;
                    else if(!stricmp(sc_parser.token, "THUNDER"))
                        sky.flags |= SKF_THUNDER;
                    else if(!stricmp(sc_parser.token, "FIRE"))
                        sky.flags |= SKF_FIRE;
                    else if(!stricmp(sc_parser.token, "FADEINBACKGROUND"))
                        sky.flags |= SKF_FADEBACK;
                    else if(!stricmp(sc_parser.token, "VOID"))
                        sky.flags |= SKF_VOID;
                    else
                        SC_Error("P_InitSkyDef");
                }

                if(sky.backdrop[0])
                    sky.flags |= SKF_BACKGROUND;
            }

            skydefs = Z_Realloc(skydefs,
                sizeof(skydef_t) * ++numskydef, PU_STATIC, 0);
            memcpy(&skydefs[numskydef - 1], &sky, sizeof(skydef_t));
        }
        else
            SC_Error("P_InitSkyDef");
    }

    SC_Close();
}

//
// P_Init
//

void P_Init(void)
{
    SC_Init();
    P_InitPicAnims();
    P_InitMapInfo();
    P_InitSkyDef();
}
