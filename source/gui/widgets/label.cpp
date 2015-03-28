/*
 *	A Label Control Implementation
 *	Copyright(C) 2003-2013 Jinhao(cnjinhao@hotmail.com)
 *
 *	Distributed under the Boost Software License, Version 1.0.
 *	(See accompanying file LICENSE_1_0.txt or copy at
 *	http://www.boost.org/LICENSE_1_0.txt)
 *
 *	@file: source/gui/widgets/label.cpp
 */

#include <nana/gui/widgets/label.hpp>
#include <nana/system/platform.hpp>
#include <nana/unicode_bidi.hpp>
#include <nana/paint/text_renderer.hpp>
#include <stdexcept>
#include <sstream>

namespace nana
{
namespace gui
{
	namespace drawerbase
	{
		namespace label
		{
			class renderer_interface
			{
			public:
				typedef widget	&	widget_reference;
				typedef nana::paint::graphics&	graph_reference;

				virtual ~renderer_interface(){}
				virtual void parse(window, const nana::string&) = 0;
				virtual void render(widget_reference, graph_reference, align) = 0;
				virtual unsigned extent_size(graph_reference) = 0;
				virtual nana::size measure(graph_reference) = 0;
				virtual void bind_listener(std::function<void(command, const nana::string&)> &&) = 0;
				virtual void over(int x, int y) = 0;
				virtual void leave() = 0;
				virtual void click() = 0;
			};

			class simple_renderer
				: public renderer_interface
			{
				window wd_;
			
				void parse(window wd, const nana::string&)
				{
					wd_ = wd;
				}

				void render(widget_reference wd, graph_reference graph, align text_align)
				{
					nana::string text = API::dev::window_caption(wd_);
					nana::string::size_type len = text.length();
					if(len)
					{
						nana::paint::text_renderer tr(graph, text_align);
						tr.render(0, 0, wd.foreground(), text.c_str(), len, graph.width());
					}
					return;
				}

				unsigned extent_size(graph_reference graph)
				{
					nana::string text = API::dev::window_caption(wd_);
					nana::string::size_type len = text.length();
					if(len)
					{
						nana::paint::text_renderer tr(graph);
						return tr.extent_size(0, 0, text.c_str(), text.length(), graph.width()).height;
					}
					return 0;
				}

				nana::size measure(graph_reference graph)
				{
					nana::string text = API::dev::window_caption(wd_);

					nana::size extsize;
					if(0 == text.length())
						return extsize;

					nana::size txt_size = graph.text_extent_size(STR("jH"));
					nana::string::size_type start = 0;
					auto pos = text.find(nana::char_t('\n'), start);

					//[Feature:1]GUI.Label supports that transforms '\n' into new line autometically
					extsize.height = txt_size.height;
					while(true)
					{
						if(pos > start)
						{
							auto px = graph.text_extent_size(text.substr(start, pos - start)).width;
							if(extsize.width < px)
								extsize.width = px;
						}

						if(nana::string::npos == pos)
							break;

						start = pos + 1;
						pos = text.find(nana::char_t('\n'), start );
						extsize.height += txt_size.height;
					}
					return extsize;
				}

				void bind_listener(std::function<void(command, const nana::string&)> &&){}
				void over(int x, int y){}
				void leave(){}
				void click(){}
			};

			class content
			{
				struct tokens
				{
					enum t{string, pure_string, number, beg, end, font, bold, size, color, url, target, equal, comma, backslash, _true, _false, endl, eof};
				};

				struct states
				{
					enum t{init, init_fmt, pstr, escape, eof};
				};

				typedef tokens::t token_t;

				struct section_t
				{
					nana::string font;
					unsigned size;
					bool bold;
					nana::color_t color;
					nana::string url;
					nana::string target;
					nana::string str;
					std::vector<nana::rectangle> areas;
				};

				struct font_t
				{
					nana::string font;
					unsigned size;
					bool bold;
					nana::color_t color;
				};

				std::vector<std::vector<section_t*> > container_;
			public:
				typedef std::vector<section_t*> line_container;
				const static unsigned nsize = static_cast<unsigned>(-1);

				~content()
				{
					_m_clear();
				}

				const line_container* getline(unsigned x) const
				{
					return (x < container_.size() ? &container_[x] : nullptr);
				}

