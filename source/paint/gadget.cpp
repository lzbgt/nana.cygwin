/*
 *	Graphics Gadget Implementation
 *	Copyright(C) 2003-2013 Jinhao(cnjinhao@hotmail.com)
 *
 *	Distributed under the Boost Software License, Version 1.0. 
 *	(See accompanying file LICENSE_1_0.txt or copy at 
 *	http://www.boost.org/LICENSE_1_0.txt)
 *
 *	@file: nana/paint/gadget.cpp
 */

#include <nana/paint/graphics.hpp>
#include <nana/paint/gadget.hpp>

namespace nana
{
namespace paint
{
namespace gadget
{
	namespace detail
	{
		typedef nana::paint::graphics& graph_reference;

		void hollow_triangle(graph_reference graph, int x, int y, nana::color_t color, uint32_t direction)
		{
			x += 3;
			y += 3;
			switch(direction)
			{
			case directions::to_east:
				graph.line(x + 3, y + 1, x + 3, y + 9, color);
				graph.line(x + 4, y + 2 , x + 7, y + 5, color);
				graph.line(x + 6, y + 6, x + 4, y + 8, color);
				break;
			case directions::to_southeast:
				graph.line(x + 2, y + 7, x + 7, y + 7, color);
				graph.line(x + 7, y + 2, x + 7, y + 6, color);
				graph.line(x + 3, y + 6, x + 6, y + 3, color);
				break;
			case directions::to_south:
				y += 3;
				graph.line(x, y, x + 8, y, color);
				graph.line(x + 1, y + 1, x + 4, y + 4, color);
				graph.line(x + 7, y + 1, x + 5, y + 3, color);
				break;
			case directions::to_west:
				x += 5;
				y += 1;
				graph.line(x, y, x, y + 8, color);
				graph.line(x - 4, y + 4, x - 1, y + 1, color);
				graph.line(x - 3, y + 5, x - 1, y + 7, color);
				break;
			case directions::to_north:
				y += 7;
				graph.line(x, y, x + 8, y, color);
				graph.line(x + 1, y - 1, x + 4, y - 4, color);
				graph.line(x + 5, y - 3, x + 7, y - 1, color);
				break;
			}		
		}

		void solid_triangle(graph_reference graph, int x, int y, nana::color_t color, uint32_t dir)
		{
			x += 3;
			y += 3;
			switch(dir)
			{
			case directions::to_east:
				for(int i = 0; i < 5; ++i)
					graph.line(x + 3 + i, y + 1 + i, x + 3 + i, y + 9 - i, color);
				break;
			case directions::to_southeast:
				for(int i = 0; i < 6; ++i)
					graph.line(x + 2 + i, y + 7 - i, x + 7, y + 7 - i, color);
				break;
			case directions::to_south:
				y += 3;
				for(int i = 0; i < 5; ++i)
					graph.line(x + i, y + i, x + 8 - i, y + i, color);
				break;
			case directions::to_west:
				x += 5;
				y += 1;
				for(int i = 0; i < 5; ++i)
					graph.line(x - i, y + i, x - i, y + 8 - i, color);
				break;
			case directions::to_north:
				y += 7;
				for(int i = 0; i < 5; ++i)
					graph.line(x + i, y - i, x + 8 - i, y - i, color);
				break;
			}		
		}

		void direction_arrow(graph_reference graph, int x, int y, nana::color_t color, uint32_t dir)
		{
			y += 5;
			switch(dir)
			{
			case directions::to_north:
				{
					x += 3;

					int pixels = 1;
					for(int l = 0; l < 4; ++l)
					{
						for(int i = 0; i < pixels; ++i)
						{
							if(l ==3 && i == 3)
							{}
							else
								graph.set_pixel(x + i, y, 0x262);
						}

						x--;
						y++;
						pixels += 2;
					}

					graph.set_pixel(x + 1, y, 0x262);
					graph.set_pixel(x + 2, y, 0x262);
					graph.set_pixel(x + 6, y, 0x262);
					graph.set_pixel(x + 7, y, 0x262);
				}
				break;
			case directions::to_south:
				{

					graph.set_pixel(x, y, 0x262);
					graph.set_pixel(x + 1, y, 0x262);
					graph.set_pixel(x + 5, y, 0x262);
					graph.set_pixel(x + 6, y, 0x262);

					++y;
					int pixels = 7;
					for(int l = 0; l < 4; ++l)
					{
						for(int i = 0; i < pixels; ++i)
						{
							if(l == 0 && i == 3){}
							else
							graph.set_pixel(x + i, y, 0x262);
						}

						x++;
						y++;
						pixels -= 2;
					}
				}
				break;
			}
		}

