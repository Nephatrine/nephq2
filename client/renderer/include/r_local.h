/*
Copyright (C) 1997-2001 Id Software, Inc.

This program is free software; you can redistribute it and/or
modify it under the terms of the GNU General Public License
as published by the Free Software Foundation; either version 2
of the License, or (at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  

See the GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program; if not, write to the Free Software
Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.

*/

#ifndef __R_LOCAL_H
#define __R_LOCAL_H


# include <SDL.h>

#include <stdio.h>

//#include <GL/gl.h>
//#include "include/GL/glew.h"
//#include <GL/glu.h>
#include <math.h>

#include "../../ref.h"

#include "qgl.h"
#include "../../vid.h"

#ifndef TRUE
#define TRUE 1
#endif
#ifndef FALSE
#define FALSE 0
#endif
#ifndef FAR
#define FAR
#endif

// up / down
#define	PITCH	0
// left / right
#define	YAW		1
// fall over
#define	ROLL	2

#ifndef __VIDDEF_T
#define __VIDDEF_T
typedef struct
{
	int		width, height;			// coordinates from main game
} viddef_t;
#endif

extern	viddef_t	vid;


/*
  skins will be outline flood filled and mip mapped
  pics and sprites with alpha will be outline flood filled
  pic won't be mip mapped

  model skin
  sprite frame
  wall texture
  pic
*/

typedef enum 
{
	it_skin,
	it_sprite,
	it_wall,
	it_pic,
	it_sky,
	it_part //Knightmare added
} imagetype_t;

typedef struct image_s
{
	char		name[MAX_QPATH];			// game path, including extension
	hash32_t		hash;						// to speed up searching
	imagetype_t	type;
	int32_t			width, height;				// source image
	int32_t			upload_width, upload_height;	// after power of two and picmip
	int32_t			registration_sequence;		// 0 = free
	struct		msurface_s	*texturechain;	// for sort-by-texture world drawing
	struct		msurface_s	*warp_texturechain;	// same as above, for warp surfaces
	uint32_t			texnum;						// gl texture binding
	float		sl, tl, sh, th;				// 0,0 - 1,1 unless part of the scrap
	qboolean	scrap;
	qboolean	has_alpha;

	qboolean	paletted;

	qboolean	is_cin;					// Heffo - To identify a cin texture's image_t
	float		replace_scale_w;			// Knightmare- for scaling hi-res replacement images
	float		replace_scale_h;			// Knightmare- for scaling hi-res replacement images
} image_t;

#define	MAX_LIGHTMAPS	256	// change by Brendon Chung, was 128

#define	TEXNUM_LIGHTMAPS	1024
//#define	TEXNUM_SCRAPS		1152
//#define	TEXNUM_IMAGES		1153
#define	TEXNUM_SCRAPS		TEXNUM_LIGHTMAPS + MAX_LIGHTMAPS
#define	TEXNUM_IMAGES		TEXNUM_SCRAPS + 1

#define	MAX_GLTEXTURES	4096 // Knightmare increased, was 1024

//Harven MD3 ++
#include "r_alias.h" // needs image_t struct
//Harven MD3 --

//===================================================================

typedef enum
{
	rserr_ok,

	rserr_invalid_fullscreen,
	rserr_invalid_mode,

	rserr_unknown
} rserr_t;

#include "r_model.h"

void GL_BeginRendering (int32_t *x, int32_t *y, int32_t *width, int32_t *height);
void GL_EndRendering (void);

void GL_UpdateSwapInterval( void );

extern	float	gldepthmin, gldepthmax;

typedef struct
{
	float	x, y, z;
	float	s, t;
	float	r, g, b;
} glvert_t;


#define	MAX_LBM_HEIGHT		480

#define BACKFACE_EPSILON	0.01


//====================================================

extern	image_t		gltextures[MAX_GLTEXTURES];
extern	int32_t			numgltextures;


#define PARTICLE_TYPES 256
/*
extern	image_t		*r_notexture;
#ifdef	ROQ_SUPPORT
extern	image_t		*r_rawtexture;
#endif // ROQ_SUPPORT
extern	image_t		*r_envmappic;
extern	image_t		*r_spheremappic;
extern	image_t		*r_shelltexture;
extern	image_t		*r_causticwaterpic;
extern	image_t		*r_causticslimepic;
extern	image_t		*r_causticlavapic;
extern	image_t		*r_particlebeam;
extern	image_t		*r_particletextures[PARTICLE_TYPES];
*/
typedef struct glmedia_s {
	image_t		*notexture;			// used for bad textures
	image_t		*whitetexture;		// used for solid colors
	image_t		*rawtexture;		// used for cinematics
	image_t		*envmappic;
	image_t		*spheremappic;
	image_t		*shelltexture;
	image_t		*causticwaterpic;
	image_t		*causticslimepic;
	image_t		*causticlavapic;
	image_t		*particlebeam;
	image_t		*particletextures[PARTICLE_TYPES];
} glmedia_t;

