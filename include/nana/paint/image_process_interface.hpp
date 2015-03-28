/*
 *	Image Processing Interfaces
 *	Copyright(C) 2003-2013 Jinhao(cnjinhao@hotmail.com)
 *
 *	Distributed under the Boost Software License, Version 1.0. 
 *	(See accompanying file LICENSE_1_0.txt or copy at 
 *	http://www.boost.org/LICENSE_1_0.txt)
 *
 *	@file: nana/paint/image_process_interface.hpp
 */

#ifndef NANA_PAINT_IMAGE_PROCESS_INTERFACE_HPP
#define NANA_PAINT_IMAGE_PROCESS_INTERFACE_HPP

#include <nana/basic_types.hpp>
#include <nana/paint/pixel_buffer.hpp>

namespace nana
{
	namespace paint
	{
		namespace image_process
		{
			class stretch_interface
			{
			public:
				virtual ~stretch_interface() = 0;
				virtual void process(const paint::pixel_buffer & s_pixbuf, const nana::rectangle& r_src, drawable_type dw_dst, const nana::rectangle& r_dst) const = 0;
			};

			class blend_interface
			{
			public:
				virtual ~blend_interface() = 0;
				virtual void process(drawable_type dw_dst, const nana::rectangle& r_dst, const paint::pixel_buffer& s_pixbuf, const nana::point& s_pos, double fade_rate) const = 0;
			};

			class line_interface
			{
			public:
				virtual ~line_interface() = 0;

				//process
				//@brief: interface of algorithm, pos_beg is a left point, pos_end is a right point.
				virtual void process(paint::pixel_buffer & pixbuf, const nana::point& pos_beg, const nana::point& pos_end, nana::color_t color, double fade_rate) const = 0;
			};
		}
	}//end namespace paint
}//end namespace nana
#endif
