#ifndef NANA_PAINT_IMAGE_PROCESS_PROVIDER_HPP
#define NANA_PAINT_IMAGE_PROCESS_PROVIDER_HPP
#include <nana/pat/cloneable.hpp>
#include <nana/paint/image_process_interface.hpp>
#include <string>
#include <map>

namespace nana
{
	namespace paint
	{
		namespace detail
		{
			class image_process_provider
				: nana::noncopyable
			{
				image_process_provider();

				struct stretch_tag
				{
					typedef paint::image_process::stretch_interface	interface_t;
					typedef pat::cloneable_interface<interface_t>	cloneable_t;
					typedef	std::map<std::string, cloneable_t*>		table_t;

					template<typename ImageProcessor>
					struct generator
					{
						typedef pat::cloneable<ImageProcessor, interface_t> type;
					};

					table_t table;
					interface_t* employee;
				}stretch_;

				struct blend_tag
				{
					typedef paint::image_process::blend_interface	interface_t;
					typedef pat::cloneable_interface<interface_t>	cloneable_t;
					typedef std::map<std::string, cloneable_t*>		table_t;

					template<typename ImageProcessor>
					struct generator
					{
						typedef pat::cloneable<ImageProcessor, interface_t> type;
					};

					table_t	table;
					interface_t	* employee;
				}blend_;

				struct line_tag
				{
					typedef paint::image_process::line_interface	interface_t;
					typedef pat::cloneable_interface<interface_t>	cloneable_t;
					typedef std::map<std::string, cloneable_t*>		table_t;

					template<typename ImageProcessor>
					struct generator
					{
						typedef pat::cloneable<ImageProcessor, interface_t>	type;
					};

					table_t	table;
					interface_t * employee;
				}line_;
			public:

				~image_process_provider();
				static image_process_provider & instance();

				stretch_tag & ref_stretch_tag();
				paint::image_process::stretch_interface * const * stretch() const;
				paint::image_process::stretch_interface * ref_stretch(const std::string& name) const;

				blend_tag & ref_blend_tag();
				paint::image_process::blend_interface * const * blend() const;
				paint::image_process::blend_interface * ref_blend(const std::string& name) const;

				line_tag & ref_line_tag();
				paint::image_process::line_interface * const * line() const;
				paint::image_process::line_interface * ref_line(const std::string& name) const;
			public:
				template<typename Tag>
				void set(Tag & tag, const std::string& name)
				{
					auto i = tag.table.find(name);
					if(i != tag.table.end())
						tag.employee = &(i->second->refer());
				}

				//add
				//@brief:	The add operation is successful if the name does not exist.
				//			it does not replace the existing object by new object, becuase this
				//			feature is thread safe and efficiency.
				template<typename ImageProcessor, typename Tag>
				void add(Tag & tag, const std::string& name)
				{
					if(tag.table.count(name) == 0)
					{
						typename Tag::cloneable_t * obj = typename Tag::template generator<ImageProcessor>::type().clone();
						tag.table[name] = obj;
						if(nullptr == tag.employee)
							tag.employee = &(obj->refer());
					}
				}
			private:
				template<typename Tag>
				typename Tag::interface_t* _m_read(const Tag& tag, const std::string& name) const
				{
					auto i = tag.table.find(name);
					return (i != tag.table.end() ? &(i->second->refer()) : tag.employee);
				}

				template<typename Tag>
				void _m_release(Tag & tag)
				{
					for(auto m : tag.table)
						m.second->self_delete();

					tag.table.clear();
				}
			};
		}
	}
}//end namespace nana
#endif
