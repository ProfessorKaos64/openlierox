/////////////////////////////////////////
//
//   OpenLieroX
//
//   Auxiliary Software class library
//
//   based on the work of JasonB
//   enhanced by Dark Charlie and Albert Zeyer
//
//   code under LGPL
//
/////////////////////////////////////////


// Graphics primitives
// Created 12/11/01
// By Jason Boettcher


#ifndef __GFXPRIMITIVES_H__
#define __GFXPRIMITIVES_H__

#include <SDL/SDL_image.h>

#include "LieroX.h"



extern	int		iSurfaceFormat;


SDL_Surface*	LoadImage(const std::string& _filename, bool withalpha = false);

#define		LOAD_IMAGE(bmp,name) if (!Load_Image(bmp,name)) {return false;}
#define		LOAD_IMAGE_WITHALPHA(bmp,name) if (!Load_Image_WithAlpha(bmp,name)) {return false;}
#define		COLORKEY(bmp) SDLColourToNativeColour((bmp)->format->colorkey)


inline Uint32 SDLColourToNativeColour(Uint32 pixel) {
#if SDL_BYTEORDER == SDL_BIG_ENDIAN
	return (pixel << (32 - SDL_GetVideoSurface()->format->BitsPerPixel));
#else
	return pixel;
#endif
}

inline Uint32 NativeColourToSDLColour(Uint32 pixel) {
#if SDL_BYTEORDER == SDL_BIG_ENDIAN
	return (pixel >> (32 - SDL_GetVideoSurface()->format->BitsPerPixel));
#else
	return pixel;
#endif
}


inline bool Load_Image(SDL_Surface*& bmp, const std::string& name)  {
	bmp = LoadImage(name); 
	if (bmp == NULL)  { 
		printf("WARNING: could not load image %s\n", name.c_str()); 
		return false;
	}
	return true;
}

inline bool Load_Image_WithAlpha(SDL_Surface*& bmp, const std::string& name)  {
	bmp = LoadImage(name, true);
	if (bmp == NULL)  { 
		printf("WARNING: could not load image %s\n", name.c_str()); 
		return false;
	}
	return true;
}

// Surface stuff

// Creates a buffer with the same details as the screen
inline SDL_Surface* gfxCreateSurface(int width, int height) {
	if (width <= 0 || height <= 0) // Nonsense, can cause trouble
		return NULL;

	SDL_PixelFormat *fmt = SDL_GetVideoSurface()->format;

	SDL_Surface *result = SDL_CreateRGBSurface(iSurfaceFormat, width, height, 
		fmt->BitsPerPixel, fmt->Rmask, fmt->Gmask, fmt->Bmask, 0x00000000);

	if (result)
		SDL_FillRect( result, NULL, SDL_MapRGB(result->format,0,0,0)); // OpenGL strictly requires the surface to be cleared
	return result;
}


// Creates a buffer with the same details as screen, but with alpha channel
inline SDL_Surface* gfxCreateSurfaceAlpha(int width, int height) {
	if (width <= 0 || height <= 0) // Nonsense, can cause trouble
		return NULL;

	SDL_PixelFormat *fmt = SDL_GetVideoSurface()->format;

	// it's also correct for big endian
	// TODO: is it realy??
	const Uint32 alpha = 0xff000000;

	SDL_Surface *result = SDL_CreateRGBSurface(iSurfaceFormat | SDL_SRCALPHA,
		width, height, fmt->BitsPerPixel,
		fmt->Rmask, fmt->Gmask, fmt->Bmask, alpha);

	if (result)
		SDL_FillRect( result, NULL, SDL_MapRGB(result->format,0,0,0)); // OpenGL strictly requires the surface to be cleared
	return result;
}



// Image drawing

// Simply draw the image
inline void DrawImage(SDL_Surface *bmpDest, SDL_Surface *bmpSrc, int x, int y) {
	static SDL_Rect	rDest;
	rDest.x = x; rDest.y = y;
	SDL_BlitSurface(bmpSrc,NULL,bmpDest,&rDest);
}

