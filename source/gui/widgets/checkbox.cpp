/*
 *	A CheckBox Implementation
 *	Copyright(C) 2003-2013 Jinhao(cnjinhao@hotmail.com)
 *
 *	Distributed under the Boost Software License, Version 1.0.
 *	(See accompanying file LICENSE_1_0.txt or copy at
 *	http://www.boost.org/LICENSE_1_0.txt)
 *
 *	@file: nana/gui/widgets/checkbox.cpp
 */

#include <nana/gui/widgets/checkbox.hpp>
#include <nana/paint/gadget.hpp>
#include <nana/paint/text_renderer.hpp>

namespace nana
{
namespace gui
{
namespace xcheckbox
{
		//class drawer
			drawer::drawer():widget_(nullptr)
			{
				checker_.react = true;
				checker_.checked = checker_.radio = false;
				checker_.type = paint::gadget::check_renderer::clasp;
			}

			void drawer::bind_window(widget_reference w)
			{
				widget_ = &w;
			}

			void drawer::attached(graph_reference)
			{
				window wd = *widget_;
				using namespace API::dev;
				make_drawer_event<events::mouse_down>(wd);
				make_drawer_event<events::mouse_up>(wd);
				make_drawer_event<events::mouse_enter>(wd);
				make_drawer_event<events::mouse_leave>(wd);
			}

			void drawer::detached()
			{
				API::dev::umake_drawer_event(*widget_);
			}

			void drawer::refresh(graph_reference graph)
			{
				_m_draw(graph);
			}

			void drawer::mouse_down(graph_reference graph, const eventinfo&)
			{
				_m_draw(graph);
			}

			void drawer::mouse_up(graph_reference graph, const eventinfo&)
			{
				if(checker_.react)
					checker_.checked = ! checker_.checked;

				_m_draw(graph);
			}

			void drawer::mouse_enter(graph_reference graph, const eventinfo&)
			{
				_m_draw(graph);
			}

			void drawer::mouse_leave(graph_reference graph, const eventinfo&)
			{
				_m_draw(graph);
			}

			paint::gadget::check_renderer& drawer::check_renderer()
			{
				return checker_.renderer;
			}

			void drawer::react(bool is_react)
			{
				checker_.react = is_react;
			}

			void drawer::checked(bool chk)
			{
				checker_.checked = chk;
				API::refresh_window(*widget_);
			}

			bool drawer::checked() const
			{
				return checker_.checked;
			}

			void drawer::radio(bool is_radio)
			{
				checker_.radio = is_radio;
			}

			void drawer::style(check_renderer_t::checker_t chk)
			{
				if(false == checker_.radio)
				{
					switch(chk)
					{
					case checkbox::blocker:
						checker_.type = paint::gadget::check_renderer::blocker;
						break;
					case checkbox::clasp:
					default:
						checker_.type = paint::gadget::check_renderer::clasp;
						break;
					}
				}
			}

			drawer::check_renderer_t::checker_t drawer::style() const
			{
				return checker_.type;
			}

			void drawer::_m_draw(graph_reference graph)
			{
				_m_draw_background(graph);
				_m_draw_title(graph);
				_m_draw_checkbox(graph, graph.text_extent_size(STR("jN"), 2).height + 2);
				API::lazy_refresh();
			}

			void drawer::_m_draw_background(graph_reference graph)
			{
				if(false == API::glass_window(*widget_))
					graph.rectangle(API::background(*widget_), true);
			}

			void drawer::_m_draw_checkbox(graph_reference graph, unsigned first_line_height)
			{
				checker_.renderer.render(graph, 0, (first_line_height > 16 ? (first_line_height - 16) / 2 : 0), 16, 16, API::mouse_action(*widget_), (checker_.radio ? paint::gadget::check_renderer::radio : checker_.type), checker_.checked);
			}