				void parse(const nana::string& txt)
				{
					data_.init(txt);
					_m_clear();
					_m_parse();
				}
			private:
				void _m_append(const section_t& sec)
				{
					if(container_.size() == 0)
						container_.emplace_back();

					container_.back().push_back(new section_t(sec));
				}

				void _m_endl()
				{
					container_.emplace_back();
				}

				void _m_clear()
				{
					for(auto & line : container_)
					{
						for(auto section : line)
							delete section;
					}
					container_.clear();
				}
			private:
				void _m_parse()
				{
					section_t section;
					section.size = nsize;
					section.bold = false;
					section.color = 0xFF000000;

					while(true)
					{
						token_t tk = _m_token();
						switch(tk)
						{
						case tokens::string:
							section.str += data_.tokenstr;
							_m_append(section);
							section.str.clear();
							break;
						case tokens::endl:
							_m_endl();
							break;
						case tokens::beg:
							tk = _m_token();
							if(tk == tokens::backslash)
							{
								do
								{
									tk = _m_token();
								}while(tokens::end != tk && tokens::eof != tk);
							}
							else
								_m_enter_format(tk, section);

							break;
						case tokens::eof:
							return;
						default:
							throw std::runtime_error( "Nana.GUI.Label: Bad format" );
						}
					}
				}

				int _m_tokenstr_to_number()
				{
					std::stringstream ss;
					ss<<static_cast<std::string>(nana::charset(data_.tokenstr));
					if(data_.tokenstr.size() > 2)
					{
						if(data_.tokenstr[0] == '0' && (data_.tokenstr[1] == 'x' || data_.tokenstr[1] == 'X'))
							ss>>std::hex;
					}

					int n;
					ss>>n;

					return n;
				}

				bool _m_enter_format(token_t tk, const section_t & init_s)
				{
					section_t sect = init_s;
					sect.str.clear();

					if(tk == tokens::backslash)
					{
						tk = _m_token();
						if(tk == tokens::end)
							return false;
					}
					while(tk != tokens::end && tk != tokens::eof)
					{
						switch(tk)
						{
						case tokens::font:
							tk = _m_token();
							if(tk == tokens::equal)
							{
								tk = _m_token();
								if(tk == tokens::pure_string)
									sect.font = data_.tokenstr;
							}
							break;
						case tokens::url:
							tk = _m_token();
							if(tokens::equal == tk)
							{
								tk = _m_token();
								if(tk == tokens::pure_string)
									sect.url = data_.tokenstr;
							}
							break;
						case tokens::target:
							tk = _m_token();
							if(tokens::equal == tk)
							{
								tk = _m_token();
								if(tk == tokens::pure_string)
									sect.target = data_.tokenstr;
							}
							break;
						case tokens::bold:
							tk = _m_token();
							if(tk == tokens::equal)
							{
								tk = _m_token();
								switch(tk)
								{
								case tokens::_true:
									sect.bold = true;
									break;
								case tokens::_false:
									sect.bold = false;
									break;
								default:
									throw std::runtime_error("Nana.GUI.Label: The value of bool should be \"true\" or \"false\"");
								}
							}
							else
								sect.bold = true;
							break;
						case tokens::size:
							tk = _m_token();
							if(tk == tokens::equal)
							{
								tk = _m_token();
								if(tk == tokens::number)
									sect.size = _m_tokenstr_to_number();
							}
							break;
						case tokens::color:
							tk = _m_token();
							if(tk == tokens::equal)
							{
								tk = _m_token();
								if(tk == tokens::number)
									sect.color = _m_tokenstr_to_number();
							}
							break;
						default:
							tk = _m_token();
						}
					}

					tk = _m_token();
					bool loop = true;
					while(loop)
					{
						switch(tk)
						{
						case tokens::string:
							sect.str = data_.tokenstr;
							_m_append(sect);
							sect.str.clear();
							tk = _m_token();
							break;
						case tokens::endl:
							_m_endl();
							tk = _m_token();
							break;
						case tokens::beg:
							tk = _m_token();
							if(tk == tokens::backslash)
							{
								do
								{
									tk = _m_token();
								}while(tokens::end != tk && tokens::eof != tk);
								return false;
							}
							else
							{
								if(false == _m_enter_format(tk, sect))
									tk = _m_token();
								else
									loop = false;
							}
							break;
						case tokens::eof:
							loop = false;
							break;
						default:
							throw std::runtime_error("Nana.GUI.Label: Bad format");
						}
					}
					return true;
				}