		void double_arrow_line(nana::paint::graphics & graph, int x, int y, color_t color, bool horizontal)
		{
			graph.set_pixel(x, y, color);
			if(horizontal)
			{
				graph.set_pixel(x + 1, y, color);
				graph.set_pixel(x + 4, y, color);
				graph.set_pixel(x + 5, y, color);
			}
			else
			{
				graph.set_pixel(x, y + 1, color);
				graph.set_pixel(x, y + 4, color);
				graph.set_pixel(x, y + 5, color);
			}
		}

		void double_arrow(nana::paint::graphics& graph, int x, int y, color_t color, directions::t dir)
		{
			switch(dir)
			{
			case directions::to_east:
				double_arrow_line(graph, x + 4, y + 6, color, true);
				double_arrow_line(graph, x + 5, y + 7, color, true);
				double_arrow_line(graph, x + 6, y + 8, color, true);
				double_arrow_line(graph, x + 5, y + 9, color, true);
				double_arrow_line(graph, x + 4, y + 10, color, true);
				break;
			case directions::to_west:
				double_arrow_line(graph, x + 5, y + 6, color, true);
				double_arrow_line(graph, x + 4, y + 7, color, true);
				double_arrow_line(graph, x + 3, y + 8, color, true);
				double_arrow_line(graph, x + 4, y + 9, color, true);
				double_arrow_line(graph, x + 5, y + 10, color, true);
				break;
			case directions::to_south:
				double_arrow_line(graph, x + 5, y + 4, color, false);
				double_arrow_line(graph, x + 6, y + 5, color, false);
				double_arrow_line(graph, x + 7, y + 6, color, false);
				double_arrow_line(graph, x + 8, y + 5, color, false);
				double_arrow_line(graph, x + 9, y + 4, color, false);
				break;
			case directions::to_north:
				double_arrow_line(graph, x + 5, y + 6, color, false);
				double_arrow_line(graph, x + 6, y + 5, color, false);
				double_arrow_line(graph, x + 7, y + 4, color, false);
				double_arrow_line(graph, x + 8, y + 5, color, false);
				double_arrow_line(graph, x + 9, y + 6, color, false);
				break;
			default:
				break;
			}
		}
	}//end namespace detail

	//arrow_16_pixels
	//param@style: 0 = hollow, 1 = solid
	void arrow_16_pixels(nana::paint::graphics& graph, int x, int y, unsigned color, uint32_t style, directions::t dir)
	{
		switch(style)
		{
		case 1:
			detail::solid_triangle(graph, x, y, color, dir);
			break;
		case 2:
			detail::direction_arrow(graph, x, y, color, dir);
			break;
		case 3:
			detail::double_arrow(graph, x, y, color, dir);
			break;
		case 0:
		default:
			detail::hollow_triangle(graph, x, y, color, dir);
			break;
		}
	}

	void check_16_pixels(nana::paint::graphics& graph, int x, int y, unsigned style, unsigned state, bool checked)
	{
		if(style == 0)
		{
			if(state < 3)
			{
				++x;
				++y;
				unsigned color_border[] = {0x55B9A3, 0x55B9A3, 0x55B9A3};
				unsigned color_bkgrnd[] = {0xF4F4F4, 0xDEF9FA, 0xC2E4F6};
				unsigned color_inline[] = {0xAEB3B9, 0x79C6F9, 0x5EB6F7};

				graph.rectangle(x, y, 13, 13, color_border[state], false);
				graph.rectangle(x + 1, y + 1, 11, 11, color_bkgrnd[state], false);
				graph.rectangle(x + 2, y + 2, 9, 9, color_inline[state], false);
				graph.rectangle(x + 3, y + 3, 7, 7, color_bkgrnd[state], true);
			}

			if(checked)
			{
				int sx = x + 2;
				int sy = y + 4;

				for(int i = 0; i < 3; i++)
				{
					sx++;
					sy++;
					graph.line(sx, sy, sx, sy + 3, 0x0);
				}

				for(int i = 0; i < 4; i++)
				{
					sx++;
					sy--;
					graph.line(sx, sy, sx, sy + 3, 0x0);
				}			
			}

		}
	}

