#include <nana/paint/detail/image_process_provider.hpp>

#include <nana/paint/detail/image_processor.hpp>

namespace nana
{
namespace paint
{
	namespace image_process
	{
		//There are definitions of pure virtual destructor of image processor interfaces
		stretch_interface::~stretch_interface(){}
		blend_interface::~blend_interface(){}
		line_interface::~line_interface(){}
	}

	namespace detail
	{
	//class image_process_provider
		image_process_provider& image_process_provider::instance()
		{
			static image_process_provider obj;
			return obj;
		}

		image_process_provider::image_process_provider()
		{
			this->add<paint::detail::algorithms::bilinear_interoplation>(stretch_, "bilinear interoplation");
			this->add<paint::detail::algorithms::proximal_interoplation>(stretch_, "proximal interoplation");
			this->add<paint::detail::algorithms::blend>(blend_, "blend");
			this->add<paint::detail::algorithms::bresenham_line>(line_, "bresenham_line");
		}

		image_process_provider::~image_process_provider()
		{
			_m_release(stretch_);
		}

		image_process_provider::stretch_tag& image_process_provider::ref_stretch_tag()
		{
			return stretch_;
		}

		paint::image_process::stretch_interface * const * image_process_provider::stretch() const
		{
			return &stretch_.employee;
		}

		paint::image_process::stretch_interface * image_process_provider::ref_stretch(const std::string& name) const
		{
			return _m_read(stretch_, name);
		}
		
		//blend
		image_process_provider::blend_tag& image_process_provider::ref_blend_tag()
		{
			return blend_;
		}

		paint::image_process::blend_interface * const * image_process_provider::blend() const
		{
			return &blend_.employee;
		}

		paint::image_process::blend_interface * image_process_provider::ref_blend(const std::string& name) const
		{
			return _m_read(blend_, name);
		}

		image_process_provider::line_tag & image_process_provider::ref_line_tag()
		{
			return line_;
		}

		paint::image_process::line_interface * const * image_process_provider::line() const
		{
			return &line_.employee;
		}

		paint::image_process::line_interface * image_process_provider::ref_line(const std::string& name) const
		{
			return _m_read(line_, name);
		}
	//end class image_process_provider
	}
}
}//end namespace nana
