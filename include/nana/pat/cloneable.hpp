#ifndef NANA_PAT_PROTOTYPE_HPP
#define NANA_PAT_PROTOTYPE_HPP

namespace nana{ namespace pat{

	template<typename T>
	class cloneable_interface
	{
	public:
		typedef T interface_t;
		typedef cloneable_interface cloneable_t;
		virtual ~cloneable_interface(){}
		virtual interface_t& refer() = 0;
		virtual const interface_t& refer() const = 0;
		virtual cloneable_t* clone() const = 0;
		virtual void self_delete() const = 0;
	};


	template<typename T, typename SuperOfT>
	class cloneable
		: public cloneable_interface<SuperOfT>
	{
	public:
		typedef T value_t;
		typedef typename cloneable_interface<SuperOfT>::interface_t interface_t;

		cloneable()
		{}

		cloneable(const value_t& obj)
			:object_(obj)
		{}

		template<typename U>
		cloneable(const U& u)
			: object_(u)
		{}

		template<typename U>
		cloneable(U& u)
			:object_(u)
		{}

		virtual interface_t& refer()
		{
			return object_;
		}

		virtual const interface_t& refer() const
		{
			return object_;
		}

		virtual cloneable_interface<interface_t>* clone() const
		{
			return (new cloneable<value_t, interface_t>(*this));
		}

		virtual void self_delete() const
		{
			(delete this);
		}
	private:
		value_t object_;
	};

}//end namespace pat
}//end namespace nana

#endif
