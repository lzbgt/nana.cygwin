/*
 *	A Button Implementation
 *	Copyright(C) 2003-2013 Jinhao(cnjinhao@hotmail.com)
 *
 *	Distributed under the Boost Software License, Version 1.0. 
 *	(See accompanying file LICENSE_1_0.txt or copy at 
 *	http://www.boost.org/LICENSE_1_0.txt)
 *
 *	@file: nana/gui/widgets/button.hpp
 */

#ifndef NANA_GUI_WIDGET_BUTTON_HPP
#define NANA_GUI_WIDGET_BUTTON_HPP
#include "widget.hpp"


namespace nana{namespace gui{
	namespace drawerbase
	{
		namespace button
		{
			//button_drawer
			//@brief:	Draw the button
			class trigger: public drawer_trigger
			{
			public:
				struct bgimage_tag;
				enum class state
				{
					normal, focused, highlight, pressed, disabled
				};
				
				trigger();
				~trigger();
				void icon(const nana::paint::image&);
				void image(const nana::paint::image&);
				bgimage_tag * ref_bgimage();
				bool enable_pushed(bool);
				bool pushed(bool);
				bool pushed() const;
				void omitted(bool);
				bool focus_color(bool);
			private:
				void bind_window(widget_reference);
				void attached(graph_reference);
				void detached();
				void refresh(graph_reference);
				void mouse_enter(graph_reference, const eventinfo&);
				void mouse_leave(graph_reference, const eventinfo&);
				void mouse_down(graph_reference, const eventinfo&);
				void mouse_up(graph_reference, const eventinfo&);
				void key_char(graph_reference, const eventinfo&);
				void key_down(graph_reference, const eventinfo&);
				void focus(graph_reference, const eventinfo&);
			private:
				void _m_draw(graph_reference);
				void _m_draw_title(graph_reference, bool enabled);
				void _m_draw_background(graph_reference);
				void _m_draw_border(graph_reference);
			private:
				nana::gui::widget* widget_;
				nana::paint::graphics* graph_;

				struct bgimage_tag* bgimage_;
				struct attr_tag
				{
					bool omitted;
					bool focused;
					bool pushed;
					bool keep_pressed;
					state act_state;
					bool enable_pushed;
					bool focus_color;
					paint::image * icon;
					color_t bgcolor;
					color_t fgcolor;
				}attr_;
			};
		}//end namespace button
	}//end namespace drawerbase

		//button
		//@brief: define a button widget and it provides the interfaces to be operational
		class button
			: public widget_object<category::widget_tag, drawerbase::button::trigger>
		{
			typedef widget_object<category::widget_tag, drawerbase::button::trigger> base_type;
		public:
			typedef drawerbase::button::trigger::state state;

			button();
			button(window, bool visible);
			button(window, const nana::rectangle& = rectangle(), bool visible = true);
			void icon(const nana::paint::image&);
			void image(const char_t * filename);
			void image(const nana::paint::image&);
			void image_enable(state, bool);
			void image_join(state target, state from);
			void image_stretch(nana::arrange, int beg, int end);
			void image_valid_area(nana::arrange, const nana::rectangle&);
			void enable_pushed(bool);
			bool pushed() const;
			void pushed(bool);
			void omitted(bool);
			void enable_focus_color(bool);
		private:
			void _m_shortkey();
			void _m_complete_creation();
			void _m_caption(const nana::string&);
		};
}//end namespace gui
}//end namespace nana
#endif