	void close_16_pixels(nana::paint::graphics& graph, int x, int y, uint32_t style, uint32_t color)
	{
		if(0 == style)
		{
			x += 3;
			y += 3;

			graph.line(x, y, x + 9, y + 9, color);
			graph.line(x + 1, y, x + 9, y + 8, color);
			graph.line(x, y + 1, x + 8, y + 9, color);

			graph.line(x + 9, y, x , y + 9, color);
			graph.line(x + 8, y, x, y + 8, color);
			graph.line(x + 9, y + 1, x + 1, y + 9, color);
		}
		else
		{
			x += 4;
			y += 4;

			graph.line(x, y, x + 7, y + 7, color);
			graph.line(x + 1, y, x + 7, y + 6, color);
			graph.line(x, y + 1, x + 6, y + 7, color);

			graph.line(x + 7, y, x , y + 7, color);
			graph.line(x + 6, y, x, y + 6, color);
			graph.line(x + 7, y + 1, x + 1, y + 7, color);		
		}
	}

	void checker(nana::paint::graphics& graph, int x, int y, uint32_t owner_pixels, uint32_t style, uint32_t color)
	{
		uint32_t bld_color = graph.mix(color, 0xFFFFFF, 0.5);

		if(style == checkers::radio)
		{
			unsigned colormap[8][8] = {
				{0xFF000000,0xdee7ef,0x737baa,0x232674,0x3c3f84,0x8d96ba,0xe0e9ef,0xFF000000},
				{0xdce4ed,0x242875,0x6f71b3,0x9fa0d6,0xc3c4e9,0xb1b2da,0x5c6098,0xdbe4ed},
				{0x7b81ad,0x4f5199,0x8182c1,0xa1a2d4,0xccccea,0xcccced,0x9c9dcf,0x7981ae},
				{0x2b2d77,0x4f509a,0x696baf,0x7879ba,0xa4a6d4,0xa9aad9,0x9193ce,0x1e2271},
				{0x36387f,0x383a87,0x52549c,0x6162a8,0x6f71b3,0x7e7fbf,0x7879ba,0x282c78},
				{0x9094ba,0x1b1c71,0x3c3e8b,0x4a4b96,0x585aa1,0x6768ac,0x464893,0x828bb6},
				{0xe2eaf1,0x4b4d8d,0x16186d,0x292b7c,0x333584,0x2c2e7f,0x454b8c,0xdfe9f0},
				{0xFF000000,0xe4ecf2,0x9599bd,0x454688,0x414386,0x9095bb,0xe3ebf2,0xFF000000}
			};

			x += (static_cast<int>(owner_pixels) - 8) / 2;
			y += (static_cast<int>(owner_pixels) - 8) / 2;
			
			for(int l = 0; l < 8; ++l)
			{
				for(int r = 0; r < 8; ++r)
				{
					if(colormap[l][r] & 0xFF000000)
						continue;
					graph.set_pixel(x + r, y, colormap[l][r]);
				}
				++y;
			}
		}
		else if(style == checkers::clasp)
		{
			x += (static_cast<int>(owner_pixels) - 16) / 2;
			y += (static_cast<int>(owner_pixels) - 16) / 2;

			graph.line(x + 3, y + 7, x + 6, y + 10, color);
			graph.line(x + 7, y + 9, x + 12, y + 4, color);
			graph.line(x + 3, y + 8, x + 6, y + 11, bld_color);
			graph.line(x + 7, y + 10, x + 12, y + 5, bld_color);
			graph.line(x + 4, y + 7, x + 6, y + 9, bld_color);
			graph.line(x + 7, y + 8, x + 11, y + 4, bld_color);
		}
	}

	void cross(graphics& graph, int x, int y, uint32_t size, uint32_t thickness, nana::color_t color)
	{
		if(thickness + 2 <= size)
		{
			int gap = (size - thickness) / 2;

			nana::point ps[12];
			ps[0].x = x + gap;
			ps[1].x = ps[0].x + thickness - 1;
			ps[1].y = ps[0].y = y;

			ps[2].x = ps[1].x;
			ps[2].y = y + gap;

			ps[3].x = ps[2].x + gap;
			ps[3].y = ps[2].y;

			ps[4].x = ps[3].x;
			ps[4].y = ps[3].y + thickness - 1;

			ps[5].x = ps[1].x;
			ps[5].y = ps[4].y;

			ps[6].x = ps[5].x;
			ps[6].y = ps[5].y + gap;

			ps[7].x = x + gap;
			ps[7].y = ps[6].y;

			ps[8].x = ps[7].x;
			ps[8].y = ps[4].y;

			ps[9].x = x;
			ps[9].y = ps[4].y;

			ps[10].x = x;
			ps[10].y = y + gap;

			ps[11].x = x + gap;
			ps[11].y = y + gap;

			nana::color_t dkcolor = graph.mix(color, 0x0, 0.5);
			for(int i = 0; i < 11; ++i)
				graph.line(ps[i], ps[i + 1], dkcolor);
			graph.line(ps[11], ps[0], dkcolor);

			graph.rectangle(ps[10].x + 1, ps[10].y + 1, (gap << 1) + thickness - 2, thickness - 2, color, true);
			graph.rectangle(ps[0].x + 1, ps[0].y + 1, thickness - 2, (gap << 1) + thickness - 2, color, true);

		}
	}