// Draw the image, with more options
inline void DrawImageEx(SDL_Surface *bmpDest, SDL_Surface *bmpSrc, int x, int y, int w, int h) {
	static SDL_Rect	rDest, rSrc;
	rDest.x = x; rDest.y = y;
	rSrc.x = 0; rSrc.y = 0;
	rSrc.w = w; rSrc.h = h;
	SDL_BlitSurface(bmpSrc,&rSrc,bmpDest,&rDest);
}

// Draw the image with a huge amount of options
inline void DrawImageAdv(SDL_Surface *bmpDest, SDL_Surface *bmpSrc, int sx, int sy, int dx, int dy, int w, int h) {
	static SDL_Rect	rDest, rSrc;
	rDest.x = dx; rDest.y = dy;
	rSrc.x = sx; rSrc.y = sy;
	rSrc.w = w; rSrc.h = h;
	SDL_BlitSurface(bmpSrc,&rSrc,bmpDest,&rDest);
}


void DrawImageAdv_Mirror(SDL_Surface *bmpDest, SDL_Surface *bmpSrc, int sx, int sy, int dx, int dy, int w, int h);
void DrawImageStretch2(SDL_Surface *bmpDest, SDL_Surface *bmpSrc, int sx, int sy, int dx, int dy, int w, int h);
void DrawImageStretch2Key(SDL_Surface *bmpDest, SDL_Surface *bmpSrc, int sx, int sy, int dx, int dy, int w, int h, Uint32 key);
void DrawImageStretchMirrorKey(SDL_Surface *bmpDest, SDL_Surface *bmpSrc, int sx, int sy, int dx, int dy, int w, int h, Uint32 key);

inline SDL_Surface *GetMirroredImage(SDL_Surface *bmpSrc)  {
	SDL_Surface *result = SDL_CreateRGBSurface(bmpSrc->flags,bmpSrc->w,bmpSrc->h,bmpSrc->format->BitsPerPixel,bmpSrc->format->Rmask,bmpSrc->format->Bmask,bmpSrc->format->Gmask,bmpSrc->format->Amask);
	if (!result)
		return NULL;
	DrawImageAdv_Mirror(result,bmpSrc,0,0,0,0,bmpSrc->w,bmpSrc->h);
	return result;
}

// Draws a sprite doubly stretched but not so advanced
inline void	DrawImageStretch(SDL_Surface *bmpDest, SDL_Surface *bmpSrc, int dx, int dy) {
	DrawImageStretch2(bmpDest,bmpSrc,0,0,dx,dy,bmpSrc->w,bmpSrc->h);
}

// Draws a sprite doubly stretched, with a colour key and not so advanced
inline void	DrawImageStretchKey(SDL_Surface *bmpDest, SDL_Surface *bmpSrc, int dx, int dy, Uint32 key) {
	DrawImageStretch2Key(bmpDest,bmpSrc,0,0,dx,dy,bmpSrc->w,bmpSrc->h, key);
}


// Solid drawing



///////////////////
// Draw horizontal line
void	DrawHLine(SDL_Surface *bmpDest, int x, int x2, int y, Uint32 colour);

///////////////////
// Draw vertical line
void	DrawVLine(SDL_Surface *bmpDest, int y, int y2, int x, Uint32 colour);




// Pixel drawing
inline void PutPixel(SDL_Surface *bmpDest, int x, int y, Uint32 colour) {
	memcpy((Uint8 *)bmpDest->pixels+y*bmpDest->pitch+x*bmpDest->format->BytesPerPixel,&colour,bmpDest->format->BytesPerPixel);
}

// Get a pixel from an 8bit address
inline Uint32 GetPixelFromAddr(Uint8 *p, int bpp) {
	static Uint32 result;
	result = 0;
	memcpy(&result,p,bpp);
	return result;
}

// Get a pixel from the surface
inline Uint32 GetPixel(SDL_Surface *bmpSrc, int x, int y) {
	return GetPixelFromAddr((Uint8 *)bmpSrc->pixels + y * bmpSrc->pitch + x * bmpSrc->format->BytesPerPixel, bmpSrc->format->BytesPerPixel);
}

// Put pixel alpha blended with the background
void PutPixelA(SDL_Surface *bmpDest, int x, int y, Uint32 colour, Uint8 a);


