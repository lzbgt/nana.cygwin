/*
 *	Image Processor Algorithm Implementation
 *	Copyright(C) 2003-2013 Jinhao(cnjinhao@hotmail.com)
 *
 *	Distributed under the Boost Software License, Version 1.0.
 *	(See accompanying file LICENSE_1_0.txt or copy at
 *	http://www.boost.org/LICENSE_1_0.txt)
 *
 *	@file: nana/paint/detail/image_processor.hpp
 *	@brief: This header file implements the algorithms of image processor
 *
 *	DON'T INCLUDE THIS HEADER FILE DIRECTLY TO YOUR SOURCE FILE.
 */

#ifndef NANA_PAINT_DETAIL_IMAGE_PROCESSOR_HPP
#define NANA_PAINT_DETAIL_IMAGE_PROCESSOR_HPP
#include "../image_process_interface.hpp"
#include <nana/paint/pixel_buffer.hpp>
#include <nana/paint/detail/native_paint_interface.hpp>

namespace nana
{
namespace paint
{
namespace detail
{
	namespace algorithms
	{
		class proximal_interoplation
			: public image_process::stretch_interface
		{
			void process(const paint::pixel_buffer& s_pixbuf, const nana::rectangle& r_src, drawable_type dw_dst, const nana::rectangle& r_dst) const
			{
				const std::size_t s_pixbuf_width = s_pixbuf.size().width;

				pixel_buffer pixbuf(r_dst.width, r_dst.height);
				double rate_x = double(r_src.width) / r_dst.width;
				double rate_y = double(r_src.height) / r_dst.height;

				pixel_rgb_t * s_raw_pixbuf = s_pixbuf.raw_ptr();
				pixel_rgb_t * d_line = pixbuf.raw_ptr();

				for(std::size_t row = 0; row < r_dst.height; ++row)
				{
					const pixel_rgb_t * s_line = s_raw_pixbuf + (static_cast<int>(row * rate_y) + r_src.y) * s_pixbuf_width;

					pixel_rgb_t * i = d_line;
					d_line += r_dst.width;

					for(std::size_t x = 0; x < r_dst.width; ++x, ++i)
						*i = s_line[static_cast<int>(x * rate_x) + r_src.x];
				}

				pixbuf.paste(dw_dst, r_dst.x, r_dst.y);
			}
		};

		class bilinear_interoplation
			: public image_process::stretch_interface
		{
			struct x_u_table_tag
			{
				int x;
				int iu;
				int iu_minus_coef;
			};

			void process(const paint::pixel_buffer & s_pixbuf, const nana::rectangle& r_src, drawable_type dw_dst, const nana::rectangle& r_dst) const
			{
				const std::size_t s_pixbuf_width = s_pixbuf.size().width;

				const int shift_size = 8;
				const std::size_t coef = 1 << shift_size;
				const int double_shift_size = shift_size << 1;

				nana::paint::pixel_buffer pixbuf(r_dst.width, r_dst.height);

				double rate_x = double(r_src.width) / r_dst.width;
				double rate_y = double(r_src.height) / r_dst.height;
				

				const int right_bound = static_cast<int>(r_src.width) - 1;

				const nana::pixel_rgb_t * s_raw_pixel_buffer = s_pixbuf.raw_ptr();

				const int bottom = r_src.y + static_cast<int>(r_src.height - 1);

				x_u_table_tag * x_u_table = new x_u_table_tag[r_dst.width];

				nana::pixel_rgb_t * i = pixbuf.raw_ptr();

				for(std::size_t x = 0; x < r_dst.width; ++x)
				{
					double u = (int(x) + 0.5) * rate_x - 0.5;
					x_u_table_tag el;
					el.x = r_src.x;
					if(u < 0)
					{
						u = 0;
					}
					else
					{
						int ipart = static_cast<int>(u);
						el.x += ipart;
						u -= ipart;
					}
					el.iu = static_cast<int>(u * coef);
					el.iu_minus_coef = coef - el.iu;
					x_u_table[x] = el;
				}
				
				for(std::size_t row = 0; row < r_dst.height; ++row)
				{
					double v = (int(row) + 0.5) * rate_y - 0.5;
					int sy = r_src.y;
					if(v < 0)
					{
						v = 0;
					}
					else
					{
						int ipart = static_cast<int>(v);
						sy += ipart;
						v -= ipart;
					}

					std::size_t iv = static_cast<size_t>(v * coef);
					const std::size_t iv_minus_coef = coef - iv;

					const nana::pixel_rgb_t * s_line = s_raw_pixel_buffer + sy * s_pixbuf_width;
					const nana::pixel_rgb_t * next_s_line = s_line + ((sy < bottom) ? s_pixbuf_width : 0);

					nana::pixel_rgb_t col0;
					nana::pixel_rgb_t col1;
					nana::pixel_rgb_t col2;
					nana::pixel_rgb_t col3;

					for(std::size_t x = 0; x < r_dst.width; ++x, ++i)
					{
						x_u_table_tag el = x_u_table[x];
						
						col0 = s_line[el.x];
						col1 = next_s_line[el.x];

						if(el.x < right_bound)
						{
							col2 = s_line[el.x + 1];
							col3 = next_s_line[el.x + 1];
						}
						else
						{
							col2 = col0;
							col3 = col1;
						}
						
						std::size_t coef0 = el.iu_minus_coef * iv_minus_coef;
						std::size_t coef1 = el.iu_minus_coef * iv;
						std::size_t coef2 = el.iu * iv_minus_coef;
						std::size_t coef3 = el.iu * iv;			

						i->u.element.red = static_cast<unsigned>((coef0 * col0.u.element.red + coef1 * col1.u.element.red + (coef2 * col2.u.element.red + coef3 * col3.u.element.red)) >> double_shift_size);
						i->u.element.green = static_cast<unsigned>((coef0 * col0.u.element.green + coef1 * col1.u.element.green + (coef2 * col2.u.element.green + coef3 * col3.u.element.green)) >> double_shift_size);
						i->u.element.blue = static_cast<unsigned>((coef0 * col0.u.element.blue + coef1 * col1.u.element.blue + (coef2 * col2.u.element.blue + coef3 * col3.u.element.blue)) >> double_shift_size);
					}
				}
				delete [] x_u_table;
				pixbuf.paste(dw_dst, r_dst.x, r_dst.y);
			}
		};