extern	glmedia_t	glMedia;

extern	entity_t	*currententity;
extern	int32_t			r_worldframe; // Knightmare added for trans animations
extern	model_t		*currentmodel;
extern	int32_t			r_visframecount;
extern	int32_t			r_framecount;
extern	cplane_t	frustum[4];
extern	int32_t			c_brush_calls, c_brush_surfs, c_brush_polys, c_alias_polys, c_part_polys;

// Knightmare- saveshot buffer
extern	byte	*saveshotdata;


extern	int32_t			gl_filter_min, gl_filter_max;

//
// view origin
//
extern	vec3_t	vup;
extern	vec3_t	vpn;
extern	vec3_t	vright;
extern	vec3_t	r_origin;

//
// screen size info
//
extern	refdef_t	r_newrefdef;
extern	int32_t		r_viewcluster, r_viewcluster2, r_oldviewcluster, r_oldviewcluster2;

extern  cvar_t  *gl_driver;

extern	cvar_t	*r_norefresh;
extern	cvar_t	*r_lefthand;
extern	cvar_t	*r_drawentities;
extern	cvar_t	*r_drawworld;
extern	cvar_t	*r_speeds;
extern	cvar_t	*r_fullbright;
extern	cvar_t	*r_novis;
extern	cvar_t	*r_nocull;
extern	cvar_t	*r_lerpmodels;
extern	cvar_t	*r_ignorehwgamma; // Knightmare- hardware gamma
extern	cvar_t	*vid_refresh; // Knightmare- refresh rate control
extern  cvar_t  *vid_width;
extern  cvar_t  *vid_height;

extern	cvar_t	*r_waterwave;	// Knightmare- water waves
extern  cvar_t  *r_waterquality; // dghost - new water options
extern	cvar_t  *r_glows;		// texture glows
extern	cvar_t	*r_saveshotsize;// Knightmare- save shot size option

// Knightmare- lerped dlights on models
extern	cvar_t	*r_model_shading;
extern	cvar_t	*r_model_dlights;

extern	cvar_t	*r_lightlevel;	// FIXME: This is a HACK to get the client's light level

// Knightmare- added Vic's RGB brightening
extern	cvar_t	*r_rgbscale;

// Knightmare- added Psychospaz's console font size option
extern	cvar_t	*con_font;
extern	cvar_t	*con_font_size;

//extern	cvar_t	*gl_ext_palettedtexture;
//extern	cvar_t	*gl_ext_pointparameters;
extern	cvar_t	*r_nonpoweroftwo_mipmaps;

extern	cvar_t	*r_trans_lighting; // allow disabling of lighting on trans surfaces
extern	cvar_t	*r_warp_lighting; // allow disabling of lighting on warp surfaces
extern	cvar_t	*r_solidalpha;			// allow disabling of trans33+trans66 surface flag combining
extern	cvar_t	*r_entity_fliproll;		// allow disabling of backwards alias model roll

extern	cvar_t	*r_glass_envmaps; // Psychospaz's envmapping
extern	cvar_t	*r_trans_surf_sorting; // trans bmodel sorting
extern	cvar_t	*r_shelltype; // entity shells: 0 = solid, 1 = warp, 2 = spheremap

extern	cvar_t	*r_lightcutoff;	//** DMP - allow dynamic light cutoff to be user-settable

extern	cvar_t	*r_bitdepth;
extern	cvar_t	*r_lightmap;
extern	cvar_t	*r_shadows;
extern	cvar_t	*r_shadowalpha;
extern	cvar_t	*r_shadowrange;
extern	cvar_t	*r_shadowvolumes;
extern	cvar_t	*r_stencil; //  stenciling for color shells
extern	cvar_t	*r_transrendersort; // correct trasparent sorting
extern	cvar_t	*r_particle_lighting; //  particle lighting
extern	cvar_t	*r_particle_min;
extern	cvar_t	*r_particle_max;
extern	cvar_t	*r_particledistance;

extern	cvar_t	*r_dynamic;
extern	cvar_t	*r_nobind;
extern	cvar_t	*r_picmip;
extern	cvar_t	*r_skymip;
extern	cvar_t	*r_playermip;
extern	cvar_t	*r_showtris;
extern	cvar_t	*r_showbbox;	// Knightmare- show model bounding box
extern	cvar_t	*r_finish;
extern	cvar_t	*r_cull;
extern	cvar_t	*r_polyblend;
extern	cvar_t	*r_flashblend;
extern	cvar_t	*r_modulate;
extern	cvar_t	*r_swapinterval;
extern  cvar_t  *r_adaptivevsync;
extern	cvar_t	*r_anisotropic;
extern	cvar_t	*r_anisotropic_avail;
extern	cvar_t	*r_texturemode;

extern  cvar_t  *r_lodbias;
extern  cvar_t  *r_lockpvs;
extern	cvar_t	*r_intensity;
extern  cvar_t  *r_texturecompression;