				token_t _m_token()
				{
					if(data_.pos >= data_.str.size()) return tokens::eof;

					nana::char_t ch = data_.str.c_str()[data_.pos];

					data_.tokenstr.clear();
					states::t state = (data_.init_fmt ? states::init_fmt : states::init);

					while(true)
					{
						switch(state)
						{
						case states::init:
							switch(ch)
							{
							case '\n':
								++data_.pos;
								return tokens::endl;
							case '/':
								++data_.pos;
								return tokens::backslash;
							case '\\':
								state = states::escape;
								break;
							case '<':
								++data_.pos;
								++data_.init_fmt;
								return tokens::beg;
							default:
								while(ch != '\n' && ch != '\\' && ch != '<' && ch != 0)
								{
									data_.tokenstr += ch;
									ch = data_.str.c_str()[++data_.pos];
								}
								return tokens::string;
							}
							break;
						case states::escape:
							switch(ch)
							{
							case '\\':
							case '<':
								++data_.pos;
								data_.tokenstr += ch;
								return tokens::string;
							}
							data_.tokenstr += '\\';
							data_.tokenstr += ch;
							return tokens::string;
						case states::eof:
							return tokens::eof;
						case states::init_fmt:
							while(ch == ' ' || ch == 0) ch = data_.str.c_str()[++data_.pos];

							if(ch == ',')
							{
								++data_.pos;
								return tokens::comma;
							}
							else if(ch == '=')
							{
								++data_.pos;
								return tokens::equal;
							}
							else if(ch == '>')
							{
								if(data_.init_fmt)
									--data_.init_fmt;
								++data_.pos;
								return tokens::end;
							}
							else if(ch == '/')
							{
								++data_.pos;
								return tokens::backslash;
							}
							else if(ch == '"')
							{
								state = states::pstr;
								break;
							}
							else if('0' <= ch && ch <= '9')
							{
								data_.tokenstr += ch;
								if('0' == ch)
								{
									ch = data_.str.c_str()[++data_.pos];
									if((!('0' <= ch && ch <= '9')) && (ch != 'x' && ch != 'X'))
									{
										return tokens::number;
									}
									else if(ch == 'x' || ch == 'X')
									{
										data_.tokenstr += 'x';
										ch = data_.str.c_str()[++data_.pos];
										while(('0' <= ch && ch <= '9') || ('a' <= ch && ch <= 'f') || ('A' <= ch && ch <= 'F'))
										{
											data_.tokenstr += ch;
											ch = data_.str.c_str()[++data_.pos];
										}
										return tokens::number;
									}
								}

								ch = data_.str.c_str()[++data_.pos];
								while('0' <= ch && ch <= '9')
								{
									data_.tokenstr += ch;
									ch = data_.str.c_str()[++data_.pos];
								}

								return tokens::number;
							}
							else
							{
								while(true)
								{
									token_t rtk = tokens::string;
									switch(ch)
									{
									case 0:
									case ',':
									case '=':
									case '>':
									case '"':
									case ' ':
										if(STR("font") == data_.tokenstr)
											return tokens::font;
										else if(STR("bold") == data_.tokenstr)
											return tokens::bold;
										else if(STR("size") == data_.tokenstr)
											return tokens::size;
										else if(STR("color") == data_.tokenstr)
											return tokens::color;
										else if(STR("true") == data_.tokenstr)
											return tokens::_true;
										else if(STR("url") == data_.tokenstr)
											return tokens::url;
										else if(STR("target") == data_.tokenstr)
											return tokens::target;
										else if(STR("false") == data_.tokenstr)
											return tokens::_false;
										else
											return tokens::string;
									}

									if(rtk == tokens::string)
									{
										data_.tokenstr += ch;
										ch = data_.str.c_str()[++data_.pos];
									}
									else
										break;
								}
							}
							break;
						case states::pstr:
							while(ch && (ch != '"'))
							{
								data_.tokenstr += ch;
								ch = data_.str.c_str()[++data_.pos];
							}
							if(ch)
								++data_.pos;
							return tokens::pure_string;
						}
						ch = data_.str.c_str()[++data_.pos];
					}
					return tokens::eof;
				}


			private:
				struct data_tag
				{
					nana::string tokenstr;
					nana::string str;
					nana::string::size_type pos;
					unsigned init_fmt;


					data_tag()
						: pos(0), init_fmt(0)
					{}

