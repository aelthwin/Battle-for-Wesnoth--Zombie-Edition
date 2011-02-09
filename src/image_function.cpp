/* $Id: image_function.cpp 48153 2011-01-01 15:57:50Z mordante $ */
/*
   Copyright (C) 2009 - 2011 by Ignacio R. Morelle <shadowm2006@gmail.com>
   Part of the Battle for Wesnoth Project http://www.wesnoth.org/

   This program is free software; you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation; either version 2 of the License, or
   (at your option) any later version.
   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY.

   See the COPYING file for more details.
*/

#include "image_function.hpp"

#include "game_config.hpp"
#include "image.hpp"
#include "log.hpp"

#define GETTEXT_DOMAIN "wesnoth-lib"

static lg::log_domain log_display("display");
#define ERR_DP LOG_STREAM(err, log_display)

namespace image {

surface rc_function::operator()(const surface& src) const
{
	// unchecked
	return recolor_image(src, rc_map_);
}

surface fl_function::operator()(const surface& src) const
{
	surface ret = src;

	if(horiz_) {
		ret = flip_surface(ret);
	}

	if(vert_) {
		ret = flop_surface(ret);
	}

	return ret;
}

surface gs_function::operator()(const surface& src) const
{
	return greyscale_image(src);
}

surface crop_function::operator()(const surface& src) const
{
	SDL_Rect area = slice_;
	if(area.w == 0) {
		area.w = src->w;
	}
	if(area.h == 0) {
		area.h = src->h;
	}
	if(area.x < 0) {
		ERR_DP << "start X coordinate of SECTION function is negative - truncating to zero\n";
		area.x = 0;
	}
	if(area.y < 0) {
		ERR_DP << "start Y coordinate of SECTION function is negative - truncating to zero\n";
		area.y = 0;
	}
	return cut_surface(src, area);
}

surface blit_function::operator()(const surface& src) const
{
	//blit_surface want neutral surfaces
	surface nsrc = make_neutral_surface(src);
	surface nsurf = make_neutral_surface(surf_);
	SDL_Rect r = create_rect(x_, y_, 0, 0);
	blit_surface(nsurf, NULL, nsrc, &r);
	return nsrc;
}

surface mask_function::operator()(const surface& src) const
{
	if(src->w == mask_->w &&  src->h == mask_->h && x_ == 0 && y_ == 0)
		return mask_surface(src, mask_);
	SDL_Rect r = create_rect(x_, y_, 0, 0);
	surface new_mask = create_neutral_surface(src->w, src->h);
	blit_surface(mask_, NULL, new_mask, &r);
	return mask_surface(src, new_mask);
}

surface light_function::operator()(const surface& src) const
{
	return light_surface(src, surf_);;
}

surface scale_function::operator()(const surface& src) const
{
	const int old_w = src->w;
	const int old_h = src->h;
	int w = w_;
	int h = h_;

	if(w <= 0) {
		if(w < 0) {
			ERR_DP << "width of SCALE is negative - resetting to original width\n";
		}
		w = old_w;
	}
	if(h <= 0) {
		if(h < 0) {
			ERR_DP << "height of SCALE is negative - resetting to original height\n";
		}
		h = old_h;
	}

	return scale_surface(src, w, h);
}

surface o_function::operator()(const surface& src) const
{
	return adjust_surface_alpha(src, ftofxp(opacity_));
}

surface cs_function::operator()(const surface& src) const
{
	return(
		(r_ != 0 || g_ != 0 || b_ != 0) ?
		adjust_surface_color(src, r_, g_, b_) :
		src
	);
}

surface bl_function::operator()(const surface& src) const
{
	return blur_alpha_surface(src, depth_);
}

surface brighten_function::operator()(const surface &src) const
{
	surface ret = make_neutral_surface(src);
	surface tod_bright(image::get_image(game_config::images:: tod_bright));
	if (tod_bright)
		blit_surface(tod_bright, NULL, ret, NULL);
	return ret;
}

surface darken_function::operator()(const surface &src) const
{
	surface ret = make_neutral_surface(src);
	surface tod_dark(image::get_image(game_config::images::tod_dark));
	if (tod_dark)
		blit_surface(tod_dark, NULL, ret, NULL);
	return ret;
}

surface background_function::operator()(const surface &src) const
{
	surface ret = make_neutral_surface(src);
	SDL_FillRect(ret, NULL, SDL_MapRGBA(ret->format, color_.r, color_.g, color_.b, color_.unused));
	SDL_SetAlpha(src, SDL_SRCALPHA, SDL_ALPHA_OPAQUE);
	SDL_BlitSurface(src, NULL, ret, NULL);
	return ret;
}

} /* end namespace image */
