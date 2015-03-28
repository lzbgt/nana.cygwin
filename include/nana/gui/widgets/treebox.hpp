/*
 *	A Tree Box Implementation
 *	Copyright(C) 2003-2013 Jinhao(cnjinhao@hotmail.com)
 *
 *	Distributed under the Boost Software License, Version 1.0. 
 *	(See accompanying file LICENSE_1_0.txt or copy at 
 *	http://www.boost.org/LICENSE_1_0.txt)
 *
 *	@file: nana/gui/widgets/treebox.hpp
 *	@brief:
 *		The treebox organizes the nodes by a key string. 
 *		The treebox would have a vertical scrollbar if the node
 *	is too many to display. And it does not have a horizontal scrollbar,
 *	the widget will adjust the node's displaying position for fitting.
 */

#ifndef NANA_GUI_WIDGETS_TREEBOX_HPP
#define NANA_GUI_WIDGETS_TREEBOX_HPP
#include "widget.hpp"
#include "scroll.hpp"
#include <nana/paint/gadget.hpp>
#include "detail/tree_cont.hpp"
#include <nana/gui/timer.hpp>
#include <nana/any.hpp>
namespace nana
{
namespace gui
{
	namespace drawerbase
	{
		namespace treebox
		{
			class tooltip_window;

			template<typename NodeType>
			struct extra_events
			{
				typedef NodeType node_type;

				nana::fn_group<void(nana::gui::window, node_type, bool)> expand;
				nana::fn_group<void(nana::gui::window, node_type, bool)> selected;
			};

			struct node_image_tag
			{
				nana::paint::image normal;
				nana::paint::image highlighted;
				nana::paint::image expanded;
			};