					void init(const nana::string& txt)
					{
						str = txt;
						pos = 0;
						tokenstr.clear();
						init_fmt = 0;
					}
				}data_;
			};

			class format_renderer
				:public renderer_interface
			{
				const static unsigned nsize = static_cast<unsigned>(-1);

				void parse(window, const nana::string& s)
				{
					content_.parse(s);
				}

				void render(widget_reference wd, graph_reference graph, align text_align)
				{
					trace_.wd = wd;

					color_sect_ = color_fg_ = wd.foreground();
					nana::paint::font font;
					nana::point pos;
					unsigned bounds = graph.height();
					const content::line_container * line = nullptr;
					for(unsigned i = 0; (line = content_.getline(i)); ++i)
					{
						unsigned px = _m_line_pixels(graph, line);
						for(auto section : *line)
						{
							if(pos.y > static_cast<int>(bounds)) return;
							_m_change_font(graph, font, section);
							pos = _m_draw_string(px, pos.x, pos.y, graph, section->str, section);
						}
						pos.y += px;
						pos.x = 0;
					}
				}

				virtual unsigned extent_size(graph_reference graph)
				{
					nana::paint::font font;
					nana::point pos;

					const content::line_container * line = nullptr;
					for(unsigned i = 0; (line = content_.getline(i)); ++i)
					{
						unsigned px = _m_line_pixels(graph, line);
						for(auto section : *line)
						{
							_m_change_font(graph, font, section);
							pos = _m_extent_size(px, pos.x, pos.y, graph, section->str, section);
						}
						pos.y += px;
						pos.x = 0;
					}
					return static_cast<unsigned>(pos.y);				
				}

				virtual nana::size measure(graph_reference graph)
				{
					nana::paint::font font;
					nana::point pos;

					const content::line_container * line = nullptr;
					nana::size ts;
					for(unsigned i = 0; (line = content_.getline(i)); ++i)
					{
						unsigned px = _m_line_pixels(graph, line);
						unsigned width = 0;
						for(auto section : *line)
						{
							_m_change_font(graph, font, section);
							width += graph.text_extent_size(section->str).width;
						}
						if(ts.width < width)
							ts.width = width;
						ts.height += px;
					}
					return ts;
				}

				void bind_listener(std::function<void(command, const nana::string&)> && ls)
				{
					listener_ = std::move(ls);
				}

				void over(int x, int y)
				{
					const content::line_container * line = nullptr;
					for(unsigned i = 0; (line = content_.getline(i)); ++i)
					{
						for(auto section : *line)
							for(auto & r : section->areas)
								if(is_hit_the_rectangle(r, x, y))
								{
									API::window_cursor(trace_.wd, cursor::hand);
									trace_.url = section->url;

									if(trace_.target != section->target)
									{
										if(trace_.target.size())
											listener_(command::leave, trace_.target);

										trace_.target = section->target;
										listener_(command::enter, trace_.target);
									}
									return;
								}
					}

					leave();
				}

				void leave()
				{
					if(API::window_cursor(trace_.wd) != cursor::arrow)
					{
						if(trace_.target.size())
							listener_(command::leave, trace_.target);

						API::window_cursor(trace_.wd, cursor::arrow);
						trace_.url.clear();
						trace_.target.clear();
					}
				}

				void click()
				{
					if(trace_.url.size())
						system::open_url(trace_.url);

					if(trace_.target.size())
						listener_(command::click, trace_.target);
				}
			private:
				unsigned _m_line_pixels(graph_reference graph, const content::line_container * line) const
				{
					nana::paint::font font;
					unsigned pixels = 0;
					for(auto section : *line)
					{
						_m_change_font(graph, font, section);
						unsigned px = graph.text_extent_size(section->str).height;
						if(px > pixels)
							pixels = px;
					}
					return (pixels ? pixels : 10);
				}

				void _m_change_font(graph_reference graph, nana::paint::font& font, const content::line_container::value_type s) const
				{
					if((s->font.size() && (s->font != font.name())) || (s->size != nsize && s->size != font.size()) || (s->bold != font.bold()))
					{
						nana::string name = (s->font.size() ? s->font : font.name());
						unsigned size = (s->size != nsize ? s->size : font.size());
						nana::paint::font f(name.c_str(), size, s->bold);
						graph.typeface(f);
						font = f;
					}
				}

				nana::point _m_extent_size(unsigned line_pixels, int x, int y, graph_reference graph, const nana::string& str, const content::line_container::value_type s)
				{
					const unsigned text_area = graph.width();

					unsigned dw = text_area - x;
					int off = 0;
					int draw_y_pos;
					nana::rectangle r;
					while(true)
					{
						nana::size ts = graph.text_extent_size(str.c_str() + off, str.size() - off);
						if(ts.width <= dw)
							return nana::point(x + ts.width, y);

						unsigned pixels = 0;
						int len = 1;
						draw_y_pos = y;
						for(; off + len <= static_cast<int>(s->str.size()); ++len)
						{
							ts = graph.text_extent_size(str.c_str() + off, len);
							if(ts.width >= dw)
							{
								if(ts.width > dw)
								{
									//a line must draw at least one character even through there is not a room for one character
									if(len > 1 || text_area > ts.width)
										--len;

									y += line_pixels;
									dw = text_area;
								}
								else
								{
									pixels = ts.width;
									dw -= ts.width;
								}
								break;
							}
							else
								pixels = ts.width;
						}

						//The text is splitted for a new line
						if(y != draw_y_pos)
						{
							x = 0;
							draw_y_pos = y;
						}
						else
							x += pixels;

						off += len;
						if(off >= static_cast<int>(str.size())) break;
					}
					return nana::point(x, y);			
				}

				static bool _m_want_area(const content::line_container::value_type s)
				{
					return (s->url.size() || s->target.size());
				}

				nana::point _m_draw_string(unsigned line_height, int x, int y, graph_reference graph, const nana::string& str,  const content::line_container::value_type s)
				{
					const unsigned text_area = graph.width();
					nana::color_t clr = s->color == 0xFF000000 ? color_fg_ : s->color;

					unsigned dw = text_area - x;
					int off = 0;
					int draw_y_pos;
					nana::rectangle r;
					while(true)
					{
						nana::size ts = graph.text_extent_size(str.c_str() + off, str.size() - off);
						if(ts.width <= dw)
						{
							graph.string(x, y + (line_height - ts.height) / 2, clr, str.c_str() + off, str.size() - off);
							if(_m_want_area(s))
							{
								r.x = x;
								r.y = y;
								r.width = ts.width;
								r.height = ts.height;
								s->areas.push_back(r);
							}
							return nana::point(x + ts.width, y);
						}

						unsigned pixels = 0;
						int len = 1;
						draw_y_pos = y;
						for(; off + len <= static_cast<int>(s->str.size()); ++len)
						{
							ts = graph.text_extent_size(str.c_str() + off, len);
							if(ts.width >= dw)
							{
								if(ts.width > dw)
								{
									//a line must draw at least one character even through there is not a room for one character
									if(len > 1 || text_area > ts.width)
										--len;

									y += line_height;
									dw = graph.width();
								}
								else
								{
									pixels = ts.width;
									dw -= ts.width;
								}
								break;
							}
							else
								pixels = ts.width;
						}
						if(len)
						{
							if(_m_want_area(s))
							{
								r.x = x;
								r.y = draw_y_pos;
								r.width = ts.width;
								r.height = ts.height;
								s->areas.push_back(r);
							}
							graph.string(x, draw_y_pos + (line_height - ts.height) / 2, clr, str.c_str() + off, len);
						}

						//The text is splitted for a new line
						if(y != draw_y_pos)
						{
							x = 0;
							draw_y_pos = y;
						}
						else
							x += pixels;

						off += len;
						if(off >= static_cast<int>(str.size())) break;
					}
					return nana::point(x, y);
				}
			private:
				struct trace_tag
				{
					window wd;
					nana::string url;
					nana::string target;
				}trace_;

				std::function<void(command, const nana::string&)> listener_;
				content content_;
				nana::color_t color_sect_;
				nana::color_t color_fg_;
			};
			