extern	cvar_t	*r_skydistance; //Knightmare- variable sky range
extern	cvar_t	*r_saturation;	//** DMP

extern  cvar_t  *r_bloom;

extern  cvar_t  *r_blur;
extern  cvar_t  *r_flashblur;

extern	cvar_t	*vid_fullscreen;
extern  cvar_t  *vid_desktop_fullscreen;
extern  cvar_t	*vid_ref;
extern  cvar_t	*vid_xpos;
extern  cvar_t  *vid_ypos;
extern  cvar_t  *vid_fullscreen;
extern  cvar_t  *vid_srgb;
extern  cvar_t  *vid_brightness;

extern  cvar_t  *r_fencesync;
extern  cvar_t	*r_antialias;
extern	cvar_t  *r_directstate;
extern  cvar_t  *r_driver_workarounds;

extern  float	vid_gamma;

extern	int32_t		gl_lightmap_format;
extern	int32_t		gl_solid_format;
extern	int32_t		gl_alpha_format;
extern	int32_t		gl_raw_tex_solid_format;
extern	int32_t		gl_raw_tex_alpha_format;

extern	int32_t		c_visible_lightmaps;
extern	int32_t		c_visible_textures;


//extern	float	r_world_matrix[16];

// entity sorting struct
//typedef struct sortedelement_s sortedelement_t;
typedef struct sortedelement_s
{
	void  *data;
	vec_t len;
	vec3_t org;
	struct sortedelement_s *left, *right;
} sortedelement_t;


//
// r_image.c
//
void R_TranslatePlayerSkin (int32_t playernum);

// Knightmare- added some of Psychospaz's shortcuts
void ElementAddNode (sortedelement_t *base, sortedelement_t *thisElement);
int32_t transCompare (const void *arg1, const void *arg2);

void R_MaxColorVec (vec3_t color);

//
// r_entity.c
//
extern	sortedelement_t *ents_trans;
extern	sortedelement_t *ents_viewweaps;
extern	sortedelement_t *ents_viewweaps_trans;

void R_DrawEntitiesOnList (sortedelement_t *list);
void R_DrawAllEntities (qboolean addViewWeaps);
void R_DrawAllEntityShadows (void);
void R_DrawSolidEntities ();

//
// r_particle.c
//
extern	sortedelement_t *parts_prerender;
void R_BuildParticleList (void);
void R_SortParticlesOnList (void);
void R_DrawParticles (sortedelement_t *list);
void R_DrawAllParticles (void);
void R_DrawDecals (void);
void R_DrawAllDecals (void);

//
// r_light.c
//
#define DYNAMIC_LIGHT_WIDTH  128
#define DYNAMIC_LIGHT_HEIGHT 128

#define LIGHTMAP_BYTES 4

#define	LM_BLOCK_WIDTH	128
#define	LM_BLOCK_HEIGHT	128

//#define	MAX_LIGHTMAPS	128	// moved this up above to keeep TEXNUM_* values aligned

#define GL_LIGHTMAP_FORMAT	GL_RGBA
//#define GL_LIGHTMAP_FORMAT	GL_BGRA // was GL_RGBA
#define GL_LIGHTMAP_TYPE	GL_UNSIGNED_INT_8_8_8_8_REV	// was GL_UNSIGNED_BYTE

#define BATCH_LM_UPDATES

typedef struct
{
	Uint32	left;
	Uint32	right;
	Uint32	top;
	Uint32	bottom;
} rect_t;

typedef struct
{
	int32_t			internal_format;
	int32_t			format;
	int32_t			type;
	int32_t			current_lightmap_texture;

	msurface_t	*lightmap_surfaces[MAX_LIGHTMAPS];

	int32_t			allocated[LM_BLOCK_WIDTH];

	// the lightmap texture data needs to be kept in
	// main memory so texsubimage can update properly
	unsigned	lightmap_buffer[LM_BLOCK_WIDTH*LM_BLOCK_HEIGHT];
#ifdef BATCH_LM_UPDATES	// Knightmare added
	unsigned	*lightmap_update[MAX_LIGHTMAPS];
	rect_t		lightrect[MAX_LIGHTMAPS];
	qboolean	modified[MAX_LIGHTMAPS];
#endif // BATCH_LM_UPDATES
} gllightmapstate_t;

extern gllightmapstate_t gl_lms;


//void R_LightPoint (vec3_t p, vec3_t color);
void R_LightPoint (vec3_t p, vec3_t color, qboolean isEnt);
void R_LightPointDynamics (vec3_t p, vec3_t color, m_dlight_t *list, int32_t *amount, int32_t max);
void R_PushDlights (void);
void R_ShadowLight (vec3_t pos, vec3_t lightAdd);
void R_MarkLights (dlight_t *light, int32_t bit, mnode_t *node);

//====================================================================

extern	model_t	*r_worldmodel;

extern	unsigned	d_8to24table[256];
extern	float		d_8to24tablef[256][3]; //Knightmare added

extern	int32_t		registration_sequence;


