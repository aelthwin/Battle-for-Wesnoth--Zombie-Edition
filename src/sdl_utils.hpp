/* $Id: sdl_utils.hpp 48222 2011-01-08 20:40:46Z mordante $ */
/*
   Copyright (C) 2003 - 2011 by David White <dave@whitevine.net>
   Part of the Battle for Wesnoth Project http://www.wesnoth.org/

   This program is free software; you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation; either version 2 of the License, or
   (at your option) any later version.
   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY.

   See the COPYING file for more details.
*/

/** @file */

#ifndef SDL_UTILS_INCLUDED
#define SDL_UTILS_INCLUDED

#include "scoped_resource.hpp"
#include "util.hpp"

#include "SDL.h"

#include <cstdlib>
#include <iosfwd>
#include <map>
#include <string>

//older versions of SDL don't define the
//mouse wheel macros, so define them ourselves
//if necessary.
#ifndef SDL_BUTTON_WHEELUP
#define SDL_BUTTON_WHEELUP 4
#endif

#ifndef SDL_BUTTON_WHEELDOWN
#define SDL_BUTTON_WHEELDOWN 5
#endif

#ifndef SDL_BUTTON_WHEELLEFT
#define SDL_BUTTON_WHEELLEFT 6
#endif

#ifndef SDL_BUTTON_WHEELRIGHT
#define SDL_BUTTON_WHEELRIGHT 7
#endif

namespace {
const SDL_Rect empty_rect = { 0, 0, 0, 0 };
}

SDLKey sdl_keysym_from_name(std::string const &keyname);

bool point_in_rect(int x, int y, const SDL_Rect& rect);
bool rects_overlap(const SDL_Rect& rect1, const SDL_Rect& rect2);
SDL_Rect intersect_rects(SDL_Rect const &rect1, SDL_Rect const &rect2);
SDL_Rect union_rects(const SDL_Rect &rect1, const SDL_Rect &rect2);

/**
 *  Creates an empty SDL_Rect.
 *
 *  Since SDL_Rect doesn't have a constructor it's not possible to create it as
 *  a temporary for a function parameter. This functions overcomes this limit.
 */
SDL_Rect create_rect(const int x, const int y, const int w, const int h);

struct surface
{
private:
	static void sdl_add_ref(SDL_Surface *surf)
	{
		if (surf != NULL)
			++surf->refcount;
	}

	struct free_sdl_surface {
		void operator()(SDL_Surface *surf) const
		{
			if (surf != NULL)
				 SDL_FreeSurface(surf);
		}
	};

	typedef util::scoped_resource<SDL_Surface*,free_sdl_surface> scoped_sdl_surface;
public:
	surface() : surface_(NULL)
	{}

	surface(SDL_Surface *surf) : surface_(surf)
	{}

	surface(const surface& o) : surface_(o.surface_.get())
	{
		sdl_add_ref(surface_.get());
	}

	void assign(const surface& o)
	{
		SDL_Surface *surf = o.surface_.get();
		sdl_add_ref(surf); // need to be done before assign to avoid corruption on "a=a;"
		surface_.assign(surf);
	}

	surface& operator=(const surface& o)
	{
		assign(o);
		return *this;
	}

	operator SDL_Surface*() const { return surface_.get(); }

	SDL_Surface* get() const { return surface_.get(); }

	SDL_Surface* operator->() const { return surface_.get(); }

	void assign(SDL_Surface* surf) { surface_.assign(surf); }

	bool null() const { return surface_.get() == NULL; }

private:
	scoped_sdl_surface surface_;
};

bool operator<(const surface& a, const surface& b);

inline void sdl_blit(const surface& src, SDL_Rect* src_rect, surface& dst, SDL_Rect* dst_rect){
	SDL_BlitSurface(src, src_rect, dst, dst_rect);
}

inline void sdl_fill_rect(surface& dst, SDL_Rect* dst_rect, const Uint32 color){
	SDL_FillRect(dst, dst_rect, color);
}

surface make_neutral_surface(const surface &surf);
surface create_neutral_surface(int w, int h);
surface create_optimized_surface(const surface &surf);

/**
 * Stretches a surface in the horizontal direction.
 *
 *  The stretches a surface it uses the first pixel in the horizontal
 *  direction of the original surface and copies that to the destination.
 *  This means only the first column of the original is used for the destination.
 *  @param surf              The source surface.
 *  @param w                 The width of the resulting surface.
 *  @param optimize          Should the return surface be RLE optimized.
 *
 *  @return                  An optimized surface.
 *                           returned.
 *  @retval 0                Returned upon error.
 *  @retval surf             Returned if w == surf->w, note this ignores the
 *                           optimize flag.
 */