// Extract 4 colour components from a packed int
// TODO: remove img parameter
inline void GetColour4(Uint32 pixel, SDL_Surface *img, Uint8 *r, Uint8 *g, Uint8 *b, Uint8 *a) {
	SDL_GetRGBA(NativeColourToSDLColour(pixel),img->format,r,g,b,a);
}

// Extract 3 colour components from a packed int
// TODO: remove img parameter
inline void GetColour3(Uint32 pixel, SDL_Surface *img, Uint8 *r, Uint8 *g, Uint8 *b) {
	SDL_GetRGB(NativeColourToSDLColour(pixel),img->format,r,g,b);
}


// Creates a int colour based on the 3 components
inline Uint32 MakeColour(Uint8 r, Uint8 g, Uint8 b) {
	return SDLColourToNativeColour(SDL_MapRGB(SDL_GetVideoSurface()->format,r,g,b));
}

inline Uint32 MakeColour(Uint8 r, Uint8 g, Uint8 b, Uint8 a) {
	return SDLColourToNativeColour(SDL_MapRGBA(SDL_GetVideoSurface()->format,r,g,b,a));
}

void SetColorKeyAlpha(SDL_Surface *dst, Uint8 r, Uint8 g, Uint8 b);
// Set's the game's default color key (pink) to the surface
// Works for both alpha and nonalpha surfaces
inline void SetColorKey(SDL_Surface *dst)  {
	if (dst->flags & SDL_SRCALPHA)
		SetColorKeyAlpha(dst,255,0,255);
	else
		SDL_SetColorKey(dst,SDL_SRCCOLORKEY,SDL_MapRGB(dst->format,255,0,255));
}

// Line drawing
void DrawLine(SDL_Surface *dst, Sint16 x1, Sint16 y1, Sint16 x2, Sint16 y2, Uint32 color);


/////////////////////
// Draws a filled rectangle
inline void	DrawRectFill(SDL_Surface *bmpDest, int x, int y, int x2, int y2, Uint32 color) {
	static SDL_Rect r;
	r.x = x;
	r.y = y;
	r.w = x2-x;
	r.h = y2-y;
	SDL_FillRect(bmpDest,&r,NativeColourToSDLColour(color));
}

////////////////////
// Fills the surface with specified colour
inline void FillSurface(SDL_Surface* dst, Uint32 colour) {
	SDL_FillRect(dst, NULL, NativeColourToSDLColour(colour));
}

////////////////////
// Draws a rectangle
inline void	DrawRect(SDL_Surface *bmpDest, int x, int y, int x2, int y2, Uint32 colour) {
	DrawHLine(bmpDest, x, x2, y, colour);
	DrawHLine(bmpDest, x, x2, y2, colour);
	DrawVLine(bmpDest, y, y2, x, colour);
	DrawVLine(bmpDest, y, y2, x2, colour);
}

///////////////////
// Draws a rectangle with transparency
inline void DrawRectFillA(SDL_Surface *bmpDest, int x, int y, int x2, int y2, Uint32 color, Uint8 alpha)  {
	SDL_Surface *tmp = gfxCreateSurface(x2-x,y2-y);
	if (tmp)  {
		// TODO: optimise
		SDL_SetAlpha(tmp,SDL_SRCALPHA | SDL_RLEACCEL, alpha);
		SDL_FillRect(tmp,NULL,NativeColourToSDLColour(color));
		DrawImage(bmpDest,tmp,x,y);
		SDL_FreeSurface(tmp);
	}
}

//////////////////
// Draw a triangle
inline void DrawTriangle(SDL_Surface *bmpDest, int x1, int y1, int x2, int y2, int x3, int y3, Uint32 colour) {
	DrawLine(bmpDest, x1, y1, x2, y2, colour);
	DrawLine(bmpDest, x2, y2, x3, y3, colour);
	DrawLine(bmpDest, x3, y3, x1, y1, colour);
}



void	DrawRope(SDL_Surface *bmp, int x1, int y1, int x2, int y2, Uint32 color);
void	DrawBeam(SDL_Surface *bmp, int x1, int y1, int x2, int y2, Uint32 color);
void	DrawLaserSight(SDL_Surface *bmp, int x1, int y1, int x2, int y2, Uint32 color);

bool	SaveSurface(SDL_Surface *Image, const std::string& FileName, int Format, bool Tournament);

#endif  //  __GFXPRIMITIVES_H__
