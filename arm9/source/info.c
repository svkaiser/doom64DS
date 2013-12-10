// Data.
#include "sounds.h"
#include "m_fixed.h"
#include "info.h"
#include "p_mobj.h"

char *sprnames[NUMSPRITES+1] =	//0x5FA30
{
    "SPOT", "PLAY", "SARG", "FATT", "POSS", "TROO", "HEAD", "BOSS",
	"SKUL", "BSPI", "CYBR", "PAIN", "RECT", "MISL", "PLSS", "BFS1",
	"LASS", "BAL1", "BAL3", "BAL2", "BAL7", "BAL8", "APLS", "MANF",
	"TRCR", "DART", "FIRE", "RBAL", "PUF2", "PUF3", "PUFF", "BLUD", 
	"A027", "TFOG", "BFE2", "ARM1", "ARM2", "BON1", "BON2", "BKEY",
	"RKEY", "YKEY", "YSKU", "RSKU", "BSKU", "ART1", "ART2", "ART3",
	"STIM", "MEDI", "SOUL", "PINV", "PSTR", "PINS", "SUIT", "PMAP",
	"PVIS", "MEGA", "CLIP", "AMMO", "RCKT", "BROK", "CELL", "CELP",
	"SHEL", "SBOX", "BPAK", "BFUG", "CSAW", "MGUN", "LAUN", "PLSM",
	"SHOT", "SGN2", "LSRG", "CAND", "BAR1", "LMP1", "LMP2", "A031",
	"A030", "A032", "A033", "A034", "BFLM", "RFLM", "YFLM", "A006",
	"A021", "A003", "A020", "A014", "A016", "A008", "A007", "A015",
	"A001", "A012", "A010", "A018", "A017", "A026", "A022", "A028",
	"A029", "A035", "A036", "TRE3", "TRE2", "TRE1", "A013", "A019",
	"A004", "A005", "A023", "SAWG", "PUNG", "PISG", "SHT1", "SHT2",
	"CHGG", "ROCK", "PLAS", "BFGG", "LASR", "BOLT", NULL
};

// Doesn't work with g++, needs actionf_p1
void A_WeaponReady();
void A_ChainSawReady();
void A_Lower();
void A_Raise();
void A_Punch();
void A_ReFire();
void A_FirePistol();
void A_FireShotgun();
void A_FireShotgun2();
void A_CheckReload();
void A_LoadShotgun2();
void A_CloseShotgun2();
void A_FireCGun();
void A_GunFlash();
void A_FireMissile();
void A_Saw();
void A_FirePlasma();
void A_PlasmaAnimate();
void A_BFGsound();
void A_FireBFG();
void A_BFGSpray();
void A_BFGFlash();
void A_Explode();
void A_Pain();
void A_PlayerScream();
void A_Fall();
void A_XScream();
void A_Look();
void A_Chase();
void A_FaceTarget();
void A_PosAttack();
void A_Scream();
void A_SPosAttack();
void A_Tracer();
void A_FatRaise();
void A_FatAttack1();
void A_FatAttack2();
void A_FatAttack3();
void A_TroopAttack();
void A_TroopMelee();
void A_SargAttack();
void A_HeadAttack();
void A_BruisAttack();
void A_SkullAttack();
void A_Metal();
void A_SpidRefire();
void A_BabyMetal();
void A_BspiAttack();
void A_Hoof();
void A_CyberAttack();
void A_PainAttack();
void A_PainDie();
void A_SpawnSmoke();
void A_FadeAlpha();
void A_FireLaser();
void A_OnDeathTrigger();
void A_BarrelExplode();
void A_TargetCamera();
void A_BspiFaceTarget();
void A_PlayAttack();
void A_RectChase();
void A_RectMissile();
void A_RectGroundFire();
void A_MoveGroundFire();
void A_RectTracer();
void A_PainDeathEvent();
void A_CyberDeathEvent();
void A_RectDeathEvent();
void A_FadeOut();
void A_FadeIn();
void A_SkullSetAlpha();
void A_MissileSetAlpha();

