/*
 *	A Concepts Definition of Nana C++ Library
 *	Copyright(C) 2003-2013 Jinhao(cnjinhao@hotmail.com)
 *
 *	Distributed under the Boost Software License, Version 1.0.
 *	(See accompanying file LICENSE_1_0.txt or copy at
 *	http://www.boost.org/LICENSE_1_0.txt)
 *
 *	@file: nana/concepts.hpp
 */
#ifndef NANA_CONCEPTS_HPP
#define NANA_CONCEPTS_HPP

#include <nana/any.hpp>

namespace nana
{
	namespace concepts
	{
		template<typename IndexType, int Dimension>
		class any_objective
		{
		public:
			virtual ~any_objective(){}

			template<typename Target>
			void anyobj(const Target& t)
			{
				*_m_anyobj(true) = t;
			}

			template<typename Target>
			void anyobj(Target&& t)
			{
				*_m_anyobj(true) = std::move(t);
			}

			template<typename Target>
			Target * anyobj() const
			{
				nana::any * p = _m_anyobj(false);
				return (p ? p->get<Target>() : nullptr);
			}
		private:
			virtual nana::any* _m_anyobj(bool allocate_if_empty) const = 0;
		};

		template<typename IndexType>
		class any_objective<IndexType, 1>
		{
		public:
			typedef IndexType anyobj_index_t;

			virtual ~any_objective(){}

			template<typename Target>
			void anyobj(anyobj_index_t i, const Target& t)
			{
				*_m_anyobj(i, true) = t;
			}

			template<typename Target>
			void anyobj(anyobj_index_t i, Target&& t)
			{
				*_m_anyobj(i, true) = std::move(t);
			}

			template<typename Target>
			Target * anyobj(anyobj_index_t i) const
			{
				nana::any * p = _m_anyobj(i, false);
				return (p ? p->get<Target>() : nullptr);
			}
		private:
			virtual nana::any* _m_anyobj(anyobj_index_t i, bool allocate_if_empty) const = 0;
		};

		template<typename IndexType>
		class any_objective<IndexType, 2>
		{
		public:
			typedef IndexType anyobj_index_t;

			virtual ~any_objective(){}

			template<typename Target>
			void anyobj(anyobj_index_t i0, anyobj_index_t i1, const Target& t)
			{
				*_m_anyobj(i0, i1, true) = t;
			}

			template<typename Target>
			void anyobj(anyobj_index_t i0, anyobj_index_t i1, Target&& t)
			{
				*_m_anyobj(i0, i1, true) = std::move(t);
			}

			template<typename Target>
			Target * anyobj(anyobj_index_t i0, anyobj_index_t i1) const
			{
				nana::any * p = _m_anyobj(i0, i1, false);
				return (p ? p->get<Target>() : nullptr);
			}
		private:
			virtual nana::any* _m_anyobj(anyobj_index_t i0, anyobj_index_t i1, bool allocate_if_empty) const = 0;
		};
	}//end namespace concepts
}//end namespace nana
#endif
