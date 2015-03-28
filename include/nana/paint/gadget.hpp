/*
 *	Graphics Gadget Implementation
 *	Copyright(C) 2003-2013 Jinhao(cnjinhao@hotmail.com)
 *
 *	Distributed under the Boost Software License, Version 1.0. 
 *	(See accompanying file LICENSE_1_0.txt or copy at 
 *	http://www.boost.org/LICENSE_1_0.txt)
 *
 *	@file: nana/paint/gadget.hpp
 */

#ifndef NANA_PAINT_GADGET_HPP
#define NANA_PAINT_GADGET_HPP
#include "graphics.hpp"
#include "image.hpp"
#include <nana/basic_types.hpp>

namespace nana
{
namespace paint
{
namespace gadget
{
	struct directions
	{
		enum t{to_east, to_southeast, to_south, to_southwest, to_west, to_northwest, to_north, to_northeast};
	};

	struct checkers
	{
		enum checker_t{radio, clasp};
	};

	void arrow_16_pixels(nana::paint::graphics&, int x, int y, unsigned color, uint32_t style, directions::t direction);
	void check_16_pixels(nana::paint::graphics&, int x, int y, unsigned style, unsigned state, bool checked);
	void close_16_pixels(nana::paint::graphics&, int x, int y, uint32_t style, uint32_t color);
	void checker(nana::paint::graphics&, int x, int y, uint32_t owner_pixels, uint32_t style, uint32_t color);
	void cross(nana::paint::graphics&, int x, int y, uint32_t size, uint32_t thickness, nana::color_t color);


	class check_renderer
	{
	public:
		typedef nana::gui::mouse_action mouse_action;
		enum checker_t{radio, clasp, blocker, checker_end};

		check_renderer();

		void render(graphics&, int x, int y, uint32_t width, uint32_t height, mouse_action, checker_t, bool checked);
		void open_background_image(const paint::image&);
		void set_image_state(mouse_action, checker_t, bool checked, const nana::rectangle& r);
		void clear_image_state();
		bool has_background_image() const;
		bool in_use(mouse_action, checker_t, bool checked) const;
	private:
		struct img_state_t
		{
			bool in_use;
			int x, y;
			uint32_t width, height;
		}imgstate_[checker_end][static_cast<int>(mouse_action::end)][2];

		paint::image image_;
	};

}//end namespace gadget
	
}//end namespace paint
}//end namespace nana

#endif