surface stretch_surface_horizontal(
	const surface& surf, const unsigned w, const bool optimize = true);

/**
 *  Stretches a surface in the vertical direction.
 *
 *  The stretches a surface it uses the first pixel in the vertical
 *  direction of the original surface and copies that to the destination.
 *  This means only the first row of the original is used for the destination.
 *  @param surf              The source surface.
 *  @param h                 The height of the resulting surface.
 *  @param optimize          Should the return surface be RLE optimized.
 *
 *  @return                  An optimized surface.
 *                           returned.
 *
 *  @retval surf             Returned if h == surf->h, note this ignores the
 *                           optimize flag.
 */
surface stretch_surface_vertical(
	const surface& surf, const unsigned h, const bool optimize = true);

/** Scale a surface
 *  @param surf              The source surface.
 *  @param w                 The width of the resulting surface.
 *  @param h                 The height of the resulting surface.
 *  @param optimize          Should the return surface be RLE optimized.
 *  @return                  A surface containing the scaled version of the source.
 *  @retval 0                Returned upon error.
 *  @retval surf             Returned if w == surf->w and h == surf->h
 *                           note this ignores the optimize flag.
 */
surface scale_surface(const surface &surf, int w, int h, bool optimize=true);

surface adjust_surface_color(const surface &surf, int r, int g, int b, bool optimize=true);
surface greyscale_image(const surface &surf, bool optimize=true);
/** create an heavy shadow of the image, by blurring, increasing alpha and darkening */
surface shadow_image(const surface &surf, bool optimize=true);

/**
 * Recolors a surface using a map with source and converted palette values.
 * This is most often used for team-coloring.
 *
 * @param surf               The source surface.
 * @param map_rgb            Map of color values, with the keys corresponding to the
 *                           source palette, and the values to the recolored palette.
 * @param optimize           Whether the new surface should be RLE encoded. Only
 *                           useful when the source is not the screen and it is
 *                           going to be used multiple times.
 * @return                   A recolored surface, or a null surface if there are
 *                           problems with the source.
 */
surface recolor_image(surface surf, const std::map<Uint32, Uint32>& map_rgb,
	bool optimize=true);

surface brighten_image(const surface &surf, fixed_t amount, bool optimize=true);

/** Get a portion of the screen.
 *  Send NULL if the portion is outside of the screen.
 *  @param surf              The source surface.
 *  @param rect              The portion of the source surface to copy.
 *  @param optimize_format   Optimize by converting to result to display format.
 *                           Only useful if the source is not the screen and you
 *                           plan to blit the result on screen several times.
 *  @return                  A surface containing the portion of the source.
 *                           No RLE or Alpha bits are set.
 *  @retval 0                if error or the portion is outside of the surface.
 */
surface get_surface_portion(const surface &surf, SDL_Rect &rect,
	bool optimize_format=false);

surface adjust_surface_alpha(const surface &surf, fixed_t amount, bool optimize=true);
surface adjust_surface_alpha_add(const surface &surf, int amount, bool optimize=true);

/** Applies a mask on a surface. */
surface mask_surface(const surface &surf, const surface &mask);

/** Check if a surface fit into a mask */
bool in_mask_surface(const surface &surf, const surface &mask);

/** Progressively reduce alpha of bottom part of the surface
 *  @param surf              The source surface.
 *  @param depth             The height of the bottom part in pixels
 *  @param alpha_base        The alpha adjustement at the interface
 *  @param alpha_delta       The alpha adjustement reduction rate by pixel depth
 *  @param optimize_format   Optimize by converting to result to display
*/
surface submerge_alpha(const surface &surf, int depth, float alpha_base, float alpha_delta, bool optimize=true);

/** Light surf using lightmap (RGB=128,128,128 means no change) */
surface light_surface(const surface &surf, const surface &lightmap, bool optimize=true);

/** Cross-fades a surface. */
surface blur_surface(const surface &surf, int depth = 1, bool optimize=true);

/**
 * Cross-fades a surface in place.
 *
 * @param surf                    The surface to blur, must be not optimized
 *                                and have 32 bits per pixel.
 * @param rect                    The part of the surface to blur.
 * @param depth                   The depth of the blurring.
 */