		//blend
		class blend
			: public image_process::blend_interface
		{
			//process
			virtual void process(drawable_type dw_dst, const nana::rectangle& r_dst, const paint::pixel_buffer& s_pixbuf, const nana::point& s_pos, double fade_rate) const
			{
				paint::pixel_buffer d_pixbuf(dw_dst, r_dst.y, r_dst.height);

				nana::pixel_rgb_t * d_rgb = d_pixbuf.raw_ptr() + r_dst.x;
				nana::pixel_rgb_t * s_rgb = s_pixbuf.raw_ptr(s_pos.y) + s_pos.x;

				if(d_rgb && s_rgb)
				{
					unsigned char* tablebuf = detail::fade_table(fade_rate);//new unsigned char[0x100 * 2];
					unsigned char* d_table = tablebuf;
					unsigned char* s_table = d_table + 0x100;

					const unsigned rest = r_dst.width & 0x3;
					const unsigned length_align4 = r_dst.width - rest;

					unsigned d_step_width = d_pixbuf.size().width - r_dst.width + rest;
					unsigned s_step_width = s_pixbuf.size().width - r_dst.width + rest;
					for(unsigned line = 0; line < r_dst.height; ++line)
					{
						pixel_rgb_t* end = d_rgb + length_align4;
						for(; d_rgb < end; d_rgb += 4, s_rgb += 4)
						{
							//0
							d_rgb[0].u.element.red = unsigned(d_table[d_rgb[0].u.element.red] + s_table[s_rgb[0].u.element.red]);
							d_rgb[0].u.element.green = unsigned(d_table[d_rgb[0].u.element.green] + s_table[s_rgb[0].u.element.green]);
							d_rgb[0].u.element.blue = unsigned(d_table[d_rgb[0].u.element.blue] + s_table[s_rgb[0].u.element.blue]);

							//1
							d_rgb[1].u.element.red = unsigned(d_table[d_rgb[1].u.element.red] + s_table[s_rgb[1].u.element.red]);
							d_rgb[1].u.element.green = unsigned(d_table[d_rgb[1].u.element.green] + s_table[s_rgb[1].u.element.green]);
							d_rgb[1].u.element.blue = unsigned(d_table[d_rgb[1].u.element.blue] + s_table[s_rgb[1].u.element.blue]);

							//2
							d_rgb[2].u.element.red = unsigned(d_table[d_rgb[2].u.element.red] + s_table[s_rgb[2].u.element.red]);
							d_rgb[2].u.element.green = unsigned(d_table[d_rgb[2].u.element.green] + s_table[s_rgb[2].u.element.green]);
							d_rgb[2].u.element.blue = unsigned(d_table[d_rgb[2].u.element.blue] + s_table[s_rgb[2].u.element.blue]);

							//3
							d_rgb[3].u.element.red = unsigned(d_table[d_rgb[3].u.element.red] + s_table[s_rgb[3].u.element.red]);
							d_rgb[3].u.element.green = unsigned(d_table[d_rgb[3].u.element.green] + s_table[s_rgb[3].u.element.green]);
							d_rgb[3].u.element.blue = unsigned(d_table[d_rgb[3].u.element.blue] + s_table[s_rgb[3].u.element.blue]);
						}

						for(unsigned i = 0; i < rest; ++i)
						{
							d_rgb[i].u.element.red = unsigned(d_table[d_rgb[i].u.element.red] + s_table[s_rgb[i].u.element.red]);
							d_rgb[i].u.element.green = unsigned(d_table[d_rgb[i].u.element.green] + s_table[s_rgb[i].u.element.green]);
							d_rgb[i].u.element.blue = unsigned(d_table[d_rgb[i].u.element.blue] + s_table[s_rgb[i].u.element.blue]);
						}
						d_rgb += d_step_width;
						s_rgb += s_step_width;
					}

					nana::rectangle r = r_dst;
					r.y = 0;
					d_pixbuf.paste(r, dw_dst, r_dst.x, r_dst.y);
					detail::free_fade_table(tablebuf);
				}
			}
		};