			class trigger
				:public drawer_trigger
			{
			public:
				struct treebox_node_type
				{
					treebox_node_type();
					treebox_node_type(const nana::any&);
					treebox_node_type(const nana::string& text, const nana::any&);
					treebox_node_type& operator=(const treebox_node_type&);

					nana::string text;
					nana::any value;
					bool expanded;
					nana::string img_idstr;
				};

				struct pseudo_node_type{};

				typedef nana::gui::widgets::detail::tree_cont<treebox_node_type> tree_cont_type;
				typedef tree_cont_type::node_type	node_type;
				typedef extra_events<pseudo_node_type*>	ext_event_type;

				trigger();
				void auto_draw(bool);
				bool check(const node_type*) const;
				nana::any & value(node_type*) const;
				node_type* find(const nana::string& key_path);
				node_type* get_owner(const node_type*) const;
				node_type* get_root() const;
				node_type* insert(node_type* node, const nana::string& key, const nana::string& title, const nana::any& v);
				node_type* insert(const nana::string& path, const nana::string& title, const nana::any& v);

				bool check(node_type* parent, node_type* child) const;
				void remove(node_type*);
				node_type * selected() const;
				void selected(node_type*);
				void set_expand(node_type* node, bool);
				void set_expand(const nana::string& path, bool);
				unsigned visual_item_size() const;

				void image(const nana::string& id, const node_image_tag&);
				node_image_tag& image(const nana::string&) const;
				void image_erase(const nana::string&);
				void node_image(node_type* node, const nana::string& id);

				unsigned long node_width(const node_type *node) const;

				bool rename(node_type *node, const nana::char_t* key, const nana::char_t* name);
				ext_event_type& ext_event() const;
			private:
				void bind_window(widget_reference);
				void attached(graph_reference);
				void detached();
				void refresh(graph_reference);
				void dbl_click(graph_reference, const eventinfo&);
				void mouse_down(graph_reference, const eventinfo&);
				void mouse_up(graph_reference, const eventinfo&);
				void mouse_move(graph_reference, const eventinfo&);
				void mouse_wheel(graph_reference, const eventinfo&);
				void resize(graph_reference, const eventinfo&);
				void key_down(graph_reference, const eventinfo&);
				void key_char(graph_reference, const eventinfo&);
			private:
				void _m_find_first(unsigned long offset);
				unsigned _m_node_height() const;
				unsigned _m_max_allow() const;
			private:
				const node_type* _m_find_track_node(nana::char_t);
				nana::paint::image* _m_image(const node_type*);
				bool _m_track_mouse(int x, int y);
				void _m_tooltip_window(node_type* node, const nana::point& pos, const nana::size& size);
				void _m_close_tooltip_window();
				void _m_mouse_move_tooltip_window();
				void _m_click_tooltip_window(const eventinfo&);
				bool _m_draw(bool scrollbar_react);
				void _m_draw_tree();
				unsigned _m_visible_width() const;
				void _m_show_scrollbar();
				void _m_event_scrollbar(const eventinfo&);
				bool _m_adjust(node_type * node, int reason);
				bool _m_set_selected(node_type * node);
				bool _m_set_expanded(node_type* node, bool value);
				void _m_deal_adjust();
			private:
				//Functor
				class item_renderer
				{
				public:
					typedef tree_cont_type::node_type node_type;

					item_renderer(trigger&, const nana::point&);

					//affect
					//0 = Sibling, the last is a sibling of node
					//1 = Owner, the last is the owner of node
					//>=2 = Children, the last is a child of a node that before this node.
					int operator()(const node_type& node, int affect);
					unsigned width(const node_type &node) const;
				private:
					void _m_draw_arrow(const node_type& node, unsigned item_height, bool expand);
					void _m_background(const node_type& node, bool has_child, bool expand);
				private:
					trigger& drawer_;
					nana::point pos_;
				};

				class item_locator
				{
				public:
					struct object
					{ enum{none, item, arrow};};

					item_locator(trigger& drawer, int item_pos, int x, int y);
					int operator()(tree_cont_type::node_type &node, int affect);
					tree_cont_type::node_type * node() const;
					unsigned what() const;
					nana::point pos() const;
					nana::size size() const;
				private:
					trigger& drawer_;
					int item_pos_;
					int item_ypos_;
					nana::point pos_;
					unsigned object_;
					tree_cont_type::node_type * node_;
				};

				struct pred_allow_child
				{
					bool operator()(const tree_cont_type::node_type& node);
				};
			private:
				nana::paint::graphics	*graph_;
				widget		*widget_;

				struct drawing_flags
				{
					drawing_flags();

					bool pause;	//It is a drawing flag, if it is true, the draw function dose nothing.
				}dwflags_;

				struct shape_data_type
				{
					shape_data_type();

					nana::upoint border;
					nana::gui::scroll<true> scrollbar;
					unsigned long prev_first_value; //

					mutable std::map<nana::string, node_image_tag> image_table;
				}shape_;

				struct attribute_type
				{
					attribute_type();

					bool auto_draw;
					mutable ext_event_type ext_event;
					std::size_t mutable visual_item_size;
					uint32_t	button_width;
					tree_cont_type tree_cont;
				}attr_;

				struct node_desc_type
				{
					node_desc_type();

					tree_cont_type::node_type * first;
					unsigned indent_size;
					int offset_x;
					int	item_offset;	//the offset of item to the start pos
					int	text_offset;	//the offset of text to the item

					unsigned long image_width;
				}node_desc_;

				struct node_state
				{
					node_state();

					tooltip_window	*tooltip;

					tree_cont_type::node_type * highlight;
					unsigned highlight_object;

					tree_cont_type::node_type * selected;
					tree_cont_type::node_type * event_node;
				}node_state_;

				struct track_node_tag
				{
					track_node_tag();
					nana::string key_buf;
					unsigned long key_time;
				}track_node_;
			
				struct adjust_desc_type
				{
					adjust_desc_type();

					int offset_x_adjust; //It is a new value of offset_x, and offset_x will be adjusted to the new value.
					tree_cont_type::node_type * node;
					unsigned long scroll_timestamp;
					nana::gui::timer timer;

				}adjust_;
			}; //end class trigger
		}//end namespace treebox
	}//end namespace drawerbase