			void drawer::_m_draw_title(graph_reference graph)
			{
				if(graph.width() > 16 + interval)
				{
					nana::string title = widget_->caption();

					unsigned fgcolor = widget_->foreground();
					unsigned pixels = graph.width() - (16 + interval);

					nana::paint::text_renderer tr(graph);
					if(API::window_enabled(widget_->handle()) == false)
					{
						tr.render(17 + interval, 2, 0xFFFFFF, title.c_str(), title.length(), pixels);
						fgcolor = 0x808080;
					}

					tr.render(16 + interval, 1, fgcolor, title.c_str(), title.length(), pixels);
				}
			}
		//end class drawer
}//end namespace xcheckbox

	//class checkbox

		checkbox::checkbox(){}

		checkbox::checkbox(window wd, bool visible)
		{
			create(wd, rectangle(), visible);
		}

		checkbox::checkbox(window wd, const nana::rectangle& r, bool visible)
		{
			create(wd, r, visible);
		}

		void checkbox::react(bool want)
		{
			get_drawer_trigger().react(want);
		}

		bool checkbox::checked() const
		{
			return get_drawer_trigger().checked();
		}

		void checkbox::check(bool chk)
		{
			get_drawer_trigger().checked(chk);
		}

		void checkbox::radio(bool is_radio)
		{
			get_drawer_trigger().radio(is_radio);
		}

		void checkbox::style(checker_t chk)
		{
			get_drawer_trigger().style(static_cast<drawer_trigger_t::check_renderer_t::checker_t>(chk));
			API::refresh_window(*this);
		}

		auto checkbox::style() const -> checker_t
		{
			return static_cast<checker_t>(get_drawer_trigger().style());
		}

		void checkbox::transparent(bool value)
		{
			if(API::glass_window(*this, value) != value)
				API::refresh_window(*this);
		}

		bool checkbox::transparent() const
		{
			return API::glass_window(*this);
		}

		void checkbox::open_check_image(const nana::paint::image& img)
		{
			get_drawer_trigger().check_renderer().open_background_image(img);
		}

		void checkbox::set_check_image(mouse_action act, checkbox::checker_t chk, bool checked, const nana::rectangle& r)
		{
			typedef nana::paint::gadget::check_renderer checks;
			get_drawer_trigger().check_renderer().set_image_state(act, (checkbox::blocker == chk ? checks::blocker : checks::clasp), checked, r);
		}
	//end class checkbox

	//class radio_group
		radio_group::~radio_group()
		{
			for(auto & i : ui_container_)
			{
				API::umake_event(i.eh_checked);
				API::umake_event(i.eh_destroy);
			}
		}

		void radio_group::add(checkbox& uiobj)
		{
			uiobj.radio(true);
			uiobj.check(false);
			uiobj.react(false);

			element_tag el;

			el.uiobj = &uiobj;
			el.eh_checked = uiobj.make_event<events::click>(*this, &radio_group::_m_checked);
			el.eh_destroy = uiobj.make_event<events::destroy>(*this, &radio_group::_m_destroy);
			ui_container_.push_back(el);
		}

		std::size_t radio_group::checked() const
		{
			auto i = std::find_if(ui_container_.cbegin(), ui_container_.cend(), [](decltype(*ui_container_.cbegin())& x)
				{
					return (x.uiobj->checked());
				});
			return static_cast<std::size_t>(i - ui_container_.cbegin());
		}

		void radio_group::_m_checked(const eventinfo& ei)
		{
			for(auto & i : ui_container_)
				i.uiobj->check(ei.window == i.uiobj->handle());
		}

		void radio_group::_m_destroy(const eventinfo& ei)
		{
			auto i = std::find_if(ui_container_.begin(), ui_container_.end(), [&ei](decltype(*ui_container_.begin()) & x)
					{
						return (ei.window == x.uiobj->handle());
					});
			if(i != ui_container_.end())
				ui_container_.erase(i);
		}
	//end class radio_group
}//end namespace gui
}//end namespace nana
