/*
 *	A Tooltip Implementation
 *	Copyright(C) 2003-2013 Jinhao(cnjinhao@hotmail.com)
 *
 *	Distributed under the Boost Software License, Version 1.0.
 *	(See accompanying file LICENSE_1_0.txt or copy at
 *	http://www.boost.org/LICENSE_1_0.txt)
 *
 *	@file: nana/gui/tooltip.cpp
 */

#include <nana/gui/tooltip.hpp>
#include <nana/gui/timer.hpp>

namespace nana{ namespace gui{
	namespace drawerbase
	{
		namespace tooltip
		{
			class drawer
				: public drawer_trigger
			{
			public:
				void set_tooltip_text(const nana::string& text)
				{
					text_ = text;

					nana::size txt_s;
					nana::string::size_type beg = 0;
					nana::string::size_type off = text.find('\n', beg);

					for(;off != nana::string::npos; off = text.find('\n', beg))
					{
						nana::size s = graph_->text_extent_size(text_.c_str() + beg, off - beg);
						if(s.width > txt_s.width)
							txt_s.width = s.width;
						txt_s.height += (s.height + 2);
						beg = off + 1;
					}

					nana::size s = graph_->text_extent_size(text_.c_str() + beg);
					if(s.width > txt_s.width)
						txt_s.width = s.width;

					widget_->size(txt_s.width + 10, txt_s.height + s.height + 10);
				}
			private:
				void bind_window(widget_reference widget)
				{
					widget_ = &widget;
				}

				void attached(graph_reference graph)
				{
					graph_ = &graph;
				}

				void refresh(graph_reference graph)
				{
					nana::size text_s = graph.text_extent_size(text_);

					graph.rectangle(0x0, false);
					graph.rectangle(1, 1, graph.width() - 2, graph.height() - 2, 0xF0F0F0, true);

					const int x = 5;
					int y = 5;

					nana::string::size_type beg = 0;
					nana::string::size_type off = text_.find('\n', beg);

					for(;off != nana::string::npos; off = text_.find('\n', beg))
					{
						graph.string(x, y, 0x0, text_.substr(beg, off - beg));
						y += text_s.height + 2;
						beg = off + 1;
					}

					graph.string(x, y, 0x0, text_.c_str() + beg, text_.size() - beg);
				}
			private:
				widget	*widget_;
				nana::paint::graphics * graph_;
				nana::string text_;
			};

			class uiform
				: public widget_object<category::root_tag, drawer>
			{
				typedef widget_object<category::root_tag, drawer> base_type;
			public:
				uiform()
					:base_type(nana::rectangle(), appear::bald<appear::floating>())
				{
					API::take_active(this->handle(), false, nullptr);
					timer_.interval(500);
					timer_.make_tick(std::bind(&uiform::_m_tick, this));
				}

				void set_tooltip_text(const nana::string& text)
				{
					get_drawer_trigger().set_tooltip_text(text);
				}
			private:
				void _m_tick()
				{
					timer_.enable(false);
					show();
				}

			private:
				timer timer_;
			};//class uiform

			class controller
			{
				typedef std::pair<window, nana::string> pair_t;
				typedef controller self_type;

				controller()
					:window_(nullptr), count_ref_(0)
				{}
			public:
				static std::recursive_mutex& mutex()
				{
					static std::recursive_mutex rcs_mutex;
					return rcs_mutex;
				}

				static controller* object(bool destroy = false)
				{
					static controller* ptr;
					if(destroy)
					{
						delete ptr;
						ptr = nullptr;
					}
					else if(nullptr == ptr)
					{
						ptr = new controller;
					}
					return ptr;
				}

				void inc()
				{
					++count_ref_;
				}

				unsigned long dec()
				{
					return --count_ref_;
				}

				void set(window wd, const nana::string& str)
				{
					_m_get(wd)->second = str;
				}

				void show(int x, int y, const nana::string& text)
				{
					if(nullptr == window_)
						window_ = new uiform;

					window_->set_tooltip_text(text);
					nana::size sz = window_->size();
					nana::size screen_size = API::screen_size();
					if(x + sz.width >= screen_size.width)
						x = static_cast<int>(screen_size.width) - static_cast<int>(sz.width);

					if(y + sz.height >= screen_size.height)
						y = static_cast<int>(screen_size.height) - static_cast<int>(sz.height);

					if(x < 0) x = 0;
					if(y < 0) y = 0;
					window_->move(x, y);
				}

				void close()
				{
					delete window_;
					window_ = nullptr;
				}
			private:
				void _m_enter(const eventinfo& ei)
				{
					pair_t * p = _m_get(ei.window);
					if(p && p->second.size())
					{
						nana::point pos = API::cursor_position();
						this->show(pos.x, pos.y + 20, p->second);
					}
				}

				void _m_leave(const eventinfo& ei)
				{
					delete window_;
					window_ = nullptr;
				}

				void _m_destroy(const eventinfo& ei)
				{
					for(auto i = cont_.begin(); i != cont_.end(); ++i)
					{
						if((*i)->first == ei.window)
						{
							cont_.erase(i);
							return;
						}
					}
				}
			private:
				pair_t* _m_get(window wd)
				{
					for(auto pr : cont_)
					{
						if(pr->first == wd)
							return pr;
					}

					API::make_event<events::mouse_enter>(wd, std::bind(&self_type::_m_enter, this, std::placeholders::_1));
					auto leave_fn = std::bind(&self_type::_m_leave, this, std::placeholders::_1);
					API::make_event<events::mouse_leave>(wd, leave_fn);
					API::make_event<events::mouse_down>(wd, leave_fn);
					API::make_event<events::destroy>(wd, std::bind(&self_type::_m_destroy, this, std::placeholders::_1));

					pair_t * newp = new pair_t(wd, nana::string());
					cont_.push_back(newp);
					return newp;
				}
			private:
				uiform * window_;
				std::vector<pair_t*> cont_;
				unsigned long count_ref_;
			};
		}//namespace tooltip
	}//namespace drawerbase

	//class tooltip
		tooltip::tooltip()
		{
			typedef drawerbase::tooltip::controller ctrl;
			std::lock_guard<decltype(ctrl::mutex())> lock(ctrl::mutex());
			ctrl::object()->inc();
		}

		tooltip::~tooltip()
		{
			typedef drawerbase::tooltip::controller ctrl;
			std::lock_guard<decltype(ctrl::mutex())> lock(ctrl::mutex());

			if(0 == ctrl::object()->dec())
				ctrl::object(true);
		}

		void tooltip::set(window wd, const nana::string& text)
		{
			if(false == API::empty_window(wd))
			{
				typedef drawerbase::tooltip::controller ctrl;
				std::lock_guard<decltype(ctrl::mutex())> lock(ctrl::mutex());
				ctrl::object()->set(wd, text);
			}
		}

		void tooltip::show(window wnd, int x, int y, const nana::string& text)
		{
			typedef drawerbase::tooltip::controller ctrl;
			std::lock_guard<decltype(ctrl::mutex())> lock(ctrl::mutex());

			nana::point pos(x, y);
			API::calc_screen_point(wnd, pos);
			ctrl::object()->show(pos.x, pos.y, text);
		}

		void tooltip::close()
		{
			typedef drawerbase::tooltip::controller ctrl;
			std::lock_guard<decltype(ctrl::mutex())> lock(ctrl::mutex());
			ctrl::object()->close();
		}
	//end class tooltip

}//namespace gui
}//namespace nana