void V_AddBlend (float r, float g, float b, float a, float *v_blend);


/*
====================================================================

r_fbo.c

====================================================================
*/

typedef struct {
	GLuint framebuffer;
	GLuint texture;
	GLuint depthbuffer;
	int32_t width, height;
	GLenum format;
	int32_t valid;
} fbo_t;

qboolean R_GenFBO(int32_t width, int32_t height, int32_t bilinear, GLenum format, fbo_t *FBO);
qboolean R_ResizeFBO(int32_t width, int32_t height, int32_t bilinear,GLenum format, fbo_t *FBO);
void R_SetFBOFilter(int32_t bilinear, fbo_t *FBO);
void R_DelFBO(fbo_t *FBO);
void R_InitFBO(fbo_t *FBO);
void R_BindFBO(fbo_t *FBO);
void R_ClearFBO(fbo_t *FBO);


typedef struct {
	float scale;
	float offset;
} scaleOffset_t;

typedef struct {
	scaleOffset_t x;
	scaleOffset_t y;
} eyeScaleOffset_t;

typedef struct {
	vec3_t viewOffset;
	eyeScaleOffset_t projection;
} eye_param_t;

//
// r_main.c
//
qboolean R_Init ( char *reason );
void R_ClearState (void);
void R_Shutdown (void);
void R_RenderCommon (refdef_t *fd);
void R_RenderView (refdef_t *fd);
void R_RenderViewIntoFBO (refdef_t *fd, eye_param_t parameters, fbo_t *destination, vrect_t *viewRect);
fbo_t* R_GetViewFBO(void);
void R_BeginFrame();
void R_EndFrame ( void );
void R_SwapBuffers( int32_t );
void R_SetPalette ( const Uint8 *palette);

//
// r_model.c
//
void R_DrawAliasMD2Model (entity_t *e);
void R_DrawAliasMD2ModelShadow (entity_t *e);
//Harven++ MD3
void R_DrawAliasModel (entity_t *e);
void R_DrawAliasModelShadow (entity_t *e);
//Harven-- MD3
void R_DrawBrushModel (entity_t *e);
void R_DrawSpriteModel (entity_t *e);
void R_DrawBeam( entity_t *e );
void R_DrawWorld (void);
void R_RenderDlights (void);
void R_DrawAlphaSurfaces (void);
void R_RenderBrushPoly (msurface_t *fa);
void R_InitMedia (void);
void R_DrawInitLocal (void);
void R_SubdivideSurface (msurface_t *fa);
qboolean R_CullBox (vec3_t mins, vec3_t maxs);
void R_RotateForEntity (entity_t *e, qboolean full);
int32_t R_RollMult (void);
void R_MarkLeaves (void);

//
// r_alias_misc.c
//
#define NUMVERTEXNORMALS	162
extern	float	r_avertexnormals[NUMVERTEXNORMALS][3];

// precalculated dot products for quantized angles
#define SHADEDOT_QUANT 16
extern	float	r_avertexnormal_dots[SHADEDOT_QUANT][256];

extern	float	*shadedots;

#define MAX_MODEL_DLIGHTS 32
extern	m_dlight_t	model_dlights[MAX_MODEL_DLIGHTS];
extern	int32_t			model_dlights_num;

extern	vec3_t	shadelight;
extern	vec3_t	lightspot;
extern	float	shellFlowH, shellFlowV;

void	R_ShadowLight (vec3_t pos, vec3_t lightAdd);
void	R_SetShellBlend (qboolean toggle);
void	R_SetVertexRGBScale (qboolean toggle);
qboolean FlowingShell (void);
float	R_CalcEntAlpha (float alpha, vec3_t point);
float	R_CalcShadowAlpha (entity_t *e);
void	R_ShadowBlend (float alpha);
void	R_FlipModel (qboolean on, qboolean cullOnly);
void	R_SetBlendModeOn (image_t *skin);
void	R_SetBlendModeOff (void);
void	R_SetShadeLight (void);
void	R_DrawAliasModelBBox (vec3_t bbox[8], entity_t *e);

//
// r_backend.c
//
// MrG's Vertex array stuff
#define MAX_TEXTURE_UNITS	4 // was 2
#define MAX_VERTICES	16384
#define MAX_INDICES		MAX_VERTICES * 4


#define VA_SetElem2(v,a,b)		((v)[0]=(a),(v)[1]=(b))
#define VA_SetElem3(v,a,b,c)	((v)[0]=(a),(v)[1]=(b),(v)[2]=(c))
#define VA_SetElem4(v,a,b,c,d)	((v)[0]=(a),(v)[1]=(b),(v)[2]=(c),(v)[3]=(d))

#define VA_SetElem2v(v,a)	((v)[0]=(a)[0],(v)[1]=(a)[1])
#define VA_SetElem3v(v,a)	((v)[0]=(a)[0],(v)[1]=(a)[1],(v)[2]=(a)[2])
#define VA_SetElem4v(v,a)	((v)[0]=(a)[0],(v)[1]=(a)[1],(v)[2]=(a)[2],(v)[3]=(a)[3])


