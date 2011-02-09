/* $Id: marked-up_text.cpp 48153 2011-01-01 15:57:50Z mordante $ */
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

/**
 * @file
 * Support for simple markup in text (fonts, colors, images).
 * E.g. "@Victory" will be shown in green.
 */

#define GETTEXT_DOMAIN "wesnoth-lib"

#include "global.hpp"

#include "font.hpp"
#include "gettext.hpp"
#include "marked-up_text.hpp"
#include "serialization/string_utils.hpp"
#include "video.hpp"
#include "wml_exception.hpp"

namespace font {

const char LARGE_TEXT='*', SMALL_TEXT='`',
		   BOLD_TEXT='~',  NORMAL_TEXT='{',
		   NULL_MARKUP='^',
		   BLACK_TEXT='}', GRAY_TEXT='|',
           GOOD_TEXT='@',  BAD_TEXT='#',
           GREEN_TEXT='@', RED_TEXT='#',
           COLOR_TEXT='<', IMAGE='&';

const std::string weapon = "<245,230,193>",
		weapon_details = "<166,146,117>",
		unit_type = "<245,230,193>",
		race = "<166,146,117>";

const SDL_Color
	weapon_color = { 245, 230, 193, 255 },
	good_dmg_color = { 130, 240, 50, 255 },
	bad_dmg_color = { 250, 140, 80, 255 },
	weapon_details_color = { 166, 146, 117, 255 },
	unit_type_color = { 245, 230, 193, 255 },
	race_color = { 166, 146, 117, 255 };

const std::string weapon_numbers_sep = "–", weapon_details_sep = "–";

std::string::const_iterator parse_markup(std::string::const_iterator i1,
												std::string::const_iterator i2,
												int* font_size,
												SDL_Color* color, int* style)
{
	std::string::const_iterator i_start=i1;
	while(i1 != i2) {
		switch(*i1) {
		case '\\':
			// This must either be a quoted special character or a
			// quoted backslash - either way, remove leading backslash
			break;
		case BAD_TEXT:
			if (color) *color = BAD_COLOR;
			break;
		case GOOD_TEXT:
			if (color) *color = GOOD_COLOR;
			break;
		case NORMAL_TEXT:
			if (color) *color = NORMAL_COLOR;
			break;
		case BLACK_TEXT:
			if (color) *color = BLACK_COLOR;
			break;
		case GRAY_TEXT:
			if (color) *color = GRAY_COLOR;
			break;
		case LARGE_TEXT:
			if (font_size) *font_size += 2;
			break;
		case SMALL_TEXT:
			if (font_size) *font_size -= 2;
			break;
		case BOLD_TEXT:
			if (style) *style |= TTF_STYLE_BOLD;
			break;
		case NULL_MARKUP:
			return i1+1;
		case COLOR_TEXT:
			{
				std::string::const_iterator start = i1;
				// Very primitive parsing for rgb value
				// should look like <213,14,151>
				++i1;
				Uint8 red=0, green=0, blue=0, temp=0;
				while (i1 != i2 && *i1 >= '0' && *i1<='9') {
					temp*=10;
					temp += lexical_cast<int, char>(*i1);
					++i1;
				}
				red=temp;
				temp=0;
				if (i1 != i2 && ',' == (*i1)) {
					++i1;
					while(i1 != i2 && *i1 >= '0' && *i1<='9'){
						temp*=10;
						temp += lexical_cast<int, char>(*i1);
						++i1;
					}
					green=temp;
					temp=0;
				}
				if (i1 != i2 && ',' == (*i1)) {
					++i1;
					while(i1 != i2 && *i1 >= '0' && *i1<='9'){
						temp*=10;
						temp += lexical_cast<int, char>(*i1);
						++i1;
					}
				}
				blue=temp;
				if (i1 != i2 && '>' == (*i1)) {
					SDL_Color temp_color = {red, green, blue, 0};
					if (color) *color = temp_color;
				} else {
					// stop parsing and do not consume any chars
					return start;
				}
				if (i1 == i2) return i1;
				break;
			}
		default:
			return i1;
		}
		++i1;
	}
	return i1;
}

std::string del_tags(const std::string& text){
	int ignore_int;
	SDL_Color ignore_color;
	std::vector<std::string> lines = utils::split(text, '\n', 0);
	std::vector<std::string>::iterator line;
	for(line = lines.begin(); line != lines.end(); ++line) {
		std::string::const_iterator i1 = line->begin(),
			i2 = line->end();
		*line = std::string(parse_markup(i1,i2,&ignore_int,&ignore_color,&ignore_int),i2);
	}
	return utils::join(lines, "\n");
}

std::string color2markup(const SDL_Color &color)
{
	std::stringstream markup;
	// The RGB of SDL_Color are Uint8, we need to cast them to int.
	// Without cast, it gives their char equivalent.
	markup << "<"
		   << static_cast<int>(color.r) << ","
		   << static_cast<int>(color.g) << ","
		   << static_cast<int>(color.b) << ">";
	return markup.str();
}

std::string color2hexa(const SDL_Color &color)
{
	char buf[7];
	sprintf(buf, "%02x%02x%02x", color.r, color.g, color.b);
	return buf;
}

std::string span_color(const SDL_Color &color)
{
	return "<span foreground=\"#" + font::color2hexa(color) + "\">";
}

SDL_Rect text_area(const std::string& text, int size, int style)
{
	const SDL_Rect area = {0,0,10000,10000};
	return draw_text(NULL, area, size, font::NORMAL_COLOR, text, 0, 0, false, style);
}

SDL_Rect draw_text(surface dst, const SDL_Rect& area, int size,
                   const SDL_Color& color, const std::string& txt,
                   int x, int y, bool use_tooltips, int style)
{
	// Make sure there's always at least a space,
	// so we can ensure that we can return a rectangle for height
	static const std::string blank_text(" ");
	const std::string& text = txt.empty() ? blank_text : txt;

	SDL_Rect res;
	res.x = x;
	res.y = y;
	res.w = 0;
	res.h = 0;

	std::string::const_iterator i1 = text.begin();
	std::string::const_iterator i2 = std::find(i1,text.end(),'\n');
	for(;;) {
		SDL_Color col = color;
		int sz = size;
		int text_style = style;

		i1 = parse_markup(i1,i2,&sz,&col,&text_style);

		if(i1 != i2) {
			std::string new_string = utils::unescape(std::string(i1, i2));

			const SDL_Rect rect = draw_text_line(dst, area, sz, col, new_string, x, y, use_tooltips, text_style);
			if(rect.w > res.w) {
				res.w = rect.w;
			}

			res.h += rect.h;
			y += rect.h;
		}

		if(i2 == text.end()) {
			break;
		}

		i1 = i2+1;
		i2 = std::find(i1,text.end(),'\n');
	}

	return res;
}

SDL_Rect draw_text(CVideo* gui, const SDL_Rect& area, int size,
                   const SDL_Color& color, const std::string& txt,
                   int x, int y, bool use_tooltips, int style)
{
	return draw_text(gui != NULL ? gui->getSurface() : NULL, area, size, color, txt, x, y, use_tooltips, style);
}

bool is_format_char(char c)
{
	switch(c) {
	case LARGE_TEXT:
	case SMALL_TEXT:
	case GOOD_TEXT:
	case BAD_TEXT:
	case NORMAL_TEXT:
	case BLACK_TEXT:
	case GRAY_TEXT:
	case BOLD_TEXT:
	case NULL_MARKUP:
		return true;
	default:
		return false;
	}
}

bool is_cjk_char(const wchar_t c)
{
	/**
	 * You can check these range at http://unicode.org/charts/
	 * see the "East Asian Scripts" part.
	 * Notice that not all characters in that part is still in use today, so don't list them all here.
	 * Below are characters that I guess may be used in wesnoth translations.
	 */

	// cast to silence a windows warning (uses only 16bit for wchar_t)
	const unsigned int ch = static_cast<unsigned int>(c);

	//FIXME add range from Japanese-specific and Korean-specific section if you know the characters are used today.

	if (ch < 0x2e80) return false; // shorcut for common non-CJK

	return
		//Han Ideographs: all except Supplement
		(ch >= 0x4e00 && ch < 0x9fcf) ||
		(ch >= 0x3400 && ch < 0x4dbf) ||
		(ch >= 0x20000 && ch < 0x2a6df) ||
		(ch >= 0xf900 && ch < 0xfaff) ||
		(ch >= 0x3190 && ch < 0x319f) ||

		//Radicals: all except Ideographic Description
		(ch >= 0x2e80 && ch < 0x2eff) ||
		(ch >= 0x2f00 && ch < 0x2fdf) ||
		(ch >= 0x31c0 && ch < 0x31ef) ||

		//Chinese-specific: Bopomofo
		(ch >= 0x3000 && ch < 0x303f) ||

		//Japanese-specific: Halfwidth Katakana
		(ch >= 0xff00 && ch < 0xffef) ||

		//Japanese-specific: Hiragana, Katakana
		(ch >= 0x3040 && ch <= 0x309f) ||
		(ch >= 0x30a0 && ch <= 0x30ff) ||

		//Korean-specific: Hangul Syllables, Halfwidth Jamo
		(ch >= 0xac00 && ch < 0xd7af) ||
		(ch >= 0xff00 && ch < 0xffef);
}
static void cut_word(std::string& line, std::string& word, int font_size, int style, int max_width)
{
	std::string tmp = line;
	utils::utf8_iterator tc(word);
	bool first = true;

	for(;tc != utils::utf8_iterator::end(word); ++tc) {
		tmp.append(tc.substr().first, tc.substr().second);
		SDL_Rect tsize = line_size(tmp, font_size, style);
		if(tsize.w > max_width) {
			const std::string& w = word;
			if(line.empty() && first) {
				line += std::string(w.begin(), tc.substr().second);
				word = std::string(tc.substr().second, w.end());
			} else {
				line += std::string(w.begin(), tc.substr().first);
				word = std::string(tc.substr().first, w.end());
			}
			break;
		}
		first = false;
	}
}

namespace {

/*
 * According to Kinsoku-Shori, Japanese rules about line-breaking:
 *
 * * the following characters cannot begin a line (so we will never break before them):
 * 、。，．）〕］｝〉》」』】’”ゝゞヽヾ々？！：；ぁぃぅぇぉゃゅょゎァィゥェォャュョヮっヵッヶ・…ー
 *
 * * the following characters cannot end a line (so we will never break after them):
 * （〔［｛〈《「『【‘“
 *
 * Unicode range that concerns word wrap for Chinese:
 *   全角ASCII、全角中英文标点 (Fullwidth Character for ASCII, English punctuations and part of Chinese punctuations)
 *   http://www.unicode.org/charts/PDF/UFF00.pdf
 *   CJK 标点符号 (CJK punctuations)
 *   http://www.unicode.org/charts/PDF/U3000.pdf
 */
inline bool no_break_after(const wchar_t ch)
{
	return
		/**
		 * don't break after these Japanese characters
		 */
		ch == 0x2018 || ch == 0x201c || ch == 0x3008 || ch == 0x300a || ch == 0x300c ||
		ch == 0x300e || ch == 0x3010 || ch == 0x3014 || ch == 0xff08 || ch == 0xff3b ||
		ch == 0xff5b ||

		/**
		 * FIXME don't break after these Korean characters
		 */

		/**
		 * don't break after these Chinese characters
		 * contains left side of different kinds of brackets and quotes
		 */
		ch == 0x3014 || ch == 0x3016 || ch == 0x3008 || ch == 0x301a || ch == 0x3008 ||
		ch == 0x300a || ch == 0x300c || ch == 0x300e || ch == 0x3010 || ch == 0x301d;
}

inline bool no_break_before(const wchar_t ch)
{
	return
		/**
		 * don't break before these Japanese characters
		 */
		ch == 0x2019 || ch == 0x201d || ch == 0x2026 || ch == 0x3001 || ch == 0x3002 ||
		ch == 0x3005 || ch == 0x3009 || ch == 0x300b || ch == 0x300d || ch == 0x300f ||
		ch == 0x3011 || ch == 0x3015 || ch == 0x3041 || ch == 0x3043 || ch == 0x3045 ||
		ch == 0x3047 || ch == 0x3049 || ch == 0x3063 || ch == 0x3083 || ch == 0x3085 ||
		ch == 0x3087 || ch == 0x308e || ch == 0x309d || ch == 0x309e || ch == 0x30a1 ||
		ch == 0x30a3 || ch == 0x30a5 || ch == 0x30a7 || ch == 0x30a9 || ch == 0x30c3 ||
		ch == 0x30e3 || ch == 0x30e5 || ch == 0x30e7 || ch == 0x30ee || ch == 0x30f5 ||
		ch == 0x30f6 || ch == 0x30fb || ch == 0x30fc || ch == 0x30fd || ch == 0x30fe ||
		ch == 0xff01 || ch == 0xff09 || ch == 0xff0c || ch == 0xff0e || ch == 0xff1a ||
		ch == 0xff1b || ch == 0xff1f || ch == 0xff3d || ch == 0xff5d ||

		/**
		 * FIXME don't break before these Korean characters
		 */

		/**
		 * don't break before these Chinese characters
		 * contains
		 *   many Chinese punctuations that should not start a line
		 *   and right side of different kinds of brackets, quotes
		 */
		ch == 0x3001 || ch == 0x3002 || ch == 0x301c || ch == 0xff01 || ch == 0xff0c ||
		ch == 0xff0d || ch == 0xff0e || ch == 0xff1a || ch == 0xff1b || ch == 0xff1f ||
		ch == 0xff64 || ch == 0xff65 || ch == 0x3015 || ch == 0x3017 || ch == 0x3009 ||
		ch == 0x301b || ch == 0x3009 || ch == 0x300b || ch == 0x300d || ch == 0x300f ||
		ch == 0x3011 || ch == 0x301e;
}

inline bool break_before(const wchar_t ch)
{
	if(no_break_before(ch))
		return false;

	return is_cjk_char(ch);
}

inline bool break_after(const wchar_t ch)
{
	if(no_break_after(ch))
		return false;

	return is_cjk_char(ch);
}

} // end of anon namespace

std::string word_wrap_text(const std::string& unwrapped_text, int font_size,
	int max_width, int max_height, int max_lines, bool partial_line)
{
	VALIDATE(max_width > 0, _("The maximum text width is less than 1."));

	utils::utf8_iterator ch(unwrapped_text);
	std::string current_word;
	std::string current_line;
	size_t line_width = 0;
	size_t current_height = 0;
	bool line_break = false;
	bool first = true;
	bool start_of_line = true;
	std::string wrapped_text;
	std::string format_string;
	SDL_Color color;
	int font_sz = font_size;
	int style = TTF_STYLE_NORMAL;
	utils::utf8_iterator end = utils::utf8_iterator::end(unwrapped_text);

	while(1) {
		if(start_of_line) {
			line_width = 0;
			format_string.clear();
			while(ch != end && *ch < static_cast<wchar_t>(0x100)
					&& is_format_char(*ch) && !ch.next_is_end()) {

				format_string.append(ch.substr().first, ch.substr().second);
				++ch;
			}
			// We need to parse the special format characters
			// to give the proper font_size and style to line_size()
			font_sz = font_size;
			style = TTF_STYLE_NORMAL;
			parse_markup(format_string.begin(),format_string.end(),&font_sz,&color,&style);
			current_line.clear();
			start_of_line = false;
		}

		// If there is no current word, get one
		if(current_word.empty() && ch == end) {
			break;
		} else if(current_word.empty()) {
			if(*ch == ' ' || *ch == '\n') {
				current_word = *ch;
				++ch;
			} else {
				wchar_t previous = 0;
				for(;ch != utils::utf8_iterator::end(unwrapped_text) &&
						*ch != ' ' && *ch != '\n'; ++ch) {

					if(!current_word.empty() &&
							break_before(*ch) &&
							!no_break_after(previous))
						break;

					if(!current_word.empty() &&
							break_after(previous) &&
							!no_break_before(*ch))
						break;

					current_word.append(ch.substr().first, ch.substr().second);

					previous = *ch;
				}
			}
		}

		if(current_word == "\n") {
			line_break = true;
			current_word.clear();
			start_of_line = true;
		} else {

			const size_t word_width = line_size(current_word, font_sz, style).w;

			line_width += word_width;

			if(static_cast<long>(line_width) > max_width) {
				if (!partial_line && static_cast<long>(word_width) > max_width) {
					cut_word(current_line,
						current_word, font_sz, style, max_width);
				}
				if(current_word == " ")
					current_word = "";
				line_break = true;
			} else {
				current_line += current_word;
				current_word = "";
			}
		}

		if(line_break || (current_word.empty() && ch == end)) {
			SDL_Rect size = line_size(current_line, font_sz, style);
			if(max_height > 0 && current_height + size.h >= size_t(max_height)) {
				return wrapped_text;
			}

			if(!first) {
				wrapped_text += '\n';
			}

			wrapped_text += format_string + current_line;
			current_line.clear();
			line_width = 0;
			current_height += size.h;
			line_break = false;
			first = false;

			if(--max_lines == 0) {
				return wrapped_text;
			}
		}
	}
	return wrapped_text;
}

SDL_Rect draw_wrapped_text(CVideo* gui, const SDL_Rect& area, int font_size,
		     const SDL_Color& color, const std::string& text,
		     int x, int y, int max_width)
{
	std::string wrapped_text = word_wrap_text(text, font_size, max_width);
	return font::draw_text(gui, area, font_size, color, wrapped_text, x, y, false);
}

} // end namespace font