	//class check_renderer
	check_renderer::check_renderer()
	{
		this->clear_image_state();
	}

	void check_renderer::render(graphics& graph, int x, int y, uint32_t width, uint32_t height, mouse_action act, check_renderer::checker_t chk, bool checked)
	{
		if(mouse_action::begin <= act && act < mouse_action::end && 0 <= chk && chk < checker_end)
		{
			img_state_t & is = imgstate_[chk][static_cast<int>(act)][checked ? 0 : 1];
			if(is.in_use && image_)
			{
				x += static_cast<int>(width - is.width) / 2;
				y += static_cast<int>(height - is.height) / 2;
				image_.paste(nana::rectangle(is.x, is.y, is.width, is.height), graph, nana::point(x, y));
			}
			else
			{
				if(chk == radio)
				{
					unsigned bmp_unchecked[12][12] = {
						{0xFFFFFF, 0xFFFFFF, 0xFFFFFF, 0xCFD0D0, 0xAAABAB, 0x919292, 0x919292, 0xAAABAB, 0xCFD0D0, 0xFFFFFF, 0xFFFFFF, 0xFFFFFF},
						{0xFFFFFF, 0xFFFFFF, 0xA3A4A4, 0xB9BABA, 0xDBDBDB, 0xF2F2F2, 0xF2F2F2, 0xDBDBDB, 0xB9BABA, 0xA3A4A4, 0xFFFFFF, 0xFFFFFF},
						{0xFFFFFF, 0xA2A3A3, 0xC3C3C3, 0xEDEDEE, 0xC6C9CD, 0xB5BABF, 0xB5BABF, 0xC8CBCE, 0xEDEEEE, 0xC3C3C3, 0xA2A3A3, 0xFFFFFF},
						{0xCFD0D0, 0xB9BABA, 0xE9EAEB, 0xB3B8BD, 0xBDC2C7, 0xC8CDD2, 0xC9CED3, 0xC5C8CC, 0xBEC1C5, 0xEBECEC, 0xB9BABA, 0xCFD0D0},
						{0xA9A9A9, 0xDCDCDC, 0xC5C8CC, 0xBEC3C9, 0xCBCFD5, 0xCED2D7, 0xD5D8DC, 0xDCDEE0, 0xD3D4D7, 0xD4D5D5, 0xDCDCDC, 0xA9A9A9},
						{0x919292, 0xF2F2F2, 0xB4B9BD, 0xCDD1D6, 0xD3D6DA, 0xDBDDDF, 0xE4E4E5, 0xE9E9E9, 0xE8E8E9, 0xD0D1D2, 0xF2F2F2, 0x919292},
						{0x919292, 0xF2F2F2, 0xBBBEC2, 0xD7DADD, 0xE0E1E3, 0xE9E9E9, 0xEFEFEF, 0xF0F0F0, 0xEFEFF0, 0xDBDCDC, 0xEFEFEF, 0x939494},
						{0xA7A8A8, 0xDDDDDD, 0xCFD1D3, 0xD5D6D8, 0xE9E9E9, 0xEFEFEF, 0xF4F4F4, 0xF5F5F5, 0xEEEEEE, 0xE8E8E8, 0xDDDDDD, 0xA7A8A8},
						{0xCECECE, 0xBABBBB, 0xECECED, 0xCDCECF, 0xE1E2E2, 0xF0F0F0, 0xF4F4F4, 0xF1F1F1, 0xEBEBEB, 0xF2F2F2, 0xBABBBB, 0xCECECE},
						{0xFFFFFF, 0xA2A3A3, 0xC3C3C3, 0xF0F0F1, 0xE2E3E3, 0xE4E4E5, 0xE9EAEA, 0xEEEEEF, 0xF3F3F3, 0xC3C3C3, 0xA2A3A3, 0xFFFFFF},
						{0xFFFFFF, 0xFFFFFF, 0xA2A3A3, 0xBABBBB, 0xDBDBDB, 0xF4F4F4, 0xF4F4F4, 0xDCDCDC, 0xBABBBB, 0xA2A3A3, 0xFFFFFF, 0xFFFFFF},
						{0xFFFFFF, 0xFFFFFF, 0xFFFFFF, 0xCECECE, 0xAAABAB, 0x8E8F8F, 0x8E8F8F, 0xA9A9A9, 0xCECECE, 0xFFFFFF, 0xFFFFFF, 0xFFFFFF}
									};
					unsigned bmp_unchecked_highlight[12][12] = {
						{0xFFFFFF, 0xFFFFFF, 0xFFFFFF, 0xB7CCD8, 0x7FA4BA, 0x5989A5, 0x5989A5, 0x7FA4BA, 0xB7CCD8, 0xFFFFFF, 0xFFFFFF, 0xFFFFFF},
						{0xFFFFFF, 0xFFFFFF, 0x759DB4, 0x8FB7C8, 0xBCDDE5, 0xDBF6F8, 0xDBF6F8, 0xBCDDE5, 0x8FB7C8, 0x759DB4, 0xFFFFFF, 0xFFFFFF},
						{0xFFFFFF, 0x739BB3, 0x9CC2D0, 0xD3F4FA, 0x9BD7F9, 0x84CBF9, 0x84CBF9, 0x9CD8F9, 0xD4F4FA, 0x9CC2D0, 0x739BB3, 0xFFFFFF},
						{0xB7CCD8, 0x8FB7C8, 0xCFF1FA, 0x80CAF9, 0x96D3FB, 0xAADDFD, 0xABDDFD, 0x9AD5FB, 0x86CEF9, 0xCFF2FA, 0x8FB7C8, 0xB7CCD8},
						{0x7DA2B9, 0xBEDEE6, 0x9AD7F9, 0x98D5FB, 0xB1DFFD, 0xB2E0FD, 0xB7E3FD, 0xBCE5FD, 0xA6DCFB, 0xA1DCF9, 0xBEDEE6, 0x7DA2B9},
						{0x5989A5, 0xDBF6F8, 0x80CAF9, 0xAFDEFD, 0xB6E2FD, 0xBBE5FD, 0xC1E8FD, 0xC5EAFD, 0xC7EBFD, 0x99D8FA, 0xDBF6F8, 0x5989A5},
						{0x5989A5, 0xDBF6F8, 0x84CDF9, 0xB6E2FD, 0xBFE7FD, 0xC7EBFD, 0xD5F0FE, 0xDAF2FE, 0xD8F1FE, 0xB1E1FB, 0xD8F4F6, 0x5D8CA7},
						{0x7BA1B8, 0xBFDFE7, 0x9FDBF9, 0xA7DDFB, 0xC8EBFD, 0xD6F1FE, 0xE2F5FE, 0xE5F6FE, 0xD8F0FD, 0xCAEDFB, 0xBFDFE7, 0x7BA1B8},
						{0xB5CAD7, 0x91B8C9, 0xCFF2FA, 0x92D5F9, 0xBAE5FC, 0xDAF2FE, 0xE4F5FE, 0xDFF3FE, 0xD2EEFD, 0xDBF7FA, 0x91B8C9, 0xB5CAD7},
						{0xFFFFFF, 0x739BB3, 0x9CC2D0, 0xD7F6FA, 0xBDE8FB, 0xC2E8FC, 0xD0EDFD, 0xD7F2FC, 0xDDF8FA, 0x9CC2D0, 0x739BB3, 0xFFFFFF},
						{0xFFFFFF, 0xFFFFFF, 0x739BB3, 0x91B8C9, 0xBCDDE5, 0xDEF9FA, 0xDEF9FA, 0xBEDEE6, 0x91B8C9, 0x739BB3, 0xFFFFFF, 0xFFFFFF},
						{0xFFFFFF, 0xFFFFFF, 0xFFFFFF, 0xB5CAD7, 0x7FA4BA, 0x5586A3, 0x5586A3, 0x7DA2B9, 0xB5CAD7, 0xFFFFFF, 0xFFFFFF, 0xFFFFFF}
									};
					unsigned bmp_checked[12][12] = {
						{0xFFFFFF, 0xFFFFFF, 0xFFFFFF, 0xCFD0D0, 0xAAABAB, 0x919292, 0x919292, 0xAAABAB, 0xCFD0D0, 0xFCFCFC, 0xFFFFFF, 0xFFFFFF},
						{0xFFFFFF, 0xFFFFFF, 0xA3A4A4, 0xB9BABA, 0xDBDBDB, 0xF2F2F2, 0xF2F2F2, 0xDBDBDB, 0xB9BABA, 0xA3A4A4, 0xF3F3F3, 0xFFFFFF},
						{0xFFFFFF, 0xA2A3A3, 0xC3C3C3, 0xEDEDEE, 0xBABFC5, 0x85939F, 0x85939F, 0xBCC1C5, 0xEDEEEE, 0xC3C3C3, 0xA2A3A3, 0xFCFCFC},
						{0xCFD0D0, 0xB9BABA, 0xE9EAEB, 0x8997A2, 0x274760, 0x486378, 0x365166, 0x204058, 0x8E9AA4, 0xEBECEC, 0xB9BABA, 0xCFD0D0},
						{0xA9A9A9, 0xDCDCDC, 0xB9BEC4, 0x24445D, 0x91B2C6, 0xC7EBFD, 0x69C2D4, 0x14405C, 0x1E3F57, 0xC9CCCD, 0xDCDCDC, 0xA9A9A9},
						{0x919292, 0xF2F2F2, 0x7D8D98, 0x304B5F, 0x90D5E5, 0x5DCEDD, 0x28A2D1, 0x178AC7, 0x183348, 0x8F9CA6, 0xF2F2F2, 0x919292},
						{0x919292, 0xF2F2F2, 0x82909C, 0x183347, 0x228FC6, 0x209DD1, 0x1898D1, 0x0E84C6, 0x183348, 0x97A3AC, 0xEFEFEF, 0x939494},
						{0xA7A8A8, 0xDDDDDD, 0xC0C5C9, 0x1E3F57, 0x0F3F5D, 0x0F83C7, 0x0B82C7, 0x0C3D5D, 0x1F3F58, 0xD9DCDE, 0xDDDDDD, 0xA7A8A8},
						{0xCECECE, 0xBABBBB, 0xECECED, 0x99A3AB, 0x1D3E57, 0x18354A, 0x19344A, 0x1E3E57, 0xAEB8BF, 0xF2F2F2, 0xBABBBB, 0xCECECE},
						{0xFFFFFF, 0xA2A3A3, 0xC3C3C3, 0xF0F0F1, 0xD1D5D7, 0xA6B0B9, 0xA9B4BC, 0xDCDFE2, 0xF3F3F3, 0xC3C3C3, 0xA2A3A3, 0xFFFFFF},
						{0xFFFFFF, 0xFFFFFF, 0xA2A3A3, 0xBABBBB, 0xDBDBDB, 0xF4F4F4, 0xF4F4F4, 0xDCDCDC, 0xBABBBB, 0xA2A3A3, 0xFFFFFF, 0xFFFFFF},
						{0xFFFFFF, 0xFFFFFF, 0xFFFFFF, 0xCECECE, 0xAAABAB, 0x8E8F8F, 0x8E8F8F, 0xA9A9A9, 0xCECECE, 0xFFFFFF, 0xFFFFFF, 0xFFFFFF}
									};

					unsigned bmp_checked_highlight[12][12] = {
						{0xFFFFFF, 0xFFFFFF, 0xFFFFFF, 0xB7CCD8, 0x7FA4BA, 0x5989A5, 0x5989A5, 0x7FA4BA, 0xB7CCD8, 0xFFFFFF, 0xFFFFFF, 0xFFFFFF},
						{0xFFFFFF, 0xFFFFFF, 0x759DB4, 0x8FB7C8, 0xBCDDE5, 0xDBF6F8, 0xDBF6F8, 0xBCDDE5, 0x8FB7C8, 0x759DB4, 0xFFFFFF, 0xFFFFFF},
						{0xFFFFFF, 0x739BB3, 0x9CC2D0, 0xD3F4FA, 0x92CCED, 0x639FC7, 0x639FC7, 0x93CDED, 0xD4F4FA, 0x9CC2D0, 0x739BB3, 0xFFFFFF},
						{0xB7CCD8, 0x8FB7C8, 0xCFF1FA, 0x66A3CC, 0x264862, 0x47647A, 0x355268, 0x1E405A, 0x66A3C9, 0xCFF2FA, 0x8FB7C8, 0xB7CCD8},
						{0x7DA2B9, 0xBEDEE6, 0x91CCED, 0x22445E, 0x9DBBCD, 0xE9F7FE, 0x7FE6EE, 0x154664, 0x1D3F58, 0x99D3EF, 0xBEDEE6, 0x7DA2B9},
						{0x5989A5, 0xDBF6F8, 0x5C98BF, 0x2F4B60, 0xB1F6FA, 0x74FFFF, 0x32CAFF, 0x1DAAF3, 0x173348, 0x6CA1C0, 0xDBF6F8, 0x5989A5},
						{0x5989A5, 0xDBF6F8, 0x5E99BF, 0x173348, 0x2AB0F2, 0x28C4FF, 0x1EBEFF, 0x11A3F2, 0x173348, 0x7BA6C0, 0xD8F4F6, 0x5D8CA7},
						{0x7BA1B8, 0xBFDFE7, 0x94CEEB, 0x1D3F58, 0x114567, 0x13A2F3, 0x0DA0F3, 0x0D4367, 0x1E3F58, 0xBEE0EF, 0xBFDFE7, 0x7BA1B8},
						{0xB5CAD7, 0x91B8C9, 0xCFF2FA, 0x6FA8C9, 0x1C3E58, 0x18354B, 0x18354B, 0x1D3E58, 0x9CBACC, 0xDBF7FA, 0x91B8C9, 0xB5CAD7},
						{0xFFFFFF, 0x739BB3, 0x9CC2D0, 0xD7F6FA, 0xAFDAED, 0x8EB3C9, 0x98B7CA, 0xC7E3EE, 0xDDF8FA, 0x9CC2D0, 0x739BB3, 0xFFFFFF},
						{0xFFFFFF, 0xFFFFFF, 0x739BB3, 0x91B8C9, 0xBCDDE5, 0xDEF9FA, 0xDEF9FA, 0xBEDEE6, 0x91B8C9, 0x739BB3, 0xFFFFFF, 0xFFFFFF},
						{0xFFFFFF, 0xFFFFFF, 0xFFFFFF, 0xB5CAD7, 0x7FA4BA, 0x5586A3, 0x5586A3, 0x7DA2B9, 0xB5CAD7, 0xFFFFFF, 0xFFFFFF, 0xFFFFFF}
									};

					unsigned bmp_checked_press[12][12] = {
						{0xFFFFFF, 0xFFFFFF, 0xFFFFFF, 0xA6BDCE, 0x6089A8, 0x31668E, 0x31668E, 0x6089A8, 0xA6BDCE, 0xFFFFFF, 0xFFFFFF, 0xFFFFFF},
						{0xFFFFFF, 0xFFFFFF, 0x5480A1, 0x6C99B8, 0x9DC4DC, 0xBEE1F3, 0xBEE1F3, 0x9DC4DC, 0x6C99B8, 0x5480A1, 0xFFFFFF, 0xFFFFFF},
						{0xFFFFFF, 0x517E9F, 0x7AA5C2, 0xB6DDF3, 0x73B2D6, 0x4A8AB0, 0x4A8AB0, 0x74B3D8, 0xB7DEF3, 0x7AA5C2, 0x517E9F, 0xFFFFFF},
						{0xA6BDCE, 0x6C99B8, 0xB1DBF1, 0x4B8DB4, 0x244660, 0x456279, 0x335167, 0x1D3F59, 0x4C90B7, 0xB1DCF3, 0x6C99B8, 0xA6BDCE},
						{0x5E87A6, 0x9FC5DD, 0x71B1D6, 0x21435D, 0x7EA5BC, 0x95D9FC, 0x478BAE, 0x113858, 0x1B3E58, 0x78BDE2, 0x9FC5DD, 0x5E87A6},
						{0x31668E, 0xBEE1F3, 0x4484AA, 0x2E4A60, 0x5DA2C6, 0x3A84AA, 0x19658D, 0x0F5984, 0x153248, 0x5794B7, 0xBEE1F3, 0x31668E},
						{0x31668E, 0xBEE1F3, 0x4687AE, 0x153247, 0x165D84, 0x14628D, 0x0F5F8D, 0x095684, 0x163248, 0x6B9DB9, 0xBBDEF1, 0x366990},
						{0x5B85A5, 0xA0C7DE, 0x74B7DC, 0x1B3E58, 0x0D3659, 0x0A5583, 0x075483, 0x0A3459, 0x1E3F58, 0xA9D2E9, 0xA0C7DE, 0x5B85A5},
						{0xA3BBCD, 0x6D9BBA, 0xB2DDF3, 0x5599BE, 0x1C3E57, 0x17344A, 0x17344B, 0x1D3E57, 0x91B3C7, 0xC1E4F6, 0x6D9BBA, 0xA3BBCD},
						{0xFFFFFF, 0x517E9F, 0x7AA5C2, 0xBBE1F5, 0x98CAE4, 0x80AAC3, 0x8CAFC5, 0xB7D7EA, 0xC2E4F6, 0x7AA5C2, 0x517E9F, 0xFFFFFF},
						{0xFFFFFF, 0xFFFFFF, 0x517E9F, 0x6D9BBA, 0x9DC4DC, 0xC2E4F6, 0xC2E4F6, 0x9FC5DD, 0x6D9BBA, 0x517E9F, 0xFFFFFF, 0xFFFFFF},
						{0xFFFFFF, 0xFFFFFF, 0xFFFFFF, 0xA3BBCD, 0x6089A8, 0x2C628B, 0x2C628B, 0x5E87A6, 0xA3BBCD, 0xFFFFFF, 0xFFFFFF, 0xFFFFFF}
									};

					unsigned (*colormap)[12][12];

					switch(act)
					{
					case gui::mouse_action::normal:
						colormap = (checked ? &bmp_checked : &bmp_unchecked);
						break;
					case gui::mouse_action::over:
						colormap = (checked ? &bmp_checked_highlight : &bmp_unchecked_highlight);
						break;
					case gui::mouse_action::pressed:
						colormap = &bmp_checked_press;
						break;
					default:
						colormap = 0;
					}
					
					if(colormap)
					{
						x += 2;
						y += 2;

						for(int top = 0; top < 12; ++top)
						{
							for(int left = 0; left < 12; ++left)
							{
								if((*colormap)[top][left] != 0xFFFFFF)
									graph.set_pixel(left + x, top + y, (*colormap)[top][left]);
							}
						}
					}
				}
				else
				{
					++x;
					++y;
					unsigned color_border[] = {0x55B9A3, 0x55B9A3, 0x55B9A3};
					unsigned color_bkgrnd[] = {0xF4F4F4, 0xDEF9FA, 0xC2E4F6};
					unsigned color_inline[] = {0xAEB3B9, 0x79C6F9, 0x5EB6F7};

					graph.rectangle(x, y, 13, 13, color_border[static_cast<int>(act)], false);
					graph.rectangle(x + 1, y + 1, 11, 11, color_bkgrnd[static_cast<int>(act)], false);
					graph.rectangle(x + 2, y + 2, 9, 9, color_inline[static_cast<int>(act)], false);
					graph.rectangle(x + 3, y + 3, 7, 7, color_bkgrnd[static_cast<int>(act)], true);

					if(checked)
					{
						if(chk == clasp)
						{
							int sx = x + 2;
							int sy = y + 4;

							for(int i = 0; i < 3; i++)
							{
								sx++;
								sy++;
								graph.line(sx, sy, sx, sy + 3, 0x0);
							}

							for(int i = 0; i < 4; i++)
							{
								sx++;
								sy--;
								graph.line(sx, sy, sx, sy + 3, 0x0);
							}
						}
						else if(chk == blocker)
							graph.rectangle(x + 2, y + 2, 9, 9, 0x79C6F9, true);
					}
				}
			}
		}
	}