	template<typename UserData>
	class treebox
		:public widget_object<category::widget_tag, drawerbase::treebox::trigger>
	{
	public:
		typedef UserData value_type;
		typedef typename drawer_trigger_t::pseudo_node_type* node_type;
		typedef typename drawer_trigger_t::ext_event_type ext_event_type;
		typedef drawerbase::treebox::node_image_tag node_image_type;

		treebox(){}

		treebox(window wd, bool visible)
		{
			create(wd, rectangle(), visible);
		}

		treebox(window wd, const rectangle& r, bool visible)
		{
			create(wd, r, visible);
		}

		void auto_draw(bool ad)
		{
			get_drawer_trigger().auto_draw(ad);
		}

		ext_event_type& ext_event() const
		{
			return get_drawer_trigger().ext_event();
		}

		value_type& value(node_type node) const
		{
			return get_drawer_trigger().value(reinterpret_cast<drawer_trigger_t::node_type*>(node));
		}

		treebox& image(const nana::string& id, const nana::paint::image& img)
		{
			node_image_type node_img;
			node_img.normal = img;
			get_drawer_trigger().image(id, node_img);
			return *this;
		}

		treebox& image(const nana::string& id, const node_image_type& node_img)
		{
			get_drawer_trigger().image(id, node_img);
			return *this;
		}

		node_image_type& image(const nana::string& id) const
		{
			return get_drawer_trigger().image(id);
		}

		void image_erase(const nana::string& id)
		{
			get_drawer_trigger().image_erase(id);
		}

		void node_image(node_type node, const nana::string& id)
		{
			get_drawer_trigger().node_image(reinterpret_cast<drawer_trigger_t::node_type*>(node), id);
		}

		node_type insert(const nana::string& path_key, const nana::string& title, value_type value)
		{
			return reinterpret_cast<node_type>(get_drawer_trigger().insert(path_key, title, value));
		}

		node_type insert(node_type node, const nana::string& key, const nana::string& title, value_type value)
		{
			return reinterpret_cast<node_type>(get_drawer_trigger().insert(reinterpret_cast<drawer_trigger_t::node_type*>(node), key, title, value));
		}

		void remove(node_type node)
		{
			get_drawer_trigger().remove(reinterpret_cast<drawer_trigger_t::node_type*>(node));
		}

		void remove(const nana::string& key_path)
		{
			get_drawer_trigger().remove(
				get_drawer_trigger().find(key_path)
				);
		}

		void expand(node_type node, bool exp)
		{
			get_drawer_trigger().set_expand(reinterpret_cast<drawer_trigger_t::node_type*>(node), exp);
		}

		void expand(const nana::string& path_key, bool exp)
		{
			get_drawer_trigger().set_expand(path_key, exp);
		}

		bool expend(node_type node) const
		{
			if(get_drawer_trigger().check(reinterpret_cast<drawer_trigger_t::node_type*>(node)))
				return reinterpret_cast<drawer_trigger_t::node_type*>(node)->value.second.expanded;
			return false;	
		}

		node_type node(const nana::string& keypath)
		{
			return reinterpret_cast<node_type>(get_drawer_trigger().find(keypath));
		}

		nana::string key(node_type node) const
		{
			if(get_drawer_trigger().check(reinterpret_cast<drawer_trigger_t::node_type*>(node)))
				return reinterpret_cast<drawer_trigger_t::node_type*>(node)->value.first;
			
			return nana::string();
		}

		bool key(node_type node, const nana::string& key)
		{
			return (get_drawer_trigger().rename(reinterpret_cast<drawer_trigger_t::node_type*>(node), key.c_str(), 0));
		}

		bool text(node_type node, const nana::string& str)
		{
			return (get_drawer_trigger().rename(reinterpret_cast<drawer_trigger_t::node_type*>(node), 0, str.c_str()));
		}

		nana::string text(node_type node) const
		{
			if(get_drawer_trigger().check(reinterpret_cast<drawer_trigger_t::node_type*>(node)))
				return reinterpret_cast<drawer_trigger_t::node_type*>(node)->value.second.text;

			return nana::string();
		}

		node_type selected() const
		{
			return reinterpret_cast<node_type>(get_drawer_trigger().selected());
		}

		void selected(node_type node)
		{
			get_drawer_trigger().selected(reinterpret_cast<drawer_trigger_t::node_type*>(node));
		}

		unsigned children_size(node_type node) const
		{
			if(get_drawer_trigger().check(reinterpret_cast<drawer_trigger_t::node_type*>(node)))
			{
				drawer_trigger_t::node_type* child = reinterpret_cast<drawer_trigger_t::node_type*>(node)->child;

				unsigned n = 0;
				for(; child; child = child->next)
					++n;
				return n;
			}
			return 0;
		}

		node_type get_sibling(node_type node) const
		{
			if(get_drawer_trigger().check(reinterpret_cast<drawer_trigger_t::node_type*>(node)))
				return reinterpret_cast<node_type>(
							reinterpret_cast<drawer_trigger_t::node_type*>(node)->next
							);
			return 0;
		}

		node_type get_child(node_type node) const
		{
			if(get_drawer_trigger().check(reinterpret_cast<drawer_trigger_t::node_type*>(node)))
				return reinterpret_cast<node_type>(
							reinterpret_cast<drawer_trigger_t::node_type*>(node)->child
							);
			return 0;
		}

		node_type get_owner(node_type node) const
		{
			return reinterpret_cast<node_type>(
					get_drawer_trigger().get_owner(reinterpret_cast<drawer_trigger_t::node_type*>(node))
					);
		}

		nana::string make_key_path(node_type node, const nana::string& splitter) const
		{
			const typename drawer_trigger_t::node_type *pnode = reinterpret_cast<drawer_trigger_t::node_type*>(node);
			if(get_drawer_trigger().check(pnode))
			{
				const typename drawer_trigger_t::node_type* root = get_drawer_trigger().get_root();
				nana::string path;
				nana::string temp;
				while(pnode->owner != root)
				{
					temp = splitter;
					temp += pnode->value.first;
					path.insert(0, temp);
					pnode = pnode->owner;
				}

				path.insert(0, pnode->value.first);
				return path;
			}
			return STR("");
		}
	};
}//end namespace gui
}//end namespace nana

#endif