			//class trigger
			//@brief: Draw the button
				struct trigger::impl_t
				{
					widget * wd;
					nana::paint::graphics * graph;
					renderer_interface * renderer;
					align	text_align;

					impl_t()
						:	wd(nullptr), graph(nullptr), text_align(align::left),
							format_state_(false)
					{
						renderer = new simple_renderer;
					}

					~impl_t()
					{
						delete renderer;
					}

					bool format(bool enabled)
					{
						if((enabled != format_state_) && wd)
						{
							internal_scope_guard isg;
							renderer_interface * rnd_if = renderer;
							if(enabled)
							{
								renderer = new format_renderer;
								renderer->bind_listener(std::bind(&impl_t::_m_call_listener, this, std::placeholders::_1, std::placeholders::_2));
							}
							else
								renderer = new simple_renderer;
							format_state_ = enabled;
							delete rnd_if;
							return true;
						}
						return false;
					}

					void add_listener(const std::function<void(command, const nana::string&)> & f)
					{
						listener_ += f;
					}

					void add_listener(std::function<void(command, const nana::string&)> && f)
					{
						listener_ += std::move(f);
					}
				private:
					void _m_call_listener(command cmd, const nana::string& tar)
					{
						listener_(cmd, tar);
					}
				private:
					bool format_state_;
					nana::fn_group<void(command, const nana::string&)> listener_;
				};

