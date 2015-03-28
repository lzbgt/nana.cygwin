#include <nana/paint/image_process_selector.hpp>

namespace nana
{
	namespace paint
	{
		namespace image_process
		{
			//class selector
			void selector::stretch(const std::string& name)
			{
				detail::image_process_provider & p = detail::image_process_provider::instance();
				p.set(p.ref_stretch_tag(), name);
			}

			void selector::blend(const std::string& name)
			{
				detail::image_process_provider & p = detail::image_process_provider::instance();
				p.set(p.ref_blend_tag(), name);
			}

			void selector::line(const std::string& name)
			{
				detail::image_process_provider & p = detail::image_process_provider::instance();
				p.set(p.ref_line_tag(), name);			
			}
			//end class selector
		}
	}
}