void blur_surface(surface& surf, SDL_Rect rect, unsigned depth = 1);

/**
 * Cross-fades a surface with alpha channel.
 *
 * @todo FIXME: This is just an adapted copy-paste
 * of the normal blur but with blur alpha channel too
 */
surface blur_alpha_surface(const surface &surf, int depth = 1, bool optimize=true);

/** Cuts a rectangle from a surface. */
surface cut_surface(const surface &surf, SDL_Rect const &r);
surface blend_surface(const surface &surf, double amount, Uint32 color, bool optimize=true);
surface flip_surface(const surface &surf, bool optimize=true);
surface flop_surface(const surface &surf, bool optimize=true);
surface create_compatible_surface(const surface &surf, int width = -1, int height = -1);

/**
 * Replacement for sdl_blit.
 *
 * sdl_blit has problems with blitting partly transparent surfaces so
 * this is a replacement. It ignores the SDL_SRCALPHA and SDL_SRCCOLORKEY
 * flags. src and dst will have the SDL_RLEACCEL flag removed.
 * The return value of SDL_BlistSurface is normally ignored so no return value.
 * The rectangles are const and will not be modified.
 *
 * @pre @p src contains a valid canvas.
 * @pre @p dst contains a valid canvas.
 * @pre The caller must make sure the @p src fits on the @p dst.
 *
 * @param src          The surface to blit.
 * @param srcrect      The region of the surface to blit
 * @param dst          The surface to blit on.
 * @param dstrect      The offset to blit the surface on, only x and y are used.
 */
void blit_surface(const surface& src,
	const SDL_Rect* srcrect, surface& dst, const SDL_Rect* dstrect);

void fill_rect_alpha(SDL_Rect &rect, Uint32 color, Uint8 alpha, surface &target);

SDL_Rect get_non_transparent_portion(const surface &surf);

bool operator==(const SDL_Rect& a, const SDL_Rect& b);
bool operator!=(const SDL_Rect& a, const SDL_Rect& b);
bool operator==(const SDL_Color& a, const SDL_Color& b);
bool operator!=(const SDL_Color& a, const SDL_Color& b);
SDL_Color inverse(const SDL_Color& color);
SDL_Color int_to_color(const Uint32 rgb);

SDL_Color create_color(const unsigned char red
		, unsigned char green
		, unsigned char blue
		, unsigned char unused = 255);

/**
 * Helper class for pinning SDL surfaces into memory.
 * @note This class should be used only with neutral surfaces, so that
 *       the pointer returned by #pixels is meaningful.
 */
struct surface_lock
{
	surface_lock(surface &surf);
	~surface_lock();

	Uint32* pixels() { return reinterpret_cast<Uint32*>(surface_->pixels); }
private:
	surface& surface_;
	bool locked_;
};

struct const_surface_lock
{
	const_surface_lock(const surface &surf);
	~const_surface_lock();

	const Uint32* pixels() const { return reinterpret_cast<const Uint32*>(surface_->pixels); }
private:
	const surface& surface_;
	bool locked_;
};

struct surface_restorer
{
	surface_restorer();
	surface_restorer(class CVideo* target, const SDL_Rect& rect);
	~surface_restorer();

	void restore() const;
	void restore(SDL_Rect const &dst) const;
	void update();
	void cancel();

	const SDL_Rect& area() const { return rect_; }

private:
	class CVideo* target_;
	SDL_Rect rect_;
	surface surface_;
};

struct clip_rect_setter
{
	// if r is NULL, clip to the full size of the surface.
	clip_rect_setter(const surface &surf, const SDL_Rect* r, bool operate = true) : surface_(surf), rect_(), operate_(operate)
	{
		if(operate_){
			SDL_GetClipRect(surface_, &rect_);
			SDL_SetClipRect(surface_, r);
		}
	}

	~clip_rect_setter() {
		if (operate_)
			SDL_SetClipRect(surface_, &rect_);
	}

private:
	surface surface_;
	SDL_Rect rect_;
	const bool operate_;
};


void draw_rectangle(int x, int y, int w, int h, Uint32 color, surface tg);

void draw_solid_tinted_rectangle(int x, int y, int w, int h,
                                 int r, int g, int b,
				 double alpha, surface target);

// blit the image on the center of the rectangle
// and a add a colored background
void draw_centered_on_background(surface surf, const SDL_Rect& rect,
	const SDL_Color& color, surface target);

std::ostream& operator<<(std::ostream& s, const SDL_Rect& rect);

#endif