extern unsigned	indexArray[MAX_INDICES];
extern float	texCoordArray[MAX_TEXTURE_UNITS][MAX_VERTICES][2];
extern float	inTexCoordArray[MAX_VERTICES][2];
extern float	vertexArray[MAX_VERTICES][3];
extern float	colorArray[MAX_VERTICES][4];
extern unsigned rb_vertex, rb_index;
// end vertex array stuff

void RB_InitBackend (void);
float RB_CalcGlowColor (renderparms_t parms);
void RB_ModifyTextureCoords (float *inArray, float *vertexArray, int32_t numVerts, renderparms_t parms);
qboolean RB_CheckArrayOverflow (int32_t numVerts, int32_t numIndex);
void RB_RenderMeshGeneric (qboolean drawTris);
void RB_DrawArrays (void);
void RB_DrawMeshTris (void);


//
// r_warp.c
//
//glpoly_t *WaterWarpPolyVerts (glpoly_t *p);
void R_InitDSTTex (void);
void R_DrawWarpSurface (msurface_t *fa, float alpha, qboolean render);


//
// r_sky.c
//
void R_AddSkySurface (msurface_t *fa);
void R_ClearSkyBox (void);
void R_DrawSkyBox (void);


//
// r_surf.c
//


#if 0
Sint16 LittleShort (Sint16 l);
Sint16 BigShort (Sint16 l);
int32_t	LittleLong (int32_t l);
float LittleFloat (float f);

char	*va(char *format, ...);
// does a varargs printf into a temp buffer
#endif


void COM_StripExtension (char *in, char *out);

//
// r_draw.c
//
void    R_DrawGetImageSize(int32_t *w, int32_t *h, struct image_s *gl);
void	R_DrawGetPicSize (int32_t *w, int32_t *h, char *name);
void    R_DrawImage (int32_t x, int32_t y, struct image_s *gl);
void	R_DrawPic (int32_t x, int32_t y, char *name);
// added alpha for Psychospaz's transparent console
void    R_DrawStretchImage (int32_t x, int32_t y, int32_t w, int32_t h, struct image_s *gl, float alpha);
void	R_DrawStretchPic (int32_t x, int32_t y, int32_t w, int32_t h, char *name, float alpha);
// Psychospaz's scaled crosshair support
void    R_DrawScaledImage (int32_t x, int32_t y, float scale, float alpha, struct image_s *gl);
void	R_DrawScaledPic (int32_t x, int32_t y, float scale, float alpha, char *pic);
void	R_InitChars (void);
void	R_FlushChars (void);
void	R_DrawChar (float x, float y, int32_t num, float scale, 
			int32_t red, int32_t green, int32_t blue, int32_t alpha, qboolean italic, qboolean last);
void	R_DrawTileClear (int32_t x, int32_t y, int32_t w, int32_t h, char *name);
void	R_DrawFill (int32_t x, int32_t y, int32_t w, int32_t h, int32_t red, int32_t green, int32_t blue, int32_t alpha);
float	R_CharMapScale (void);

#ifdef ROQ_SUPPORT
void	R_DrawStretchRaw (int32_t x, int32_t y, int32_t w, int32_t h, const byte *raw, int32_t rawWidth, int32_t rawHeight);
#else // old 8-bit, 256x256 version
void	r_DrawStretchRaw (int32_t x, int32_t y, int32_t w, int32_t h, int32_t cols, int32_t rows, byte *data);
#endif // ROQ_SUPPORT


//
// r_image.c
//
int32_t		Draw_GetPalette (void);
struct image_s *R_RegisterSkin (char *name);

void LoadPCX (char *filename, byte **pic, byte **palette, int32_t *width, int32_t *height);
//Knightmare added

qboolean R_IsSupportedImageType(char *name);
image_t *R_LoadPic (char *name, byte *pic, int32_t width, int32_t height, imagetype_t type, int32_t bits);
image_t	*R_FindImage (char *name, imagetype_t type);
void	GL_TextureMode( char *string );
void	R_ImageList_f (void);
//void	GL_SetTexturePalette( unsigned palette[256] );
void R_InitFailedImgList (void);
void R_InitImages (void);
void R_ShutdownImages (void);
void R_FreeUnusedImages (void);

/*
** GL extension emulation functions
*/
void GL_DrawParticles( int32_t num_particles );

//
// r_fog.c
//
void R_SetFog (void);
void R_InitFogVars (void);
void R_SetFogVars (qboolean enable, int32_t model, int32_t density,
				   int32_t start, int32_t end, int32_t red, int32_t green, int32_t blue);