				trigger::trigger()
					:impl_(new impl_t)
				{}

				trigger::~trigger()
				{
					delete impl_;
				}

				void trigger::bind_window(widget_reference w)
				{
					impl_->wd = &w;
				}

				trigger::impl_t * trigger::impl() const
				{
					return impl_;
				}

				void trigger::attached(graph_reference graph)
				{
					impl_->graph = &graph;
					window wd = impl_->wd->handle();
					API::dev::make_drawer_event<events::mouse_move>(wd);
					API::dev::make_drawer_event<events::mouse_leave>(wd);
					API::dev::make_drawer_event<events::click>(wd);
				}

				void trigger::detached()
				{
					API::dev::umake_drawer_event(impl_->wd->handle());
				}

				void trigger::mouse_move(graph_reference, const eventinfo& ei)
				{
					impl_->renderer->over(ei.mouse.x, ei.mouse.y);
				}

				void trigger::mouse_leave(graph_reference, const eventinfo& ei)
				{
					impl_->renderer->leave();
				}

				void trigger::click(graph_reference, const eventinfo&)
				{
					impl_->renderer->click();
				}

				void trigger::refresh(graph_reference graph)
				{
					if(0 == impl_->wd) return;

					window wd = impl_->wd->handle();
					if(false == API::glass_window(wd))
						graph.rectangle(API::background(wd), true);

					impl_->renderer->render(*impl_->wd, graph, impl_->text_align);
				}

			//end class label_drawer
		}//end namespace label
	}//end namespace drawerbase


	//
	//class label
		label::label(){}

		label::label(window wd, bool visible)
		{
			create(wd, rectangle(), visible);
		}

		label::label(window wd, const nana::rectangle& r, bool visible)
		{
			create(wd, r, visible);
		}

		void label::transparent(bool value)
		{
			internal_scope_guard isg;
			if(API::glass_window(this->handle(), value))
				API::refresh_window(this->handle());
		}

		bool label::transparent() const
		{
			return API::glass_window(this->handle());
		}

		void label::format(bool f)
		{
			auto impl = get_drawer_trigger().impl();
			if(impl->format(f))
			{
				window wd = *this;
				impl->renderer->parse(wd, API::dev::window_caption(wd));
				API::refresh_window(wd);
			}
		}

		void label::add_format_listener(const std::function<void(command, const nana::string&)> & f)
		{
			get_drawer_trigger().impl()->add_listener(f);
		}

		void label::add_format_listener(std::function<void(command, const nana::string&)> && f)
		{
			get_drawer_trigger().impl()->add_listener(std::move(f));
		}

		nana::size label::measure() const
		{
			if(this->empty())	return nana::size();

			auto impl = get_drawer_trigger().impl();
			return impl->renderer->measure(*(impl->graph));
		}

		unsigned label::extent_size() const
		{
			if(this->empty())	return 0;

			auto impl = get_drawer_trigger().impl();
			return impl->renderer->extent_size(*(impl->graph));
		}

		void label::text_align(align dir)
		{
			internal_scope_guard isg;
			auto impl = get_drawer_trigger().impl();
			if(impl->text_align != dir)
			{
				impl->text_align = dir;
				API::refresh_window(*this);
			}
		}

		void label::_m_caption(const nana::string& s)
		{
			internal_scope_guard isg;
			window wd = *this;
			auto impl = get_drawer_trigger().impl();
			impl->renderer->parse(wd, s);
			API::dev::window_caption(wd, s);
			API::refresh_window(wd);
		}
	//end class label
}//end namespace gui
}//end namespace nana