		//class line
		class bresenham_line
			: public image_process::line_interface
		{
			virtual void process(paint::pixel_buffer & pixbuf, const nana::point& pos_beg, const nana::point& pos_end, nana::color_t color, double fade_rate) const
			{
				unsigned char * fade_table = 0;
				nana::pixel_rgb_t rgb_imd;
				if(fade_rate != 0.0)
				{
					fade_table = detail::fade_table(1 - fade_rate);
					rgb_imd.u.color = color;
					rgb_imd = detail::fade_color_intermedia(rgb_imd, fade_table);
				}

				nana::size px_size = pixbuf.size();
				nana::pixel_rgb_t * i = pixbuf.raw_ptr() + pos_beg.y * px_size.width + pos_beg.x;

				int dx = pos_end.x - pos_beg.x;
				int dy = pos_end.y - pos_beg.y;
				
				int step_line;
				if(dy < 0)
				{
					dy = -dy;
					step_line = -static_cast<int>(px_size.width);
				}
				else
					step_line = static_cast<int>(px_size.width);

				if(dx == dy)
				{
					++step_line;
					++dx;

					if(fade_table)
					{
						for(int x = 0; x < dx; ++x)
						{
							*i = detail::fade_color_by_intermedia(*i, rgb_imd, fade_table);
							i += step_line;
						}
					}
					else
					{
						for(int x = 0; x < dx; ++x)
						{
							i->u.color = color;
							i += step_line;
						}			
					}
				}
				else
				{
					int dx_2 = dx << 1;
					int dy_2 = dy << 1;
					if(dx > dy)
					{
						int error = dy_2 - dx;
						++dx;						//Include the end poing

						if(fade_table)
						{
							for(int x = 0; x < dx; ++x)
							{
								*i = detail::fade_color_by_intermedia(*i, rgb_imd, fade_table);
								if(error >= 0)
								{
									error -= dx_2;
									i += step_line;
								}
								error += dy_2;
								++i;
							}		
						}
						else
						{
							for(int x = 0; x < dx; ++x)
							{
								i->u.color = color;
								if(error >= 0)
								{
									error -= dx_2;
									i += step_line;
								}
								error += dy_2;
								++i;
							}
						}
					}
					else
					{
						int error = dx_2 - dy;
						++dy;						//Include the end point

						if(fade_table)
						{
							for(int y = 0; y < dy; ++y)
							{
								*i = detail::fade_color_by_intermedia(*i, rgb_imd, fade_table);
								if(error >= 0)
								{
									error -= dy_2;
									++i;
								}
								error += dx_2;
								i += step_line;
							}					
						}
						else
						{
							for(int y = 0; y < dy; ++y)
							{
								i->u.color = color;
								if(error >= 0)
								{
									error -= dy_2;
									++i;
								}
								error += dx_2;
								i += step_line;
							}
						}
					}
				}

				detail::free_fade_table(fade_table);
			}
		};
	}
}
}
}

#endif