state_t	states[NUMSTATES] =		//0x4DFF4
{
/*S_000*/		 { SPR_SPOT, 0, -1, {NULL}, S_000 },
/*S_001*/		 { SPR_PLAY, 0, -1, {NULL}, S_000 },
/*S_002*/		 { SPR_PLAY, 0, 4, {NULL}, S_003 },
/*S_003*/		 { SPR_PLAY, 1, 4, {NULL}, S_004 },
/*S_004*/		 { SPR_PLAY, 2, 4, {NULL}, S_005 },
/*S_005*/		 { SPR_PLAY, 3, 4, {NULL}, S_002 },
/*S_006*/		 { SPR_PLAY, 4, 12, {NULL}, S_007 },
/*S_007*/		 { SPR_PLAY, 32773, 6, {NULL}, S_001 },
/*S_008*/		 { SPR_PLAY, 6, 4, {NULL}, S_009 },
/*S_009*/		 { SPR_PLAY, 6, 4, {A_Pain/*ACTION_6148*/}, S_001 },
/*S_010*/		 { SPR_PLAY, 7, 10, {NULL}, S_011 },
/*S_011*/		 { SPR_PLAY, 8, 10, {A_Scream/*ACTION_5952*/}, S_012 },
/*S_012*/		 { SPR_PLAY, 9, 10, {A_Fall/*ACTION_6236*/}, S_013 },
/*S_013*/		 { SPR_PLAY, 10, 10, {NULL}, S_014 },
/*S_014*/		 { SPR_PLAY, 11, 10, {NULL}, S_015 },
/*S_015*/		 { SPR_PLAY, 12, -1, {NULL}, S_000 },
/*S_016*/		 { SPR_PLAY, 13, 5, {NULL}, S_017 },
/*S_017*/		 { SPR_PLAY, 14, 5, {A_XScream/*ACTION_6116*/}, S_018 },
/*S_018*/		 { SPR_PLAY, 15, 5, {A_Fall/*ACTION_6236*/}, S_019 },
/*S_019*/		 { SPR_PLAY, 16, 5, {NULL}, S_020 },
/*S_020*/		 { SPR_PLAY, 17, 5, {NULL}, S_021 },
/*S_021*/		 { SPR_PLAY, 18, 5, {NULL}, S_022 },
/*S_022*/		 { SPR_PLAY, 19, 5, {NULL}, S_023 },
/*S_023*/		 { SPR_PLAY, 20, 5, {NULL}, S_024 },
/*S_024*/		 { SPR_PLAY, 21, -1, {NULL}, S_000 },
/*S_025*/		 { SPR_PLAY, 4, 10, {A_Look/*ACTION_4928*/}, S_025 },
/*S_026*/		 { SPR_PLAY, 0, 4, {A_Chase/*ACTION_5228*/}, S_027 },
/*S_027*/		 { SPR_PLAY, 1, 4, {A_Chase/*ACTION_5228*/}, S_028 },
/*S_028*/		 { SPR_PLAY, 2, 4, {A_Chase/*ACTION_5228*/}, S_029 },
/*S_029*/		 { SPR_PLAY, 3, 4, {A_Chase/*ACTION_5228*/}, S_026 },
/*S_030*/		 { SPR_PLAY, 4, 4, {A_PosAttack/*ACTION_6484*/}, S_031 },
/*S_031*/		 { SPR_PLAY, 32773, 4, {A_PosAttack/*ACTION_6484*/}, S_032 },
/*S_032*/		 { SPR_PLAY, 4, 4, {A_PosAttack/*ACTION_6484*/}, S_033 },
/*S_033*/		 { SPR_PLAY, 32773, 4, {A_PosAttack/*ACTION_6484*/}, S_034 },
/*S_034*/		 { SPR_PLAY, 4, 4, {A_PosAttack/*ACTION_6484*/}, S_035 },
/*S_035*/		 { SPR_PLAY, 32773, 4, {A_PosAttack/*ACTION_6484*/}, S_026 },
/*S_036*/		 { SPR_PLAY, 6, 4, {NULL}, S_037 },
/*S_037*/		 { SPR_PLAY, 6, 4, {A_Pain/*ACTION_6148*/}, S_026 },
/*S_038*/		 { SPR_PLAY, 4, 4, {A_PlayAttack/*ACTION_6940*/}, S_039 },
/*S_039*/		 { SPR_PLAY, 32773, 4, {A_PlayAttack/*ACTION_6940*/}, S_040 },
/*S_040*/		 { SPR_PLAY, 4, 4, {A_PlayAttack/*ACTION_6940*/}, S_041 },
/*S_041*/		 { SPR_PLAY, 32773, 4, {A_PlayAttack/*ACTION_6940*/}, S_042 },
/*S_042*/		 { SPR_PLAY, 4, 4, {A_PlayAttack/*ACTION_6940*/}, S_043 },
/*S_043*/		 { SPR_PLAY, 32773, 4, {A_PlayAttack/*ACTION_6940*/}, S_026 },
/*S_044*/		 { SPR_SARG, 1, 8, {A_Look/*ACTION_4928*/}, S_045 },
/*S_045*/		 { SPR_SARG, 3, 8, {A_Look/*ACTION_4928*/}, S_044 },
/*S_046*/		 { SPR_SARG, 0, 2, {A_Chase/*ACTION_5228*/}, S_047 },
/*S_047*/		 { SPR_SARG, 0, 2, {A_Chase/*ACTION_5228*/}, S_048 },
/*S_048*/		 { SPR_SARG, 1, 2, {A_Chase/*ACTION_5228*/}, S_049 },
/*S_049*/		 { SPR_SARG, 1, 2, {A_Chase/*ACTION_5228*/}, S_050 },
/*S_050*/		 { SPR_SARG, 2, 2, {A_Chase/*ACTION_5228*/}, S_051 },
/*S_051*/		 { SPR_SARG, 2, 2, {A_Chase/*ACTION_5228*/}, S_052 },
/*S_052*/		 { SPR_SARG, 3, 2, {A_Chase/*ACTION_5228*/}, S_053 },
/*S_053*/		 { SPR_SARG, 3, 2, {A_Chase/*ACTION_5228*/}, S_046 },
/*S_054*/		 { SPR_SARG, 4, 8, {A_FaceTarget/*ACTION_5800*/}, S_055 },
/*S_055*/		 { SPR_SARG, 5, 8, {A_FaceTarget/*ACTION_5800*/}, S_056 },
/*S_056*/		 { SPR_SARG, 6, 8, {A_SargAttack/*ACTION_7720*/}, S_046 },
/*S_057*/		 { SPR_SARG, 7, 2, {NULL}, S_058 },
/*S_058*/		 { SPR_SARG, 7, 2, {A_Pain/*ACTION_6148*/}, S_046 },
/*S_059*/		 { SPR_SARG, 8, 8, {NULL}, S_060 },
/*S_060*/		 { SPR_SARG, 9, 8, {A_Scream/*ACTION_5952*/}, S_061 },
/*S_061*/		 { SPR_SARG, 10, 4, {NULL}, S_062 },
/*S_062*/		 { SPR_SARG, 11, 4, {A_Fall/*ACTION_6236*/}, S_063 },
/*S_063*/		 { SPR_SARG, 12, 4, {A_OnDeathTrigger/*ACTION_6292*/}, S_064 },
/*S_064*/		 { SPR_SARG, 13, -1, {NULL}, S_000 },
/*S_065*/		 { SPR_SARG, 0, 1, {A_FadeOut/*ACTION_13216*/}, S_046 },
/*S_066*/		 { SPR_SARG, 4, 1, {A_FadeOut/*ACTION_13216*/}, S_054 },
/*S_067*/		 { SPR_SARG, 7, 1, {A_FadeOut/*ACTION_13216*/}, S_057 },
/*S_068*/		 { SPR_SARG, 8, 1, {A_FadeIn/*ACTION_13352*/}, S_059 },
/*S_069*/		 { SPR_FATT, 0, 15, {A_Look/*ACTION_4928*/}, S_070 },
/*S_070*/		 { SPR_FATT, 1, 15, {A_Look/*ACTION_4928*/}, S_069 },
/*S_071*/		 { SPR_FATT, 0, 4, {A_Chase/*ACTION_5228*/}, S_072 },
/*S_072*/		 { SPR_FATT, 0, 4, {A_Chase/*ACTION_5228*/}, S_073 },
/*S_073*/		 { SPR_FATT, 1, 4, {A_Chase/*ACTION_5228*/}, S_074 },
/*S_074*/		 { SPR_FATT, 1, 4, {A_Chase/*ACTION_5228*/}, S_075 },
/*S_075*/		 { SPR_FATT, 2, 4, {A_Chase/*ACTION_5228*/}, S_076 },
/*S_076*/		 { SPR_FATT, 2, 4, {A_Chase/*ACTION_5228*/}, S_077 },
/*S_077*/		 { SPR_FATT, 3, 4, {A_Chase/*ACTION_5228*/}, S_078 },
/*S_078*/		 { SPR_FATT, 3, 4, {A_Chase/*ACTION_5228*/}, S_079 },
/*S_079*/		 { SPR_FATT, 4, 4, {A_Chase/*ACTION_5228*/}, S_080 },
/*S_080*/		 { SPR_FATT, 4, 4, {A_Chase/*ACTION_5228*/}, S_081 },
/*S_081*/		 { SPR_FATT, 5, 4, {A_Chase/*ACTION_5228*/}, S_082 },
/*S_082*/		 { SPR_FATT, 5, 4, {A_Chase/*ACTION_5228*/}, S_071 },
/*S_083*/		 { SPR_FATT, 7, 20, {A_FatRaise/*ACTION_8948*/}, S_084 },
/*S_084*/		 { SPR_FATT, 32774, 10, {A_FatAttack1/*ACTION_8992*/}, S_085 },
/*S_085*/		 { SPR_FATT, 7, 10, {A_FaceTarget/*ACTION_5800*/}, S_086 },
/*S_086*/		 { SPR_FATT, 32774, 10, {A_FatAttack2/*ACTION_9136*/}, S_087 },
/*S_087*/		 { SPR_FATT, 7, 10, {A_FaceTarget/*ACTION_5800*/}, S_088 },
/*S_088*/		 { SPR_FATT, 32774, 10, {A_FatAttack3/*ACTION_9280*/}, S_089 },
/*S_089*/		 { SPR_FATT, 7, 10, {A_FaceTarget/*ACTION_5800*/}, S_071 },
/*S_090*/		 { SPR_FATT, 8, 3, {NULL}, S_091 },
/*S_091*/		 { SPR_FATT, 8, 3, {A_Pain/*ACTION_6148*/}, S_071 },
/*S_092*/		 { SPR_FATT, 9, 6, {NULL}, S_093 },
/*S_093*/		 { SPR_FATT, 10, 6, {A_Scream/*ACTION_5952*/}, S_094 },
/*S_094*/		 { SPR_FATT, 11, 6, {A_Fall/*ACTION_6236*/}, S_095 },
/*S_095*/		 { SPR_FATT, 12, 6, {NULL}, S_096 },
/*S_096*/		 { SPR_FATT, 13, 6, {A_OnDeathTrigger/*ACTION_6292*/}, S_097 },
/*S_097*/		 { SPR_FATT, 14, -1, {NULL}, S_000 },
/*S_098*/		 { SPR_POSS, 0, 10, {A_Look/*ACTION_4928*/}, S_099 },
/*S_099*/		 { SPR_POSS, 1, 10, {A_Look/*ACTION_4928*/}, S_098 },
/*S_100*/		 { SPR_POSS, 0, 4, {A_Chase/*ACTION_5228*/}, S_101 },
/*S_101*/		 { SPR_POSS, 0, 4, {A_Chase/*ACTION_5228*/}, S_102 },
/*S_102*/		 { SPR_POSS, 1, 4, {A_Chase/*ACTION_5228*/}, S_103 },
/*S_103*/		 { SPR_POSS, 1, 4, {A_Chase/*ACTION_5228*/}, S_104 },
/*S_104*/		 { SPR_POSS, 2, 4, {A_Chase/*ACTION_5228*/}, S_105 },
/*S_105*/		 { SPR_POSS, 2, 4, {A_Chase/*ACTION_5228*/}, S_106 },
/*S_106*/		 { SPR_POSS, 3, 4, {A_Chase/*ACTION_5228*/}, S_107 },
/*S_107*/		 { SPR_POSS, 3, 4, {A_Chase/*ACTION_5228*/}, S_100 },
/*S_108*/		 { SPR_POSS, 4, 10, {A_FaceTarget/*ACTION_5800*/}, S_109 },
/*S_109*/		 { SPR_POSS, 5, 8, {A_PosAttack/*ACTION_6484*/}, S_110 },
/*S_110*/		 { SPR_POSS, 4, 8, {NULL}, S_100 },
/*S_111*/		 { SPR_POSS, 6, 3, {NULL}, S_112 },
/*S_112*/		 { SPR_POSS, 6, 3, {A_Pain/*ACTION_6148*/}, S_100 },
/*S_113*/		 { SPR_POSS, 7, 5, {NULL}, S_114 },
/*S_114*/		 { SPR_POSS, 8, 5, {A_Scream/*ACTION_5952*/}, S_115 },
/*S_115*/		 { SPR_POSS, 9, 5, {A_Fall/*ACTION_6236*/}, S_116 },
/*S_116*/		 { SPR_POSS, 10, 5, {A_OnDeathTrigger/*ACTION_6292*/}, S_117 },
/*S_117*/		 { SPR_POSS, 11, -1, {NULL}, S_000 },
/*S_118*/		 { SPR_POSS, 12, 5, {NULL}, S_119 },
/*S_119*/		 { SPR_POSS, 13, 5, {A_XScream/*ACTION_6116*/}, S_120 },
/*S_120*/		 { SPR_POSS, 14, 5, {A_Fall/*ACTION_6236*/}, S_121 },
/*S_121*/		 { SPR_POSS, 15, 5, {NULL}, S_122 },
/*S_122*/		 { SPR_POSS, 16, 5, {NULL}, S_123 },
/*S_123*/		 { SPR_POSS, 17, 5, {NULL}, S_124 },
/*S_124*/		 { SPR_POSS, 18, 5, {NULL}, S_125 },
/*S_125*/		 { SPR_POSS, 19, 5, {A_OnDeathTrigger/*ACTION_6292*/}, S_126 },
/*S_126*/		 { SPR_POSS, 20, -1, {NULL}, S_000 },
/*S_127*/		 { SPR_POSS, 0, 10, {A_Look/*ACTION_4928*/}, S_128 },
/*S_128*/		 { SPR_POSS, 1, 10, {A_Look/*ACTION_4928*/}, S_127 },
/*S_129*/		 { SPR_POSS, 0, 3, {A_Chase/*ACTION_5228*/}, S_130 },
/*S_130*/		 { SPR_POSS, 0, 3, {A_Chase/*ACTION_5228*/}, S_131 },
/*S_131*/		 { SPR_POSS, 1, 3, {A_Chase/*ACTION_5228*/}, S_132 },
/*S_132*/		 { SPR_POSS, 1, 3, {A_Chase/*ACTION_5228*/}, S_133 },
/*S_133*/		 { SPR_POSS, 2, 3, {A_Chase/*ACTION_5228*/}, S_134 },
/*S_134*/		 { SPR_POSS, 2, 3, {A_Chase/*ACTION_5228*/}, S_135 },
/*S_135*/		 { SPR_POSS, 3, 3, {A_Chase/*ACTION_5228*/}, S_136 },
/*S_136*/		 { SPR_POSS, 3, 3, {A_Chase/*ACTION_5228*/}, S_129 },
/*S_137*/		 { SPR_POSS, 4, 10, {A_FaceTarget/*ACTION_5800*/}, S_138 },
/*S_138*/		 { SPR_POSS, 5, 10, {A_SPosAttack/*ACTION_6652*/}, S_139 },
/*S_139*/		 { SPR_POSS, 4, 10, {NULL}, S_129 },
/*S_140*/		 { SPR_POSS, 6, 3, {NULL}, S_141 },
/*S_141*/		 { SPR_POSS, 6, 3, {A_Pain/*ACTION_6148*/}, S_129 },
/*S_142*/		 { SPR_POSS, 7, 5, {NULL}, S_143 },
/*S_143*/		 { SPR_POSS, 8, 5, {A_Scream/*ACTION_5952*/}, S_144 },
/*S_144*/		 { SPR_POSS, 9, 5, {A_Fall/*ACTION_6236*/}, S_145 },
/*S_145*/		 { SPR_POSS, 10, 5, {A_OnDeathTrigger/*ACTION_6292*/}, S_146 },
/*S_146*/		 { SPR_POSS, 11, -1, {NULL}, S_000 },
/*S_147*/		 { SPR_POSS, 12, 5, {NULL}, S_148 },
/*S_148*/		 { SPR_POSS, 13, 5, {A_XScream/*ACTION_6116*/}, S_149 },
/*S_149*/		 { SPR_POSS, 14, 5, {A_Fall/*ACTION_6236*/}, S_150 },
/*S_150*/		 { SPR_POSS, 15, 5, {NULL}, S_151 },
/*S_151*/		 { SPR_POSS, 16, 5, {NULL}, S_152 },
/*S_152*/		 { SPR_POSS, 17, 5, {NULL}, S_153 },
/*S_153*/		 { SPR_POSS, 18, 5, {NULL}, S_154 },
/*S_154*/		 { SPR_POSS, 19, 5, {A_OnDeathTrigger/*ACTION_6292*/}, S_155 },
/*S_155*/		 { SPR_POSS, 20, -1, {NULL}, S_000 },
/*S_156*/		 { SPR_TROO, 0, 10, {A_Look/*ACTION_4928*/}, S_157 },
/*S_157*/		 { SPR_TROO, 1, 10, {A_Look/*ACTION_4928*/}, S_156 },
/*S_158*/		 { SPR_TROO, 0, 3, {A_Chase/*ACTION_5228*/}, S_159 },
/*S_159*/		 { SPR_TROO, 0, 3, {A_Chase/*ACTION_5228*/}, S_160 },
/*S_160*/		 { SPR_TROO, 1, 3, {A_Chase/*ACTION_5228*/}, S_161 },
/*S_161*/		 { SPR_TROO, 1, 3, {A_Chase/*ACTION_5228*/}, S_162 },
/*S_162*/		 { SPR_TROO, 2, 3, {A_Chase/*ACTION_5228*/}, S_163 },
/*S_163*/		 { SPR_TROO, 2, 3, {A_Chase/*ACTION_5228*/}, S_164 },
/*S_164*/		 { SPR_TROO, 3, 3, {A_Chase/*ACTION_5228*/}, S_165 },
/*S_165*/		 { SPR_TROO, 3, 3, {A_Chase/*ACTION_5228*/}, S_158 },
/*S_166*/		 { SPR_TROO, 4, 8, {A_FaceTarget/*ACTION_5800*/}, S_167 },
/*S_167*/		 { SPR_TROO, 5, 8, {A_FaceTarget/*ACTION_5800*/}, S_168 },
/*S_168*/		 { SPR_TROO, 6, 6, {A_TroopMelee/*ACTION_7544*/}, S_158 },
/*S_169*/		 { SPR_TROO, 8, 8, {A_FaceTarget/*ACTION_5800*/}, S_170 },
/*S_170*/		 { SPR_TROO, 9, 8, {A_FaceTarget/*ACTION_5800*/}, S_171 },
/*S_171*/		 { SPR_TROO, 10, 6, {A_TroopAttack/*ACTION_7660*/}, S_158 },
/*S_172*/		 { SPR_TROO, 7, 2, {NULL}, S_173 },
/*S_173*/		 { SPR_TROO, 7, 2, {A_Pain/*ACTION_6148*/}, S_158 },
/*S_174*/		 { SPR_TROO, 11, 8, {NULL}, S_175 },
/*S_175*/		 { SPR_TROO, 12, 8, {A_Scream/*ACTION_5952*/}, S_176 },
/*S_176*/		 { SPR_TROO, 13, 6, {A_Fall/*ACTION_6236*/}, S_177 },
/*S_177*/		 { SPR_TROO, 14, 6, {A_OnDeathTrigger/*ACTION_6292*/}, S_178 },
/*S_178*/		 { SPR_TROO, 15, -1, {NULL}, S_000 },
/*S_179*/		 { SPR_TROO, 16, 5, {NULL}, S_180 },
/*S_180*/		 { SPR_TROO, 17, 5, {A_XScream/*ACTION_6116*/}, S_181 },
/*S_181*/		 { SPR_TROO, 18, 5, {NULL}, S_182 },
/*S_182*/		 { SPR_TROO, 19, 5, {A_Fall/*ACTION_6236*/}, S_183 },
/*S_183*/		 { SPR_TROO, 20, 5, {NULL}, S_184 },
/*S_184*/		 { SPR_TROO, 21, 5, {NULL}, S_185 },
/*S_185*/		 { SPR_TROO, 22, 5, {A_OnDeathTrigger/*ACTION_6292*/}, S_186 },
/*S_186*/		 { SPR_TROO, 23, -1, {NULL}, S_000 },
/*S_187*/		 { SPR_HEAD, 0, 15, {A_Look/*ACTION_4928*/}, S_188 },
/*S_188*/		 { SPR_HEAD, 1, 15, {A_Look/*ACTION_4928*/}, S_189 },
/*S_189*/		 { SPR_HEAD, 2, 15, {A_Look/*ACTION_4928*/}, S_190 },
/*S_190*/		 { SPR_HEAD, 3, 15, {A_Look/*ACTION_4928*/}, S_187 },
/*S_191*/		 { SPR_HEAD, 0, 3, {A_Chase/*ACTION_5228*/}, S_192 },
/*S_192*/		 { SPR_HEAD, 0, 3, {A_Chase/*ACTION_5228*/}, S_193 },
/*S_193*/		 { SPR_HEAD, 1, 3, {A_Chase/*ACTION_5228*/}, S_194 },
/*S_194*/		 { SPR_HEAD, 1, 3, {A_Chase/*ACTION_5228*/}, S_195 },
/*S_195*/		 { SPR_HEAD, 2, 3, {A_Chase/*ACTION_5228*/}, S_196 },
/*S_196*/		 { SPR_HEAD, 2, 3, {A_Chase/*ACTION_5228*/}, S_197 },
/*S_197*/		 { SPR_HEAD, 3, 3, {A_Chase/*ACTION_5228*/}, S_198 },
/*S_198*/		 { SPR_HEAD, 3, 3, {A_Chase/*ACTION_5228*/}, S_191 },
/*S_199*/		 { SPR_HEAD, 4, 5, {A_FaceTarget/*ACTION_5800*/}, S_200 },
/*S_200*/		 { SPR_HEAD, 5, 5, {A_FaceTarget/*ACTION_5800*/}, S_201 },
/*S_201*/		 { SPR_HEAD, 6, 5, {A_FaceTarget/*ACTION_5800*/}, S_202 },
/*S_202*/		 { SPR_HEAD, 6, 0, {A_HeadAttack/*ACTION_7824*/}, S_191 },
/*S_203*/		 { SPR_HEAD, 1, 3, {NULL}, S_204 },
/*S_204*/		 { SPR_HEAD, 1, 3, {A_Pain/*ACTION_6148*/}, S_205 },
/*S_205*/		 { SPR_HEAD, 1, 6, {NULL}, S_191 },
/*S_206*/		 { SPR_HEAD, 7, 8, {NULL}, S_207 },
/*S_207*/		 { SPR_HEAD, 8, 8, {A_Scream/*ACTION_5952*/}, S_208 },
/*S_208*/		 { SPR_HEAD, 9, 8, {NULL}, S_209 },
/*S_209*/		 { SPR_HEAD, 10, 8, {A_Fall/*ACTION_6236*/}, S_210 },
/*S_210*/		 { SPR_HEAD, 11, 8, {A_OnDeathTrigger/*ACTION_6292*/}, S_211 },
/*S_211*/		 { SPR_HEAD, 12, -1, {NULL}, S_000 },
/*S_212*/		 { SPR_BOSS, 0, 10, {A_Look/*ACTION_4928*/}, S_213 },
/*S_213*/		 { SPR_BOSS, 1, 10, {A_Look/*ACTION_4928*/}, S_212 },
/*S_214*/		 { SPR_BOSS, 0, 3, {A_Chase/*ACTION_5228*/}, S_215 },
/*S_215*/		 { SPR_BOSS, 0, 3, {A_Chase/*ACTION_5228*/}, S_216 },
/*S_216*/		 { SPR_BOSS, 1, 3, {A_Chase/*ACTION_5228*/}, S_217 },
/*S_217*/		 { SPR_BOSS, 1, 3, {A_Chase/*ACTION_5228*/}, S_218 },
/*S_218*/		 { SPR_BOSS, 2, 3, {A_Chase/*ACTION_5228*/}, S_219 },
/*S_219*/		 { SPR_BOSS, 2, 3, {A_Chase/*ACTION_5228*/}, S_220 },
/*S_220*/		 { SPR_BOSS, 3, 3, {A_Chase/*ACTION_5228*/}, S_221 },
/*S_221*/		 { SPR_BOSS, 3, 3, {A_Chase/*ACTION_5228*/}, S_214 },
/*S_222*/		 { SPR_BOSS, 4, 8, {A_FaceTarget/*ACTION_5800*/}, S_223 },
/*S_223*/		 { SPR_BOSS, 5, 8, {A_FaceTarget/*ACTION_5800*/}, S_224 },
/*S_224*/		 { SPR_BOSS, 6, 8, {A_BruisAttack/*ACTION_8132*/}, S_214 },
/*S_225*/		 { SPR_BOSS, 7, 2, {NULL}, S_226 },
/*S_226*/		 { SPR_BOSS, 7, 2, {A_Pain/*ACTION_6148*/}, S_214 },
/*S_227*/		 { SPR_BOSS, 8, 8, {NULL}, S_228 },
/*S_228*/		 { SPR_BOSS, 9, 8, {A_Scream/*ACTION_5952*/}, S_229 },
/*S_229*/		 { SPR_BOSS, 10, 8, {NULL}, S_230 },
/*S_230*/		 { SPR_BOSS, 11, 8, {A_Fall/*ACTION_6236*/}, S_231 },
/*S_231*/		 { SPR_BOSS, 12, 8, {A_OnDeathTrigger/*ACTION_6292*/}, S_232 },
/*S_232*/		 { SPR_BOSS, 13, -1, {NULL}, S_000 },
/*S_233*/		 { SPR_BOSS, 0, 10, {A_Look/*ACTION_4928*/}, S_234 },
/*S_234*/		 { SPR_BOSS, 1, 10, {A_Look/*ACTION_4928*/}, S_233 },
/*S_235*/		 { SPR_BOSS, 0, 3, {A_Chase/*ACTION_5228*/}, S_236 },
/*S_236*/		 { SPR_BOSS, 0, 3, {A_Chase/*ACTION_5228*/}, S_237 },
/*S_237*/		 { SPR_BOSS, 1, 3, {A_Chase/*ACTION_5228*/}, S_238 },
/*S_238*/		 { SPR_BOSS, 1, 3, {A_Chase/*ACTION_5228*/}, S_239 },
/*S_239*/		 { SPR_BOSS, 2, 3, {A_Chase/*ACTION_5228*/}, S_240 },
/*S_240*/		 { SPR_BOSS, 2, 3, {A_Chase/*ACTION_5228*/}, S_241 },
/*S_241*/		 { SPR_BOSS, 3, 3, {A_Chase/*ACTION_5228*/}, S_242 },
/*S_242*/		 { SPR_BOSS, 3, 3, {A_Chase/*ACTION_5228*/}, S_235 },
/*S_243*/		 { SPR_BOSS, 4, 8, {A_FaceTarget/*ACTION_5800*/}, S_244 },
/*S_244*/		 { SPR_BOSS, 5, 8, {A_FaceTarget/*ACTION_5800*/}, S_245 },
/*S_245*/		 { SPR_BOSS, 6, 8, {A_BruisAttack/*ACTION_8132*/}, S_235 },
/*S_246*/		 { SPR_BOSS, 7, 2, {NULL}, S_247 },
/*S_247*/		 { SPR_BOSS, 7, 2, {A_Pain/*ACTION_6148*/}, S_235 },
/*S_248*/		 { SPR_BOSS, 8, 8, {NULL}, S_249 },
/*S_249*/		 { SPR_BOSS, 9, 8, {A_Scream/*ACTION_5952*/}, S_250 },
/*S_250*/		 { SPR_BOSS, 10, 8, {NULL}, S_251 },
/*S_251*/		 { SPR_BOSS, 11, 8, {A_Fall/*ACTION_6236*/}, S_252 },
/*S_252*/		 { SPR_BOSS, 12, 8, {A_OnDeathTrigger/*ACTION_6292*/}, S_253 },
/*S_253*/		 { SPR_BOSS, 13, -1, {NULL}, S_000 },
/*S_254*/		 { SPR_SKUL, 0, 5, {A_Look/*ACTION_4928*/}, S_255 },
/*S_255*/		 { SPR_SKUL, 1, 5, {A_Look/*ACTION_4928*/}, S_256 },
/*S_256*/		 { SPR_SKUL, 2, 5, {A_Look/*ACTION_4928*/}, S_254 },
/*S_257*/		 { SPR_SKUL, 0, 3, {A_Chase/*ACTION_5228*/}, S_258 },
/*S_258*/		 { SPR_SKUL, 1, 3, {A_Chase/*ACTION_5228*/}, S_259 },
/*S_259*/		 { SPR_SKUL, 2, 3, {A_Chase/*ACTION_5228*/}, S_257 },
/*S_260*/		 { SPR_SKUL, 3, 6, {A_FaceTarget/*ACTION_5800*/}, S_261 },
/*S_261*/		 { SPR_SKUL, 4, 4, {A_SkullAttack/*ACTION_9512*/}, S_262 },
/*S_262*/		 { SPR_SKUL, 3, 4, {NULL}, S_263 },
/*S_263*/		 { SPR_SKUL, 4, 4, {NULL}, S_262 },
/*S_264*/		 { SPR_SKUL, 5, 3, {NULL}, S_265 },
/*S_265*/		 { SPR_SKUL, 5, 3, {A_Pain/*ACTION_6148*/}, S_257 },
/*S_266*/		 { SPR_SKUL, 32774, 5, {NULL}, S_267 },
/*S_267*/		 { SPR_SKUL, 32775, 5, {A_Scream/*ACTION_5952*/}, S_268 },
/*S_268*/		 { SPR_SKUL, 32776, 5, {A_Fall/*ACTION_6236*/}, S_269 },
/*S_269*/		 { SPR_SKUL, 32777, 5, {A_OnDeathTrigger/*ACTION_6292*/}, S_270 },
/*S_270*/		 { SPR_SKUL, 32778, 4, {A_SkullSetAlpha/*ACTION_13176*/}, S_271 },
/*S_271*/		 { SPR_SKUL, 32779, 3, {NULL}, S_272 },
/*S_272*/		 { SPR_SKUL, 32780, 2, {NULL}, S_273 },
/*S_273*/		 { SPR_SKUL, 32781, 2, {NULL}, S_274 },
/*S_274*/		 { SPR_SKUL, 32782, 1, {NULL}, S_275 },
/*S_275*/		 { SPR_SKUL, 32783, 1, {NULL}, S_000 },
/*S_276*/		 { SPR_BSPI, 0, 10, {A_Look/*ACTION_4928*/}, S_277 },
/*S_277*/		 { SPR_BSPI, 1, 10, {A_Look/*ACTION_4928*/}, S_276 },
/*S_278*/		 { SPR_BSPI, 0, 20, {NULL}, S_279 },
/*S_279*/		 { SPR_BSPI, 0, 3, {A_BabyMetal/*ACTION_12608*/}, S_280 },
/*S_280*/		 { SPR_BSPI, 0, 3, {A_Chase/*ACTION_5228*/}, S_281 },
/*S_281*/		 { SPR_BSPI, 1, 3, {A_Chase/*ACTION_5228*/}, S_282 },
/*S_282*/		 { SPR_BSPI, 1, 3, {A_Chase/*ACTION_5228*/}, S_283 },
/*S_283*/		 { SPR_BSPI, 2, 3, {A_Chase/*ACTION_5228*/}, S_284 },
/*S_284*/		 { SPR_BSPI, 2, 3, {A_Chase/*ACTION_5228*/}, S_285 },
/*S_285*/		 { SPR_BSPI, 3, 3, {A_Chase/*ACTION_5228*/}, S_286 },
/*S_286*/		 { SPR_BSPI, 3, 3, {A_Chase/*ACTION_5228*/}, S_279 },
/*S_287*/		 { SPR_BSPI, 0, 15, {A_BspiFaceTarget/*ACTION_7248*/}, S_288 },
/*S_288*/		 { SPR_BSPI, 32772, 6, {A_BspiAttack/*ACTION_7284*/}, S_289 },
/*S_289*/		 { SPR_BSPI, 32772, 1, {A_SpidRefire/*ACTION_7356*/}, S_288 },
/*S_290*/		 { SPR_BSPI, 5, 3, {NULL}, S_291 },
/*S_291*/		 { SPR_BSPI, 5, 3, {A_Pain/*ACTION_6148*/}, S_279 },
/*S_292*/		 { SPR_BSPI, 6, 20, {A_Scream/*ACTION_5952*/}, S_293 },
/*S_293*/		 { SPR_BSPI, 7, 7, {A_Fall/*ACTION_6236*/}, S_294 },
/*S_294*/		 { SPR_BSPI, 8, 7, {NULL}, S_295 },
/*S_295*/		 { SPR_BSPI, 9, 7, {NULL}, S_296 },
/*S_296*/		 { SPR_BSPI, 10, 7, {A_OnDeathTrigger/*ACTION_6292*/}, S_297 },
/*S_297*/		 { SPR_BSPI, 11, -1, {NULL}, S_000 },
/*S_298*/		 { SPR_CYBR, 5, 10, {A_Look/*ACTION_4928*/}, S_298 },
/*S_299*/		 { SPR_CYBR, 0, 4, {A_Hoof/*ACTION_12512*/}, S_300 },
/*S_300*/		 { SPR_CYBR, 0, 4, {A_Chase/*ACTION_5228*/}, S_301 },
/*S_301*/		 { SPR_CYBR, 1, 4, {A_Chase/*ACTION_5228*/}, S_302 },
/*S_302*/		 { SPR_CYBR, 1, 4, {A_Chase/*ACTION_5228*/}, S_303 },
/*S_303*/		 { SPR_CYBR, 2, 4, {A_Chase/*ACTION_5228*/}, S_304 },
/*S_304*/		 { SPR_CYBR, 2, 4, {A_Chase/*ACTION_5228*/}, S_305 },
/*S_305*/		 { SPR_CYBR, 3, 4, {A_Metal/*ACTION_12560*/}, S_306 },
/*S_306*/		 { SPR_CYBR, 3, 4, {A_Chase/*ACTION_5228*/}, S_299 },
/*S_307*/		 { SPR_CYBR, 32772, 6, {A_FaceTarget/*ACTION_5800*/}, S_308 },
/*S_308*/		 { SPR_CYBR, 5, 12, {A_CyberAttack/*ACTION_7944*/}, S_309 },
/*S_309*/		 { SPR_CYBR, 32772, 12, {A_FaceTarget/*ACTION_5800*/}, S_310 },
/*S_310*/		 { SPR_CYBR, 5, 12, {A_CyberAttack/*ACTION_7944*/}, S_311 },
/*S_311*/		 { SPR_CYBR, 32772, 12, {A_FaceTarget/*ACTION_5800*/}, S_312 },
/*S_312*/		 { SPR_CYBR, 5, 12, {A_CyberAttack/*ACTION_7944*/}, S_299 },
/*S_313*/		 { SPR_CYBR, 5, 10, {A_Pain/*ACTION_6148*/}, S_299 },
/*S_314*/		 { SPR_CYBR, 6, 30, {A_CyberDeathEvent/*ACTION_8004*/}, S_315 },
/*S_315*/		 { SPR_CYBR, 7, 8, {NULL}, S_316 },
/*S_316*/		 { SPR_CYBR, 8, 7, {NULL}, S_317 },
/*S_317*/		 { SPR_CYBR, 9, 6, {NULL}, S_318 },
/*S_318*/		 { SPR_CYBR, 10, 5, {NULL}, S_319 },
/*S_319*/		 { SPR_CYBR, 11, 4, {A_Fall/*ACTION_6236*/}, S_320 },
/*S_320*/		 { SPR_CYBR, 12, 4, {NULL}, S_321 },
/*S_321*/		 { SPR_CYBR, 13, 4, {A_OnDeathTrigger/*ACTION_6292*/}, S_322 },
/*S_322*/		 { SPR_CYBR, 14, -1, {NULL}, S_000 },
/*S_323*/		 { SPR_CYBR, 5, 1, {A_TargetCamera/*ACTION_12268*/}, S_323 },
/*S_324*/		 { SPR_CYBR, 32772, 18, {A_CyberAttack/*ACTION_7944*/}, S_325 },
/*S_325*/		 { SPR_CYBR, 5, 18, {A_FaceTarget/*ACTION_5800*/}, S_324 },
/*S_326*/		 { SPR_PAIN, 0, 5, {A_Look/*ACTION_4928*/}, S_326 },
/*S_327*/		 { SPR_PAIN, 0, 3, {A_Chase/*ACTION_5228*/}, S_327 },
/*S_328*/		 { SPR_PAIN, 32769, 5, {A_FaceTarget/*ACTION_5800*/}, S_329 },
/*S_329*/		 { SPR_PAIN, 32769, 5, {A_FaceTarget/*ACTION_5800*/}, S_330 },
/*S_330*/		 { SPR_PAIN, 32770, 5, {A_FaceTarget/*ACTION_5800*/}, S_331 },
/*S_331*/		 { SPR_PAIN, 32770, 0, {A_PainAttack/*ACTION_10244*/}, S_327 },
/*S_332*/		 { SPR_PAIN, 3, 6, {NULL}, S_333 },
/*S_333*/		 { SPR_PAIN, 3, 6, {A_Pain/*ACTION_6148*/}, S_327 },
/*S_334*/		 { SPR_PAIN, 32772, 8, {NULL}, S_335 },
/*S_335*/		 { SPR_PAIN, 32773, 8, {A_Scream/*ACTION_5952*/}, S_336 },
/*S_336*/		 { SPR_PAIN, 32774, 8, {A_PainDie/*ACTION_10332*/}, S_337 },
/*S_337*/		 { SPR_PAIN, 32775, 8, {A_PainDeathEvent/*ACTION_13156*/}, S_338 },
/*S_338*/		 { SPR_PAIN, 32776, 5, {A_PainDeathEvent/*ACTION_13156*/}, S_339 },
/*S_339*/		 { SPR_PAIN, 32777, 5, {A_PainDeathEvent/*ACTION_13156*/}, S_340 },
/*S_340*/		 { SPR_PAIN, 32778, 5, {NULL}, S_341 },
/*S_341*/		 { SPR_PAIN, 32779, 5, {NULL}, S_000 },
/*S_342*/		 { SPR_RECT, 0, 8, {A_Look/*ACTION_4928*/}, S_343 },
/*S_343*/		 { SPR_RECT, 1, 8, {A_Look/*ACTION_4928*/}, S_344 },
/*S_344*/		 { SPR_RECT, 2, 8, {A_Look/*ACTION_4928*/}, S_345 },
/*S_345*/		 { SPR_RECT, 3, 8, {A_Look/*ACTION_4928*/}, S_342 },
/*S_346*/		 { SPR_RECT, 0, 3, {A_RectChase/*ACTION_10436*/}, S_347 },
/*S_347*/		 { SPR_RECT, 0, 3, {A_RectChase/*ACTION_10436*/}, S_348 },
/*S_348*/		 { SPR_RECT, 0, 3, {A_RectChase/*ACTION_10436*/}, S_349 },
/*S_349*/		 { SPR_RECT, 1, 3, {A_RectChase/*ACTION_10436*/}, S_350 },
/*S_350*/		 { SPR_RECT, 1, 3, {A_RectChase/*ACTION_10436*/}, S_351 },
/*S_351*/		 { SPR_RECT, 1, 3, {A_RectChase/*ACTION_10436*/}, S_352 },
/*S_352*/		 { SPR_RECT, 2, 3, {A_RectChase/*ACTION_10436*/}, S_353 },
/*S_353*/		 { SPR_RECT, 2, 3, {A_RectChase/*ACTION_10436*/}, S_354 },
/*S_354*/		 { SPR_RECT, 2, 3, {A_RectChase/*ACTION_10436*/}, S_355 },
/*S_355*/		 { SPR_RECT, 3, 3, {A_RectChase/*ACTION_10436*/}, S_356 },
/*S_356*/		 { SPR_RECT, 3, 3, {A_RectChase/*ACTION_10436*/}, S_357 },
/*S_357*/		 { SPR_RECT, 3, 3, {A_RectChase/*ACTION_10436*/}, S_346 },
/*S_358*/		 { SPR_RECT, 32774, 12, {A_RectGroundFire/*ACTION_10604*/}, S_359 },
/*S_359*/		 { SPR_RECT, 4, 12, {NULL}, S_360 },
/*S_360*/		 { SPR_RECT, 32773, 12, {A_RectMissile/*ACTION_11036*/}, S_361 },
/*S_361*/		 { SPR_RECT, 4, 8, {NULL}, S_346 },
/*S_362*/		 { SPR_RECT, 7, 18, {A_Pain/*ACTION_6148*/}, S_358 },
/*S_363*/		 { SPR_RECT, 8, 60, {A_RectDeathEvent/*ACTION_12140*/}, S_364 },
/*S_364*/		 { SPR_RECT, 9, 8, {NULL}, S_365 },
/*S_365*/		 { SPR_RECT, 10, 8, {NULL}, S_366 },
/*S_366*/		 { SPR_RECT, 11, 5, {NULL}, S_367 },
/*S_367*/		 { SPR_RECT, 12, 4, {NULL}, S_368 },
/*S_368*/		 { SPR_RECT, 13, 3, {A_OnDeathTrigger/*ACTION_6292*/}, S_369 },
/*S_369*/		 { SPR_RECT, 14, 2, {NULL}, S_000 },
/*S_370*/		 { SPR_SPOT, 32768, -1, {NULL}, S_000 },
/*S_371*/		 { SPR_MISL, 32768, 2, {A_SpawnSmoke/*ACTION_8268*/}, S_371 },
/*S_372*/		 { SPR_MISL, 32769, 3, {A_Explode/*ACTION_6256*/}, S_373 },
/*S_373*/		 { SPR_MISL, 32769, 3, {A_FadeAlpha/*ACTION_13116*/}, S_374 },
/*S_374*/		 { SPR_MISL, 32770, 3, {A_FadeAlpha/*ACTION_13116*/}, S_375 },
/*S_375*/		 { SPR_MISL, 32770, 3, {A_FadeAlpha/*ACTION_13116*/}, S_376 },
/*S_376*/		 { SPR_MISL, 32771, 2, {A_FadeAlpha/*ACTION_13116*/}, S_377 },
/*S_377*/		 { SPR_MISL, 32772, 2, {NULL}, S_378 },
/*S_378*/		 { SPR_MISL, 32773, 2, {NULL}, S_000 },
/*S_379*/		 { SPR_PLSS, 32768, 3, {NULL}, S_380 },
/*S_380*/		 { SPR_PLSS, 32769, 3, {NULL}, S_379 },
/*S_381*/		 { SPR_PLSS, 32770, 2, {A_FadeAlpha/*ACTION_13116*/}, S_382 },
/*S_382*/		 { SPR_PLSS, 32771, 2, {NULL}, S_383 },
/*S_383*/		 { SPR_PLSS, 32772, 2, {NULL}, S_384 },
/*S_384*/		 { SPR_PLSS, 32773, 2, {NULL}, S_385 },
/*S_385*/		 { SPR_PLSS, 32774, 2, {NULL}, S_386 },
/*S_386*/		 { SPR_PLSS, 32775, 2, {NULL}, S_000 },
/*S_387*/		 { SPR_BFS1, 32768, 2, {NULL}, S_388 },
/*S_388*/		 { SPR_BFS1, 32769, 2, {NULL}, S_387 },
/*S_389*/		 { SPR_BFS1, 32770, 8, {A_BFGFlash/*ACTION_50504*/}, S_390 },
/*S_390*/		 { SPR_BFS1, 32771, 6, {A_FadeAlpha/*ACTION_13116*/}, S_391 },
/*S_391*/		 { SPR_BFS1, 32772, 3, {A_BFGSpray/*ACTION_50528*/}, S_392 },
/*S_392*/		 { SPR_BFS1, 32773, 3, {A_FadeAlpha/*ACTION_13116*/}, S_393 },
/*S_393*/		 { SPR_BFS1, 32774, 2, {NULL}, S_394 },
/*S_394*/		 { SPR_BFS1, 32775, 2, {NULL}, S_000 },
/*S_395*/		 { SPR_LASS, 32768, 3, {NULL}, S_396 },
/*S_396*/		 { SPR_LASS, 32769, 3, {NULL}, S_395 },
/*S_397*/		 { SPR_BAL1, 32768, 4, {NULL}, S_398 },
/*S_398*/		 { SPR_BAL1, 32769, 4, {NULL}, S_399 },
/*S_399*/		 { SPR_BAL1, 32770, 4, {NULL}, S_397 },
/*S_400*/		 { SPR_BAL1, 32771, 1, {NULL}, S_401 },
/*S_401*/		 { SPR_BAL1, 32772, 1, {NULL}, S_402 },
/*S_402*/		 { SPR_BAL1, 32773, 1, {NULL}, S_403 },
/*S_403*/		 { SPR_BAL1, 32774, 1, {NULL}, S_404 },
/*S_404*/		 { SPR_BAL1, 32775, 1, {NULL}, S_405 },
/*S_405*/		 { SPR_BAL1, 32776, 1, {NULL}, S_000 },
/*S_406*/		 { SPR_BAL3, 32768, 4, {NULL}, S_407 },
/*S_407*/		 { SPR_BAL3, 32769, 4, {NULL}, S_408 },
/*S_408*/		 { SPR_BAL3, 32770, 4, {NULL}, S_406 },
/*S_409*/		 { SPR_BAL3, 32771, 2, {NULL}, S_410 },
/*S_410*/		 { SPR_BAL3, 32772, 2, {NULL}, S_411 },
/*S_411*/		 { SPR_BAL3, 32773, 2, {NULL}, S_412 },
/*S_412*/		 { SPR_BAL3, 32774, 2, {NULL}, S_413 },
/*S_413*/		 { SPR_BAL3, 32775, 2, {NULL}, S_414 },
/*S_414*/		 { SPR_BAL3, 32776, 2, {NULL}, S_000 },
/*S_415*/		 { SPR_BAL2, 32768, 4, {NULL}, S_416 },
/*S_416*/		 { SPR_BAL2, 32769, 4, {NULL}, S_417 },
/*S_417*/		 { SPR_BAL2, 32770, 4, {NULL}, S_415 },
/*S_418*/		 { SPR_BAL2, 32771, 6, {A_MissileSetAlpha/*ACTION_13196*/}, S_419 },
/*S_419*/		 { SPR_BAL2, 32772, 5, {NULL}, S_420 },
/*S_420*/		 { SPR_BAL2, 32773, 2, {NULL}, S_421 },
/*S_421*/		 { SPR_BAL2, 32774, 2, {NULL}, S_422 },
/*S_422*/		 { SPR_BAL2, 32775, 2, {NULL}, S_000 },
/*S_423*/		 { SPR_BAL7, 32768, 4, {NULL}, S_424 },
/*S_424*/		 { SPR_BAL7, 32769, 4, {NULL}, S_423 },
/*S_425*/		 { SPR_BAL7, 32770, 3, {NULL}, S_426 },
/*S_426*/		 { SPR_BAL7, 32771, 3, {A_FadeAlpha/*ACTION_13116*/}, S_427 },
/*S_427*/		 { SPR_BAL7, 32772, 3, {A_FadeAlpha/*ACTION_13116*/}, S_428 },
/*S_428*/		 { SPR_BAL7, 32773, 2, {NULL}, S_429 },
/*S_429*/		 { SPR_BAL7, 32774, 2, {NULL}, S_430 },
/*S_430*/		 { SPR_BAL7, 32775, 2, {NULL}, S_000 },
/*S_431*/		 { SPR_BAL8, 32768, 4, {NULL}, S_432 },
/*S_432*/		 { SPR_BAL8, 32769, 4, {NULL}, S_431 },
/*S_433*/		 { SPR_BAL8, 32770, 3, {NULL}, S_434 },
/*S_434*/		 { SPR_BAL8, 32771, 3, {A_FadeAlpha/*ACTION_13116*/}, S_435 },
/*S_435*/		 { SPR_BAL8, 32772, 3, {A_FadeAlpha/*ACTION_13116*/}, S_436 },
/*S_436*/		 { SPR_BAL8, 32773, 2, {NULL}, S_437 },
/*S_437*/		 { SPR_BAL8, 32774, 2, {NULL}, S_438 },
/*S_438*/		 { SPR_BAL8, 32775, 2, {NULL}, S_000 },
/*S_439*/		 { SPR_APLS, 32768, 2, {NULL}, S_440 },
/*S_440*/		 { SPR_APLS, 32769, 2, {NULL}, S_439 },
/*S_441*/		 { SPR_APLS, 32770, 3, {NULL}, S_442 },
/*S_442*/		 { SPR_APLS, 32771, 3, {NULL}, S_443 },
/*S_443*/		 { SPR_APLS, 32772, 3, {NULL}, S_444 },
/*S_444*/		 { SPR_APLS, 32773, 3, {NULL}, S_445 },
/*S_445*/		 { SPR_APLS, 32774, 3, {NULL}, S_446 },
/*S_446*/		 { SPR_APLS, 32775, 3, {NULL}, S_000 },
/*S_447*/		 { SPR_MANF, 32768, 2, {NULL}, S_448 },
/*S_448*/		 { SPR_MANF, 32769, 2, {NULL}, S_449 },
/*S_449*/		 { SPR_MANF, 32770, 2, {NULL}, S_447 },
/*S_450*/		 { SPR_MANF, 32771, 6, {NULL}, S_451 },
/*S_451*/		 { SPR_MANF, 32772, 4, {A_FadeAlpha/*ACTION_13116*/}, S_452 },
/*S_452*/		 { SPR_MANF, 32773, 3, {A_FadeAlpha/*ACTION_13116*/}, S_453 },
/*S_453*/		 { SPR_MANF, 32774, 2, {A_FadeAlpha/*ACTION_13116*/}, S_454 },
/*S_454*/		 { SPR_MANF, 32775, 2, {NULL}, S_455 },
/*S_455*/		 { SPR_MANF, 32776, 2, {NULL}, S_000 },
/*S_456*/		 { SPR_TRCR, 32768, 3, {A_Tracer/*ACTION_8328*/}, S_457 },
/*S_457*/		 { SPR_TRCR, 32769, 3, {A_Tracer/*ACTION_8328*/}, S_456 },
/*S_458*/		 { SPR_TRCR, 32770, 4, {A_FadeAlpha/*ACTION_13116*/}, S_459 },
/*S_459*/		 { SPR_TRCR, 32771, 3, {A_FadeAlpha/*ACTION_13116*/}, S_460 },
/*S_460*/		 { SPR_TRCR, 32772, 2, {A_FadeAlpha/*ACTION_13116*/}, S_461 },
/*S_461*/		 { SPR_TRCR, 32773, 2, {A_FadeAlpha/*ACTION_13116*/}, S_462 },
/*S_462*/		 { SPR_TRCR, 32774, 2, {NULL}, S_463 },
/*S_463*/		 { SPR_TRCR, 32775, 2, {NULL}, S_464 },
/*S_464*/		 { SPR_TRCR, 32776, 2, {NULL}, S_000 },
/*S_465*/		 { SPR_DART, 32768, -1, {NULL}, S_000 },
/*S_466*/		 { SPR_FIRE, 32768, 3, {A_MoveGroundFire/*ACTION_11940*/}, S_467 },
/*S_467*/		 { SPR_FIRE, 32769, 3, {A_MoveGroundFire/*ACTION_11940*/}, S_468 },
/*S_468*/		 { SPR_FIRE, 32770, 3, {A_MoveGroundFire/*ACTION_11940*/}, S_469 },
/*S_469*/		 { SPR_FIRE, 32771, 3, {A_MoveGroundFire/*ACTION_11940*/}, S_466 },
/*S_470*/		 { SPR_RBAL, 0, 2, {A_RectTracer/*ACTION_12084*/}, S_471 },
/*S_471*/		 { SPR_RBAL, 1, 2, {A_RectTracer/*ACTION_12084*/}, S_470 },
/*S_472*/		 { SPR_RBAL, 32770, 4, {NULL}, S_473 },
/*S_473*/		 { SPR_RBAL, 32771, 3, {A_MissileSetAlpha/*ACTION_13196*/}, S_474 },
/*S_474*/		 { SPR_RBAL, 32772, 2, {NULL}, S_475 },
/*S_475*/		 { SPR_RBAL, 32773, 2, {NULL}, S_476 },
/*S_476*/		 { SPR_RBAL, 32774, 2, {NULL}, S_477 },
/*S_477*/		 { SPR_RBAL, 32775, 2, {NULL}, S_000 },
/*S_478*/		 { SPR_PUF2, 32768, 2, {NULL}, S_479 },
/*S_479*/		 { SPR_PUF2, 32769, 2, {NULL}, S_480 },
/*S_480*/		 { SPR_PUF2, 32770, 2, {NULL}, S_481 },
/*S_481*/		 { SPR_PUF2, 32771, 2, {NULL}, S_482 },
/*S_482*/		 { SPR_PUF2, 32772, 2, {NULL}, S_000 },
/*S_483*/		 { SPR_PUF3, 32768, 2, {NULL}, S_484 },
/*S_484*/		 { SPR_PUF3, 32769, 2, {NULL}, S_485 },
/*S_485*/		 { SPR_PUF3, 32770, 2, {NULL}, S_486 },
/*S_486*/		 { SPR_PUF3, 32771, 2, {NULL}, S_487 },
/*S_487*/		 { SPR_PUF3, 32772, 2, {NULL}, S_000 },
/*S_488*/		 { SPR_PUFF, 32768, 4, {NULL}, S_489 },
/*S_489*/		 { SPR_PUFF, 32769, 3, {NULL}, S_490 },
/*S_490*/		 { SPR_PUFF, 2, 3, {NULL}, S_491 },
/*S_491*/		 { SPR_PUFF, 3, 3, {NULL}, S_492 },
/*S_492*/		 { SPR_PUFF, 4, 3, {NULL}, S_493 },
/*S_493*/		 { SPR_PUFF, 5, 3, {NULL}, S_000 },
/*S_494*/		 { SPR_BLUD, 0, 6, {NULL}, S_495 },
/*S_495*/		 { SPR_BLUD, 1, 6, {NULL}, S_496 },
/*S_496*/		 { SPR_BLUD, 2, 6, {NULL}, S_497 },
/*S_497*/		 { SPR_BLUD, 3, 6, {NULL}, S_000 },
/*S_498*/		 { SPR_A027, 0, -1, {NULL}, S_000 },
/*S_499*/		 { SPR_TFOG, 32772, 3, {NULL}, S_500 },
/*S_500*/		 { SPR_TFOG, 32771, 3, {NULL}, S_501 },
/*S_501*/		 { SPR_TFOG, 32770, 3, {NULL}, S_502 },
/*S_502*/		 { SPR_TFOG, 32769, 3, {NULL}, S_503 },
/*S_503*/		 { SPR_TFOG, 32768, 3, {NULL}, S_504 },
/*S_504*/		 { SPR_TFOG, 32769, 3, {NULL}, S_505 },
/*S_505*/		 { SPR_TFOG, 32770, 3, {NULL}, S_506 },
/*S_506*/		 { SPR_TFOG, 32771, 3, {NULL}, S_507 },
/*S_507*/		 { SPR_TFOG, 32772, 3, {NULL}, S_508 },
/*S_508*/		 { SPR_TFOG, 32773, 3, {NULL}, S_509 },
/*S_509*/		 { SPR_TFOG, 32774, 3, {NULL}, S_510 },
/*S_510*/		 { SPR_TFOG, 32775, 3, {NULL}, S_000 },
/*S_511*/		 { SPR_BFE2, 32768, 4, {NULL}, S_512 },
/*S_512*/		 { SPR_BFE2, 32769, 3, {A_FadeAlpha/*ACTION_13116*/}, S_513 },
/*S_513*/		 { SPR_BFE2, 32770, 2, {NULL}, S_514 },
/*S_514*/		 { SPR_BFE2, 32771, 2, {NULL}, S_515 },
/*S_515*/		 { SPR_BFE2, 32772, 2, {NULL}, S_516 },
/*S_516*/		 { SPR_BFE2, 32773, 2, {NULL}, S_000 },
/*S_517*/		 { SPR_ARM1, 32768, 6, {NULL}, S_518 },
/*S_518*/		 { SPR_ARM1, 32769, 6, {NULL}, S_517 },
/*S_519*/		 { SPR_ARM2, 32768, 6, {NULL}, S_520 },
/*S_520*/		 { SPR_ARM2, 32769, 6, {NULL}, S_519 },
/*S_521*/		 { SPR_BON1, 0, 3, {NULL}, S_522 },
/*S_522*/		 { SPR_BON1, 1, 3, {NULL}, S_523 },
/*S_523*/		 { SPR_BON1, 2, 3, {NULL}, S_524 },
/*S_524*/		 { SPR_BON1, 3, 3, {NULL}, S_521 },
/*S_525*/		 { SPR_BON2, 0, 3, {NULL}, S_526 },
/*S_526*/		 { SPR_BON2, 1, 3, {NULL}, S_527 },
/*S_527*/		 { SPR_BON2, 2, 3, {NULL}, S_528 },
/*S_528*/		 { SPR_BON2, 3, 3, {NULL}, S_529 },
/*S_529*/		 { SPR_BON2, 2, 3, {NULL}, S_530 },
/*S_530*/		 { SPR_BON2, 1, 3, {NULL}, S_525 },
/*S_531*/		 { SPR_BKEY, 0, 5, {NULL}, S_532 },
/*S_532*/		 { SPR_BKEY, 32768, 5, {NULL}, S_531 },
/*S_533*/		 { SPR_RKEY, 0, 5, {NULL}, S_534 },
/*S_534*/		 { SPR_RKEY, 32768, 5, {NULL}, S_533 },
/*S_535*/		 { SPR_YKEY, 0, 5, {NULL}, S_536 },
/*S_536*/		 { SPR_YKEY, 32768, 5, {NULL}, S_535 },
/*S_537*/		 { SPR_YSKU, 0, 5, {NULL}, S_538 },
/*S_538*/		 { SPR_YSKU, 32768, 5, {NULL}, S_537 },
/*S_539*/		 { SPR_RSKU, 0, 5, {NULL}, S_540 },
/*S_540*/		 { SPR_RSKU, 32768, 5, {NULL}, S_539 },
/*S_541*/		 { SPR_BSKU, 0, 5, {NULL}, S_542 },
/*S_542*/		 { SPR_BSKU, 32768, 5, {NULL}, S_541 },
/*S_543*/		 { SPR_ART1, 32768, 4, {NULL}, S_544 },
/*S_544*/		 { SPR_ART1, 32769, 4, {NULL}, S_545 },
/*S_545*/		 { SPR_ART1, 32770, 4, {NULL}, S_546 },
/*S_546*/		 { SPR_ART1, 32771, 4, {NULL}, S_547 },
/*S_547*/		 { SPR_ART1, 32772, 4, {NULL}, S_548 },
/*S_548*/		 { SPR_ART1, 32771, 4, {NULL}, S_549 },
/*S_549*/		 { SPR_ART1, 32770, 4, {NULL}, S_550 },
/*S_550*/		 { SPR_ART1, 32769, 4, {NULL}, S_543 },
/*S_551*/		 { SPR_ART2, 32768, 4, {NULL}, S_552 },
/*S_552*/		 { SPR_ART2, 32769, 4, {NULL}, S_553 },
/*S_553*/		 { SPR_ART2, 32770, 4, {NULL}, S_554 },
/*S_554*/		 { SPR_ART2, 32771, 4, {NULL}, S_555 },
/*S_555*/		 { SPR_ART2, 32772, 4, {NULL}, S_556 },
/*S_556*/		 { SPR_ART2, 32771, 4, {NULL}, S_557 },
/*S_557*/		 { SPR_ART2, 32770, 4, {NULL}, S_558 },
/*S_558*/		 { SPR_ART2, 32769, 4, {NULL}, S_551 },
/*S_559*/		 { SPR_ART3, 32768, 4, {NULL}, S_560 },
/*S_560*/		 { SPR_ART3, 32769, 4, {NULL}, S_561 },
/*S_561*/		 { SPR_ART3, 32770, 4, {NULL}, S_562 },
/*S_562*/		 { SPR_ART3, 32771, 4, {NULL}, S_563 },
/*S_563*/		 { SPR_ART3, 32772, 4, {NULL}, S_564 },
/*S_564*/		 { SPR_ART3, 32771, 4, {NULL}, S_565 },
/*S_565*/		 { SPR_ART3, 32770, 4, {NULL}, S_566 },
/*S_566*/		 { SPR_ART3, 32769, 4, {NULL}, S_559 },
/*S_567*/		 { SPR_STIM, 0, -1, {NULL}, S_000 },
/*S_568*/		 { SPR_MEDI, 0, -1, {NULL}, S_000 },
/*S_569*/		 { SPR_SOUL, 32768, 6, {NULL}, S_570 },
/*S_570*/		 { SPR_SOUL, 32769, 6, {NULL}, S_571 },
/*S_571*/		 { SPR_SOUL, 32770, 6, {NULL}, S_572 },
/*S_572*/		 { SPR_SOUL, 32771, 6, {NULL}, S_573 },
/*S_573*/		 { SPR_SOUL, 32770, 6, {NULL}, S_574 },
/*S_574*/		 { SPR_SOUL, 32769, 6, {NULL}, S_569 },
/*S_575*/		 { SPR_PINV, 32768, 6, {NULL}, S_576 },
/*S_576*/		 { SPR_PINV, 32769, 6, {NULL}, S_577 },
/*S_577*/		 { SPR_PINV, 32770, 6, {NULL}, S_578 },
/*S_578*/		 { SPR_PINV, 32771, 6, {NULL}, S_579 },
/*S_579*/		 { SPR_PINV, 32770, 6, {NULL}, S_580 },
/*S_580*/		 { SPR_PINV, 32769, 6, {NULL}, S_575 },
/*S_581*/		 { SPR_PSTR, 32768, -1, {NULL}, S_000 },
/*S_582*/		 { SPR_PINS, 32768, 2, {NULL}, S_583 },
/*S_583*/		 { SPR_PINS, 32769, 3, {NULL}, S_584 },
/*S_584*/		 { SPR_PINS, 32770, 2, {NULL}, S_585 },
/*S_585*/		 { SPR_PINS, 32771, 1, {NULL}, S_586 },
/*S_586*/		 { SPR_PINS, 32769, 2, {NULL}, S_587 },
/*S_587*/		 { SPR_PINS, 32770, 1, {NULL}, S_582 },
/*S_588*/		 { SPR_SUIT, 32768, 4, {NULL}, S_589 },
/*S_589*/		 { SPR_SUIT, 32769, 4, {NULL}, S_588 },
/*S_590*/		 { SPR_PMAP, 32768, 5, {NULL}, S_591 },
/*S_591*/		 { SPR_PMAP, 32769, 5, {NULL}, S_592 },
/*S_592*/		 { SPR_PMAP, 32770, 5, {NULL}, S_593 },
/*S_593*/		 { SPR_PMAP, 32771, 5, {NULL}, S_590 },
/*S_594*/		 { SPR_PVIS, 32768, 3, {NULL}, S_595 },
/*S_595*/		 { SPR_PVIS, 1, 3, {NULL}, S_594 },
/*S_596*/		 { SPR_MEGA, 32768, 4, {NULL}, S_597 },
/*S_597*/		 { SPR_MEGA, 32769, 4, {NULL}, S_598 },
/*S_598*/		 { SPR_MEGA, 32770, 4, {NULL}, S_599 },
/*S_599*/		 { SPR_MEGA, 32771, 4, {NULL}, S_600 },
/*S_600*/		 { SPR_MEGA, 32770, 4, {NULL}, S_601 },
/*S_601*/		 { SPR_MEGA, 32769, 4, {NULL}, S_596 },
/*S_602*/		 { SPR_CLIP, 0, -1, {NULL}, S_000 },
/*S_603*/		 { SPR_AMMO, 0, -1, {NULL}, S_000 },
/*S_604*/		 { SPR_RCKT, 0, -1, {NULL}, S_000 },
/*S_605*/		 { SPR_BROK, 0, -1, {NULL}, S_000 },
/*S_606*/		 { SPR_CELL, 0, -1, {NULL}, S_000 },
/*S_607*/		 { SPR_CELP, 0, -1, {NULL}, S_000 },
/*S_608*/		 { SPR_SHEL, 0, -1, {NULL}, S_000 },
/*S_609*/		 { SPR_SBOX, 0, -1, {NULL}, S_000 },
/*S_610*/		 { SPR_BPAK, 0, -1, {NULL}, S_000 },
/*S_611*/		 { SPR_BFUG, 0, -1, {NULL}, S_000 },
/*S_612*/		 { SPR_CSAW, 0, -1, {NULL}, S_000 },
/*S_613*/		 { SPR_MGUN, 0, -1, {NULL}, S_000 },
/*S_614*/		 { SPR_LAUN, 0, -1, {NULL}, S_000 },
/*S_615*/		 { SPR_PLSM, 0, -1, {NULL}, S_000 },
/*S_616*/		 { SPR_SHOT, 0, -1, {NULL}, S_000 },
/*S_617*/		 { SPR_SGN2, 0, -1, {NULL}, S_000 },
/*S_618*/		 { SPR_LSRG, 0, -1, {NULL}, S_000 },
/*S_619*/		 { SPR_FIRE, 0, 3, {NULL}, S_620 },
/*S_620*/		 { SPR_FIRE, 1, 3, {NULL}, S_621 },
/*S_621*/		 { SPR_FIRE, 2, 3, {NULL}, S_622 },
/*S_622*/		 { SPR_FIRE, 3, 3, {NULL}, S_623 },
/*S_623*/		 { SPR_FIRE, 4, 3, {NULL}, S_619 },
/*S_624*/		 { SPR_CAND, 0, 2, {NULL}, S_625 },
/*S_625*/		 { SPR_CAND, 1, 2, {NULL}, S_624 },
/*S_626*/		 { SPR_BAR1, 0, -1, {NULL}, S_000 },
/*S_627*/		 { SPR_BAR1, 1, 5, {NULL}, S_628 },
/*S_628*/		 { SPR_BAR1, 2, 5, {NULL}, S_629 },
/*S_629*/		 { SPR_BAR1, 3, 5, {NULL}, S_630 },
/*S_630*/		 { SPR_BAR1, 32772, 5, {A_BarrelExplode/*ACTION_12400*/}, S_000 },
/*S_631*/		 { SPR_MISL, 32769, 6, {NULL}, S_632 },
/*S_632*/		 { SPR_MISL, 32770, 5, {NULL}, S_633 },
/*S_633*/		 { SPR_MISL, 32771, 2, {NULL}, S_634 },
/*S_634*/		 { SPR_MISL, 32772, 2, {NULL}, S_635 },
/*S_635*/		 { SPR_MISL, 32773, 2, {NULL}, S_000 },
/*S_636*/		 { SPR_MISL, 32769, 2, {NULL}, S_637 },
/*S_637*/		 { SPR_MISL, 32770, 2, {NULL}, S_638 },
/*S_638*/		 { SPR_MISL, 32771, 2, {NULL}, S_639 },
/*S_639*/		 { SPR_MISL, 32772, 2, {NULL}, S_640 },
/*S_640*/		 { SPR_MISL, 32773, 2, {NULL}, S_000 },
/*S_641*/		 { SPR_LMP1, 0, -1, {NULL}, S_000 },
/*S_642*/		 { SPR_LMP2, 0, -1, {NULL}, S_000 },
/*S_643*/		 { SPR_A031, 0, 4, {NULL}, S_644 },
/*S_644*/		 { SPR_A031, 1, 4, {NULL}, S_645 },
/*S_645*/		 { SPR_A031, 2, 4, {NULL}, S_646 },
/*S_646*/		 { SPR_A031, 3, 4, {NULL}, S_647 },
/*S_647*/		 { SPR_A031, 4, 4, {NULL}, S_643 },
/*S_648*/		 { SPR_A030, 0, 4, {NULL}, S_649 },
/*S_649*/		 { SPR_A030, 1, 4, {NULL}, S_650 },
/*S_650*/		 { SPR_A030, 2, 4, {NULL}, S_651 },
/*S_651*/		 { SPR_A030, 3, 4, {NULL}, S_652 },
/*S_652*/		 { SPR_A030, 4, 4, {NULL}, S_648 },
/*S_653*/		 { SPR_A032, 0, 4, {NULL}, S_654 },
/*S_654*/		 { SPR_A032, 1, 4, {NULL}, S_655 },
/*S_655*/		 { SPR_A032, 2, 4, {NULL}, S_656 },
/*S_656*/		 { SPR_A032, 3, 4, {NULL}, S_657 },
/*S_657*/		 { SPR_A032, 4, 4, {NULL}, S_653 },
/*S_658*/		 { SPR_A033, 0, -1, {NULL}, S_000 },
/*S_659*/		 { SPR_A034, 0, -1, {NULL}, S_000 },
/*S_660*/		 { SPR_BFLM, 32768, 4, {NULL}, S_661 },
/*S_661*/		 { SPR_BFLM, 32769, 4, {NULL}, S_662 },
/*S_662*/		 { SPR_BFLM, 32770, 4, {NULL}, S_663 },
/*S_663*/		 { SPR_BFLM, 32771, 4, {NULL}, S_664 },
/*S_664*/		 { SPR_BFLM, 32772, 4, {NULL}, S_660 },
/*S_665*/		 { SPR_RFLM, 32768, 4, {NULL}, S_666 },
/*S_666*/		 { SPR_RFLM, 32769, 4, {NULL}, S_667 },
/*S_667*/		 { SPR_RFLM, 32770, 4, {NULL}, S_668 },
/*S_668*/		 { SPR_RFLM, 32771, 4, {NULL}, S_669 },
/*S_669*/		 { SPR_RFLM, 32772, 4, {NULL}, S_665 },
/*S_670*/		 { SPR_YFLM, 32768, 4, {NULL}, S_671 },
/*S_671*/		 { SPR_YFLM, 32769, 4, {NULL}, S_672 },
/*S_672*/		 { SPR_YFLM, 32770, 4, {NULL}, S_673 },
/*S_673*/		 { SPR_YFLM, 32771, 4, {NULL}, S_674 },
/*S_674*/		 { SPR_YFLM, 32772, 4, {NULL}, S_670 },
/*S_675*/		 { SPR_A006, 0, -1, {NULL}, S_000 },
/*S_676*/		 { SPR_A021, 0, -1, {NULL}, S_000 },
/*S_677*/		 { SPR_A003, 0, -1, {NULL}, S_000 },
/*S_678*/		 { SPR_A020, 0, -1, {NULL}, S_000 },
/*S_679*/		 { SPR_A014, 0, 6, {NULL}, S_680 },
/*S_680*/		 { SPR_A014, 1, 6, {NULL}, S_679 },
/*S_681*/		 { SPR_A016, 0, -1, {NULL}, S_000 },
/*S_682*/		 { SPR_A027, 0, -1, {NULL}, S_000 },
/*S_683*/		 { SPR_A008, 0, -1, {NULL}, S_000 },
/*S_684*/		 { SPR_A007, 0, -1, {NULL}, S_000 },
/*S_685*/		 { SPR_A015, 0, -1, {NULL}, S_000 },
/*S_686*/		 { SPR_A001, 0, -1, {NULL}, S_000 },
/*S_687*/		 { SPR_A012, 0, -1, {NULL}, S_000 },
/*S_688*/		 { SPR_A010, 0, -1, {NULL}, S_000 },
/*S_689*/		 { SPR_A018, 0, -1, {NULL}, S_000 },
/*S_690*/		 { SPR_A017, 0, -1, {NULL}, S_000 },
/*S_691*/		 { SPR_A026, 0, -1, {NULL}, S_000 },
/*S_692*/		 { SPR_A022, 0, -1, {NULL}, S_000 },
/*S_693*/		 { SPR_A028, 0, -1, {NULL}, S_000 },
/*S_694*/		 { SPR_A029, 0, -1, {NULL}, S_000 },
/*S_695*/		 { SPR_A035, 0, -1, {NULL}, S_000 },
/*S_696*/		 { SPR_A036, 0, -1, {NULL}, S_000 },
/*S_697*/		 { SPR_TRE3, 0, -1, {NULL}, S_000 },
/*S_698*/		 { SPR_TRE2, 0, -1, {NULL}, S_000 },
/*S_699*/		 { SPR_TRE1, 0, -1, {NULL}, S_000 },
/*S_700*/		 { SPR_A013, 0, -1, {NULL}, S_000 },
/*S_701*/		 { SPR_A019, 0, -1, {NULL}, S_000 },
/*S_702*/		 { SPR_A004, 0, -1, {NULL}, S_000 },
/*S_703*/		 { SPR_A005, 0, -1, {NULL}, S_000 },
/*S_704*/		 { SPR_A023, 0, -1, {NULL}, S_000 },
/*S_705*/		 { SPR_SAWG, 0, 4, {A_ChainSawReady/*ACTION_48552*/}, S_706 },
/*S_706*/		 { SPR_SAWG, 1, 4, {A_WeaponReady/*ACTION_47164*/}, S_705 },
/*S_707*/		 { SPR_SAWG, 1, 1, {A_Lower/*ACTION_47552*/}, S_707 },
/*S_708*/		 { SPR_SAWG, 1, 1, {A_Raise/*ACTION_47748*/}, S_708 },
/*S_709*/		 { SPR_SAWG, 2, 2, {A_Saw/*ACTION_48156*/}, S_710 },
/*S_710*/		 { SPR_SAWG, 3, 2, {A_Saw/*ACTION_48156*/}, S_711 },
/*S_711*/		 { SPR_SAWG, 3, 0, {A_ReFire/*ACTION_47388*/}, S_705 },
/*S_712*/		 { SPR_PUNG, 0, 1, {A_WeaponReady/*ACTION_47164*/}, S_712 },
/*S_713*/		 { SPR_PUNG, 0, 1, {A_Lower/*ACTION_47552*/}, S_713 },
/*S_714*/		 { SPR_PUNG, 0, 1, {A_Raise/*ACTION_47748*/}, S_714 },
/*S_715*/		 { SPR_PUNG, 1, 4, {NULL}, S_716 },
/*S_716*/		 { SPR_PUNG, 2, 4, {A_Punch/*ACTION_47916*/}, S_717 },
/*S_717*/		 { SPR_PUNG, 3, 5, {NULL}, S_718 },
/*S_718*/		 { SPR_PUNG, 2, 4, {NULL}, S_719 },
/*S_719*/		 { SPR_PUNG, 1, 5, {A_ReFire/*ACTION_47388*/}, S_712 },
/*S_720*/		 { SPR_PISG, 0, 1, {A_WeaponReady/*ACTION_47164*/}, S_720 },
/*S_721*/		 { SPR_PISG, 0, 1, {A_Lower/*ACTION_47552*/}, S_721 },
/*S_722*/		 { SPR_PISG, 0, 1, {A_Raise/*ACTION_47748*/}, S_722 },
/*S_723*/		 { SPR_PISG, 0, 2, {NULL}, S_724 },
/*S_724*/		 { SPR_PISG, 1, 1, {A_FirePistol/*ACTION_49332*/}, S_725 },
/*S_725*/		 { SPR_PISG, 2, 5, {NULL}, S_726 },
/*S_726*/		 { SPR_PISG, 1, 5, {NULL}, S_727 },
/*S_727*/		 { SPR_PISG, 0, 1, {A_ReFire/*ACTION_47388*/}, S_720 },
/*S_728*/		 { SPR_PISG, 32771, 3, {NULL}, S_000 },
/*S_729*/		 { SPR_SHT1, 0, 1, {A_WeaponReady/*ACTION_47164*/}, S_729 },
/*S_730*/		 { SPR_SHT1, 0, 1, {A_Lower/*ACTION_47552*/}, S_730 },
/*S_731*/		 { SPR_SHT1, 0, 1, {A_Raise/*ACTION_47748*/}, S_731 },
/*S_732*/		 { SPR_SHT1, 0, 2, {NULL}, S_733 },
/*S_733*/		 { SPR_SHT1, 0, 4, {A_FireShotgun/*ACTION_49464*/}, S_734 },
/*S_734*/		 { SPR_SHT1, 1, 18, {NULL}, S_735 },
/*S_735*/		 { SPR_SHT1, 2, 5, {NULL}, S_736 },
/*S_736*/		 { SPR_SHT1, 0, 3, {NULL}, S_737 },
/*S_737*/		 { SPR_SHT1, 0, 7, {A_ReFire/*ACTION_47388*/}, S_729 },
/*S_738*/		 { SPR_SHT1, 32771, 4, {NULL}, S_000 },
/*S_739*/		 { SPR_SHT2, 0, 1, {A_WeaponReady/*ACTION_47164*/}, S_739 },
/*S_740*/		 { SPR_SHT2, 0, 1, {A_Lower/*ACTION_47552*/}, S_740 },
/*S_741*/		 { SPR_SHT2, 0, 1, {A_Raise/*ACTION_47748*/}, S_741 },
/*S_742*/		 { SPR_SHT2, 0, 1, {NULL}, S_743 },
/*S_743*/		 { SPR_SHT2, 0, 4, {A_FireShotgun2/*ACTION_49680*/}, S_744 },
/*S_744*/		 { SPR_SHT2, 1, 7, {NULL}, S_745 },
/*S_745*/		 { SPR_SHT2, 1, 5, {A_CheckReload/*ACTION_47520*/}, S_746 },
/*S_746*/		 { SPR_SHT2, 1, 5, {NULL}, S_747 },
/*S_747*/		 { SPR_SHT2, 1, 5, {NULL}, S_748 },
/*S_748*/		 { SPR_SHT2, 1, 5, {NULL}, S_749 },
/*S_749*/		 { SPR_SHT2, 1, 5, {A_LoadShotgun2/*ACTION_50880*/}, S_750 },
/*S_750*/		 { SPR_SHT2, 2, 4, {A_CloseShotgun2/*ACTION_50920*/}, S_751 },
/*S_751*/		 { SPR_SHT2, 0, 5, {A_ReFire/*ACTION_47388*/}, S_739 },
/*S_752*/		 { SPR_SHT2, 32771, 4, {NULL}, S_000 },
/*S_753*/		 { SPR_CHGG, 0, 1, {A_WeaponReady/*ACTION_47164*/}, S_753 },
/*S_754*/		 { SPR_CHGG, 0, 1, {A_Lower/*ACTION_47552*/}, S_754 },
/*S_755*/		 { SPR_CHGG, 0, 1, {A_Raise/*ACTION_47748*/}, S_755 },
/*S_756*/		 { SPR_CHGG, 0, 3, {A_FireCGun/*ACTION_50168*/}, S_757 },
/*S_757*/		 { SPR_CHGG, 1, 3, {A_FireCGun/*ACTION_50168*/}, S_758 },
/*S_758*/		 { SPR_CHGG, 1, 0, {A_ReFire/*ACTION_47388*/}, S_753 },
/*S_759*/		 { SPR_CHGG, 32771, 3, {NULL}, S_000 },
/*S_760*/		 { SPR_CHGG, 32770, 3, {NULL}, S_000 },
/*S_761*/		 { SPR_ROCK, 0, 1, {A_WeaponReady/*ACTION_47164*/}, S_761 },
/*S_762*/		 { SPR_ROCK, 0, 1, {A_Lower/*ACTION_47552*/}, S_762 },
/*S_763*/		 { SPR_ROCK, 0, 1, {A_Raise/*ACTION_47748*/}, S_763 },
/*S_764*/		 { SPR_ROCK, 1, 8, {A_GunFlash/*ACTION_47832*/}, S_765 },
/*S_765*/		 { SPR_ROCK, 1, 10, {A_FireMissile/*ACTION_48612*/}, S_766 },
/*S_766*/		 { SPR_ROCK, 1, 0, {A_ReFire/*ACTION_47388*/}, S_761 },
/*S_767*/		 { SPR_ROCK, 32770, 3, {NULL}, S_768 },
/*S_768*/		 { SPR_ROCK, 32771, 4, {NULL}, S_769 },
/*S_769*/		 { SPR_ROCK, 32772, 4, {NULL}, S_770 },
/*S_770*/		 { SPR_ROCK, 32773, 4, {NULL}, S_000 },
/*S_771*/		 { SPR_PLAS, 0, 1, {A_WeaponReady/*ACTION_47164*/}, S_771 },
/*S_772*/		 { SPR_PLAS, 0, 1, {A_Lower/*ACTION_47552*/}, S_772 },
/*S_773*/		 { SPR_PLAS, 0, 0, {A_PlasmaAnimate/*ACTION_48856*/}, S_774 },
/*S_774*/		 { SPR_PLAS, 0, 1, {A_Raise/*ACTION_47748*/}, S_774 },
/*S_775*/		 { SPR_PLAS, 32772, 2, {A_FirePlasma/*ACTION_48940*/}, S_776 },
/*S_776*/		 { SPR_PLAS, 0, 2, {A_PlasmaAnimate/*ACTION_48856*/}, S_777 },
/*S_777*/		 { SPR_PLAS, 0, 1, {A_ReFire/*ACTION_47388*/}, S_771 },
/*S_778*/		 { SPR_PLAS, 1, 2, {NULL}, S_779 },
/*S_779*/		 { SPR_PLAS, 2, 2, {NULL}, S_780 },
/*S_780*/		 { SPR_PLAS, 3, 2, {NULL}, S_778 },
/*S_781*/		 { SPR_BFGG, 0, 1, {A_WeaponReady/*ACTION_47164*/}, S_781 },
/*S_782*/		 { SPR_BFGG, 0, 1, {A_Lower/*ACTION_47552*/}, S_782 },
/*S_783*/		 { SPR_BFGG, 0, 1, {A_Raise/*ACTION_47748*/}, S_783 },
/*S_784*/		 { SPR_BFGG, 0, 20, {A_BFGsound/*ACTION_50840*/}, S_785 },
/*S_785*/		 { SPR_BFGG, 1, 10, {A_GunFlash/*ACTION_47832*/}, S_786 },
/*S_786*/		 { SPR_BFGG, 1, 10, {A_FireBFG/*ACTION_48760*/}, S_787 },
/*S_787*/		 { SPR_BFGG, 1, 20, {A_ReFire/*ACTION_47388*/}, S_781 },
/*S_788*/		 { SPR_BFGG, 32770, 8, {NULL}, S_789 },
/*S_789*/		 { SPR_BFGG, 32771, 5, {NULL}, S_790 },
/*S_790*/		 { SPR_BFGG, 32772, 3, {NULL}, S_000 },
/*S_791*/		 { SPR_LASR, 0, 2, {A_WeaponReady/*ACTION_47164*/}, S_791 },
/*S_792*/		 { SPR_LASR, 0, 1, {A_Lower/*ACTION_47552*/}, S_792 },
/*S_793*/		 { SPR_LASR, 0, 1, {A_Raise/*ACTION_47748*/}, S_793 },
/*S_794*/		 { SPR_LASR, 0, 8, {A_FireLaser/*ACTION_51904*/}, S_795 },
/*S_795*/		 { SPR_LASR, 0, 3, {A_ReFire/*ACTION_47388*/}, S_791 },
/*S_796*/		 { SPR_LASR, 32769, 3, {NULL}, S_000 },
};