extern int32_t r_fogmodel;
extern qboolean r_fogenable;
/*
** GL config stuff
*/
/*#define GL_RENDERER_VOODOO		0x00000001
#define GL_RENDERER_VOODOO2   	0x00000002
#define GL_RENDERER_VOODOO_RUSH	0x00000004
#define GL_RENDERER_BANSHEE		0x00000008
#define	GL_RENDERER_3DFX		0x0000000F

#define GL_RENDERER_PCX1		0x00000010
#define GL_RENDERER_PCX2		0x00000020
#define GL_RENDERER_PMX			0x00000040
#define	GL_RENDERER_POWERVR		0x00000070

#define GL_RENDERER_PERMEDIA2	0x00000100
#define GL_RENDERER_GLINT_MX	0x00000200
#define GL_RENDERER_GLINT_TX	0x00000400
#define GL_RENDERER_3DLABS_MISC	0x00000800
#define	GL_RENDERER_3DLABS		0x00000F00

#define GL_RENDERER_REALIZM		0x00001000
#define GL_RENDERER_REALIZM2	0x00002000
#define	GL_RENDERER_INTERGRAPH	0x00003000

#define GL_RENDERER_3DPRO		0x00004000
#define GL_RENDERER_REAL3D		0x00008000
#define GL_RENDERER_RIVA128		0x00010000
#define GL_RENDERER_DYPIC		0x00020000

#define GL_RENDERER_V1000		0x00040000
#define GL_RENDERER_V2100		0x00080000
#define GL_RENDERER_V2200		0x00100000
#define	GL_RENDERER_RENDITION	0x001C0000

#define GL_RENDERER_O2          0x00100000
#define GL_RENDERER_IMPACT      0x00200000
#define GL_RENDERER_RE			0x00400000
#define GL_RENDERER_IR			0x00800000
#define	GL_RENDERER_SGI			0x00F00000

#define GL_RENDERER_MCD			0x01000000
#define GL_RENDERER_OTHER		0x80000000*/


enum {
	GLREND_DEFAULT		= 1 << 0,

	GLREND_POWERVR		= 1 << 1,
	
	GLREND_NVIDIA		= 1 << 2,
	GLREND_GEFORCE		= 1 << 3,

	GLREND_ATI			= 1 << 4,
	GLREND_RADEON		= 1 << 5,

	GLREND_INTEL		= 1 << 6
};


typedef struct
{
	int32_t         rendType;
	const byte	*renderer_string;
	const byte	*vendor_string;
	const byte	*version_string;
	const byte	*extensions_string;
	const byte  *shader_version_string;
	// for parsing newer OpenGL versions
	int32_t		version_major;
	int32_t		version_minor;
	int32_t		version_release;
	int32_t		shader_version_major;
	int32_t		shader_version_minor;
	int32_t		render_width;
	int32_t		render_height;
	int32_t		window_width;
	int32_t		window_height;

	qboolean	allowCDS;
	qboolean	ext_swap_control;
	qboolean	ext_swap_control_tear;

	// max texture size
	int32_t			max_texsize;
	int32_t			max_texunits;

	qboolean	vertexBufferObject;
//	qboolean	multitexture;

	qboolean	have_stencil;

	qboolean	extCompiledVertArray;

	qboolean	ext_packed_depth_stencil;
	qboolean	ext_framebuffer_object;
	qboolean	arb_sync;
	qboolean	arb_texture_float;
	qboolean	arb_texture_rg;
	qboolean	ext_direct_state_access;
	// anisotropic filtering
	qboolean	anisotropic;
	float		max_anisotropy;
	uint32_t	refresh_rate;
	qboolean	arb_vertex_buffer_object;
	qboolean	arb_vertex_array_object;

	qboolean	srgb_framebuffer;
	qboolean	ext_texture_srgb;
	qboolean	ext_framebuffer_srgb;
	qboolean	ext_texture_compression_s3tc;
	qboolean	arb_texture_compression_bptc;
} glconfig_t;


// Knightmare- OpenGL state manager
typedef struct
{
	float			inverse_intensity;
	qboolean		fullscreen;

	Uint8	*d_16to8table;

	int32_t				lightmap_textures;

	int32_t				currenttextures[MAX_TEXTURE_UNITS];
	Uint32	currenttmu;
	qboolean		activetmu[MAX_TEXTURE_UNITS];

	fbo_t			*currentFBO;
	
	// advanced state manager - MrG
	qboolean		texgen;

	qboolean		regCombiners;
	Uint32	dst_texture;

	qboolean		envAdd;
	qboolean		stencilEnabled;

	qboolean		cullFace;
	qboolean		polygonOffsetFill; // Knightmare added
	qboolean		alphaTest;
	qboolean		blend;
	qboolean		stencilTest;
	qboolean		depthTest;
	qboolean		scissorTest;
    qboolean        fog;
    
	qboolean		texRectangle;
	qboolean		in2d;
	qboolean		arraysLocked;
	// End - MrG

	GLenum			cullMode;
	GLenum			shadeModelMode;
	GLfloat			depthMin;
	GLfloat			depthMax;
	GLfloat			offsetFactor;
	GLfloat			offsetUnits;
	GLenum			alphaFunc;
	GLclampf		alphaRef;
	GLenum			blendSrc;
	GLenum			blendDst;
	GLenum			depthFunc;
	GLboolean		depthMask;

	GLenum			matrixMode;
	GLfloat			axisRotation[4][4];

	vec4_t			clearColor;
	Uint8	originalRedGammaTable[256];
	Uint8	originalGreenGammaTable[256];
	Uint8	originalBlueGammaTable[256];

} glstate_t;

