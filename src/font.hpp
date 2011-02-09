/* $Id: font.hpp 48153 2011-01-01 15:57:50Z mordante $ */
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
#ifndef FONT_HPP_INCLUDED
#define FONT_HPP_INCLUDED

#include "exceptions.hpp"
#include "SDL_ttf.h"

#include "sdl_utils.hpp"

class t_string;

namespace font {

//object which initializes and destroys structures needed for fonts
struct manager {
	manager();
	~manager();

	/**
	 * Updates the font path, when initialized it sets the fontpath to
	 * game_config::path. When this path is updated, this function should be
	 * called.
	 */
	void update_font_path() const;

	struct error : public game::error {
		error() : game::error("Font initialization failed") {}
	};
private:
	/** Initializes the font path. */
	void init() const;

	/** Deinitializes the font path. */
	void deinit() const;
};

//various standard colors
extern const SDL_Color NORMAL_COLOR, GRAY_COLOR, LOBBY_COLOR, GOOD_COLOR, BAD_COLOR,
                       BLACK_COLOR, YELLOW_COLOR, BUTTON_COLOR, BIGMAP_COLOR,
                       PETRIFIED_COLOR, TITLE_COLOR, DISABLED_COLOR, LABEL_COLOR;

// font sizes, to be made theme parameters
#ifdef USE_TINY_GUI
// this is not meant for normal play, just for checking other dimensions get adapted accordingly
const int SIZE_NORMAL = 9;
#else
const int SIZE_NORMAL = 14;
#endif
inline int relative_size(int size)
{
	return (SIZE_NORMAL * size / 14);
}

// automatic computation of other font sizes, to be made a default for theme-provided values
#ifdef USE_TINY_GUI
	const int
	SIZE_TINY	= 8,
	SIZE_SMALL	= 8,
	SIZE_15		= 9,
	SIZE_PLUS	= 9,
	SIZE_LARGE	= 10,
	SIZE_XLARGE	= 10
  ;
#else
const int
	SIZE_TINY       = relative_size(10),
	SIZE_SMALL      = relative_size(12),

	SIZE_15         = relative_size(15),
	SIZE_PLUS       = relative_size(16),
	SIZE_LARGE      = relative_size(18),
	SIZE_XLARGE     = relative_size(24)
  ;
#endif

// Returns a SDL surface containing the text rendered in a given color.
surface get_rendered_text(const std::string& text, int size, const SDL_Color& color, int style=0);

SDL_Rect draw_text_line(surface gui_surface, const SDL_Rect& area, int size,
						const SDL_Color& color, const std::string& text,
						int x, int y, bool use_tooltips, int style);

// Returns the maximum height of a font, in pixels
int get_max_height(int size);

///
/// Determine the width of a line of text given a certain font size.
/// The font type used is the default wesnoth font type.
///
int line_width(const std::string& line, int font_size, int style=TTF_STYLE_NORMAL);

///
/// Determine the size of a line of text given a certain font size. Similar to
/// line_width, but for both coordinates.
///
SDL_Rect line_size(const std::string& line, int font_size, int style=TTF_STYLE_NORMAL);

/**
 * If the text excedes the specified max width, end it with an ellipsis (...)
 */
std::string make_text_ellipsis(const std::string& text, int font_size, int max_width,
	int style = TTF_STYLE_NORMAL);


/// structure which will hide all current floating labels, and cause floating labels
/// instantiated after it is created to be displayed
struct floating_label_context
{
	floating_label_context();
	~floating_label_context();
};

enum ALIGN { LEFT_ALIGN, CENTER_ALIGN, RIGHT_ALIGN };

enum LABEL_SCROLL_MODE { ANCHOR_LABEL_SCREEN, ANCHOR_LABEL_MAP };

class floating_label
{
public:
	floating_label(const std::string& text);

	void set_font_size(int font_size) {font_size_ = font_size;}

	// set the location on the screen to display the text.
	void set_position(double xpos, double ypos){
		xpos_ = xpos;
		ypos_ = ypos;
	}
	// set the amount to move the text each frame
	void set_move(double xmove, double ymove){
		xmove_ = xmove;
		ymove_ = ymove;
	}
	// set the number of frames to display the text for, or -1 to display until removed
	void set_lifetime(int lifetime) {
		lifetime_ = lifetime;
		alpha_change_ = -255 / lifetime_;
	}
	void set_color(const SDL_Color& color) {color_ = color;}
	void set_bg_color(const SDL_Color& bg_color) {
		bgcolor_ = bg_color;
		bgalpha_ = bg_color.unused;
	}
	void set_border_size(int border) {border_ = border;}
	// set width for word wrapping (use -1 to disable it)
	void set_width(int w) {width_ = w;}
	void set_height(int h) { height_ = h; }
	void set_clip_rect(const SDL_Rect& r) {clip_rect_ = r;}
	void set_alignment(ALIGN align) {align_ = align;}
	void set_scroll_mode(LABEL_SCROLL_MODE scroll) {scroll_ = scroll;}
	void use_markup(bool b) {use_markup_ = b;}

	void move(double xmove, double ymove);

	void draw(surface screen);
	void undraw(surface screen);

	surface create_surface();

	bool expired() const { return lifetime_ == 0; }

	void show(const bool value) { visible_ = value; }

	LABEL_SCROLL_MODE scroll() const { return scroll_; }

private:

	int xpos(size_t width) const;

	surface surf_, buf_;
	std::string text_;
	int font_size_;
	SDL_Color color_, bgcolor_;
	int bgalpha_;
	double xpos_, ypos_, xmove_, ymove_;
	int lifetime_;
	int width_, height_;
	SDL_Rect clip_rect_;
	int alpha_change_;
	bool visible_;
	font::ALIGN align_;
	int border_;
	LABEL_SCROLL_MODE scroll_;
	bool use_markup_;
};


/// add a label floating on the screen above everything else.
/// @returns a handle to the label which can be used with other label functions

int add_floating_label(const floating_label& flabel);


/// moves the floating label given by 'handle' by (xmove,ymove)
void move_floating_label(int handle, double xmove, double ymove);

/// moves all floating labels that have 'scroll_mode' set to ANCHOR_LABEL_MAP
void scroll_floating_labels(double xmove, double ymove);

/// removes the floating label given by 'handle' from the screen
void remove_floating_label(int handle);

/// hides or shows a floating label
void show_floating_label(int handle, bool show);

SDL_Rect get_floating_label_rect(int handle);

void draw_floating_labels(surface screen);
void undraw_floating_labels(surface screen);

bool load_font_config();

/** Returns the currently defined fonts. */
const t_string& get_font_families();

enum CACHE { CACHE_LOBBY, CACHE_GAME };
void cache_mode(CACHE mode);

}

#endif
