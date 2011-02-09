/* $Id: canvas.cpp 48251 2011-01-10 20:38:37Z mordante $ */
/*
   Copyright (C) 2007 - 2011 by Mark de Wever <koraq@xs4all.nl>
   Part of the Battle for Wesnoth Project http://www.wesnoth.org/

   This program is free software; you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation; either version 2 of the License, or
   (at your option) any later version.
   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY.

   See the COPYING file for more details.
*/

/**
 * @file
 * Implementation of canvas.hpp.
 */

#define GETTEXT_DOMAIN "wesnoth-lib"

#include "gui/auxiliary/canvas.hpp"

#include "config.hpp"
#include "../../image.hpp"
#include "foreach.hpp"
#include "formatter.hpp"
#include "gettext.hpp"
#include "gui/auxiliary/formula.hpp"
#include "gui/auxiliary/log.hpp"
#include "gui/widgets/helper.hpp"
#include "../../text.hpp"
#include "wml_exception.hpp"

namespace gui2 {

namespace {

/*WIKI
 * @page = GUICanvasWML
 *
 * THIS PAGE IS AUTOMATICALLY GENERATED, DO NOT MODIFY DIRECTLY !!!
 *
 * = Canvas =
 *
 * A canvas is a blank drawing area on which the user can add various items.
 */

/*WIKI
 * @page = GUICanvasWML
 *
 * == Pre commit ==
 *
 * This section contains the pre commit functions. These functions will be
 * executed before the drawn canvas is applied on top of the normal
 * background. There should only be one pre commit section and its order
 * regarding the other shapes doesn't matter.
 *
 * The section can have one of the following subsections.
 *
 * === Blur ===
 *
 * Blurs the background before applying the canvas. This doesn't make sense
 * if the widget isn't semi-transparent.
 *
 * Keys:
 * @begin{table}{config}
 *     depth & unsigned & 0 &            The depth to blur. $
 * @end{table}
 */

/***** ***** ***** ***** ***** DRAWING PRIMITIVES ***** ***** ***** ***** *****/

/**
 * Draws a single pixel.
 *
 * @param start           The memory address which is the start of the surface
 *                        buffer to draw in.
 * @param color          The color of the pixel to draw.
 * @param w               The width of the surface.
 * @param x               The x coordinate of the pixel to draw.
 * @param y               The y coordinate of the pixel to draw.
 */
static void put_pixel(
		  const ptrdiff_t start
		, const Uint32 color
		, const unsigned w
		, const unsigned x
		, const unsigned y)
{
	*reinterpret_cast<Uint32*>(start + (y * w * 4) + x * 4) = color;
}

/**
 * Draws a line.
 *
 * @pre @p x2 >= @p x1
 *
 * @param canvas          The canvas to draw upon, the caller should lock the
 *                        surface before calling.
 * @param color           The color of the line to draw.
 * @param x1              The start x coordinate of the line to draw.
 * @param y1              The start y coordinate of the line to draw.
 * @param x2              The end x coordinate of the line to draw.
 * @param y2              The end y coordinate of the line to draw.
 */
static void draw_line(
		  surface& canvas
		, Uint32 color
		, unsigned x1
		, unsigned y1
		, const unsigned x2
		, unsigned y2)
{
	color = SDL_MapRGBA(canvas->format,
		((color & 0xFF000000) >> 24),
		((color & 0x00FF0000) >> 16),
		((color & 0x0000FF00) >> 8),
		((color & 0x000000FF)));

	ptrdiff_t start = reinterpret_cast<ptrdiff_t>(canvas->pixels);
	unsigned w = canvas->w;

	DBG_GUI_D << "Shape: draw line from "
			<< x1 << ',' << y1 << " to " << x2 << ',' << y2
			<< " canvas width " << w << " canvas height "
			<< canvas->h << ".\n";

	assert(static_cast<int>(x1) < canvas->w);
	assert(static_cast<int>(x2) < canvas->w);
	assert(static_cast<int>(y1) < canvas->h);
	assert(static_cast<int>(y2) < canvas->h);

	// use a special case for vertical lines
	if(x1 == x2) {
		if(y2 < y1) {
			std::swap(y1, y2);
		}

		for(unsigned y = y1; y <= y2; ++y) {
			put_pixel(start, color, w, x1, y);
		}
		return;
	}

	// use a special case for horizontal lines
	if(y1 == y2) {
		for(unsigned x  = x1; x <= x2; ++x) {
			put_pixel(start, color, w, x, y1);
		}
		return;
	}

	// Algorithm based on
	// http://de.wikipedia.org/wiki/Bresenham-Algorithmus#Kompakte_Variante
	// version of 26.12.2010.
	const int dx = x2 - x1; // precondition x2 >= x1
	const int dy = abs(static_cast<int>(y2 - y1));
	const int step_x = 1;
	const int step_y = y1 < y2 ? 1 : -1;
	int err = (dx > dy ? dx : -dy) / 2;
	int e2;

	for(;;){
		put_pixel(start, color, w, x1, y1);
		if(x1 == x2 && y1 == y2) {
			break;
		}
		e2 = err;
		if(e2 > -dx) {
			err -= dy;
			x1 += step_x;
		}
		if(e2 <  dy) {
			err += dx;
			y1 += step_y;
		}
	}
}

/***** ***** ***** ***** ***** LINE ***** ***** ***** ***** *****/

/** Definition of a line shape. */
class tline
	: public tcanvas::tshape
{
public:

	/**
	 * Constructor.
	 *
	 * @param cfg                 The config object to define the line see
	 *                            http://www.wesnoth.org/wiki/GUICanvasWML#Line
	 *                            for more info.
	 */
	explicit tline(const config& cfg);

	/** Implement shape::draw(). */
	void draw(surface& canvas
			, const game_logic::map_formula_callable& variables);

private:
	tformula<unsigned>
		x1_, /**< The start x coordinate of the line. */
		y1_, /**< The start y coordinate of the line. */
		x2_, /**< The end x coordinate of the line. */
		y2_; /**< The end y coordinate of the line. */

	/** The color of the line. */
	Uint32 color_;

	/**
	 * The thickness of the line.
	 *
	 * if the value is odd the x and y are the middle of the line.
	 * if the value is even the x and y are the middle of a line
	 * with width - 1. (0 is special case, does nothing.)
	 */
	unsigned thickness_;
};

tline::tline(const config& cfg)
	: x1_(cfg["x1"])
	, y1_(cfg["y1"])
	, x2_(cfg["x2"])
	, y2_(cfg["y2"])
	, color_(decode_color(get_renamed_config_attribute(
			  cfg
			, "colour"
			, "color"
			, "1.9.6")))
	, thickness_(cfg["thickness"])
{
/*WIKI
 * @page = GUICanvasWML
 *
 * == Line ==
 * Definition of a line. When drawing a line it doesn't get blended on the
 * surface but replaces the pixels instead. A blitting flag might be added later
 * if needed.
 *
 * Keys:
 * @begin{table}{config}
 *     x1 & f_unsigned & 0 &             The x coordinate of the startpoint. $
 *     y1 & f_unsigned & 0 &             The y coordinate of the startpoint. $
 *     x2 & f_unsigned & 0 &             The x coordinate of the endpoint. $
 *     y2 & f_unsigned & 0 &             The y coordinate of the endpoint. $
 *     color & color & "" &            The color of the line. $
 *     thickness & unsigned & 0 &      The thickness of the line if 0 nothing
 *                                     is drawn. $
 *     debug & string & "" &           Debug message to show upon creation$
 *                                     this message is not stored. $
 * @end{table}
 *
 * <span id="general_variables">Variables:</span>.
 * @begin{table}{formula}
 *     width & unsigned &                  The width of the canvas. $
 *     height & unsigned &                The height of the canvas. $
 *     text & tstring &                   The text to render on the widget. $
 *     text_maximum_width & unsigned &    The maximum width available for the text
 *                                     on the widget. $
 *     text_maximum_height & unsigned &   The maximum height available for the text
 *                                     on the widget. $
 *     text_wrap_mode & int  &             When the text doesn't fit in the
 *                                     available width there are several ways
 *                                     to fix that. This variable holds the
 *                                     best method. (NOTE this is a 'hidden'
 *                                     variable meant to copy state from a
 *                                     widget to its canvas so there's no
 *                                     reason to use this variable and thus
 *                                     its values are not listed and might
 *                                     change without further notice.) $
 *     text_alignment & h_align &         The way the text is aligned inside the
 *                                     canvas. $
 *@end{table}
 *
 * The size variables are copied to the window and will be determined at
 * runtime. This is needed since the main window can be resized and the dialog
 * needs to resize accordingly. The following variables are available:
 * @begin{table}{formula}
 *     screen_width & unsigned &        The usable width of the Wesnoth main
 *                                   window. $
 *     screen_height & unsigned &       The usable height of the Wesnoth main
 *                                   window. $
 *     gamemap_width & unsigned &       The usable width of the Wesnoth gamemap,
 *                                   if no gamemap shown it's the same value as
 *                                   screen_width. $
 *     gamemap_height & unsigned &     The usable height of the Wesnoth gamemap,
 *                                   if no gamemap shown it's the same value as
 *                                   screen_height. $
 * @end{table}
 *
 * Note when drawing the valid coordinates are:<br>
 * 0 -> width - 1 <br>
 * 0 -> height -1
 *
 * Drawing outside this area will result in unpredictable results including
 * crashing. (That should be fixed, when encountered.)
 */

/*WIKI - unclassified
 * This code can be used by a parser to generate the wiki page
 * structure
 * [tag name]
 * param type_info description
 *
 * param                               Name of the parameter.
 *
 * type_info = ( type = default_value) The info about a optional parameter.
 * type_info = ( type )                The info about a mandatory parameter
 * type_info = [ type_info ]           The info about a conditional parameter
 *                                     description should explain the reason.
 *
 * description                         Description of the parameter.
 *
 *
 *
 *
 * Formulas are a function between brackets, that way the engine can see whether
 * there is standing a plain number or a formula eg:
 * 0     A value of zero
 * (0)   A formula returning zero
 *
 * When formulas are available the text should state the available variables
 * which are available in that function.
 */

/*WIKI
 * @page = GUIVariable
 *
 * THIS PAGE IS AUTOMATICALLY GENERATED, DO NOT MODIFY DIRECTLY !!!
 *
 * = Variables =
 *
 * In various parts of the GUI there are several variables types in use. This
 * page describes them.
 *
 * == Simple types ==
 *
 * The simple types are types which have one value or a short list of options.
 *
 * @begin{table}{variable_types}
 *     unsigned &                       Unsigned number (positive whole numbers
 *                                     and zero). $
 *     f_unsigned &                     Unsigned number or formula returning an
 *                                     unsigned number. $
 *     int &                            Signed number (whole numbers). $
 *     f_int &                          Signed number or formula returning an
 *                                     signed number. $
 *     bool &                           A boolean value accepts the normal
 *                                     values as the rest of the game. $
 *     f_bool &                         Boolean value or a formula returning a
 *                                     boolean value. $
 *     string &                        A text. $
 *     tstring &                        A translatable string. $
 *     f_tstring &                      Formula returning a translatable string. $
 *
 *     color &                          A string which contains the color, this
 *                                     a group of 4 numbers between 0 and 255
 *                                     separated by a comma. The numbers are red
 *                                     component, green component, blue
 *                                     component and alpha. A color of 0 is not
 *                                     available. An alpha of 255 is fully
 *                                     transparent. Omitted values are set to 0. $
 *
 *     font_style &                     A string which contains the style of the
 *                                     font:
 *                                     @* normal    normal font
 *                                     @* bold      bold font
 *                                     @* italic    italic font
 *                                     @* underline underlined font
 *                                     @-Since SDL has problems combining these
 *                                     styles only one can be picked. Once SDL
 *                                     will allow multiple options, this type
 *                                     will be transformed to a comma separated
 *                                     list. If empty we default to the normal
 *                                     style. $
 *
 *     v_align &                        Vertical alignment; how an item is
 *                                     aligned vertically in the available
 *                                     space. Possible values:
 *                                     @* top    aligned at the top
 *                                     @* bottom aligned at the bottom
 *                                     @* center centered
 *                                     @-When nothing is set or an another
 *                                     value as in the list the item is
 *                                     centred. $
 *
 *     h_align &                       Horizontal alignment; how an item is
 *                                     aligned horizontal in the available
 *                                     space. Possible values:
 *                                     @* left   aligned at the left side
 *                                     @* right  aligned at the right side
 *                                     @* center centered $
 *
 *     f_h_align &                      A horizontal alignment or a formula
 *                                     returning a horizontal alignment.$
 *
 *     border &                         Comma separated list of borders to use.
 *                                     Possible values:
 *                                     @* left   border at the left side
 *                                     @* right  border at the right side
 *                                     @* top    border at the top
 *                                     @* bottom border at the bottom
 *                                     @* all    alias for "left, right, top,
 *                                     bottom" $
 *
 *     scrollbar_mode &                 How to show the scrollbar of a widget.
 *                                     Possible values:
 *                                     @* always       The scrollbar is always
 *                                     shown, regardless whether it's required
 *                                     or not.
 *                                     @* never        The scrollbar is never
 *                                     shown, even not when needed. (Note when
 *                                     setting this mode dialogs might
 *                                     not properly fit anymore).
 *                                     @* auto         Shows the scrollbar when
 *                                     needed. The widget will reserve space for
 *                                     the scrollbar, but only show when needed.
 *                                     @* initial_auto Like auto, but when the
 *                                     scrollbar is not needed the space is not
 *                                     reserved.
 *                                     @-Use auto when the list can be changed
 *                                     dynamically eg the game list in the
 *                                     lobby. For optimization you can also
 *                                     use auto when you really expect a
 *                                     scrollbar, but don't want it to be shown
 *                                     when not needed eg the language list
 *                                     will need a scrollbar on most screens. $
 *
 *     resize_mode &                    Determines how an image is resized.
 *                                     Possible values:
 *                                     @* scale        The image is scaled.
 *                                     @* stretch      The first row or column
 *                                     of pixels is copied over the entire
 *                                     image. (Can only be used to scale resize
 *                                     in one direction, else falls
 *                                     back to scale.)
 *                                     @* tile         The image is placed
 *                                     several times until the entire surface
 *                                     is filled. The last images are
 *                                     truncated. $
 * @end{table}
 *
 * == Section types ==
 *
 * For more complex parts, there are sections. Sections contain of several
 * lines of WML and can have sub sections. For example a grid has sub sections
 * which contain various widgets. Here's the list of sections.
 *
 * @begin{table}{variable_types}
 *     section &                        A generic section. The documentation
 *                                     about the section should describe the
 *                                     section in further detail. $
 *
 *     grid &                           A grid contains several widgets. (TODO
 *                                     add link to generic grid page.) $
 * @end{table}
 */

	const std::string& debug = (cfg["debug"]);
	if(!debug.empty()) {
		DBG_GUI_P << "Line: found debug message '" << debug << "'.\n";
	}
}

void tline::draw(surface& canvas
		, const game_logic::map_formula_callable& variables)
{
	/**
	 * @todo formulas are now recalculated every draw cycle which is a bit silly
	 * unless there has been a resize. So to optimize we should use an extra
	 * flag or do the calculation in a separate routine.
	 */

	const unsigned x1 = x1_(variables);
	const unsigned y1 = y1_(variables);
	const unsigned x2 = x2_(variables);
	const unsigned y2 = y2_(variables);

	DBG_GUI_D << "Line: draw from "
			<< x1 << ',' << y1 << " to " << x2 << ',' << y2
			<< " canvas size " << canvas->w << ',' << canvas->h << ".\n";

	VALIDATE(
			  static_cast<int>(x1) < canvas->w
				&& static_cast<int>(x2) < canvas->w
				&& static_cast<int>(y1) < canvas->h
				&& static_cast<int>(y2) < canvas->h
			, _("Line doesn't fit on canvas."));

	// @todo FIXME respect the thickness.

	// now draw the line we use Bresenham's algorithm, which doesn't
	// support antialiasing. The advantage is that it's easy for testing.

	// lock the surface
	surface_lock locker(canvas);
	if(x1 > x2) {
		// invert points
		draw_line(canvas, color_, x2, y2, x1, y1);
	} else {
		draw_line(canvas, color_, x1, y1, x2, y2);
	}
}

/***** ***** ***** ***** ***** Rectangle ***** ***** ***** ***** *****/

/** Definition of a rectangle shape. */
class trectangle
	: public tcanvas::tshape
{
public:

	/**
	 * Constructor.
	 *
	 * @param cfg                 The config object to define the rectangle see
	 *                            http://www.wesnoth.org/wiki/GUICanvasWML#Rectangle
	 *                            for more info.
	 */
	explicit trectangle(const config& cfg);

	/** Implement shape::draw(). */
	void draw(surface& canvas
			, const game_logic::map_formula_callable& variables);

private:
	tformula<unsigned>
		x_, /**< The x coordinate of the rectangle. */
		y_, /**< The y coordinate of the rectangle. */
		w_, /**< The width of the rectangle. */
		h_; /**< The height of the rectangle. */

	/**
	 * Border thickness.
	 *
	 * If 0 the fill color is used for the entire widget.
	 */
	unsigned border_thickness_;

	/**
	 * The border color of the rectangle.
	 *
	 * If the color is fully transparent the border isn't drawn.
	 */
	Uint32 border_color_;

	/**
	 * The border color of the rectangle.
	 *
	 * If the color is fully transparent the rectangle won't be filled.
	 */
	Uint32 fill_color_;
};

trectangle::trectangle(const config& cfg)
	: x_(cfg["x"])
	, y_(cfg["y"])
	, w_(cfg["w"])
	, h_(cfg["h"])
	, border_thickness_(cfg["border_thickness"])
	, border_color_(decode_color(get_renamed_config_attribute(
			  cfg
			, "border_colour"
			, "border_color"
			, "1.9.6.")))
	, fill_color_(decode_color(get_renamed_config_attribute(
			  cfg
			, "fill_colour"
			, "fill_color"
			, "1.9.6")))
{
/*WIKI
 * @page = GUICanvasWML
 *
 * == Rectangle ==
 *
 * Definition of a rectangle. When drawing a rectangle it doesn't get blended on
 * the surface but replaces the pixels instead. A blitting flag might be added
 * later if needed.
 *
 * Keys:
 * @begin{table}{config}
 *     x & f_unsigned & 0 &              The x coordinate of the top left corner. $
 *     y & f_unsigned & 0 &              The y coordinate of the top left corner. $
 *     w & f_unsigned & 0 &              The width of the rectangle. $
 *     h & f_unsigned & 0 &              The height of the rectangle. $
 *     border_thickness & unsigned & 0 & The thickness of the border if the
 *                                     thickness is zero it's not drawn. $
 *     border_color & color & "" &     The color of the border if empty it's
 *                                     not drawn. $
 *     fill_color & color & "" &       The color of the interior if omitted
 *                                     it's not drawn. $
 *     debug & string & "" &           Debug message to show upon creation
 *                                     this message is not stored. $
 * @end{table}
 * Variables:
 * See [[#general_variables|Line]].
 *
 */
	if(border_color_ == 0) {
		border_thickness_ = 0;
	}

	const std::string& debug = (cfg["debug"]);
	if(!debug.empty()) {
		DBG_GUI_P << "Rectangle: found debug message '" << debug << "'.\n";
	}
}

void trectangle::draw(surface& canvas
		, const game_logic::map_formula_callable& variables)
{
	/**
	 * @todo formulas are now recalculated every draw cycle which is a  bit
	 * silly unless there has been a resize. So to optimize we should use an
	 * extra flag or do the calculation in a separate routine.
	 */
	const unsigned x = x_(variables);
	const unsigned y = y_(variables);
	const unsigned w = w_(variables);
	const unsigned h = h_(variables);

	DBG_GUI_D << "Rectangle: draw from " << x << ',' << y
			<< " width " << w << " height " << h
			<< " canvas size " << canvas->w << ',' << canvas->h << ".\n";

	VALIDATE(
			  static_cast<int>(x) < canvas->w
				&& static_cast<int>(x + w) <= canvas->w
				&& static_cast<int>(y) < canvas->h
				&& static_cast<int>(y + h) <= canvas->h
			, _("Rectangle doesn't fit on canvas."));


	surface_lock locker(canvas);

	// draw the border
	for(unsigned i = 0; i < border_thickness_; ++i) {

		const unsigned left = x + i;
		const unsigned right = left + w - (i * 2) - 1;
		const unsigned top = y + i;
		const unsigned bottom = top + h - (i * 2) - 1;

		// top horizontal (left -> right)
		draw_line(canvas, border_color_, left, top, right, top);

		// right vertical (top -> bottom)
		draw_line(canvas, border_color_, right, top, right, bottom);

		// bottom horizontal (left -> right)
		draw_line(canvas, border_color_, left, bottom, right, bottom);

		// left vertical (top -> bottom)
		draw_line(canvas, border_color_, left, top, left, bottom);
	}

	// The fill_rect_alpha code below fails, can't remember the exact cause
	// so use the slow line drawing method to fill the rect.
	if(fill_color_) {

		const unsigned left = x + border_thickness_;
		const unsigned right = left + w - (2 * border_thickness_) - 1;
		const unsigned top = y + border_thickness_;
		const unsigned bottom = top + h - (2 * border_thickness_);

		for(unsigned i = top; i < bottom; ++i) {

			draw_line(canvas, fill_color_, left, i, right, i);
		}
	}
}

/***** ***** ***** ***** ***** IMAGE ***** ***** ***** ***** *****/

/** Definition of an image shape. */
class timage
	: public tcanvas::tshape
{
public:

	/**
	 * Constructor.
	 *
	 * @param cfg                 The config object to define the image see
	 *                            http://www.wesnoth.org/wiki/GUICanvasWML#Image
	 *                            for more info.
	 */
	explicit timage(const config& cfg);

	/** Implement shape::draw(). */
	void draw(surface& canvas
			, const game_logic::map_formula_callable& variables);

private:
	tformula<unsigned>
		x_, /**< The x coordinate of the image. */
		y_, /**< The y coordinate of the image. */
		w_, /**< The width of the image. */
		h_; /**< The height of the image. */

	/** Contains the size of the image. */
	SDL_Rect src_clip_;

	/** The image is cached in this surface. */
	surface image_;

	/**
	 * Name of the image.
	 *
	 * This value is only used when the image name is a formula. If it isn't a
	 * formula the image will be loaded at construction. If a formula it will
	 * be loaded every draw cycles. This allows 'changing' images.
	 */
	tformula<std::string> image_name_;

	/**
	 * Determines the way an image will be resized.
	 *
	 * If the image is smaller is needed it needs to resized, how is determined
	 * by the value of this enum.
	 */
	enum tresize_mode {
		  scale
		, stretch
		, tile
	};

	/** Converts a string to a resize mode. */
	tresize_mode get_resize_mode(const std::string& resize_mode);

	/** The resize mode for an image. */
	tresize_mode resize_mode_;

	/** Mirror the image over the vertical axis. */
	tformula<bool> vertical_mirror_;
};

timage::timage(const config& cfg)
	: x_(cfg["x"])
	, y_(cfg["y"])
	, w_(cfg["w"])
	, h_(cfg["h"])
	, src_clip_()
	, image_()
	, image_name_(cfg["name"])
	, resize_mode_(get_resize_mode(cfg["resize_mode"]))
	, vertical_mirror_(cfg["vertical_mirror"])
{
/*WIKI
 * @page = GUICanvasWML
 *
 * == Image ==
 * Definition of an image.
 *
 * Keys:
 * @begin{table}{config}
 *     x & f_unsigned & 0 &              The x coordinate of the top left corner. $
 *     y & f_unsigned & 0 &              The y coordinate of the top left corner. $
 *     w & f_unsigned & 0 &              The width of the image, if not zero the
 *                                     image will be scaled to the desired
 *                                     width. $
 *     h & f_unsigned & 0 &              The height of the image, if not zero the
 *                                     image will be scaled to the desired
 *                                     height. $
 *     resize_mode & resize_mode & scale &
 *                                     Determines how an image is scaled to fit
 *                                     the wanted size. $
 *     vertical_mirror & f_bool & false &
 *                                     Mirror the image over the vertical axis. $
 *     name & f_string & "" &            The name of the image. $
 *     debug & string & "" &           Debug message to show upon creation
 *                                     this message is not stored. $
 *
 * @end{table}
 * Variables:
 * @begin{table}{formula}
 *     image_width & unsigned &            The width of the image, either the
 *                                      requested width or the natural width of
 *                                      the image. This value can be used to set
 *                                      the x (or y) value of the image. (This
 *                                      means x and y are evaluated after the
 *                                      width and height.) $
 *     image_height & unsigned &           The height of the image, either the
 *                                      requested height or the natural height
 *                                      of the image. This value can be used to
 *                                      set the y (or x) value of the image.
 *                                      (This means x and y are evaluated after
 *                                      the width and height.) $
 *     image_original_width & unsigned &   The width of the image as stored on
 *                                      disk, can be used to set x or w
 *                                      (also y and h can be set). $
 *     image_original_height & unsigned &  The height of the image as stored on
 *                                      disk, can be used to set y or h
 *                                      (also x and y can be set). $
 * @end{table}
 * Also the general variables are available, see [[#general_variables|Line]].
 */

	const std::string& debug = (cfg["debug"]);
	if(!debug.empty()) {
		DBG_GUI_P << "Image: found debug message '" << debug << "'.\n";
	}
}

void timage::draw(surface& canvas
		, const game_logic::map_formula_callable& variables)
{
	DBG_GUI_D << "Image: draw.\n";

	/**
	 * @todo formulas are now recalculated every draw cycle which is a  bit
	 * silly unless there has been a resize. So to optimize we should use an
	 * extra flag or do the calculation in a separate routine.
	 */
	const std::string& name = image_name_(variables);

	if(name.empty()) {
		DBG_GUI_D
				<< "Image: formula returned no value, will not be drawn.\n";
		return;
	}

	/*
	 * The locator might return a different surface for every call so we can't
	 * cache the output, also not if no formula is used.
	 */
	surface tmp(image::get_image(image::locator(name)));

	if(!tmp) {
		ERR_GUI_D << "Image: '" << name
				<< "' not found and won't be drawn.\n";
		return;
	}

	image_.assign(make_neutral_surface(tmp));
	assert(image_);
	src_clip_ = ::create_rect(0, 0, image_->w, image_->h);

	game_logic::map_formula_callable local_variables(variables);
	local_variables.add("image_original_width", variant(image_->w));
	local_variables.add("image_original_height", variant(image_->h));

	unsigned w = w_(local_variables);
	VALIDATE_WITH_DEV_MESSAGE(
			  static_cast<int>(w) >= 0
			, _("Image doesn't fit on canvas.")
			, (formatter() << "Image '" << name
				<< "', w = " << static_cast<int>(w) << ".").str());

	unsigned h = h_(local_variables);
	VALIDATE_WITH_DEV_MESSAGE(
			  static_cast<int>(h) >= 0
			, _("Image doesn't fit on canvas.")
			, (formatter() << "Image '" << name
				<< "', h = " << static_cast<int>(h) << ".").str());

	local_variables.add("image_width", variant(w ? w : image_->w));
	local_variables.add("image_height", variant(h ? h : image_->h));

	const unsigned x = x_(local_variables);
	VALIDATE_WITH_DEV_MESSAGE(
			  static_cast<int>(x) >= 0
			, _("Image doesn't fit on canvas.")
			, (formatter() << "Image '" << name
				<< "', x = " << static_cast<int>(x) << ".").str());

	const unsigned y = y_(local_variables);
	VALIDATE_WITH_DEV_MESSAGE(
			  static_cast<int>(y) >= 0
			, _("Image doesn't fit on canvas.")
			, (formatter() << "Image '" << name
				<< "', y = " << static_cast<int>(y) << ".").str());

	// Copy the data to local variables to avoid overwriting the originals.
	SDL_Rect src_clip = src_clip_;
	SDL_Rect dst_clip = ::create_rect(x, y, 0, 0);
	surface surf;

	// Test whether we need to scale and do the scaling if needed.
	if(w || h) {
		bool done = false;
		bool stretch_image = (resize_mode_ == stretch) && (!!w ^ !!h);
		if(!w) {
			if(stretch_image) {
				DBG_GUI_D << "Image: vertical stretch from " << image_->w
						<< ',' << image_->h << " to a height of " << h << ".\n";

				surf = stretch_surface_vertical(image_, h, false);
				done = true;
			}
			w = image_->w;
		}

		if(!h) {
			if(stretch_image) {
				DBG_GUI_D << "Image: horizontal stretch from " << image_->w
						<< ',' << image_->h << " to a width of " << w << ".\n";

				surf = stretch_surface_horizontal(image_, w, false);
				done = true;
			}
			h = image_->h;
		}

		if(!done) {

			if(resize_mode_ == tile) {
				DBG_GUI_D << "Image: tiling from " << image_->w
						<< ',' << image_->h << " to " << w << ',' << h << ".\n";

				const int columns = (w + image_->w - 1) / image_->w;
				const int rows = (h + image_->h - 1) / image_->h;
				surf = create_neutral_surface(w, h);

				for(int x = 0; x < columns; ++x) {
					for(int y = 0; y < rows; ++y) {
						const SDL_Rect dest = ::create_rect(
								  x * image_->w
								, y * image_->h
								, 0
								, 0);
						blit_surface(image_, NULL, surf, &dest);
					}
				}

			} else {
				if(resize_mode_ == stretch) {
					ERR_GUI_D << "Image: failed to stretch image, "
							"fall back to scaling.\n";
				}

				DBG_GUI_D << "Image: scaling from " << image_->w
						<< ',' << image_->h << " to " << w << ',' << h << ".\n";

				surf = scale_surface(image_, w, h, false);
			}
		}
		src_clip.w = w;
		src_clip.h = h;
	} else {
		surf = image_;
	}

	if(vertical_mirror_(local_variables)) {
		surf = flip_surface(surf, false);
	}

	blit_surface(surf, &src_clip, canvas, &dst_clip);
}

timage::tresize_mode timage::get_resize_mode(const std::string& resize_mode)
{
	if(resize_mode == "tile") {
		return timage::tile;
	} else if(resize_mode == "stretch") {
		return timage::stretch;
	} else {
		if(!resize_mode.empty() && resize_mode != "scale") {
			ERR_GUI_E << "Invalid resize mode '"
					<< resize_mode << "' falling back to 'scale'.\n";
		}
		return timage::scale;
	}
}

/***** ***** ***** ***** ***** TEXT ***** ***** ***** ***** *****/

/** Definition of a text shape. */
class ttext
	: public tcanvas::tshape
{
public:

	/**
	 * Constructor.
	 *
	 * @param cfg                 The config object to define the text see
	 *                            http://www.wesnoth.org/wiki/GUICanvasWML#Text
	 *                            for more info.
	 */
	explicit ttext(const config& cfg);

	/** Implement shape::draw(). */
	void draw(surface& canvas
			, const game_logic::map_formula_callable& variables);

private:
	tformula<unsigned>
		x_, /**< The x coordinate of the text. */
		y_, /**< The y coordinate of the text. */
		w_, /**< The width of the text. */
		h_; /**< The height of the text. */

	/** The font size of the text. */
	unsigned font_size_;

	/** The style of the text. */
	unsigned font_style_;

	/** The alignment of the text. */
	tformula<PangoAlignment> text_alignment_;

	/** The color of the text. */
	Uint32 color_;

	/** The text to draw. */
	tformula<t_string> text_;

	/** The text markup switch of the text. */
	tformula<bool> text_markup_;

	/** The maximum width for the text. */
	tformula<int> maximum_width_;

	/** The maximum height for the text. */
	tformula<int> maximum_height_;
};

ttext::ttext(const config& cfg)
	: x_(cfg["x"])
	, y_(cfg["y"])
	, w_(cfg["w"])
	, h_(cfg["h"])
	, font_size_(cfg["font_size"])
	, font_style_(decode_font_style(cfg["font_style"]))
	, text_alignment_(cfg["text_alignment"])
	, color_(decode_color(get_renamed_config_attribute(
			  cfg
			, "colour"
			, "color"
			, "1.9.6")))
	, text_(cfg["text"])
	, text_markup_(cfg["text_markup"], false)
	, maximum_width_(cfg["maximum_width"], -1)
	, maximum_height_(cfg["maximum_height"], -1)
{

/*WIKI
 * @page = GUICanvasWML
 *
 * == Text ==
 * Definition of text.
 *
 * Keys:
 * @begin{table}{config}
 *     x & f_unsigned & 0 &              The x coordinate of the top left corner. $
 *     y & f_unsigned & 0 &              The y coordinate of the top left corner. $
 *     w & f_unsigned & 0 &              The width of the text's bounding
 *                                     rectangle. $
 *     h & f_unsigned & 0 &              The height of the text's bounding
 *                                     rectangle. $
 *     font_size & unsigned & &           The size of the text font. $
 *     font_style & font_style & "" &    The style of the text. $
 *     text_alignment & f_h_align & "left" &
 *                                     The alignment of the text. $
 *     color & color & "" &            The color of the text. $
 *     text & f_tstring & "" &           The text to draw (translatable). $
 *     text_markup & f_bool & false &    Can the text have markup? $
 *     maximum_width & f_int & -1 &      The maximum width the text is allowed to
 *                                     be. $
 *     maximum_height & f_int & -1 &     The maximum height the text is allowed
 *                                     to be. $
 *     debug & string & "" &           Debug message to show upon creation
 *                                     this message is not stored. $
 * @end{table}
 * NOTE alignment could only be done with the formulas, but now with the
 * text_alignment flag as well, older widgets might still use the formulas and
 * not all widgets may expose the text alignment yet and when exposed not use
 * it yet.
 *
 * Variables:
 * @begin{table}{formula}
 *     text_width & unsigned &            The width of the rendered text. $
 *     text_height & unsigned &           The height of the rendered text. $
 * @end{table}
 * Also the general variables are available, see [[#general_variables|Line]].
 */

	VALIDATE(font_size_, _("Text has a font size of 0."));

	const std::string& debug = (cfg["debug"]);
	if(!debug.empty()) {
		DBG_GUI_P << "Text: found debug message '" << debug << "'.\n";
	}
}

void ttext::draw(surface& canvas
		, const game_logic::map_formula_callable& variables)
{
	assert(variables.has_key("text"));

	// We first need to determine the size of the text which need the rendered
	// text. So resolve and render the text first and then start to resolve
	// the other formulas.
	const t_string text = text_(variables);

	if(text.empty()) {
		DBG_GUI_D << "Text: no text to render, leave.\n";
		return;
	}

	static font::ttext text_renderer;
	text_renderer.set_text(text, text_markup_(variables));

	text_renderer.set_font_size(font_size_)
			.set_font_style(font_style_)
			.set_alignment(text_alignment_(variables))
			.set_foreground_color(color_)
			.set_maximum_width(maximum_width_(variables))
			.set_maximum_height(maximum_height_(variables))
			.set_ellipse_mode(variables.has_key("text_wrap_mode")
				? static_cast<PangoEllipsizeMode>
					(variables.query_value("text_wrap_mode").as_int())
				: PANGO_ELLIPSIZE_END);

	surface surf = text_renderer.render();
	if(surf->w == 0) {
		DBG_GUI_D  << "Text: Rendering '"
				<< text << "' resulted in an empty canvas, leave.\n";
		return;
	}

	game_logic::map_formula_callable local_variables(variables);
	local_variables.add("text_width", variant(surf->w));
	local_variables.add("text_height", variant(surf->h));
/*
	std::cerr << "Text: drawing text '" << text
		<< " maximum width " << maximum_width_(variables)
		<< " maximum height " << maximum_height_(variables)
		<< " text width " << surf->w
		<< " text height " << surf->h;
*/
	///@todo formulas are now recalculated every draw cycle which is a
	// bit silly unless there has been a resize. So to optimize we should
	// use an extra flag or do the calculation in a separate routine.

	const unsigned x = x_(local_variables);
	const unsigned y = y_(local_variables);
	const unsigned w = w_(local_variables);
	const unsigned h = h_(local_variables);

	DBG_GUI_D << "Text: drawing text '" << text
			<< "' drawn from " << x << ',' << y
			<< " width " << w << " height " << h
			<< " canvas size " << canvas->w << ',' << canvas->h << ".\n";

	VALIDATE(static_cast<int>(x) < canvas->w && static_cast<int>(y) < canvas->h
			, _("Text doesn't start on canvas."));

	// A text might be to long and will be clipped.
	if(surf->w > static_cast<int>(w)) {
		WRN_GUI_D << "Text: text is too wide for the "
				"canvas and will be clipped.\n";
	}

	if(surf->h > static_cast<int>(h)) {
		WRN_GUI_D << "Text: text is too high for the "
				"canvas and will be clipped.\n";
	}

	SDL_Rect dst = ::create_rect(x, y, canvas->w, canvas->h);
	blit_surface(surf, 0, canvas, &dst);
}

} // namespace

/***** ***** ***** ***** ***** CANVAS ***** ***** ***** ***** *****/

tcanvas::tcanvas()
	: shapes_()
	, blur_depth_(0)
	, w_(0)
	, h_(0)
	, canvas_()
	, variables_()
	, dirty_(true)
{
}

void tcanvas::draw(const bool force)
{
	log_scope2(log_gui_draw, "Canvas: drawing.");
	if(!dirty_ && !force) {
		DBG_GUI_D << "Canvas: nothing to draw.\n";
		return;
	}

	if(dirty_) {
		get_screen_size_variables(variables_);
		variables_.add("width",variant(w_));
		variables_.add("height",variant(h_));
	}

	// create surface
	DBG_GUI_D << "Canvas: create new empty canvas.\n";
	canvas_.assign(create_neutral_surface(w_, h_));

	// draw items
	for(std::vector<tshape_ptr>::iterator itor =
			shapes_.begin(); itor != shapes_.end(); ++itor) {
		log_scope2(log_gui_draw, "Canvas: draw shape.");

		(*itor)->draw(canvas_, variables_);
	}

	dirty_ = false;
}

void tcanvas::blit(surface& surf, SDL_Rect rect)
{
	draw();

	if(blur_depth_) {
		if(surf->format->BitsPerPixel == 32) {
			blur_surface(surf, rect, blur_depth_);
		} else {
			// Can't directly blur the surface if not 32 bpp.
			SDL_Rect r = rect;
			///@todo we should use: get_surface_portion(surf, r, false)
			///no need to optimize format, since blur_surface will undo it
			surface s = get_surface_portion(surf, r, true);
			s = blur_surface(s, blur_depth_);
			sdl_blit(s, NULL, surf, &rect);
		}
	}

	sdl_blit(canvas_, NULL, surf, &rect);
}

void tcanvas::parse_cfg(const config& cfg)
{
	log_scope2(log_gui_parse, "Canvas: parsing config.");
	shapes_.clear();

	foreach(const config::any_child& shape, cfg.all_children_range()) {
		const std::string &type = shape.key;
		const config &data = shape.cfg;

		DBG_GUI_P << "Canvas: found shape of the type " << type << ".\n";

		if(type == "line") {
			shapes_.push_back(new tline(data));
		} else if(type == "rectangle") {
			shapes_.push_back(new trectangle(data));
		} else if(type == "image") {
			shapes_.push_back(new timage(data));
		} else if(type == "text") {
			shapes_.push_back(new ttext(data));
		} else if(type == "pre_commit") {

			/* note this should get split if more preprocessing is used. */
			foreach(const config::any_child& function,
					data.all_children_range()) {

				if(function.key == "blur") {
					blur_depth_ = function.cfg["depth"];
				} else {
					ERR_GUI_P << "Canvas: found a pre commit function"
							<< " of an invalid type " << type << ".\n";
				}
			}

		} else {
			ERR_GUI_P << "Canvas: found a shape of an invalid type "
					<< type << ".\n";

			assert(false);
		}
	}
}

/***** ***** ***** ***** ***** SHAPE ***** ***** ***** ***** *****/

} // namespace gui2
/*WIKI
 * @page = GUICanvasWML
 * @order = ZZZZZZ_footer
 *
 * [[Category: WML Reference]]
 * [[Category: GUI WML Reference]]
 * [[Category: Generated]]
 *
 */

/*WIKI
 * @page = GUIVariable
 * @order = ZZZZZZ_footer
 *
 * [[Category: WML Reference]]
 * [[Category: GUI WML Reference]]
 * [[Category: Generated]]
 *
 */