extern glconfig_t  glConfig;
extern glstate_t   glState;

//
// r_glstate.c
//
extern fbo_t screenFBO;
extern fbo_t *lastFrame;
void	GL_Stencil (qboolean enable, qboolean shell);
void	R_ParticleStencil (int32_t passnum);
qboolean GL_HasStencil (void);

void	GL_Enable (GLenum cap);
void	GL_Disable (GLenum cap);
void	GL_ShadeModel (GLenum mode);
void	GL_TexEnv (GLenum value);
void	GL_CullFace (GLenum mode);
void	GL_PolygonOffset (GLfloat factor, GLfloat units);
void	GL_AlphaFunc (GLenum func, GLclampf ref);
void	GL_BlendFunc (GLenum src, GLenum dst);
void	GL_DepthFunc (GLenum func);
void	GL_DepthMask (GLboolean mask);
void	GL_DepthRange (GLfloat rMin, GLfloat rMax);
void	GL_LockArrays (int32_t numVerts);
void	GL_UnlockArrays (void);
void	GL_EnableTexture (unsigned tmu);
void	GL_DisableTexture (unsigned tmu);
void	GL_EnableMultitexture(qboolean enable);
void	GL_SelectTexture (unsigned tmu);
void	GL_Bind (int32_t texnum);
void	GL_MBind (unsigned tmu, int32_t texnum);
void	GL_BindFBO (fbo_t *fbo);
void	GL_SetDefaultState (void);
void	GL_MatrixMode(GLenum matrixMode);
void	GL_SetIdentity(GLenum matrixMode);
void	GL_LoadIdentity(GLenum matrixMode);
void	GL_SetIdentityOrtho(GLenum matrixMode, float l, float r, float b, float t, float n, float f);

void	GL_LoadMatrix(GLenum matrixMode, const float m[4][4]);
void	GL_MultiplyMatrix(GLenum matrixMode, const float m[4][4]);
void	GL_PushMatrix(GLenum matrixMode);
void	GL_PopMatrix(GLenum matrixMode);

void	GL_ClearColor(float r, float g, float b, float a);
void	GL_SetDefaultClearColor();
/*
====================================================================

IMPORTED FUNCTIONS

====================================================================
*/

void	VID_Error (int32_t err_level, char *str, ...);

void	CL_SetParticleImages (void);

void	Cmd_AddCommand (char *name, void(*cmd)(void));
void	Cmd_RemoveCommand (char *name);
int32_t		Cmd_Argc (void);
char	*Cmd_Argv (int32_t i);
void	Cbuf_ExecuteText (int32_t exec_when, char *text);

void	VID_Printf (int32_t print_level, char *str, ...);

// files will be memory mapped read only
// the returned buffer may be part of a larger pak file,
// or a discrete file from anywhere in the quake search path
// a -1 return means the file does not exist
// NULL can be passed for buf to just determine existance
char	**FS_ListPak (char *find, int32_t *num);
int32_t		FS_LoadFile (char *name, void **buf);
void	FS_FreeFile (void *buf);

// gamedir will be the current directory that generated
// files should be stored to, ie: "f:\quake\id1"
char	*FS_Gamedir (void);

cvar_t	*Cvar_Get (char *name, char *value, int32_t flags);
cvar_t	*Cvar_Set (char *name, char *value );
void	 Cvar_SetValue (char *name, float value);

qboolean	VID_GetModeInfo (int32_t *width, int32_t *height, int32_t mode);
void		VID_NewWindow (int32_t width, int32_t height);
// Knightmare- added import of text color for renderer
void		ColorLookup (int32_t colornum, int32_t *red, int32_t *green, int32_t *blue);

/*
====================================================================

IMPLEMENTATION SPECIFIC FUNCTIONS

====================================================================
*/

void		GLimp_BeginFrame( );
void		GLimp_EndFrame( void );
qboolean 		GLimp_Init( );
void		GLimp_Shutdown( void );
rserr_t    	GLimp_SetMode( int32_t *pwidth, int32_t *pheight );
void		GLimp_AppActivate( qboolean active );


/*
====================================================================

r_shaderobjects.c

====================================================================
*/

typedef struct {
	const char *name;
	GLuint index;
} r_attrib_t;

typedef struct {
	GLuint program;
	const char *vert_source;
	const char *frag_source;
	r_attrib_t *attribs;
} r_shaderobject_t;

typedef struct {
	r_shaderobject_t *shader;
	GLint rgbscale_uniform;
	GLint scale_uniform;
	GLint time_uniform;
	GLint displacement_uniform;
    GLint fogmodel_uniform;
} r_warpshader_t;