	void check_renderer::open_background_image(const paint::image& img)
	{
		this->image_ = img;
	}

	void check_renderer::set_image_state(mouse_action act, checker_t chk, bool checked, const nana::rectangle& r)
	{
		if(mouse_action::begin <= act && act < mouse_action::end && 0 <= chk && chk < checker_end)
		{
			img_state_t & is = imgstate_[chk][static_cast<int>(act)][checked ? 0 : 1];

			is.in_use = true;
			is.x = r.x;
			is.y = r.y;
			is.width = r.width;
			is.height = r.height;
		}
	}

	void check_renderer::clear_image_state()
	{
		for(int i = 0 ; i < static_cast<int>(mouse_action::end); ++i)
		{
			for(int u = 0; u < checker_end; ++u)
			{
				imgstate_[u][i][0].in_use = imgstate_[u][i][1].in_use = false;
			}
		}
	}

	bool check_renderer::has_background_image() const
	{
		return (image_.empty() == false);
	}

	bool check_renderer::in_use(mouse_action act, checker_t chk, bool checked) const
	{
		if(false == image_.empty())
		{
			if(mouse_action::begin <= act && act < mouse_action::end && 0 <= chk && chk < checker_end)
				return imgstate_[chk][static_cast<int>(act)][checked ? 0 : 1].in_use;
		}
		return false;
	}

	//end class check_renderer
}//end namespace gadget
	
}//end namespace paint
}//end namespace nana