mobjinfo_t mobjinfo[NUMMOBJTYPES] =		//0x51E38
{

	{		/*MT_PLAYER*/
		-1,		//doomednum
		S_001,		//spawnstate
		100,		//spawnhealth
		S_002,		//seestate
		sfx_None/*sfx_000*/,		//seesound
		0,		//reactiontime
		sfx_None/*sfx_000*/,		//attacksound
		S_008,		//painstate
		255,		//painchance
		sfx_plrpain,		//painsound
		S_000,		//meleestate
		S_006,		//missilestate
		S_010,		//deathstate
		S_016,		//xdeathstate
		sfx_plrdie,		//deathsound
		0,		//speed
		19*FRACUNIT,		//radius
		64*FRACUNIT,		//height
		100,		//mass
		0,		//damage
		sfx_None/*sfx_000*/,		//activesound
		MF_SOLID|MF_SHOOTABLE|MF_GRAVITY|MF_DROPOFF|MF_PICKUP|MF_NOTDMATCH,		//flags
		0,		//palette
		255		//alpha
	},

	{		/*MT_PLAYERBOT1*/
		3008,		//doomednum
		S_025,		//spawnstate
		100,		//spawnhealth
		S_026,		//seestate
		sfx_None/*sfx_000*/,		//seesound
		0,		//reactiontime
		sfx_None/*sfx_000*/,		//attacksound
		S_036,		//painstate
		255,		//painchance
		sfx_plrpain,		//painsound
		S_000,		//meleestate
		S_030,		//missilestate
		S_016,		//deathstate
		S_016,		//xdeathstate
		sfx_plrdie,		//deathsound
		16,		//speed
		32*FRACUNIT,		//radius
		87*FRACUNIT,		//height
		100,		//mass
		0,		//damage
		sfx_None/*sfx_000*/,		//activesound
		MF_SOLID|MF_SHOOTABLE|MF_GRAVITY|MF_DROPOFF|MF_COUNTKILL|MF_NOTDMATCH,		//flags
		1,		//palette
		255		//alpha
	},

	{		/*MT_PLAYERBOT2*/
		3009,		//doomednum
		S_025,		//spawnstate
		100,		//spawnhealth
		S_026,		//seestate
		sfx_None/*sfx_000*/,		//seesound
		0,		//reactiontime
		sfx_None/*sfx_000*/,		//attacksound
		S_036,		//painstate
		255,		//painchance
		sfx_plrpain,		//painsound
		S_000,		//meleestate
		S_038,		//missilestate
		S_010,		//deathstate
		S_016,		//xdeathstate
		sfx_plrdie,		//deathsound
		16,		//speed
		32*FRACUNIT,		//radius
		87*FRACUNIT,		//height
		100,		//mass
		0,		//damage
		sfx_None/*sfx_000*/,		//activesound
		MF_SOLID|MF_SHOOTABLE|MF_GRAVITY|MF_DROPOFF|MF_COUNTKILL|MF_NOTDMATCH,		//flags
		2,		//palette
		255		//alpha
	},

	{		/*MT_PLAYERBOT3*/
		3010,		//doomednum
		S_025,		//spawnstate
		100,		//spawnhealth
		S_026,		//seestate
		sfx_None/*sfx_000*/,		//seesound
		0,		//reactiontime
		sfx_None/*sfx_000*/,		//attacksound
		S_036,		//painstate
		255,		//painchance
		sfx_plrpain,		//painsound
		S_000,		//meleestate
		S_030,		//missilestate
		S_016,		//deathstate
		S_016,		//xdeathstate
		sfx_plrdie,		//deathsound
		0,		//speed
		32*FRACUNIT,		//radius
		87*FRACUNIT,		//height
		100,		//mass
		0,		//damage
		sfx_None/*sfx_000*/,		//activesound
		MF_SOLID|MF_SHOOTABLE|MF_GRAVITY|MF_DROPOFF|MF_COUNTKILL|MF_NOTDMATCH,		//flags
		0,		//palette
		255		//alpha
	},

	{		/*MT_DEMON1*/
		3002,		//doomednum
		S_044,		//spawnstate
		150,		//spawnhealth
		S_046,		//seestate
		sfx_sargsit,		//seesound
		8,		//reactiontime
		sfx_sargatk,		//attacksound
		S_057,		//painstate
		180,		//painchance
		sfx_dbpain2,		//painsound
		S_054,		//meleestate
		S_000,		//missilestate
		S_059,		//deathstate
		S_000,		//xdeathstate
		sfx_sargdie,		//deathsound
		12,		//speed
		44*FRACUNIT,		//radius
		100*FRACUNIT,		//height
		400,		//mass
		0,		//damage
		sfx_dbact,		//activesound
		MF_SOLID|MF_SHOOTABLE|MF_GRAVITY|MF_COUNTKILL,		//flags
		0,		//palette
		255		//alpha
	},

	{		/*MT_DEMON2*/
		58,		//doomednum
		S_044,		//spawnstate
		150,		//spawnhealth
		S_065,		//seestate
		sfx_sargsit,		//seesound
		8,		//reactiontime
		sfx_sargatk,		//attacksound
		S_067,		//painstate
		180,		//painchance
		sfx_dbpain2,		//painsound
		S_066,		//meleestate
		S_000,		//missilestate
		S_068,		//deathstate
		S_000,		//xdeathstate
		sfx_sargdie,		//deathsound
		12,		//speed
		50*FRACUNIT,		//radius
		100*FRACUNIT,		//height
		400,		//mass
		0,		//damage
		sfx_dbact,		//activesound
		MF_SOLID|MF_SHOOTABLE|MF_GRAVITY|MF_COUNTKILL,		//flags
		1,		//palette
		255		//alpha
	},

	{		/*MT_MANCUBUS*/
		67,		//doomednum
		S_069,		//spawnstate
		600,		//spawnhealth
		S_071,		//seestate
		sfx_fattsit,		//seesound
		8,		//reactiontime
		sfx_None/*sfx_000*/,		//attacksound
		S_090,		//painstate
		80,		//painchance
		sfx_fatthit,		//painsound
		S_000,		//meleestate
		S_083,		//missilestate
		S_092,		//deathstate
		S_000,		//xdeathstate
		sfx_fattdie,		//deathsound
		8,		//speed
		60*FRACUNIT,		//radius
		108*FRACUNIT,		//height
		1000,		//mass
		0,		//damage
		sfx_posact,		//activesound
		MF_SOLID|MF_SHOOTABLE|MF_GRAVITY|MF_COUNTKILL,		//flags
		0,		//palette
		255		//alpha
	},

	{		/*MT_POSSESSED1*/
		3004,		//doomednum
		S_098,		//spawnstate
		20,		//spawnhealth
		S_100,		//seestate
		sfx_possit1,		//seesound
		8,		//reactiontime
		sfx_pistol,		//attacksound
		S_111,		//painstate
		200,		//painchance
		sfx_dbpain1,		//painsound
		S_000,		//meleestate
		S_108,		//missilestate
		S_113,		//deathstate
		S_118,		//xdeathstate
		sfx_posdie1,		//deathsound
		8,		//speed
		32*FRACUNIT,		//radius
		87*FRACUNIT,		//height
		100,		//mass
		0,		//damage
		sfx_posact,		//activesound
		MF_SOLID|MF_SHOOTABLE|MF_GRAVITY|MF_COUNTKILL,		//flags
		0,		//palette
		255		//alpha
	},

	{		/*MT_POSSESSED2*/
		9,		//doomednum
		S_127,		//spawnstate
		30,		//spawnhealth
		S_129,		//seestate
		sfx_possit2,		//seesound
		8,		//reactiontime
		sfx_None/*sfx_000*/,		//attacksound
		S_140,		//painstate
		170,		//painchance
		sfx_dbpain1,		//painsound
		S_000,		//meleestate
		S_137,		//missilestate
		S_142,		//deathstate
		S_147,		//xdeathstate
		sfx_posdie2,		//deathsound
		8,		//speed
		32*FRACUNIT,		//radius
		87*FRACUNIT,		//height
		100,		//mass
		0,		//damage
		sfx_posact,		//activesound
		MF_SOLID|MF_SHOOTABLE|MF_GRAVITY|MF_COUNTKILL,		//flags
		1,		//palette
		255		//alpha
	},

	{		/*MT_IMP1*/
		3001,		//doomednum
		S_156,		//spawnstate
		60,		//spawnhealth
		S_158,		//seestate
		sfx_impsit1,		//seesound
		8,		//reactiontime
		sfx_None/*sfx_000*/,		//attacksound
		S_172,		//painstate
		200,		//painchance
		sfx_dbpain1,		//painsound
		S_166,		//meleestate
		S_169,		//missilestate
		S_174,		//deathstate
		S_179,		//xdeathstate
		sfx_impdth1,		//deathsound
		8,		//speed
		42*FRACUNIT,		//radius
		94*FRACUNIT,		//height
		100,		//mass
		0,		//damage
		sfx_impact,		//activesound
		MF_SOLID|MF_SHOOTABLE|MF_GRAVITY|MF_COUNTKILL,		//flags
		0,		//palette
		255		//alpha
	},

	{		/*MT_IMP2*/
		3007,		//doomednum
		S_156,		//spawnstate
		60,		//spawnhealth
		S_158,		//seestate
		sfx_impsit1,		//seesound
		8,		//reactiontime
		sfx_None/*sfx_000*/,		//attacksound
		S_172,		//painstate
		128,		//painchance
		sfx_dbpain1,		//painsound
		S_166,		//meleestate
		S_169,		//missilestate
		S_174,		//deathstate
		S_179,		//xdeathstate
		sfx_impdth1,		//deathsound
		16,		//speed
		42*FRACUNIT,		//radius
		94*FRACUNIT,		//height
		100,		//mass
		0,		//damage
		sfx_impact,		//activesound
		MF_SOLID|MF_SHOOTABLE|MF_GRAVITY|MF_COUNTKILL|MF_SHADOW,		//flags
		1,		//palette
		180		//alpha
	},

	{		/*MT_CACODEMON*/
		3005,		//doomednum
		S_187,		//spawnstate
		400,		//spawnhealth
		S_191,		//seestate
		sfx_headsit,		//seesound
		8,		//reactiontime
		sfx_None/*sfx_000*/,		//attacksound
		S_203,		//painstate
		128,		//painchance
		sfx_dbpain2,		//painsound
		S_000,		//meleestate
		S_199,		//missilestate
		S_206,		//deathstate
		S_000,		//xdeathstate
		sfx_headdie,		//deathsound
		8,		//speed
		55*FRACUNIT,		//radius
		90*FRACUNIT,		//height
		400,		//mass
		0,		//damage
		sfx_dbact,		//activesound
		MF_SOLID|MF_SHOOTABLE|MF_FLOAT|MF_COUNTKILL,		//flags
		0,		//palette
		255		//alpha
	},

	{		/*MT_BRUISER1*/
		3003,		//doomednum
		S_212,		//spawnstate
		1000,		//spawnhealth
		S_214,		//seestate
		sfx_bos1sit,		//seesound
		8,		//reactiontime
		sfx_None/*sfx_000*/,		//attacksound
		S_225,		//painstate
		50,		//painchance
		sfx_dbpain2,		//painsound
		S_222,		//meleestate
		S_222,		//missilestate
		S_227,		//deathstate
		S_000,		//xdeathstate
		sfx_bos1die,		//deathsound
		8,		//speed
		24*FRACUNIT,		//radius
		100*FRACUNIT,		//height
		1000,		//mass
		0,		//damage
		sfx_dbact,		//activesound
		MF_SOLID|MF_SHOOTABLE|MF_GRAVITY|MF_COUNTKILL,		//flags
		1,		//palette
		255		//alpha
	},

	{		/*MT_BRUISER2*/
		69,		//doomednum
		S_233,		//spawnstate
		500,		//spawnhealth
		S_235,		//seestate
		sfx_bos2sit,		//seesound
		8,		//reactiontime
		sfx_None/*sfx_000*/,		//attacksound
		S_246,		//painstate
		50,		//painchance
		sfx_dbpain2,		//painsound
		S_243,		//meleestate
		S_243,		//missilestate
		S_248,		//deathstate
		S_000,		//xdeathstate
		sfx_bos2die,		//deathsound
		8,		//speed
		24*FRACUNIT,		//radius
		100*FRACUNIT,		//height
		1000,		//mass
		0,		//damage
		sfx_dbact,		//activesound
		MF_SOLID|MF_SHOOTABLE|MF_GRAVITY|MF_COUNTKILL,		//flags
		0,		//palette
		255		//alpha
	},

	{		/*MT_SKULL*/
		3006,		//doomednum
		S_254,		//spawnstate
		60,		//spawnhealth
		S_257,		//seestate
		sfx_None/*sfx_000*/,		//seesound
		8,		//reactiontime
		sfx_skullatk,		//attacksound
		S_264,		//painstate
		256,		//painchance
		sfx_dbpain2,		//painsound
		S_000,		//meleestate
		S_260,		//missilestate
		S_266,		//deathstate
		S_000,		//xdeathstate
		sfx_implod,		//deathsound
		8,		//speed
		28*FRACUNIT,		//radius
		64*FRACUNIT,		//height
		50,		//mass
		3,		//damage
		sfx_dbact,		//activesound
		MF_SOLID|MF_SHOOTABLE|MF_FLOAT|MF_COUNTKILL,		//flags
		0,		//palette
		192		//alpha
	},

	{		/*MT_BABY*/
		68,		//doomednum
		S_276,		//spawnstate
		500,		//spawnhealth
		S_278,		//seestate
		sfx_bspisit,		//seesound
		8,		//reactiontime
		sfx_None/*sfx_000*/,		//attacksound
		S_290,		//painstate
		128,		//painchance
		sfx_dbpain2,		//painsound
		S_000,		//meleestate
		S_287,		//missilestate
		S_292,		//deathstate
		S_000,		//xdeathstate
		sfx_bspidie,		//deathsound
		12,		//speed
		64*FRACUNIT,		//radius
		80*FRACUNIT,		//height
		600,		//mass
		0,		//damage
		sfx_bspilift,		//activesound
		MF_SOLID|MF_SHOOTABLE|MF_GRAVITY|MF_COUNTKILL,		//flags
		0,		//palette
		255		//alpha
	},

	{		/*MT_CYBORG*/
		16,		//doomednum
		S_298,		//spawnstate
		4000,		//spawnhealth
		S_299,		//seestate
		sfx_cybsit,		//seesound
		8,		//reactiontime
		sfx_None/*sfx_000*/,		//attacksound
		S_313,		//painstate
		20,		//painchance
		sfx_dbpain2,		//painsound
		S_000,		//meleestate
		S_307,		//missilestate
		S_314,		//deathstate
		S_000,		//xdeathstate
		sfx_cybdth,		//deathsound
		16,		//speed
		70*FRACUNIT,		//radius
		170*FRACUNIT,		//height
		1000,		//mass
		0,		//damage
		sfx_dbact,		//activesound
		MF_SOLID|MF_SHOOTABLE|MF_GRAVITY|MF_COUNTKILL,		//flags
		0,		//palette
		255		//alpha
	},

	{		/*MT_CYBORG_TITLE*/
		3014,		//doomednum
		S_323,		//spawnstate
		4000,		//spawnhealth
		S_000,		//seestate
		sfx_None/*sfx_000*/,		//seesound
		8,		//reactiontime
		sfx_None/*sfx_000*/,		//attacksound
		S_000,		//painstate
		0,		//painchance
		sfx_None/*sfx_000*/,		//painsound
		S_000,		//meleestate
		S_324,		//missilestate
		S_000,		//deathstate
		S_000,		//xdeathstate
		sfx_None/*sfx_000*/,		//deathsound
		0,		//speed
		40*FRACUNIT,		//radius
		110*FRACUNIT,		//height
		1000,		//mass
		0,		//damage
		sfx_dbact,		//activesound
		MF_SOLID|MF_SHOOTABLE|MF_GRAVITY|MF_COUNTKILL,		//flags
		0,		//palette
		255		//alpha
	},

	{		/*MT_PAIN*/
		71,		//doomednum
		S_326,		//spawnstate
		400,		//spawnhealth
		S_327,		//seestate
		sfx_pesit,		//seesound
		8,		//reactiontime
		sfx_None/*sfx_000*/,		//attacksound
		S_332,		//painstate
		128,		//painchance
		sfx_pepain,		//painsound
		S_000,		//meleestate
		S_328,		//missilestate
		S_334,		//deathstate
		S_000,		//xdeathstate
		sfx_pedie,		//deathsound
		8,		//speed
		60*FRACUNIT,		//radius
		112*FRACUNIT,		//height
		400,		//mass
		0,		//damage
		sfx_dbact,		//activesound
		MF_SOLID|MF_SHOOTABLE|MF_FLOAT|MF_COUNTKILL,		//flags
		0,		//palette
		255		//alpha
	},

	{		/*MT_RESURRECTOR*/
		3013,		//doomednum
		S_342,		//spawnstate
		5000,		//spawnhealth
		S_346,		//seestate
		sfx_rectsit,		//seesound
		8,		//reactiontime
		sfx_rectatk,		//attacksound
		S_362,		//painstate
		50,		//painchance
		sfx_rectpain,		//painsound
		S_358,		//meleestate
		S_000,		//missilestate
		S_363,		//deathstate
		S_000,		//xdeathstate
		sfx_rectdie,		//deathsound
		30,		//speed
		80*FRACUNIT,		//radius
		150*FRACUNIT,		//height
		1000,		//mass
		0,		//damage
		sfx_rectact,		//activesound
		MF_SOLID|MF_SHOOTABLE|MF_GRAVITY|MF_DROPOFF|MF_COUNTKILL,		//flags
		0,		//palette
		255		//alpha
	},

	{		/*MT_CAMERA*/
		0,		//doomednum
		S_000,		//spawnstate
		1000,		//spawnhealth
		S_000,		//seestate
		sfx_None/*sfx_000*/,		//seesound
		8,		//reactiontime
		sfx_None/*sfx_000*/,		//attacksound
		S_000,		//painstate
		0,		//painchance
		sfx_None/*sfx_000*/,		//painsound
		S_000,		//meleestate
		S_000,		//missilestate
		S_000,		//deathstate
		S_000,		//xdeathstate
		sfx_None/*sfx_000*/,		//deathsound
		0,		//speed
		20*FRACUNIT,		//radius
		16*FRACUNIT,		//height
		100,		//mass
		0,		//damage
		sfx_None/*sfx_000*/,		//activesound
		MF_NOSECTOR|MF_NOBLOCKMAP,		//flags
		0,		//palette
		255		//alpha
	},

	{		/*MT_DEST_TELEPORT*/
		14,		//doomednum
		S_000,		//spawnstate
		1000,		//spawnhealth
		S_000,		//seestate
		sfx_None/*sfx_000*/,		//seesound
		8,		//reactiontime
		sfx_None/*sfx_000*/,		//attacksound
		S_000,		//painstate
		0,		//painchance
		sfx_None/*sfx_000*/,		//painsound
		S_000,		//meleestate
		S_000,		//missilestate
		S_000,		//deathstate
		S_000,		//xdeathstate
		sfx_None/*sfx_000*/,		//deathsound
		0,		//speed
		20*FRACUNIT,		//radius
		16*FRACUNIT,		//height
		100,		//mass
		0,		//damage
		sfx_None/*sfx_000*/,		//activesound
		MF_NOSECTOR|MF_NOBLOCKMAP,		//flags
		0,		//palette
		255		//alpha
	},

	{		/*MT_DEST_PROJECTILE*/
		2050,		//doomednum
		S_000,		//spawnstate
		1000,		//spawnhealth
		S_000,		//seestate
		sfx_None/*sfx_000*/,		//seesound
		8,		//reactiontime
		sfx_None/*sfx_000*/,		//attacksound
		S_000,		//painstate
		0,		//painchance
		sfx_None/*sfx_000*/,		//painsound
		S_000,		//meleestate
		S_000,		//missilestate
		S_000,		//deathstate
		S_000,		//xdeathstate
		sfx_None/*sfx_000*/,		//deathsound
		0,		//speed
		20*FRACUNIT,		//radius
		16*FRACUNIT,		//height
		100,		//mass
		0,		//damage
		sfx_None/*sfx_000*/,		//activesound
		MF_NOSECTOR|MF_NOBLOCKMAP,		//flags
		0,		//palette
		255		//alpha
	},

	{		/*MT_FAKEITEM*/
		89,		//doomednum
		S_370,		//spawnstate
		1000,		//spawnhealth
		S_000,		//seestate
		sfx_None/*sfx_000*/,		//seesound
		8,		//reactiontime
		sfx_None/*sfx_000*/,		//attacksound
		S_000,		//painstate
		0,		//painchance
		sfx_None/*sfx_000*/,		//painsound
		S_000,		//meleestate
		S_000,		//missilestate
		S_000,		//deathstate
		S_000,		//xdeathstate
		sfx_None/*sfx_000*/,		//deathsound
		0,		//speed
		32*FRACUNIT,		//radius
		16*FRACUNIT,		//height
		100,		//mass
		0,		//damage
		sfx_None/*sfx_000*/,		//activesound
		MF_SPECIAL|MF_NOSECTOR,		//flags
		0,		//palette
		255		//alpha
	},

	{		/*MT_LASERMARKER*/
		90,		//doomednum
		S_000,		//spawnstate
		1000,		//spawnhealth
		S_000,		//seestate
		sfx_None/*sfx_000*/,		//seesound
		8,		//reactiontime
		sfx_None/*sfx_000*/,		//attacksound
		S_000,		//painstate
		0,		//painchance
		sfx_None/*sfx_000*/,		//painsound
		S_000,		//meleestate
		S_000,		//missilestate
		S_000,		//deathstate
		S_000,		//xdeathstate
		sfx_None/*sfx_000*/,		//deathsound
		0,		//speed
		20*FRACUNIT,		//radius
		16*FRACUNIT,		//height
		100,		//mass
		0,		//damage
		sfx_None/*sfx_000*/,		//activesound
		MF_NOBLOCKMAP|MF_RENDERLASER,		//flags
		0,		//palette
		255		//alpha
	},

	{		/*MT_PROJ_ROCKET*/
		-1,		//doomednum
		S_371,		//spawnstate
		1000,		//spawnhealth
		S_000,		//seestate
		sfx_missile,		//seesound
		8,		//reactiontime
		sfx_None/*sfx_000*/,		//attacksound
		S_000,		//painstate
		0,		//painchance
		sfx_None/*sfx_000*/,		//painsound
		S_000,		//meleestate
		S_000,		//missilestate
		S_372,		//deathstate
		S_000,		//xdeathstate
		sfx_explode,		//deathsound
		30*FRACUNIT,		//speed
		11*FRACUNIT,		//radius
		8*FRACUNIT,		//height
		100,		//mass
		20,		//damage
		sfx_None/*sfx_000*/,		//activesound
		MF_NOBLOCKMAP|MF_DROPOFF|MF_MISSILE,		//flags
		0,		//palette
		255		//alpha
	},

	{		/*MT_PROJ_PLASMA*/
		-1,		//doomednum
		S_379,		//spawnstate
		1000,		//spawnhealth
		S_000,		//seestate
		sfx_plasma,		//seesound
		8,		//reactiontime
		sfx_None/*sfx_000*/,		//attacksound
		S_000,		//painstate
		0,		//painchance
		sfx_None/*sfx_000*/,		//painsound
		S_000,		//meleestate
		S_000,		//missilestate
		S_381,		//deathstate
		S_000,		//xdeathstate
		sfx_implod,		//deathsound
		40*FRACUNIT,		//speed
		13*FRACUNIT,		//radius
		8*FRACUNIT,		//height
		100,		//mass
		5,		//damage
		sfx_None/*sfx_000*/,		//activesound
		MF_NOBLOCKMAP|MF_DROPOFF|MF_MISSILE,		//flags
		0,		//palette
		255		//alpha
	},

	{		/*MT_PROJ_BFG*/
		-1,		//doomednum
		S_387,		//spawnstate
		1000,		//spawnhealth
		S_000,		//seestate
		sfx_None/*sfx_000*/,		//seesound
		8,		//reactiontime
		sfx_None/*sfx_000*/,		//attacksound
		S_000,		//painstate
		0,		//painchance
		sfx_None/*sfx_000*/,		//painsound
		S_000,		//meleestate
		S_000,		//missilestate
		S_389,		//deathstate
		S_000,		//xdeathstate
		sfx_bfgexp,		//deathsound
		40*FRACUNIT,		//speed
		13*FRACUNIT,		//radius
		8*FRACUNIT,		//height
		100,		//mass
		100,		//damage
		sfx_None/*sfx_000*/,		//activesound
		MF_NOBLOCKMAP|MF_DROPOFF|MF_MISSILE,		//flags
		0,		//palette
		255		//alpha
	},

	{		/*MT_PROJ_LASER*/
		-1,		//doomednum
		S_395,		//spawnstate
		1000,		//spawnhealth
		S_000,		//seestate
		sfx_None/*sfx_000*/,		//seesound
		8,		//reactiontime
		sfx_None/*sfx_000*/,		//attacksound
		S_000,		//painstate
		0,		//painchance
		sfx_None/*sfx_000*/,		//painsound
		S_000,		//meleestate
		S_000,		//missilestate
		S_000,		//deathstate
		S_000,		//xdeathstate
		sfx_None/*sfx_000*/,		//deathsound
		0,		//speed
		8*FRACUNIT,		//radius
		8*FRACUNIT,		//height
		100,		//mass
		0,		//damage
		sfx_None/*sfx_000*/,		//activesound
		MF_NOBLOCKMAP,		//flags
		0,		//palette
		255		//alpha
	},

	{		/*MT_PROJ_IMP1*/
		-1,		//doomednum
		S_397,		//spawnstate
		1000,		//spawnhealth
		S_000,		//seestate
		sfx_bdmissile,		//seesound
		8,		//reactiontime
		sfx_None/*sfx_000*/,		//attacksound
		S_000,		//painstate
		0,		//painchance
		sfx_None/*sfx_000*/,		//painsound
		S_000,		//meleestate
		S_000,		//missilestate
		S_400,		//deathstate
		S_000,		//xdeathstate
		sfx_implod,		//deathsound
		10*FRACUNIT,		//speed
		6*FRACUNIT,		//radius
		8*FRACUNIT,		//height
		100,		//mass
		3,		//damage
		sfx_None/*sfx_000*/,		//activesound
		MF_NOBLOCKMAP|MF_DROPOFF|MF_MISSILE,		//flags
		0,		//palette
		255		//alpha
	},

	{		/*MT_PROJ_IMP2*/
		-1,		//doomednum
		S_406,		//spawnstate
		1000,		//spawnhealth
		S_000,		//seestate
		sfx_bdmissile,		//seesound
		8,		//reactiontime
		sfx_None/*sfx_000*/,		//attacksound
		S_000,		//painstate
		0,		//painchance
		sfx_None/*sfx_000*/,		//painsound
		S_000,		//meleestate
		S_000,		//missilestate
		S_409,		//deathstate
		S_000,		//xdeathstate
		sfx_implod,		//deathsound
		20*FRACUNIT,		//speed
		6*FRACUNIT,		//radius
		8*FRACUNIT,		//height
		100,		//mass
		3,		//damage
		sfx_None/*sfx_000*/,		//activesound
		MF_NOBLOCKMAP|MF_DROPOFF|MF_MISSILE,		//flags
		0,		//palette
		100		//alpha
	},

	{		/*MT_PROJ_HEAD*/
		-1,		//doomednum
		S_415,		//spawnstate
		1000,		//spawnhealth
		S_000,		//seestate
		sfx_bdmissile,		//seesound
		8,		//reactiontime
		sfx_None/*sfx_000*/,		//attacksound
		S_000,		//painstate
		0,		//painchance
		sfx_None/*sfx_000*/,		//painsound
		S_000,		//meleestate
		S_000,		//missilestate
		S_418,		//deathstate
		S_000,		//xdeathstate
		sfx_implod,		//deathsound
		20*FRACUNIT,		//speed
		6*FRACUNIT,		//radius
		8*FRACUNIT,		//height
		100,		//mass
		5,		//damage
		sfx_None/*sfx_000*/,		//activesound
		MF_NOBLOCKMAP|MF_DROPOFF|MF_MISSILE,		//flags
		0,		//palette
		255		//alpha
	},

	{		/*MT_PROJ_BRUISER1*/
		-1,		//doomednum
		S_423,		//spawnstate
		1000,		//spawnhealth
		S_000,		//seestate
		sfx_bdmissile,		//seesound
		8,		//reactiontime
		sfx_None/*sfx_000*/,		//attacksound
		S_000,		//painstate
		0,		//painchance
		sfx_None/*sfx_000*/,		//painsound
		S_000,		//meleestate
		S_000,		//missilestate
		S_425,		//deathstate
		S_000,		//xdeathstate
		sfx_implod,		//deathsound
		15*FRACUNIT,		//speed
		6*FRACUNIT,		//radius
		8*FRACUNIT,		//height
		100,		//mass
		8,		//damage
		sfx_None/*sfx_000*/,		//activesound
		MF_NOBLOCKMAP|MF_DROPOFF|MF_MISSILE,		//flags
		0,		//palette
		255		//alpha
	},

	{		/*MT_PROJ_BRUISER2*/
		-1,		//doomednum
		S_431,		//spawnstate
		1000,		//spawnhealth
		S_000,		//seestate
		sfx_bdmissile,		//seesound
		8,		//reactiontime
		sfx_None/*sfx_000*/,		//attacksound
		S_000,		//painstate
		0,		//painchance
		sfx_None/*sfx_000*/,		//painsound
		S_000,		//meleestate
		S_000,		//missilestate
		S_433,		//deathstate
		S_000,		//xdeathstate
		sfx_implod,		//deathsound
		15*FRACUNIT,		//speed
		6*FRACUNIT,		//radius
		8*FRACUNIT,		//height
		100,		//mass
		8,		//damage
		sfx_None/*sfx_000*/,		//activesound
		MF_NOBLOCKMAP|MF_DROPOFF|MF_MISSILE,		//flags
		0,		//palette
		255		//alpha
	},

	{		/*MT_PROJ_BABY*/
		-1,		//doomednum
		S_439,		//spawnstate
		1000,		//spawnhealth
		S_000,		//seestate
		sfx_plasma,		//seesound
		8,		//reactiontime
		sfx_None/*sfx_000*/,		//attacksound
		S_000,		//painstate
		0,		//painchance
		sfx_None/*sfx_000*/,		//painsound
		S_000,		//meleestate
		S_000,		//missilestate
		S_441,		//deathstate
		S_000,		//xdeathstate
		sfx_implod,		//deathsound
		25*FRACUNIT,		//speed
		13*FRACUNIT,		//radius
		8*FRACUNIT,		//height
		100,		//mass
		3,		//damage
		sfx_None/*sfx_000*/,		//activesound
		MF_NOBLOCKMAP|MF_DROPOFF|MF_MISSILE,		//flags
		0,		//palette
		255		//alpha
	},

	{		/*MT_PROJ_FATSO*/
		-1,		//doomednum
		S_447,		//spawnstate
		1000,		//spawnhealth
		S_000,		//seestate
		sfx_bdmissile,		//seesound
		8,		//reactiontime
		sfx_None/*sfx_000*/,		//attacksound
		S_000,		//painstate
		0,		//painchance
		sfx_None/*sfx_000*/,		//painsound
		S_000,		//meleestate
		S_000,		//missilestate
		S_450,		//deathstate
		S_000,		//xdeathstate
		sfx_implod,		//deathsound
		20*FRACUNIT,		//speed
		6*FRACUNIT,		//radius
		8*FRACUNIT,		//height
		100,		//mass
		8,		//damage
		sfx_None/*sfx_000*/,		//activesound
		MF_NOBLOCKMAP|MF_DROPOFF|MF_MISSILE,		//flags
		0,		//palette
		255		//alpha
	},

	{		/*MT_PROJ_TRACER*/
		-1,		//doomednum
		S_456,		//spawnstate
		1000,		//spawnhealth
		S_000,		//seestate
		sfx_tracer,		//seesound
		8,		//reactiontime
		sfx_None/*sfx_000*/,		//attacksound
		S_000,		//painstate
		0,		//painchance
		sfx_None/*sfx_000*/,		//painsound
		S_000,		//meleestate
		S_000,		//missilestate
		S_458,		//deathstate
		S_000,		//xdeathstate
		sfx_explode,		//deathsound
		10*FRACUNIT,		//speed
		11*FRACUNIT,		//radius
		8*FRACUNIT,		//height
		100,		//mass
		10,		//damage
		sfx_None/*sfx_000*/,		//activesound
		MF_NOBLOCKMAP|MF_DROPOFF|MF_MISSILE,		//flags
		0,		//palette
		255		//alpha
	},

	{		/*MT_PROJ_DART*/
		-1,		//doomednum
		S_465,		//spawnstate
		1000,		//spawnhealth
		S_000,		//seestate
		sfx_dart,		//seesound
		8,		//reactiontime
		sfx_None/*sfx_000*/,		//attacksound
		S_000,		//painstate
		0,		//painchance
		sfx_None/*sfx_000*/,		//painsound
		S_000,		//meleestate
		S_000,		//missilestate
		S_000,		//deathstate
		S_000,		//xdeathstate
		sfx_None/*sfx_000*/,		//deathsound
		16*FRACUNIT,		//speed
		13*FRACUNIT,		//radius
		8*FRACUNIT,		//height
		100,		//mass
		4,		//damage
		sfx_None/*sfx_000*/,		//activesound
		MF_NOBLOCKMAP|MF_DROPOFF|MF_MISSILE,		//flags
		0,		//palette
		255		//alpha
	},

	{		/*MT_PROJ_RECTFIRE*/
		-1,		//doomednum
		S_466,		//spawnstate
		1000,		//spawnhealth
		S_000,		//seestate
		sfx_tracer,		//seesound
		8,		//reactiontime
		sfx_None/*sfx_000*/,		//attacksound
		S_000,		//painstate
		0,		//painchance
		sfx_None/*sfx_000*/,		//painsound
		S_000,		//meleestate
		S_000,		//missilestate
		S_000,		//deathstate
		S_000,		//xdeathstate
		sfx_None/*sfx_000*/,		//deathsound
		20*FRACUNIT,		//speed
		16*FRACUNIT,		//radius
		64*FRACUNIT,		//height
		100,		//mass
		5,		//damage
		sfx_None/*sfx_000*/,		//activesound
		MF_NOBLOCKMAP|MF_GRAVITY|MF_DROPOFF|MF_MISSILE|MF_SHADOW,		//flags
		0,		//palette
		180		//alpha
	},

	{		/*MT_PROJ_RECT*/
		-1,		//doomednum
		S_470,		//spawnstate
		1000,		//spawnhealth
		S_000,		//seestate
		sfx_tracer,		//seesound
		8,		//reactiontime
		sfx_None/*sfx_000*/,		//attacksound
		S_000,		//painstate
		0,		//painchance
		sfx_None/*sfx_000*/,		//painsound
		S_000,		//meleestate
		S_000,		//missilestate
		S_472,		//deathstate
		S_000,		//xdeathstate
		sfx_explode,		//deathsound
		18*FRACUNIT,		//speed
		11*FRACUNIT,		//radius
		8*FRACUNIT,		//height
		100,		//mass
		10,		//damage
		sfx_None/*sfx_000*/,		//activesound
		MF_NOBLOCKMAP|MF_DROPOFF|MF_MISSILE,		//flags
		0,		//palette
		255		//alpha
	},

	{		/*MT_SMOKE_GRAY*/
		-1,		//doomednum
		S_478,		//spawnstate
		1000,		//spawnhealth
		S_000,		//seestate
		sfx_None/*sfx_000*/,		//seesound
		8,		//reactiontime
		sfx_None/*sfx_000*/,		//attacksound
		S_000,		//painstate
		0,		//painchance
		sfx_None/*sfx_000*/,		//painsound
		S_000,		//meleestate
		S_000,		//missilestate
		S_000,		//deathstate
		S_000,		//xdeathstate
		sfx_None/*sfx_000*/,		//deathsound
		0,		//speed
		20*FRACUNIT,		//radius
		16*FRACUNIT,		//height
		100,		//mass
		0,		//damage
		sfx_None/*sfx_000*/,		//activesound
		MF_NOBLOCKMAP,		//flags
		0,		//palette
		120		//alpha
	},

	{		/*MT_SMOKE_RED*/
		-1,		//doomednum
		S_483,		//spawnstate
		1000,		//spawnhealth
		S_000,		//seestate
		sfx_None/*sfx_000*/,		//seesound
		8,		//reactiontime
		sfx_None/*sfx_000*/,		//attacksound
		S_000,		//painstate
		0,		//painchance
		sfx_None/*sfx_000*/,		//painsound
		S_000,		//meleestate
		S_000,		//missilestate
		S_000,		//deathstate
		S_000,		//xdeathstate
		sfx_None/*sfx_000*/,		//deathsound
		0,		//speed
		20*FRACUNIT,		//radius
		16*FRACUNIT,		//height
		100,		//mass
		0,		//damage
		sfx_None/*sfx_000*/,		//activesound
		MF_NOBLOCKMAP,		//flags
		0,		//palette
		120		//alpha
	},

	{		/*MT_SMOKE_SMALL*/
		-1,		//doomednum
		S_488,		//spawnstate
		1000,		//spawnhealth
		S_000,		//seestate
		sfx_None/*sfx_000*/,		//seesound
		8,		//reactiontime
		sfx_None/*sfx_000*/,		//attacksound
		S_000,		//painstate
		0,		//painchance
		sfx_None/*sfx_000*/,		//painsound
		S_000,		//meleestate
		S_000,		//missilestate
		S_000,		//deathstate
		S_000,		//xdeathstate
		sfx_None/*sfx_000*/,		//deathsound
		0,		//speed
		20*FRACUNIT,		//radius
		16*FRACUNIT,		//height
		100,		//mass
		0,		//damage
		sfx_None/*sfx_000*/,		//activesound
		MF_NOBLOCKMAP|MF_SHADOW,		//flags
		0,		//palette
		255		//alpha
	},

	{		/*MT_BLOOD*/
		-1,		//doomednum
		S_494,		//spawnstate
		1000,		//spawnhealth
		S_000,		//seestate
		sfx_None/*sfx_000*/,		//seesound
		8,		//reactiontime
		sfx_None/*sfx_000*/,		//attacksound
		S_000,		//painstate
		0,		//painchance
		sfx_None/*sfx_000*/,		//painsound
		S_000,		//meleestate
		S_000,		//missilestate
		S_000,		//deathstate
		S_000,		//xdeathstate
		sfx_None/*sfx_000*/,		//deathsound
		0,		//speed
		20*FRACUNIT,		//radius
		16*FRACUNIT,		//height
		100,		//mass
		0,		//damage
		sfx_None/*sfx_000*/,		//activesound
		MF_NOBLOCKMAP|MF_GRAVITY,		//flags
		0,		//palette
		255		//alpha
	},

	{		/*MT_GIB_CRUSHED*/
		24,		//doomednum
		S_498,		//spawnstate
		1000,		//spawnhealth
		S_000,		//seestate
		sfx_None/*sfx_000*/,		//seesound
		8,		//reactiontime
		sfx_None/*sfx_000*/,		//attacksound
		S_000,		//painstate
		0,		//painchance
		sfx_None/*sfx_000*/,		//painsound
		S_000,		//meleestate
		S_000,		//missilestate
		S_000,		//deathstate
		S_000,		//xdeathstate
		sfx_None/*sfx_000*/,		//deathsound
		0,		//speed
		20*FRACUNIT,		//radius
		16*FRACUNIT,		//height
		100,		//mass
		0,		//damage
		sfx_None/*sfx_000*/,		//activesound
		0, 		//flags
		0,		//palette
		255		//alpha
	},

	{		/*MT_TELEPORTFOG*/
		-1,		//doomednum
		S_499,		//spawnstate
		1000,		//spawnhealth
		S_000,		//seestate
		sfx_None/*sfx_000*/,		//seesound
		8,		//reactiontime
		sfx_None/*sfx_000*/,		//attacksound
		S_000,		//painstate
		0,		//painchance
		sfx_None/*sfx_000*/,		//painsound
		S_000,		//meleestate
		S_000,		//missilestate
		S_000,		//deathstate
		S_000,		//xdeathstate
		sfx_None/*sfx_000*/,		//deathsound
		0,		//speed
		20*FRACUNIT,		//radius
		16*FRACUNIT,		//height
		100,		//mass
		0,		//damage
		sfx_None/*sfx_000*/,		//activesound
		MF_NOBLOCKMAP|MF_SHADOW,		//flags
		0,		//palette
		140		//alpha
	},

	{		/*MT_BFGSPREAD*/
		-1,		//doomednum
		S_511,		//spawnstate
		1000,		//spawnhealth
		S_000,		//seestate
		sfx_None/*sfx_000*/,		//seesound
		8,		//reactiontime
		sfx_None/*sfx_000*/,		//attacksound
		S_000,		//painstate
		0,		//painchance
		sfx_None/*sfx_000*/,		//painsound
		S_000,		//meleestate
		S_000,		//missilestate
		S_000,		//deathstate
		S_000,		//xdeathstate
		sfx_None/*sfx_000*/,		//deathsound
		0,		//speed
		20*FRACUNIT,		//radius
		16*FRACUNIT,		//height
		100,		//mass
		0,		//damage
		sfx_None/*sfx_000*/,		//activesound
		MF_NOBLOCKMAP,		//flags
		0,		//palette
		255		//alpha
	},

	{		/*MT_ITEM_ARMOR1*/
		2018,		//doomednum
		S_517,		//spawnstate
		1000,		//spawnhealth
		S_000,		//seestate
		sfx_None/*sfx_000*/,		//seesound
		8,		//reactiontime
		sfx_None/*sfx_000*/,		//attacksound
		S_000,		//painstate
		0,		//painchance
		sfx_None/*sfx_000*/,		//painsound
		S_000,		//meleestate
		S_000,		//missilestate
		S_000,		//deathstate
		S_000,		//xdeathstate
		sfx_None/*sfx_000*/,		//deathsound
		0,		//speed
		20*FRACUNIT,		//radius
		16*FRACUNIT,		//height
		100,		//mass
		0,		//damage
		sfx_None/*sfx_000*/,		//activesound
		MF_SPECIAL,		//flags
		0,		//palette
		255		//alpha
	},

	{		/*MT_ITEM_ARMOR2*/
		2019,		//doomednum
		S_519,		//spawnstate
		1000,		//spawnhealth
		S_000,		//seestate
		sfx_None/*sfx_000*/,		//seesound
		8,		//reactiontime
		sfx_None/*sfx_000*/,		//attacksound
		S_000,		//painstate
		0,		//painchance
		sfx_None/*sfx_000*/,		//painsound
		S_000,		//meleestate
		S_000,		//missilestate
		S_000,		//deathstate
		S_000,		//xdeathstate
		sfx_None/*sfx_000*/,		//deathsound
		0,		//speed
		20*FRACUNIT,		//radius
		16*FRACUNIT,		//height
		100,		//mass
		0,		//damage
		sfx_None/*sfx_000*/,		//activesound
		MF_SPECIAL,		//flags
		0,		//palette
		255		//alpha
	},

	{		/*MT_ITEM_BONUSHEALTH*/
		2014,		//doomednum
		S_521,		//spawnstate
		1000,		//spawnhealth
		S_000,		//seestate
		sfx_None/*sfx_000*/,		//seesound
		8,		//reactiontime
		sfx_None/*sfx_000*/,		//attacksound
		S_000,		//painstate
		0,		//painchance
		sfx_None/*sfx_000*/,		//painsound
		S_000,		//meleestate
		S_000,		//missilestate
		S_000,		//deathstate
		S_000,		//xdeathstate
		sfx_None/*sfx_000*/,		//deathsound
		0,		//speed
		20*FRACUNIT,		//radius
		16*FRACUNIT,		//height
		100,		//mass
		0,		//damage
		sfx_None/*sfx_000*/,		//activesound
		MF_SPECIAL|MF_COUNTITEM,		//flags
		0,		//palette
		255		//alpha
	},

	{		/*MT_ITEM_BONUSARMOR*/
		2015,		//doomednum
		S_525,		//spawnstate
		1000,		//spawnhealth
		S_000,		//seestate
		sfx_None/*sfx_000*/,		//seesound
		8,		//reactiontime
		sfx_None/*sfx_000*/,		//attacksound
		S_000,		//painstate
		0,		//painchance
		sfx_None/*sfx_000*/,		//painsound
		S_000,		//meleestate
		S_000,		//missilestate
		S_000,		//deathstate
		S_000,		//xdeathstate
		sfx_None/*sfx_000*/,		//deathsound
		0,		//speed
		20*FRACUNIT,		//radius
		16*FRACUNIT,		//height
		100,		//mass
		0,		//damage
		sfx_None/*sfx_000*/,		//activesound
		MF_SPECIAL|MF_COUNTITEM,		//flags
		0,		//palette
		255		//alpha
	},

	{		/*MT_ITEM_BLUECARDKEY*/
		5,		//doomednum
		S_531,		//spawnstate
		1000,		//spawnhealth
		S_000,		//seestate
		sfx_None/*sfx_000*/,		//seesound
		8,		//reactiontime
		sfx_None/*sfx_000*/,		//attacksound
		S_000,		//painstate
		0,		//painchance
		sfx_None/*sfx_000*/,		//painsound
		S_000,		//meleestate
		S_000,		//missilestate
		S_000,		//deathstate
		S_000,		//xdeathstate
		sfx_None/*sfx_000*/,		//deathsound
		0,		//speed
		20*FRACUNIT,		//radius
		16*FRACUNIT,		//height
		100,		//mass
		0,		//damage
		sfx_None/*sfx_000*/,		//activesound
		MF_SPECIAL|MF_NOTDMATCH,		//flags
		0,		//palette
		255		//alpha
	},

	{		/*MT_ITEM_REDCARDKEY*/
		13,		//doomednum
		S_533,		//spawnstate
		1000,		//spawnhealth
		S_000,		//seestate
		sfx_None/*sfx_000*/,		//seesound
		8,		//reactiontime
		sfx_None/*sfx_000*/,		//attacksound
		S_000,		//painstate
		0,		//painchance
		sfx_None/*sfx_000*/,		//painsound
		S_000,		//meleestate
		S_000,		//missilestate
		S_000,		//deathstate
		S_000,		//xdeathstate
		sfx_None/*sfx_000*/,		//deathsound
		0,		//speed
		20*FRACUNIT,		//radius
		16*FRACUNIT,		//height
		100,		//mass
		0,		//damage
		sfx_None/*sfx_000*/,		//activesound
		MF_SPECIAL|MF_NOTDMATCH,		//flags
		0,		//palette
		255		//alpha
	},

	{		/*MT_ITEM_YELLOWCARDKEY*/
		6,		//doomednum
		S_535,		//spawnstate
		1000,		//spawnhealth
		S_000,		//seestate
		sfx_None/*sfx_000*/,		//seesound
		8,		//reactiontime
		sfx_None/*sfx_000*/,		//attacksound
		S_000,		//painstate
		0,		//painchance
		sfx_None/*sfx_000*/,		//painsound
		S_000,		//meleestate
		S_000,		//missilestate
		S_000,		//deathstate
		S_000,		//xdeathstate
		sfx_None/*sfx_000*/,		//deathsound
		0,		//speed
		20*FRACUNIT,		//radius
		16*FRACUNIT,		//height
		100,		//mass
		0,		//damage
		sfx_None/*sfx_000*/,		//activesound
		MF_SPECIAL|MF_NOTDMATCH,		//flags
		0,		//palette
		255		//alpha
	},

	{		/*MT_ITEM_YELLOWSKULLKEY*/
		39,		//doomednum
		S_537,		//spawnstate
		1000,		//spawnhealth
		S_000,		//seestate
		sfx_None/*sfx_000*/,		//seesound
		8,		//reactiontime
		sfx_None/*sfx_000*/,		//attacksound
		S_000,		//painstate
		0,		//painchance
		sfx_None/*sfx_000*/,		//painsound
		S_000,		//meleestate
		S_000,		//missilestate
		S_000,		//deathstate
		S_000,		//xdeathstate
		sfx_None/*sfx_000*/,		//deathsound
		0,		//speed
		20*FRACUNIT,		//radius
		16*FRACUNIT,		//height
		100,		//mass
		0,		//damage
		sfx_None/*sfx_000*/,		//activesound
		MF_SPECIAL|MF_NOTDMATCH,		//flags
		0,		//palette
		255		//alpha
	},

	{		/*MT_ITEM_REDSKULLKEY*/
		38,		//doomednum
		S_539,		//spawnstate
		1000,		//spawnhealth
		S_000,		//seestate
		sfx_None/*sfx_000*/,		//seesound
		8,		//reactiontime
		sfx_None/*sfx_000*/,		//attacksound
		S_000,		//painstate
		0,		//painchance
		sfx_None/*sfx_000*/,		//painsound
		S_000,		//meleestate
		S_000,		//missilestate
		S_000,		//deathstate
		S_000,		//xdeathstate
		sfx_None/*sfx_000*/,		//deathsound
		0,		//speed
		20*FRACUNIT,		//radius
		16*FRACUNIT,		//height
		100,		//mass
		0,		//damage
		sfx_None/*sfx_000*/,		//activesound
		MF_SPECIAL|MF_NOTDMATCH,		//flags
		0,		//palette
		255		//alpha
	},

	{		/*MT_ITEM_BLUESKULLKEY*/
		40,		//doomednum
		S_541,		//spawnstate
		1000,		//spawnhealth
		S_000,		//seestate
		sfx_None/*sfx_000*/,		//seesound
		8,		//reactiontime
		sfx_None/*sfx_000*/,		//attacksound
		S_000,		//painstate
		0,		//painchance
		sfx_None/*sfx_000*/,		//painsound
		S_000,		//meleestate
		S_000,		//missilestate
		S_000,		//deathstate
		S_000,		//xdeathstate
		sfx_None/*sfx_000*/,		//deathsound
		0,		//speed
		20*FRACUNIT,		//radius
		16*FRACUNIT,		//height
		100,		//mass
		0,		//damage
		sfx_None/*sfx_000*/,		//activesound
		MF_SPECIAL|MF_NOTDMATCH,		//flags
		0,		//palette
		255		//alpha
	},

	{		/*MT_ITEM_ARTIFACT1*/
		1042,		//doomednum
		S_543,		//spawnstate
		1000,		//spawnhealth
		S_000,		//seestate
		sfx_None/*sfx_000*/,		//seesound
		8,		//reactiontime
		sfx_None/*sfx_000*/,		//attacksound
		S_000,		//painstate
		0,		//painchance
		sfx_None/*sfx_000*/,		//painsound
		S_000,		//meleestate
		S_000,		//missilestate
		S_000,		//deathstate
		S_000,		//xdeathstate
		sfx_None/*sfx_000*/,		//deathsound
		0,		//speed
		20*FRACUNIT,		//radius
		16*FRACUNIT,		//height
		100,		//mass
		0,		//damage
		sfx_None/*sfx_000*/,		//activesound
		MF_SPECIAL,		//flags
		0,		//palette
		255		//alpha
	},

	{		/*MT_ITEM_ARTIFACT2*/
		1043,		//doomednum
		S_551,		//spawnstate
		1000,		//spawnhealth
		S_000,		//seestate
		sfx_None/*sfx_000*/,		//seesound
		8,		//reactiontime
		sfx_None/*sfx_000*/,		//attacksound
		S_000,		//painstate
		0,		//painchance
		sfx_None/*sfx_000*/,		//painsound
		S_000,		//meleestate
		S_000,		//missilestate
		S_000,		//deathstate
		S_000,		//xdeathstate
		sfx_None/*sfx_000*/,		//deathsound
		0,		//speed
		20*FRACUNIT,		//radius
		16*FRACUNIT,		//height
		100,		//mass
		0,		//damage
		sfx_None/*sfx_000*/,		//activesound
		MF_SPECIAL,		//flags
		0,		//palette
		255		//alpha
	},

	{		/*MT_ITEM_ARTIFACT3*/
		1044,		//doomednum
		S_559,		//spawnstate
		1000,		//spawnhealth
		S_000,		//seestate
		sfx_None/*sfx_000*/,		//seesound
		8,		//reactiontime
		sfx_None/*sfx_000*/,		//attacksound
		S_000,		//painstate
		0,		//painchance
		sfx_None/*sfx_000*/,		//painsound
		S_000,		//meleestate
		S_000,		//missilestate
		S_000,		//deathstate
		S_000,		//xdeathstate
		sfx_None/*sfx_000*/,		//deathsound
		0,		//speed
		20*FRACUNIT,		//radius
		16*FRACUNIT,		//height
		100,		//mass
		0,		//damage
		sfx_None/*sfx_000*/,		//activesound
		MF_SPECIAL,		//flags
		0,		//palette
		255		//alpha
	},

	{		/*MT_ITEM_STIMPACK*/
		2011,		//doomednum
		S_567,		//spawnstate
		1000,		//spawnhealth
		S_000,		//seestate
		sfx_None/*sfx_000*/,		//seesound
		8,		//reactiontime
		sfx_None/*sfx_000*/,		//attacksound
		S_000,		//painstate
		0,		//painchance
		sfx_None/*sfx_000*/,		//painsound
		S_000,		//meleestate
		S_000,		//missilestate
		S_000,		//deathstate
		S_000,		//xdeathstate
		sfx_None/*sfx_000*/,		//deathsound
		0,		//speed
		20*FRACUNIT,		//radius
		16*FRACUNIT,		//height
		100,		//mass
		0,		//damage
		sfx_None/*sfx_000*/,		//activesound
		MF_SPECIAL,		//flags
		0,		//palette
		255		//alpha
	},

	{		/*MT_ITEM_MEDKIT*/
		2012,		//doomednum
		S_568,		//spawnstate
		1000,		//spawnhealth
		S_000,		//seestate
		sfx_None/*sfx_000*/,		//seesound
		8,		//reactiontime
		sfx_None/*sfx_000*/,		//attacksound
		S_000,		//painstate
		0,		//painchance
		sfx_None/*sfx_000*/,		//painsound
		S_000,		//meleestate
		S_000,		//missilestate
		S_000,		//deathstate
		S_000,		//xdeathstate
		sfx_None/*sfx_000*/,		//deathsound
		0,		//speed
		20*FRACUNIT,		//radius
		16*FRACUNIT,		//height
		100,		//mass
		0,		//damage
		sfx_None/*sfx_000*/,		//activesound
		MF_SPECIAL,		//flags
		0,		//palette
		255		//alpha
	},

	{		/*MT_ITEM_SOULSPHERE*/
		2013,		//doomednum
		S_569,		//spawnstate
		1000,		//spawnhealth
		S_000,		//seestate
		sfx_None/*sfx_000*/,		//seesound
		8,		//reactiontime
		sfx_None/*sfx_000*/,		//attacksound
		S_000,		//painstate
		0,		//painchance
		sfx_None/*sfx_000*/,		//painsound
		S_000,		//meleestate
		S_000,		//missilestate
		S_000,		//deathstate
		S_000,		//xdeathstate
		sfx_None/*sfx_000*/,		//deathsound
		0,		//speed
		20*FRACUNIT,		//radius
		16*FRACUNIT,		//height
		100,		//mass
		0,		//damage
		sfx_None/*sfx_000*/,		//activesound
		MF_SPECIAL|MF_COUNTITEM,		//flags
		0,		//palette
		255		//alpha
	},

	{		/*MT_ITEM_INVULSPHERE*/
		2022,		//doomednum
		S_575,		//spawnstate
		1000,		//spawnhealth
		S_000,		//seestate
		sfx_None/*sfx_000*/,		//seesound
		8,		//reactiontime
		sfx_None/*sfx_000*/,		//attacksound
		S_000,		//painstate
		0,		//painchance
		sfx_None/*sfx_000*/,		//painsound
		S_000,		//meleestate
		S_000,		//missilestate
		S_000,		//deathstate
		S_000,		//xdeathstate
		sfx_None/*sfx_000*/,		//deathsound
		0,		//speed
		20*FRACUNIT,		//radius
		16*FRACUNIT,		//height
		100,		//mass
		0,		//damage
		sfx_None/*sfx_000*/,		//activesound
		MF_SPECIAL|MF_COUNTITEM,		//flags
		0,		//palette
		255		//alpha
	},

	{		/*MT_ITEM_BERSERK*/
		2023,		//doomednum
		S_581,		//spawnstate
		1000,		//spawnhealth
		S_000,		//seestate
		sfx_None/*sfx_000*/,		//seesound
		8,		//reactiontime
		sfx_None/*sfx_000*/,		//attacksound
		S_000,		//painstate
		0,		//painchance
		sfx_None/*sfx_000*/,		//painsound
		S_000,		//meleestate
		S_000,		//missilestate
		S_000,		//deathstate
		S_000,		//xdeathstate
		sfx_None/*sfx_000*/,		//deathsound
		0,		//speed
		20*FRACUNIT,		//radius
		16*FRACUNIT,		//height
		100,		//mass
		0,		//damage
		sfx_None/*sfx_000*/,		//activesound
		MF_SPECIAL|MF_COUNTITEM,		//flags
		0,		//palette
		255		//alpha
	},

	{		/*MT_ITEM_INVISSPHERE*/
		2024,		//doomednum
		S_582,		//spawnstate
		1000,		//spawnhealth
		S_000,		//seestate
		sfx_None/*sfx_000*/,		//seesound
		8,		//reactiontime
		sfx_None/*sfx_000*/,		//attacksound
		S_000,		//painstate
		0,		//painchance
		sfx_None/*sfx_000*/,		//painsound
		S_000,		//meleestate
		S_000,		//missilestate
		S_000,		//deathstate
		S_000,		//xdeathstate
		sfx_None/*sfx_000*/,		//deathsound
		0,		//speed
		20*FRACUNIT,		//radius
		16*FRACUNIT,		//height
		100,		//mass
		0,		//damage
		sfx_None/*sfx_000*/,		//activesound
		MF_SPECIAL|MF_COUNTITEM|MF_SHADOW,		//flags
		0,		//palette
		176		//alpha
	},

	{		/*MT_ITEM_RADSPHERE*/
		2025,		//doomednum
		S_588,		//spawnstate
		1000,		//spawnhealth
		S_000,		//seestate
		sfx_None/*sfx_000*/,		//seesound
		8,		//reactiontime
		sfx_None/*sfx_000*/,		//attacksound
		S_000,		//painstate
		0,		//painchance
		sfx_None/*sfx_000*/,		//painsound
		S_000,		//meleestate
		S_000,		//missilestate
		S_000,		//deathstate
		S_000,		//xdeathstate
		sfx_None/*sfx_000*/,		//deathsound
		0,		//speed
		20*FRACUNIT,		//radius
		16*FRACUNIT,		//height
		100,		//mass
		0,		//damage
		sfx_None/*sfx_000*/,		//activesound
		MF_SPECIAL,		//flags
		0,		//palette
		255		//alpha
	},

	{		/*MT_ITEM_AUTOMAP*/
		2026,		//doomednum
		S_590,		//spawnstate
		1000,		//spawnhealth
		S_000,		//seestate
		sfx_None/*sfx_000*/,		//seesound
		8,		//reactiontime
		sfx_None/*sfx_000*/,		//attacksound
		S_000,		//painstate
		0,		//painchance
		sfx_None/*sfx_000*/,		//painsound
		S_000,		//meleestate
		S_000,		//missilestate
		S_000,		//deathstate
		S_000,		//xdeathstate
		sfx_None/*sfx_000*/,		//deathsound
		0,		//speed
		20*FRACUNIT,		//radius
		16*FRACUNIT,		//height
		100,		//mass
		0,		//damage
		sfx_None/*sfx_000*/,		//activesound
		MF_SPECIAL|MF_COUNTITEM,		//flags
		0,		//palette
		255		//alpha
	},

	{		/*MT_ITEM_PVIS*/
		2045,		//doomednum
		S_594,		//spawnstate
		1000,		//spawnhealth
		S_000,		//seestate
		sfx_None/*sfx_000*/,		//seesound
		8,		//reactiontime
		sfx_None/*sfx_000*/,		//attacksound
		S_000,		//painstate
		0,		//painchance
		sfx_None/*sfx_000*/,		//painsound
		S_000,		//meleestate
		S_000,		//missilestate
		S_000,		//deathstate
		S_000,		//xdeathstate
		sfx_None/*sfx_000*/,		//deathsound
		0,		//speed
		20*FRACUNIT,		//radius
		16*FRACUNIT,		//height
		100,		//mass
		0,		//damage
		sfx_None/*sfx_000*/,		//activesound
		MF_SPECIAL|MF_COUNTITEM,		//flags
		0,		//palette
		255		//alpha
	},

	{		/*MT_ITEM_MEGASPHERE*/
		83,		//doomednum
		S_596,		//spawnstate
		1000,		//spawnhealth
		S_000,		//seestate
		sfx_None/*sfx_000*/,		//seesound
		8,		//reactiontime
		sfx_None/*sfx_000*/,		//attacksound
		S_000,		//painstate
		0,		//painchance
		sfx_None/*sfx_000*/,		//painsound
		S_000,		//meleestate
		S_000,		//missilestate
		S_000,		//deathstate
		S_000,		//xdeathstate
		sfx_None/*sfx_000*/,		//deathsound
		0,		//speed
		20*FRACUNIT,		//radius
		16*FRACUNIT,		//height
		100,		//mass
		0,		//damage
		sfx_None/*sfx_000*/,		//activesound
		MF_SPECIAL|MF_COUNTITEM,		//flags
		0,		//palette
		255		//alpha
	},

	{		/*MT_AMMO_CLIP*/
		2007,		//doomednum
		S_602,		//spawnstate
		1000,		//spawnhealth
		S_000,		//seestate
		sfx_None/*sfx_000*/,		//seesound
		8,		//reactiontime
		sfx_None/*sfx_000*/,		//attacksound
		S_000,		//painstate
		0,		//painchance
		sfx_None/*sfx_000*/,		//painsound
		S_000,		//meleestate
		S_000,		//missilestate
		S_000,		//deathstate
		S_000,		//xdeathstate
		sfx_None/*sfx_000*/,		//deathsound
		0,		//speed
		20*FRACUNIT,		//radius
		16*FRACUNIT,		//height
		100,		//mass
		0,		//damage
		sfx_None/*sfx_000*/,		//activesound
		MF_SPECIAL,		//flags
		0,		//palette
		255		//alpha
	},

	{		/*MT_AMMO_CLIPBOX*/
		2048,		//doomednum
		S_603,		//spawnstate
		1000,		//spawnhealth
		S_000,		//seestate
		sfx_None/*sfx_000*/,		//seesound
		8,		//reactiontime
		sfx_None/*sfx_000*/,		//attacksound
		S_000,		//painstate
		0,		//painchance
		sfx_None/*sfx_000*/,		//painsound
		S_000,		//meleestate
		S_000,		//missilestate
		S_000,		//deathstate
		S_000,		//xdeathstate
		sfx_None/*sfx_000*/,		//deathsound
		0,		//speed
		20*FRACUNIT,		//radius
		16*FRACUNIT,		//height
		100,		//mass
		0,		//damage
		sfx_None/*sfx_000*/,		//activesound
		MF_SPECIAL,		//flags
		0,		//palette
		255		//alpha
	},

	{		/*MT_AMMO_ROCKET*/
		2010,		//doomednum
		S_604,		//spawnstate
		1000,		//spawnhealth
		S_000,		//seestate
		sfx_None/*sfx_000*/,		//seesound
		8,		//reactiontime
		sfx_None/*sfx_000*/,		//attacksound
		S_000,		//painstate
		0,		//painchance
		sfx_None/*sfx_000*/,		//painsound
		S_000,		//meleestate
		S_000,		//missilestate
		S_000,		//deathstate
		S_000,		//xdeathstate
		sfx_None/*sfx_000*/,		//deathsound
		0,		//speed
		20*FRACUNIT,		//radius
		16*FRACUNIT,		//height
		100,		//mass
		0,		//damage
		sfx_None/*sfx_000*/,		//activesound
		MF_SPECIAL,		//flags
		0,		//palette
		255		//alpha
	},

	{		/*MT_AMMO_ROCKETBOX*/
		2046,		//doomednum
		S_605,		//spawnstate
		1000,		//spawnhealth
		S_000,		//seestate
		sfx_None/*sfx_000*/,		//seesound
		8,		//reactiontime
		sfx_None/*sfx_000*/,		//attacksound
		S_000,		//painstate
		0,		//painchance
		sfx_None/*sfx_000*/,		//painsound
		S_000,		//meleestate
		S_000,		//missilestate
		S_000,		//deathstate
		S_000,		//xdeathstate
		sfx_None/*sfx_000*/,		//deathsound
		0,		//speed
		20*FRACUNIT,		//radius
		16*FRACUNIT,		//height
		100,		//mass
		0,		//damage
		sfx_None/*sfx_000*/,		//activesound
		MF_SPECIAL,		//flags
		0,		//palette
		255		//alpha
	},

	{		/*MT_AMMO_CELL*/
		2047,		//doomednum
		S_606,		//spawnstate
		1000,		//spawnhealth
		S_000,		//seestate
		sfx_None/*sfx_000*/,		//seesound
		8,		//reactiontime
		sfx_None/*sfx_000*/,		//attacksound
		S_000,		//painstate
		0,		//painchance
		sfx_None/*sfx_000*/,		//painsound
		S_000,		//meleestate
		S_000,		//missilestate
		S_000,		//deathstate
		S_000,		//xdeathstate
		sfx_None/*sfx_000*/,		//deathsound
		0,		//speed
		20*FRACUNIT,		//radius
		16*FRACUNIT,		//height
		100,		//mass
		0,		//damage
		sfx_None/*sfx_000*/,		//activesound
		MF_SPECIAL,		//flags
		0,		//palette
		255		//alpha
	},

	{		/*MT_AMMO_CELLPACK*/
		17,		//doomednum
		S_607,		//spawnstate
		1000,		//spawnhealth
		S_000,		//seestate
		sfx_None/*sfx_000*/,		//seesound
		8,		//reactiontime
		sfx_None/*sfx_000*/,		//attacksound
		S_000,		//painstate
		0,		//painchance
		sfx_None/*sfx_000*/,		//painsound
		S_000,		//meleestate
		S_000,		//missilestate
		S_000,		//deathstate
		S_000,		//xdeathstate
		sfx_None/*sfx_000*/,		//deathsound
		0,		//speed
		20*FRACUNIT,		//radius
		16*FRACUNIT,		//height
		100,		//mass
		0,		//damage
		sfx_None/*sfx_000*/,		//activesound
		MF_SPECIAL,		//flags
		0,		//palette
		255		//alpha
	},

	{		/*MT_AMMO_SHELL*/
		2008,		//doomednum
		S_608,		//spawnstate
		1000,		//spawnhealth
		S_000,		//seestate
		sfx_None/*sfx_000*/,		//seesound
		8,		//reactiontime
		sfx_None/*sfx_000*/,		//attacksound
		S_000,		//painstate
		0,		//painchance
		sfx_None/*sfx_000*/,		//painsound
		S_000,		//meleestate
		S_000,		//missilestate
		S_000,		//deathstate
		S_000,		//xdeathstate
		sfx_None/*sfx_000*/,		//deathsound
		0,		//speed
		20*FRACUNIT,		//radius
		16*FRACUNIT,		//height
		100,		//mass
		0,		//damage
		sfx_None/*sfx_000*/,		//activesound
		MF_SPECIAL,		//flags
		0,		//palette
		255		//alpha
	},

	{		/*MT_AMMO_SHELLBOX*/
		2049,		//doomednum
		S_609,		//spawnstate
		1000,		//spawnhealth
		S_000,		//seestate
		sfx_None/*sfx_000*/,		//seesound
		8,		//reactiontime
		sfx_None/*sfx_000*/,		//attacksound
		S_000,		//painstate
		0,		//painchance
		sfx_None/*sfx_000*/,		//painsound
		S_000,		//meleestate
		S_000,		//missilestate
		S_000,		//deathstate
		S_000,		//xdeathstate
		sfx_None/*sfx_000*/,		//deathsound
		0,		//speed
		20*FRACUNIT,		//radius
		16*FRACUNIT,		//height
		100,		//mass
		0,		//damage
		sfx_None/*sfx_000*/,		//activesound
		MF_SPECIAL,		//flags
		0,		//palette
		255		//alpha
	},

	{		/*MT_AMMO_BACKPACK*/
		8,		//doomednum
		S_610,		//spawnstate
		1000,		//spawnhealth
		S_000,		//seestate
		sfx_None/*sfx_000*/,		//seesound
		8,		//reactiontime
		sfx_None/*sfx_000*/,		//attacksound
		S_000,		//painstate
		0,		//painchance
		sfx_None/*sfx_000*/,		//painsound
		S_000,		//meleestate
		S_000,		//missilestate
		S_000,		//deathstate
		S_000,		//xdeathstate
		sfx_None/*sfx_000*/,		//deathsound
		0,		//speed
		20*FRACUNIT,		//radius
		16*FRACUNIT,		//height
		100,		//mass
		0,		//damage
		sfx_None/*sfx_000*/,		//activesound
		MF_SPECIAL,		//flags
		0,		//palette
		255		//alpha
	},

	{		/*MT_WEAP_BFG*/
		2006,		//doomednum
		S_611,		//spawnstate
		1000,		//spawnhealth
		S_000,		//seestate
		sfx_None/*sfx_000*/,		//seesound
		8,		//reactiontime
		sfx_None/*sfx_000*/,		//attacksound
		S_000,		//painstate
		0,		//painchance
		sfx_None/*sfx_000*/,		//painsound
		S_000,		//meleestate
		S_000,		//missilestate
		S_000,		//deathstate
		S_000,		//xdeathstate
		sfx_None/*sfx_000*/,		//deathsound
		0,		//speed
		20*FRACUNIT,		//radius
		16*FRACUNIT,		//height
		100,		//mass
		0,		//damage
		sfx_None/*sfx_000*/,		//activesound
		MF_SPECIAL,		//flags
		0,		//palette
		255		//alpha
	},

	{		/*MT_WEAP_CHAINSAW*/
		2005,		//doomednum
		S_612,		//spawnstate
		1000,		//spawnhealth
		S_000,		//seestate
		sfx_None/*sfx_000*/,		//seesound
		8,		//reactiontime
		sfx_None/*sfx_000*/,		//attacksound
		S_000,		//painstate
		0,		//painchance
		sfx_None/*sfx_000*/,		//painsound
		S_000,		//meleestate
		S_000,		//missilestate
		S_000,		//deathstate
		S_000,		//xdeathstate
		sfx_None/*sfx_000*/,		//deathsound
		0,		//speed
		20*FRACUNIT,		//radius
		16*FRACUNIT,		//height
		100,		//mass
		0,		//damage
		sfx_None/*sfx_000*/,		//activesound
		MF_SPECIAL,		//flags
		0,		//palette
		255		//alpha
	},

	{		/*MT_WEAP_CHAINGUN*/
		2002,		//doomednum
		S_613,		//spawnstate
		1000,		//spawnhealth
		S_000,		//seestate
		sfx_None/*sfx_000*/,		//seesound
		8,		//reactiontime
		sfx_None/*sfx_000*/,		//attacksound
		S_000,		//painstate
		0,		//painchance
		sfx_None/*sfx_000*/,		//painsound
		S_000,		//meleestate
		S_000,		//missilestate
		S_000,		//deathstate
		S_000,		//xdeathstate
		sfx_None/*sfx_000*/,		//deathsound
		0,		//speed
		20*FRACUNIT,		//radius
		16*FRACUNIT,		//height
		100,		//mass
		0,		//damage
		sfx_None/*sfx_000*/,		//activesound
		MF_SPECIAL,		//flags
		0,		//palette
		255		//alpha
	},

	{		/*MT_WEAP_LAUNCHER*/
		2003,		//doomednum
		S_614,		//spawnstate
		1000,		//spawnhealth
		S_000,		//seestate
		sfx_None/*sfx_000*/,		//seesound
		8,		//reactiontime
		sfx_None/*sfx_000*/,		//attacksound
		S_000,		//painstate
		0,		//painchance
		sfx_None/*sfx_000*/,		//painsound
		S_000,		//meleestate
		S_000,		//missilestate
		S_000,		//deathstate
		S_000,		//xdeathstate
		sfx_None/*sfx_000*/,		//deathsound
		0,		//speed
		20*FRACUNIT,		//radius
		16*FRACUNIT,		//height
		100,		//mass
		0,		//damage
		sfx_None/*sfx_000*/,		//activesound
		MF_SPECIAL,		//flags
		0,		//palette
		255		//alpha
	},

	{		/*MT_WEAP_PLASMA*/
		2004,		//doomednum
		S_615,		//spawnstate
		1000,		//spawnhealth
		S_000,		//seestate
		sfx_None/*sfx_000*/,		//seesound
		8,		//reactiontime
		sfx_None/*sfx_000*/,		//attacksound
		S_000,		//painstate
		0,		//painchance
		sfx_None/*sfx_000*/,		//painsound
		S_000,		//meleestate
		S_000,		//missilestate
		S_000,		//deathstate
		S_000,		//xdeathstate
		sfx_None/*sfx_000*/,		//deathsound
		0,		//speed
		20*FRACUNIT,		//radius
		16*FRACUNIT,		//height
		100,		//mass
		0,		//damage
		sfx_None/*sfx_000*/,		//activesound
		MF_SPECIAL,		//flags
		0,		//palette
		255		//alpha
	},

	{		/*MT_WEAP_SHOTGUN*/
		2001,		//doomednum
		S_616,		//spawnstate
		1000,		//spawnhealth
		S_000,		//seestate
		sfx_None/*sfx_000*/,		//seesound
		8,		//reactiontime
		sfx_None/*sfx_000*/,		//attacksound
		S_000,		//painstate
		0,		//painchance
		sfx_None/*sfx_000*/,		//painsound
		S_000,		//meleestate
		S_000,		//missilestate
		S_000,		//deathstate
		S_000,		//xdeathstate
		sfx_None/*sfx_000*/,		//deathsound
		0,		//speed
		20*FRACUNIT,		//radius
		16*FRACUNIT,		//height
		100,		//mass
		0,		//damage
		sfx_None/*sfx_000*/,		//activesound
		MF_SPECIAL,		//flags
		0,		//palette
		255		//alpha
	},

	{		/*MT_WEAP_SSHOTGUN*/
		82,		//doomednum
		S_617,		//spawnstate
		1000,		//spawnhealth
		S_000,		//seestate
		sfx_None/*sfx_000*/,		//seesound
		8,		//reactiontime
		sfx_None/*sfx_000*/,		//attacksound
		S_000,		//painstate
		0,		//painchance
		sfx_None/*sfx_000*/,		//painsound
		S_000,		//meleestate
		S_000,		//missilestate
		S_000,		//deathstate
		S_000,		//xdeathstate
		sfx_None/*sfx_000*/,		//deathsound
		0,		//speed
		20*FRACUNIT,		//radius
		16*FRACUNIT,		//height
		100,		//mass
		0,		//damage
		sfx_None/*sfx_000*/,		//activesound
		MF_SPECIAL,		//flags
		0,		//palette
		255		//alpha
	},

	{		/*MT_WEAP_LCARBINE*/
		84,		//doomednum
		S_618,		//spawnstate
		1000,		//spawnhealth
		S_000,		//seestate
		sfx_None/*sfx_000*/,		//seesound
		8,		//reactiontime
		sfx_None/*sfx_000*/,		//attacksound
		S_000,		//painstate
		0,		//painchance
		sfx_None/*sfx_000*/,		//painsound
		S_000,		//meleestate
		S_000,		//missilestate
		S_000,		//deathstate
		S_000,		//xdeathstate
		sfx_None/*sfx_000*/,		//deathsound
		0,		//speed
		20*FRACUNIT,		//radius
		16*FRACUNIT,		//height
		100,		//mass
		0,		//damage
		sfx_None/*sfx_000*/,		//activesound
		MF_SPECIAL,		//flags
		0,		//palette
		255		//alpha
	},

	{		/*MT_PROP_FIRE*/
		2051,		//doomednum
		S_619,		//spawnstate
		1000,		//spawnhealth
		S_000,		//seestate
		sfx_None/*sfx_000*/,		//seesound
		8,		//reactiontime
		sfx_None/*sfx_000*/,		//attacksound
		S_000,		//painstate
		0,		//painchance
		sfx_None/*sfx_000*/,		//painsound
		S_000,		//meleestate
		S_000,		//missilestate
		S_000,		//deathstate
		S_000,		//xdeathstate
		sfx_None/*sfx_000*/,		//deathsound
		0,		//speed
		16*FRACUNIT,		//radius
		64*FRACUNIT,		//height
		100,		//mass
		0,		//damage
		sfx_None/*sfx_000*/,		//activesound
		MF_SHADOW,		//flags
		0,		//palette
		140		//alpha
	},

	{		/*MT_PROP_CANDLE*/
		34,		//doomednum
		S_624,		//spawnstate
		1000,		//spawnhealth
		S_000,		//seestate
		sfx_None/*sfx_000*/,		//seesound
		8,		//reactiontime
		sfx_None/*sfx_000*/,		//attacksound
		S_000,		//painstate
		0,		//painchance
		sfx_None/*sfx_000*/,		//painsound
		S_000,		//meleestate
		S_000,		//missilestate
		S_000,		//deathstate
		S_000,		//xdeathstate
		sfx_None/*sfx_000*/,		//deathsound
		0,		//speed
		20*FRACUNIT,		//radius
		16*FRACUNIT,		//height
		100,		//mass
		0,		//damage
		sfx_None/*sfx_000*/,		//activesound
		0, 		//flags
		0,		//palette
		255		//alpha
	},

	{		/*MT_PROP_BARREL*/
		1001,		//doomednum
		S_626,		//spawnstate
		20,		//spawnhealth
		S_000,		//seestate
		sfx_None/*sfx_000*/,		//seesound
		8,		//reactiontime
		sfx_None/*sfx_000*/,		//attacksound
		S_000,		//painstate
		0,		//painchance
		sfx_None/*sfx_000*/,		//painsound
		S_000,		//meleestate
		S_000,		//missilestate
		S_627,		//deathstate
		S_000,		//xdeathstate
		sfx_explode,		//deathsound
		0,		//speed
		16*FRACUNIT,		//radius
		50*FRACUNIT,		//height
		100,		//mass
		0,		//damage
		sfx_None/*sfx_000*/,		//activesound
		MF_SOLID|MF_SHOOTABLE|MF_NOBLOOD,		//flags
		0,		//palette
		255		//alpha
	},

	{		/*MT_EXPLOSION1*/
		-1,		//doomednum
		S_631,		//spawnstate
		1000,		//spawnhealth
		S_000,		//seestate
		sfx_None/*sfx_000*/,		//seesound
		8,		//reactiontime
		sfx_None/*sfx_000*/,		//attacksound
		S_000,		//painstate
		0,		//painchance
		sfx_None/*sfx_000*/,		//painsound
		S_000,		//meleestate
		S_000,		//missilestate
		S_000,		//deathstate
		S_000,		//xdeathstate
		sfx_None/*sfx_000*/,		//deathsound
		0,		//speed
		20*FRACUNIT,		//radius
		16*FRACUNIT,		//height
		100,		//mass
		0,		//damage
		sfx_None/*sfx_000*/,		//activesound
		MF_NOBLOCKMAP,		//flags
		0,		//palette
		80		//alpha
	},

	{		/*MT_EXPLOSION2*/
		-1,		//doomednum
		S_636,		//spawnstate
		1000,		//spawnhealth
		S_000,		//seestate
		sfx_None/*sfx_000*/,		//seesound
		8,		//reactiontime
		sfx_None/*sfx_000*/,		//attacksound
		S_000,		//painstate
		0,		//painchance
		sfx_None/*sfx_000*/,		//painsound
		S_000,		//meleestate
		S_000,		//missilestate
		S_000,		//deathstate
		S_000,		//xdeathstate
		sfx_None/*sfx_000*/,		//deathsound
		0,		//speed
		20*FRACUNIT,		//radius
		16*FRACUNIT,		//height
		100,		//mass
		0,		//damage
		sfx_None/*sfx_000*/,		//activesound
		MF_NOBLOCKMAP,		//flags
		0,		//palette
		80		//alpha
	},

	{		/*MT_PROP_TECHLAMP1*/
		1015,		//doomednum
		S_641,		//spawnstate
		1000,		//spawnhealth
		S_000,		//seestate
		sfx_None/*sfx_000*/,		//seesound
		8,		//reactiontime
		sfx_None/*sfx_000*/,		//attacksound
		S_000,		//painstate
		0,		//painchance
		sfx_None/*sfx_000*/,		//painsound
		S_000,		//meleestate
		S_000,		//missilestate
		S_000,		//deathstate
		S_000,		//xdeathstate
		sfx_None/*sfx_000*/,		//deathsound
		0,		//speed
		20*FRACUNIT,		//radius
		54*FRACUNIT,		//height
		100,		//mass
		0,		//damage
		sfx_None/*sfx_000*/,		//activesound
		MF_NOBLOCKMAP|MF_SPAWNCEILING,		//flags
		0,		//palette
		255		//alpha
	},

	{		/*MT_PROP_TECHLAMP2*/
		1016,		//doomednum
		S_642,		//spawnstate
		1000,		//spawnhealth
		S_000,		//seestate
		sfx_None/*sfx_000*/,		//seesound
		8,		//reactiontime
		sfx_None/*sfx_000*/,		//attacksound
		S_000,		//painstate
		0,		//painchance
		sfx_None/*sfx_000*/,		//painsound
		S_000,		//meleestate
		S_000,		//missilestate
		S_000,		//deathstate
		S_000,		//xdeathstate
		sfx_None/*sfx_000*/,		//deathsound
		0,		//speed
		20*FRACUNIT,		//radius
		12*FRACUNIT,		//height
		100,		//mass
		0,		//damage
		sfx_None/*sfx_000*/,		//activesound
		MF_NOBLOCKMAP|MF_SPAWNCEILING,		//flags
		0,		//palette
		255		//alpha
	},

	{		/*MT_PROP_TORCHBLUE*/
		1003,		//doomednum
		S_643,		//spawnstate
		1000,		//spawnhealth
		S_000,		//seestate
		sfx_None/*sfx_000*/,		//seesound
		8,		//reactiontime
		sfx_None/*sfx_000*/,		//attacksound
		S_000,		//painstate
		0,		//painchance
		sfx_None/*sfx_000*/,		//painsound
		S_000,		//meleestate
		S_000,		//missilestate
		S_000,		//deathstate
		S_000,		//xdeathstate
		sfx_None/*sfx_000*/,		//deathsound
		0,		//speed
		20*FRACUNIT,		//radius
		16*FRACUNIT,		//height
		100,		//mass
		0,		//damage
		sfx_None/*sfx_000*/,		//activesound
		0, 		//flags
		0,		//palette
		255		//alpha
	},

	{		/*MT_PROP_TORCHYELLOW*/
		1039,		//doomednum
		S_648,		//spawnstate
		1000,		//spawnhealth
		S_000,		//seestate
		sfx_None/*sfx_000*/,		//seesound
		8,		//reactiontime
		sfx_None/*sfx_000*/,		//attacksound
		S_000,		//painstate
		0,		//painchance
		sfx_None/*sfx_000*/,		//painsound
		S_000,		//meleestate
		S_000,		//missilestate
		S_000,		//deathstate
		S_000,		//xdeathstate
		sfx_None/*sfx_000*/,		//deathsound
		0,		//speed
		20*FRACUNIT,		//radius
		16*FRACUNIT,		//height
		100,		//mass
		0,		//damage
		sfx_None/*sfx_000*/,		//activesound
		0, 		//flags
		0,		//palette
		255		//alpha
	},

	{		/*MT_PROP_TORCHRED*/
		1025,		//doomednum
		S_653,		//spawnstate
		1000,		//spawnhealth
		S_000,		//seestate
		sfx_None/*sfx_000*/,		//seesound
		8,		//reactiontime
		sfx_None/*sfx_000*/,		//attacksound
		S_000,		//painstate
		0,		//painchance
		sfx_None/*sfx_000*/,		//painsound
		S_000,		//meleestate
		S_000,		//missilestate
		S_000,		//deathstate
		S_000,		//xdeathstate
		sfx_None/*sfx_000*/,		//deathsound
		0,		//speed
		20*FRACUNIT,		//radius
		16*FRACUNIT,		//height
		100,		//mass
		0,		//damage
		sfx_None/*sfx_000*/,		//activesound
		0, 		//flags
		0,		//palette
		255		//alpha
	},

	{		/*MT_PROP_POLEBASELONG*/
		1050,		//doomednum
		S_658,		//spawnstate
		1000,		//spawnhealth
		S_000,		//seestate
		sfx_None/*sfx_000*/,		//seesound
		8,		//reactiontime
		sfx_None/*sfx_000*/,		//attacksound
		S_000,		//painstate
		0,		//painchance
		sfx_None/*sfx_000*/,		//painsound
		S_000,		//meleestate
		S_000,		//missilestate
		S_000,		//deathstate
		S_000,		//xdeathstate
		sfx_None/*sfx_000*/,		//deathsound
		0,		//speed
		12*FRACUNIT,		//radius
		16*FRACUNIT,		//height
		100,		//mass
		0,		//damage
		sfx_None/*sfx_000*/,		//activesound
		MF_SOLID,		//flags
		0,		//palette
		255		//alpha
	},

	{		/*MT_PROP_POLEBASESHORT*/
		1051,		//doomednum
		S_659,		//spawnstate
		1000,		//spawnhealth
		S_000,		//seestate
		sfx_None/*sfx_000*/,		//seesound
		8,		//reactiontime
		sfx_None/*sfx_000*/,		//attacksound
		S_000,		//painstate
		0,		//painchance
		sfx_None/*sfx_000*/,		//painsound
		S_000,		//meleestate
		S_000,		//missilestate
		S_000,		//deathstate
		S_000,		//xdeathstate
		sfx_None/*sfx_000*/,		//deathsound
		0,		//speed
		8*FRACUNIT,		//radius
		16*FRACUNIT,		//height
		100,		//mass
		0,		//damage
		sfx_None/*sfx_000*/,		//activesound
		MF_SOLID,		//flags
		0,		//palette
		255		//alpha
	},

	{		/*MT_PROP_FIREBLUE*/
		1033,		//doomednum
		S_660,		//spawnstate
		1000,		//spawnhealth
		S_000,		//seestate
		sfx_None/*sfx_000*/,		//seesound
		8,		//reactiontime
		sfx_None/*sfx_000*/,		//attacksound
		S_000,		//painstate
		0,		//painchance
		sfx_None/*sfx_000*/,		//painsound
		S_000,		//meleestate
		S_000,		//missilestate
		S_000,		//deathstate
		S_000,		//xdeathstate
		sfx_None/*sfx_000*/,		//deathsound
		0,		//speed
		20*FRACUNIT,		//radius
		16*FRACUNIT,		//height
		100,		//mass
		0,		//damage
		sfx_None/*sfx_000*/,		//activesound
		0, 		//flags
		0,		//palette
		192		//alpha
	},

	{		/*MT_PROP_FIRERED*/
		1034,		//doomednum
		S_665,		//spawnstate
		1000,		//spawnhealth
		S_000,		//seestate
		sfx_None/*sfx_000*/,		//seesound
		8,		//reactiontime
		sfx_None/*sfx_000*/,		//attacksound
		S_000,		//painstate
		0,		//painchance
		sfx_None/*sfx_000*/,		//painsound
		S_000,		//meleestate
		S_000,		//missilestate
		S_000,		//deathstate
		S_000,		//xdeathstate
		sfx_None/*sfx_000*/,		//deathsound
		0,		//speed
		20*FRACUNIT,		//radius
		16*FRACUNIT,		//height
		100,		//mass
		0,		//damage
		sfx_None/*sfx_000*/,		//activesound
		0, 		//flags
		0,		//palette
		192		//alpha
	},

	{		/*MT_PROP_FIREYELLOW*/
		1035,		//doomednum
		S_670,		//spawnstate
		1000,		//spawnhealth
		S_000,		//seestate
		sfx_None/*sfx_000*/,		//seesound
		8,		//reactiontime
		sfx_None/*sfx_000*/,		//attacksound
		S_000,		//painstate
		0,		//painchance
		sfx_None/*sfx_000*/,		//painsound
		S_000,		//meleestate
		S_000,		//missilestate
		S_000,		//deathstate
		S_000,		//xdeathstate
		sfx_None/*sfx_000*/,		//deathsound
		0,		//speed
		20*FRACUNIT,		//radius
		16*FRACUNIT,		//height
		100,		//mass
		0,		//damage
		sfx_None/*sfx_000*/,		//activesound
		0, 		//flags
		0,		//palette
		192		//alpha
	},

	{		/*MT_GIB_MEATSTICK*/
		1005,		//doomednum
		S_675,		//spawnstate
		1000,		//spawnhealth
		S_000,		//seestate
		sfx_None/*sfx_000*/,		//seesound
		8,		//reactiontime
		sfx_None/*sfx_000*/,		//attacksound
		S_000,		//painstate
		0,		//painchance
		sfx_None/*sfx_000*/,		//painsound
		S_000,		//meleestate
		S_000,		//missilestate
		S_000,		//deathstate
		S_000,		//xdeathstate
		sfx_None/*sfx_000*/,		//deathsound
		0,		//speed
		20*FRACUNIT,		//radius
		16*FRACUNIT,		//height
		100,		//mass
		0,		//damage
		sfx_None/*sfx_000*/,		//activesound
		MF_SOLID,		//flags
		0,		//palette
		255		//alpha
	},

	{		/*MT_GIB_MEATHANG*/
		1006,		//doomednum
		S_676,		//spawnstate
		1000,		//spawnhealth
		S_000,		//seestate
		sfx_None/*sfx_000*/,		//seesound
		8,		//reactiontime
		sfx_None/*sfx_000*/,		//attacksound
		S_000,		//painstate
		0,		//painchance
		sfx_None/*sfx_000*/,		//painsound
		S_000,		//meleestate
		S_000,		//missilestate
		S_000,		//deathstate
		S_000,		//xdeathstate
		sfx_None/*sfx_000*/,		//deathsound
		0,		//speed
		20*FRACUNIT,		//radius
		95*FRACUNIT,		//height
		100,		//mass
		0,		//damage
		sfx_None/*sfx_000*/,		//activesound
		MF_NOBLOCKMAP|MF_SPAWNCEILING,		//flags
		0,		//palette
		255		//alpha
	},

	{		/*MT_GIB_TORSOHANG*/
		1007,		//doomednum
		S_677,		//spawnstate
		1000,		//spawnhealth
		S_000,		//seestate
		sfx_None/*sfx_000*/,		//seesound
		8,		//reactiontime
		sfx_None/*sfx_000*/,		//attacksound
		S_000,		//painstate
		0,		//painchance
		sfx_None/*sfx_000*/,		//painsound
		S_000,		//meleestate
		S_000,		//missilestate
		S_000,		//deathstate
		S_000,		//xdeathstate
		sfx_None/*sfx_000*/,		//deathsound
		0,		//speed
		20*FRACUNIT,		//radius
		83*FRACUNIT,		//height
		100,		//mass
		0,		//damage
		sfx_None/*sfx_000*/,		//activesound
		MF_NOBLOCKMAP|MF_SPAWNCEILING,		//flags
		0,		//palette
		255		//alpha
	},

	{		/*MT_GIB_RIBFLOOR*/
		1008,		//doomednum
		S_678,		//spawnstate
		1000,		//spawnhealth
		S_000,		//seestate
		sfx_None/*sfx_000*/,		//seesound
		8,		//reactiontime
		sfx_None/*sfx_000*/,		//attacksound
		S_000,		//painstate
		0,		//painchance
		sfx_None/*sfx_000*/,		//painsound
		S_000,		//meleestate
		S_000,		//missilestate
		S_000,		//deathstate
		S_000,		//xdeathstate
		sfx_None/*sfx_000*/,		//deathsound
		0,		//speed
		20*FRACUNIT,		//radius
		16*FRACUNIT,		//height
		100,		//mass
		0,		//damage
		sfx_None/*sfx_000*/,		//activesound
		0, 		//flags
		0,		//palette
		255		//alpha
	},

	{		/*MT_GIB_TWITCHFLOOR*/
		1009,		//doomednum
		S_679,		//spawnstate
		1000,		//spawnhealth
		S_000,		//seestate
		sfx_None/*sfx_000*/,		//seesound
		8,		//reactiontime
		sfx_None/*sfx_000*/,		//attacksound
		S_000,		//painstate
		0,		//painchance
		sfx_None/*sfx_000*/,		//painsound
		S_000,		//meleestate
		S_000,		//missilestate
		S_000,		//deathstate
		S_000,		//xdeathstate
		sfx_None/*sfx_000*/,		//deathsound
		0,		//speed
		20*FRACUNIT,		//radius
		16*FRACUNIT,		//height
		100,		//mass
		0,		//damage
		sfx_None/*sfx_000*/,		//activesound
		0, 		//flags
		0,		//palette
		255		//alpha
	},

	{		/*MT_GIB_BLOODPOOL*/
		1010,		//doomednum
		S_681,		//spawnstate
		1000,		//spawnhealth
		S_000,		//seestate
		sfx_None/*sfx_000*/,		//seesound
		8,		//reactiontime
		sfx_None/*sfx_000*/,		//attacksound
		S_000,		//painstate
		0,		//painchance
		sfx_None/*sfx_000*/,		//painsound
		S_000,		//meleestate
		S_000,		//missilestate
		S_000,		//deathstate
		S_000,		//xdeathstate
		sfx_None/*sfx_000*/,		//deathsound
		0,		//speed
		20*FRACUNIT,		//radius
		16*FRACUNIT,		//height
		100,		//mass
		0,		//damage
		sfx_None/*sfx_000*/,		//activesound
		0, 		//flags
		0,		//palette
		255		//alpha
	},

	{		/*MT_GIB_BONEFLOOR*/
		1011,		//doomednum
		S_682,		//spawnstate
		1000,		//spawnhealth
		S_000,		//seestate
		sfx_None/*sfx_000*/,		//seesound
		8,		//reactiontime
		sfx_None/*sfx_000*/,		//attacksound
		S_000,		//painstate
		0,		//painchance
		sfx_None/*sfx_000*/,		//painsound
		S_000,		//meleestate
		S_000,		//missilestate
		S_000,		//deathstate
		S_000,		//xdeathstate
		sfx_None/*sfx_000*/,		//deathsound
		0,		//speed
		20*FRACUNIT,		//radius
		16*FRACUNIT,		//height
		100,		//mass
		0,		//damage
		sfx_None/*sfx_000*/,		//activesound
		0, 		//flags
		0,		//palette
		255		//alpha
	},

	{		/*MT_GIB_MEATRIBFLOOR*/
		1012,		//doomednum
		S_683,		//spawnstate
		1000,		//spawnhealth
		S_000,		//seestate
		sfx_None/*sfx_000*/,		//seesound
		8,		//reactiontime
		sfx_None/*sfx_000*/,		//attacksound
		S_000,		//painstate
		0,		//painchance
		sfx_None/*sfx_000*/,		//painsound
		S_000,		//meleestate
		S_000,		//missilestate
		S_000,		//deathstate
		S_000,		//xdeathstate
		sfx_None/*sfx_000*/,		//deathsound
		0,		//speed
		20*FRACUNIT,		//radius
		16*FRACUNIT,		//height
		100,		//mass
		0,		//damage
		sfx_None/*sfx_000*/,		//activesound
		0, 		//flags
		0,		//palette
		255		//alpha
	},

	{		/*MT_GIB_MEATRIBCAGE*/
		1013,		//doomednum
		S_684,		//spawnstate
		1000,		//spawnhealth
		S_000,		//seestate
		sfx_None/*sfx_000*/,		//seesound
		8,		//reactiontime
		sfx_None/*sfx_000*/,		//attacksound
		S_000,		//painstate
		0,		//painchance
		sfx_None/*sfx_000*/,		//painsound
		S_000,		//meleestate
		S_000,		//missilestate
		S_000,		//deathstate
		S_000,		//xdeathstate
		sfx_None/*sfx_000*/,		//deathsound
		0,		//speed
		20*FRACUNIT,		//radius
		16*FRACUNIT,		//height
		100,		//mass
		0,		//damage
		sfx_None/*sfx_000*/,		//activesound
		0, 		//flags
		0,		//palette
		255		//alpha
	},

	{		/*MT_GIB_CHAINHOOK*/
		1014,		//doomednum
		S_685,		//spawnstate
		1000,		//spawnhealth
		S_000,		//seestate
		sfx_None/*sfx_000*/,		//seesound
		8,		//reactiontime
		sfx_None/*sfx_000*/,		//attacksound
		S_000,		//painstate
		0,		//painchance
		sfx_None/*sfx_000*/,		//painsound
		S_000,		//meleestate
		S_000,		//missilestate
		S_000,		//deathstate
		S_000,		//xdeathstate
		sfx_None/*sfx_000*/,		//deathsound
		0,		//speed
		20*FRACUNIT,		//radius
		95*FRACUNIT,		//height
		100,		//mass
		0,		//damage
		sfx_None/*sfx_000*/,		//activesound
		MF_NOBLOCKMAP|MF_SPAWNCEILING,		//flags
		0,		//palette
		255		//alpha
	},

	{		/*MT_GIB_HANGCAGE*/
		1017,		//doomednum
		S_686,		//spawnstate
		1000,		//spawnhealth
		S_000,		//seestate
		sfx_None/*sfx_000*/,		//seesound
		8,		//reactiontime
		sfx_None/*sfx_000*/,		//attacksound
		S_000,		//painstate
		0,		//painchance
		sfx_None/*sfx_000*/,		//painsound
		S_000,		//meleestate
		S_000,		//missilestate
		S_000,		//deathstate
		S_000,		//xdeathstate
		sfx_None/*sfx_000*/,		//deathsound
		0,		//speed
		20*FRACUNIT,		//radius
		91*FRACUNIT,		//height
		100,		//mass
		0,		//damage
		sfx_None/*sfx_000*/,		//activesound
		MF_NOBLOCKMAP|MF_SPAWNCEILING,		//flags
		0,		//palette
		255		//alpha
	},

	{		/*MT_GIB_CHAINPINSER*/
		1018,		//doomednum
		S_687,		//spawnstate
		1000,		//spawnhealth
		S_000,		//seestate
		sfx_None/*sfx_000*/,		//seesound
		8,		//reactiontime
		sfx_None/*sfx_000*/,		//attacksound
		S_000,		//painstate
		0,		//painchance
		sfx_None/*sfx_000*/,		//painsound
		S_000,		//meleestate
		S_000,		//missilestate
		S_000,		//deathstate
		S_000,		//xdeathstate
		sfx_None/*sfx_000*/,		//deathsound
		0,		//speed
		20*FRACUNIT,		//radius
		101*FRACUNIT,		//height
		100,		//mass
		0,		//damage
		sfx_None/*sfx_000*/,		//activesound
		MF_NOBLOCKMAP|MF_SPAWNCEILING,		//flags
		0,		//palette
		255		//alpha
	},

	{		/*MT_GIB_CHAINARM*/
		1019,		//doomednum
		S_688,		//spawnstate
		1000,		//spawnhealth
		S_000,		//seestate
		sfx_None/*sfx_000*/,		//seesound
		8,		//reactiontime
		sfx_None/*sfx_000*/,		//attacksound
		S_000,		//painstate
		0,		//painchance
		sfx_None/*sfx_000*/,		//painsound
		S_000,		//meleestate
		S_000,		//missilestate
		S_000,		//deathstate
		S_000,		//xdeathstate
		sfx_None/*sfx_000*/,		//deathsound
		0,		//speed
		20*FRACUNIT,		//radius
		58*FRACUNIT,		//height
		100,		//mass
		0,		//damage
		sfx_None/*sfx_000*/,		//activesound
		MF_NOBLOCKMAP|MF_SPAWNCEILING,		//flags
		0,		//palette
		255		//alpha
	},

	{		/*MT_GIB_HANGMACE1*/
		1020,		//doomednum
		S_689,		//spawnstate
		1000,		//spawnhealth
		S_000,		//seestate
		sfx_None/*sfx_000*/,		//seesound
		8,		//reactiontime
		sfx_None/*sfx_000*/,		//attacksound
		S_000,		//painstate
		0,		//painchance
		sfx_None/*sfx_000*/,		//painsound
		S_000,		//meleestate
		S_000,		//missilestate
		S_000,		//deathstate
		S_000,		//xdeathstate
		sfx_None/*sfx_000*/,		//deathsound
		0,		//speed
		20*FRACUNIT,		//radius
		80*FRACUNIT,		//height
		100,		//mass
		0,		//damage
		sfx_None/*sfx_000*/,		//activesound
		MF_NOBLOCKMAP|MF_SPAWNCEILING,		//flags
		0,		//palette
		255		//alpha
	},

	{		/*MT_GIB_HEADSTICK1*/
		1022,		//doomednum
		S_690,		//spawnstate
		1000,		//spawnhealth
		S_000,		//seestate
		sfx_None/*sfx_000*/,		//seesound
		8,		//reactiontime
		sfx_None/*sfx_000*/,		//attacksound
		S_000,		//painstate
		0,		//painchance
		sfx_None/*sfx_000*/,		//painsound
		S_000,		//meleestate
		S_000,		//missilestate
		S_000,		//deathstate
		S_000,		//xdeathstate
		sfx_None/*sfx_000*/,		//deathsound
		0,		//speed
		8*FRACUNIT,		//radius
		16*FRACUNIT,		//height
		100,		//mass
		0,		//damage
		sfx_None/*sfx_000*/,		//activesound
		MF_SOLID,		//flags
		0,		//palette
		255		//alpha
	},

	{		/*MT_GIB_HEADSTICK2*/
		1023,		//doomednum
		S_691,		//spawnstate
		1000,		//spawnhealth
		S_000,		//seestate
		sfx_None/*sfx_000*/,		//seesound
		8,		//reactiontime
		sfx_None/*sfx_000*/,		//attacksound
		S_000,		//painstate
		0,		//painchance
		sfx_None/*sfx_000*/,		//painsound
		S_000,		//meleestate
		S_000,		//missilestate
		S_000,		//deathstate
		S_000,		//xdeathstate
		sfx_None/*sfx_000*/,		//deathsound
		0,		//speed
		8*FRACUNIT,		//radius
		16*FRACUNIT,		//height
		100,		//mass
		0,		//damage
		sfx_None/*sfx_000*/,		//activesound
		MF_SOLID,		//flags
		0,		//palette
		255		//alpha
	},

	{		/*MT_GIB_DOUBLEMEATSTICK*/
		1024,		//doomednum
		S_692,		//spawnstate
		1000,		//spawnhealth
		S_000,		//seestate
		sfx_None/*sfx_000*/,		//seesound
		8,		//reactiontime
		sfx_None/*sfx_000*/,		//attacksound
		S_000,		//painstate
		0,		//painchance
		sfx_None/*sfx_000*/,		//painsound
		S_000,		//meleestate
		S_000,		//missilestate
		S_000,		//deathstate
		S_000,		//xdeathstate
		sfx_None/*sfx_000*/,		//deathsound
		0,		//speed
		8*FRACUNIT,		//radius
		16*FRACUNIT,		//height
		100,		//mass
		0,		//damage
		sfx_None/*sfx_000*/,		//activesound
		MF_SOLID,		//flags
		0,		//palette
		255		//alpha
	},

	{		/*MT_PROP_STATUE1*/
		1028,		//doomednum
		S_693,		//spawnstate
		1000,		//spawnhealth
		S_000,		//seestate
		sfx_None/*sfx_000*/,		//seesound
		8,		//reactiontime
		sfx_None/*sfx_000*/,		//attacksound
		S_000,		//painstate
		0,		//painchance
		sfx_None/*sfx_000*/,		//painsound
		S_000,		//meleestate
		S_000,		//missilestate
		S_000,		//deathstate
		S_000,		//xdeathstate
		sfx_None/*sfx_000*/,		//deathsound
		0,		//speed
		20*FRACUNIT,		//radius
		16*FRACUNIT,		//height
		100,		//mass
		0,		//damage
		sfx_None/*sfx_000*/,		//activesound
		MF_SOLID,		//flags
		0,		//palette
		255		//alpha
	},

	{		/*MT_PROP_STATUE2*/
		1029,		//doomednum
		S_694,		//spawnstate
		1000,		//spawnhealth
		S_000,		//seestate
		sfx_None/*sfx_000*/,		//seesound
		8,		//reactiontime
		sfx_None/*sfx_000*/,		//attacksound
		S_000,		//painstate
		0,		//painchance
		sfx_None/*sfx_000*/,		//painsound
		S_000,		//meleestate
		S_000,		//missilestate
		S_000,		//deathstate
		S_000,		//xdeathstate
		sfx_None/*sfx_000*/,		//deathsound
		0,		//speed
		20*FRACUNIT,		//radius
		16*FRACUNIT,		//height
		100,		//mass
		0,		//damage
		sfx_None/*sfx_000*/,		//activesound
		MF_SOLID,		//flags
		0,		//palette
		255		//alpha
	},

	{		/*MT_PROP_TECHPOLELONG*/
		1031,		//doomednum
		S_695,		//spawnstate
		1000,		//spawnhealth
		S_000,		//seestate
		sfx_None/*sfx_000*/,		//seesound
		8,		//reactiontime
		sfx_None/*sfx_000*/,		//attacksound
		S_000,		//painstate
		0,		//painchance
		sfx_None/*sfx_000*/,		//painsound
		S_000,		//meleestate
		S_000,		//missilestate
		S_000,		//deathstate
		S_000,		//xdeathstate
		sfx_None/*sfx_000*/,		//deathsound
		0,		//speed
		8*FRACUNIT,		//radius
		80*FRACUNIT,		//height
		100,		//mass
		0,		//damage
		sfx_None/*sfx_000*/,		//activesound
		MF_SOLID,		//flags
		0,		//palette
		255		//alpha
	},

	{		/*MT_PROP_TECHPOLESHORT*/
		1032,		//doomednum
		S_696,		//spawnstate
		1000,		//spawnhealth
		S_000,		//seestate
		sfx_None/*sfx_000*/,		//seesound
		8,		//reactiontime
		sfx_None/*sfx_000*/,		//attacksound
		S_000,		//painstate
		0,		//painchance
		sfx_None/*sfx_000*/,		//painsound
		S_000,		//meleestate
		S_000,		//missilestate
		S_000,		//deathstate
		S_000,		//xdeathstate
		sfx_None/*sfx_000*/,		//deathsound
		0,		//speed
		8*FRACUNIT,		//radius
		62*FRACUNIT,		//height
		100,		//mass
		0,		//damage
		sfx_None/*sfx_000*/,		//activesound
		MF_SOLID,		//flags
		0,		//palette
		255		//alpha
	},

	{		/*MT_PROP_TREESTUMPSMALL*/
		1036,		//doomednum
		S_697,		//spawnstate
		1000,		//spawnhealth
		S_000,		//seestate
		sfx_None/*sfx_000*/,		//seesound
		8,		//reactiontime
		sfx_None/*sfx_000*/,		//attacksound
		S_000,		//painstate
		0,		//painchance
		sfx_None/*sfx_000*/,		//painsound
		S_000,		//meleestate
		S_000,		//missilestate
		S_000,		//deathstate
		S_000,		//xdeathstate
		sfx_None/*sfx_000*/,		//deathsound
		0,		//speed
		16*FRACUNIT,		//radius
		16*FRACUNIT,		//height
		100,		//mass
		0,		//damage
		sfx_None/*sfx_000*/,		//activesound
		MF_SOLID,		//flags
		0,		//palette
		255		//alpha
	},

	{		/*MT_PROP_TREESTUMPLARGE*/
		1037,		//doomednum
		S_698,		//spawnstate
		1000,		//spawnhealth
		S_000,		//seestate
		sfx_None/*sfx_000*/,		//seesound
		8,		//reactiontime
		sfx_None/*sfx_000*/,		//attacksound
		S_000,		//painstate
		0,		//painchance
		sfx_None/*sfx_000*/,		//painsound
		S_000,		//meleestate
		S_000,		//missilestate
		S_000,		//deathstate
		S_000,		//xdeathstate
		sfx_None/*sfx_000*/,		//deathsound
		0,		//speed
		16*FRACUNIT,		//radius
		16*FRACUNIT,		//height
		100,		//mass
		0,		//damage
		sfx_None/*sfx_000*/,		//activesound
		MF_SOLID,		//flags
		0,		//palette
		255		//alpha
	},

	{		/*MT_PROP_TREE*/
		1038,		//doomednum
		S_699,		//spawnstate
		1000,		//spawnhealth
		S_000,		//seestate
		sfx_None/*sfx_000*/,		//seesound
		8,		//reactiontime
		sfx_None/*sfx_000*/,		//attacksound
		S_000,		//painstate
		0,		//painchance
		sfx_None/*sfx_000*/,		//painsound
		S_000,		//meleestate
		S_000,		//missilestate
		S_000,		//deathstate
		S_000,		//xdeathstate
		sfx_None/*sfx_000*/,		//deathsound
		0,		//speed
		16*FRACUNIT,		//radius
		16*FRACUNIT,		//height
		100,		//mass
		0,		//damage
		sfx_None/*sfx_000*/,		//activesound
		MF_SOLID,		//flags
		0,		//palette
		255		//alpha
	},

	{		/*MT_PROP_BLOODYPOLE*/
		1045,		//doomednum
		S_700,		//spawnstate
		1000,		//spawnhealth
		S_000,		//seestate
		sfx_None/*sfx_000*/,		//seesound
		8,		//reactiontime
		sfx_None/*sfx_000*/,		//attacksound
		S_000,		//painstate
		0,		//painchance
		sfx_None/*sfx_000*/,		//painsound
		S_000,		//meleestate
		S_000,		//missilestate
		S_000,		//deathstate
		S_000,		//xdeathstate
		sfx_None/*sfx_000*/,		//deathsound
		0,		//speed
		8*FRACUNIT,		//radius
		16*FRACUNIT,		//height
		100,		//mass
		0,		//damage
		sfx_None/*sfx_000*/,		//activesound
		MF_SOLID,		//flags
		0,		//palette
		255		//alpha
	},

	{		/*MT_GIB_HANGMACE2*/
		1046,		//doomednum
		S_701,		//spawnstate
		1000,		//spawnhealth
		S_000,		//seestate
		sfx_None/*sfx_000*/,		//seesound
		8,		//reactiontime
		sfx_None/*sfx_000*/,		//attacksound
		S_000,		//painstate
		0,		//painchance
		sfx_None/*sfx_000*/,		//painsound
		S_000,		//meleestate
		S_000,		//missilestate
		S_000,		//deathstate
		S_000,		//xdeathstate
		sfx_None/*sfx_000*/,		//deathsound
		0,		//speed
		20*FRACUNIT,		//radius
		56*FRACUNIT,		//height
		100,		//mass
		0,		//damage
		sfx_None/*sfx_000*/,		//activesound
		MF_NOBLOCKMAP|MF_SPAWNCEILING,		//flags
		0,		//palette
		255		//alpha
	},

	{		/*MT_GIB_HANGWHITEMEAT*/
		1047,		//doomednum
		S_702,		//spawnstate
		1000,		//spawnhealth
		S_000,		//seestate
		sfx_None/*sfx_000*/,		//seesound
		8,		//reactiontime
		sfx_None/*sfx_000*/,		//attacksound
		S_000,		//painstate
		0,		//painchance
		sfx_None/*sfx_000*/,		//painsound
		S_000,		//meleestate
		S_000,		//missilestate
		S_000,		//deathstate
		S_000,		//xdeathstate
		sfx_None/*sfx_000*/,		//deathsound
		0,		//speed
		20*FRACUNIT,		//radius
		64*FRACUNIT,		//height
		100,		//mass
		0,		//damage
		sfx_None/*sfx_000*/,		//activesound
		MF_NOBLOCKMAP|MF_SPAWNCEILING,		//flags
		0,		//palette
		255		//alpha
	},

	{		/*MT_GIB_HANGHEAD*/
		1048,		//doomednum
		S_703,		//spawnstate
		1000,		//spawnhealth
		S_000,		//seestate
		sfx_None/*sfx_000*/,		//seesound
		8,		//reactiontime
		sfx_None/*sfx_000*/,		//attacksound
		S_000,		//painstate
		0,		//painchance
		sfx_None/*sfx_000*/,		//painsound
		S_000,		//meleestate
		S_000,		//missilestate
		S_000,		//deathstate
		S_000,		//xdeathstate
		sfx_None/*sfx_000*/,		//deathsound
		0,		//speed
		20*FRACUNIT,		//radius
		60*FRACUNIT,		//height
		100,		//mass
		0,		//damage
		sfx_None/*sfx_000*/,		//activesound
		MF_NOBLOCKMAP|MF_SPAWNCEILING,		//flags
		0,		//palette
		255		//alpha
	},

	{		/*MT_GIB_HANGRIB*/
		1049,		//doomednum
		S_704,		//spawnstate
		1000,		//spawnhealth
		S_000,		//seestate
		sfx_None/*sfx_000*/,		//seesound
		8,		//reactiontime
		sfx_None/*sfx_000*/,		//attacksound
		S_000,		//painstate
		0,		//painchance
		sfx_None/*sfx_000*/,		//painsound
		S_000,		//meleestate
		S_000,		//missilestate
		S_000,		//deathstate
		S_000,		//xdeathstate
		sfx_None/*sfx_000*/,		//deathsound
		0,		//speed
		20*FRACUNIT,		//radius
		98*FRACUNIT,		//height
		100,		//mass
		0,		//damage
		sfx_None/*sfx_000*/,		//activesound
		MF_NOBLOCKMAP|MF_SPAWNCEILING,		//flags
		0,		//palette
		255		//alpha
	},
};