typedef struct {
	r_shaderobject_t *shader;
	GLint rgbscale_uniform;
    GLint fogmodel_uniform;
} r_causticshader_t;


extern r_warpshader_t warpshader;
extern r_warpshader_t simplexwarpshader;
extern r_causticshader_t causticshader;

qboolean R_CompileShader(GLuint shader, const char *source);
qboolean R_CompileShaderFromFiles(r_shaderobject_t *shader);
qboolean R_CompileShaderProgram(r_shaderobject_t *shader);
void R_DelShaderProgram(r_shaderobject_t *shader);
void R_ShaderObjectsInit();
void R_ShaderObjectsShutdown();

/*
====================================================================

r_antialias.c

====================================================================
*/

enum r_antialias_t {
	ANTIALIAS_NONE,
	ANTIALIAS_4X_FSAA,
	ANTIALIAS_FXAA,
	ANTIALIAS_FXAA_FSS,
	NUM_ANTIALIAS_MODES
};

void R_AntialiasStartFrame (void);
void R_AntialiasInit (void);
void R_AntialiasShutdown(void);
void R_AntialiasSetFBOSize(fbo_t *fbo);
float R_AntialiasGetScale();

//
// r_misc.c
//

void R_ScreenShot_f (void);
void R_ScreenShot_Silent_f (void);
void R_FrameFence (void);
int32_t R_FrameSync (void);
void R_PerspectiveOffset(GLfloat fovy, GLfloat aspect, GLfloat zNear, GLfloat zFar, GLfloat offset);
void R_PerspectiveScale(eyeScaleOffset_t eye, GLfloat zNear, GLfloat zFar);
void R_MakePerspectiveFromScale(eyeScaleOffset_t eye, GLfloat zNear, GLfloat zFar, vec4_t out[4]);
//
// r_blur.c
//

void R_SetupQuadState();
void R_TeardownQuadState();
void R_DrawQuad();
void R_SetupBlit();
void R_TeardownBlit();
void R_BlitTextureToScreen(GLuint texture);
void R_BlitFlipped(GLuint texture);
void R_BlitWithGamma(GLuint texture, float gamma);
void R_BlitWithGammaFlipped(GLuint texture, float gamma);
void R_FXAAFBO(fbo_t *source);
void R_BlurFBO(float blurScale, float blendColor[4], fbo_t *source);
void R_BloomFBO(fbo_t *source);
void R_ApplyPostProcess(fbo_t *source);
void R_PostProcessInit();
void R_PostProcessShutdown();
void R_PostProcessInit();
qboolean R_InitPostsprocessShaders();
void R_TeardownPostprocessShaders();

/*
====================================================================

r_vao.c

====================================================================
*/

/*
typedef struct {
	
} vao_t;

int32_t R_GenVAO(int32_t width, int32_t height, int32_t bilinear, GLenum format, vao_t *VAO);
int32_t R_ResizeVAO(int32_t width, int32_t height, int32_t bilinear,GLenum format, vao_t *VAO);
void R_SetVAOFilter(int32_t bilinear, vao_t *VAO);
void R_DelVAO(vao_t *VAO);
void R_InitVAO(vao_t *VAO);
void R_BindVAO(vao_t *VAO);
*/

/*
====================================================================

r_vbo.c

====================================================================
*/

typedef struct {
	GLuint handle;
	GLenum target;
	GLenum usage;
} buffer_t;

int32_t R_CreateBuffer(buffer_t *buffer, GLenum target, GLenum usage);
void R_SetBuffer(buffer_t *buffer, GLsizeiptr size,  const GLvoid * data);
void R_BindBuffer(buffer_t *buffer);
void R_ReleaseBuffer(buffer_t *buffer);
void R_DelBuffer(buffer_t *buffer);
void R_InitBuffer(buffer_t *buffer);

typedef struct {
	GLuint index;
	GLint size;
	GLenum type;
	GLboolean normalized;
	GLsizei stride;
	GLvoid *offset;
} attribs_t;

void R_SetAttribsVBO(attribs_t *attribs, unsigned int count);

typedef struct {
	GLuint handles[2];
	GLenum usage;
	GLenum mode;
	GLsizei count;
	GLenum type;
} vbo_t;

void R_InitIVBO(vbo_t *buffer);
void R_CreateIVBO(vbo_t *buffer, GLenum usage);
void R_VertexData(vbo_t *buffer, GLsizeiptr size,  const GLvoid * data);
void R_IndexData(vbo_t *buffer, GLenum mode, GLenum type, GLsizei count, GLsizeiptr size, const GLvoid * data);
void R_BindIVBO(vbo_t *buffer, attribs_t *attribs, unsigned int attribCount);
void R_ReleaseIVBO();
void R_DrawIVBO(vbo_t *buffer);
void R_DrawRangeIVBO(vbo_t *buffer, GLvoid *offset, GLsizei count);
void R_DelIVBO(vbo_t *buffer);


#endif
