/// @file	sf_single_instance.h
///
/// @brief	Declares the sf single instance class.

#pragma once
#include <mutex>
#include "sf_empty_class.h"

/// @namespace	skyfire
///
/// @brief	.

namespace skyfire
{
	/// @class	sf_single_instance
	///
	/// @brief	A sf single instance.
	///
	/// @author	SkyFire
	/// @date	2017/8/16
	///
	/// @tparam	ThisClass	Type of this class.
	/// @tparam	BaseClass	Type of the base class.

	template<typename ThisClass,typename BaseClass=sf_empty_class>

	/// @class	sf_single_instance
	///
	/// @brief	A sf single instance.
	///
	/// @author	SkyFire
	/// @date	2017/8/16

	class sf_single_instance:public BaseClass
	{
	public:

		/// @fn	static ThisClass* sf_single_instance::get_instance();
		///
		/// @brief	Gets the instance.
		///
		/// @author	SkyFire
		/// @date	2017/8/16
		///
		/// @return	The instance.

		static ThisClass* get_instance();

		/// @fn	sf_single_instance::~sf_single_instance();
		///
		/// @brief	Destructor.
		///
		/// @author	SkyFire
		/// @date	2017/8/16

		~sf_single_instance();

	private:
		sf_single_instance() = default;
		static ThisClass * instance;
		static std::mutex mu;
		friend ThisClass;
	};

	template <typename ThisClass, typename BaseClass>
	ThisClass* sf_single_instance<ThisClass, BaseClass>::get_instance()
	{
		if (instance == nullptr)
		{
			std::lock_guard<std::mutex> lck(mu);
			if (instance == nullptr)
			{
				instance = new ThisClass();
			}
		}
		return instance;
	}

	template <typename ThisClass, typename BaseClass>
	sf_single_instance<ThisClass, BaseClass>::~sf_single_instance()
	{
		if (instance != nullptr)
			delete instance;
	}

	template<typename ThisClass, typename BaseClass>
	ThisClass* sf_single_instance<ThisClass, BaseClass>::instance = nullptr;
	template<typename ThisClass, typename BaseClass>
	std::mutex sf_single_instance<ThisClass, BaseClass>::mu;
}

/// @def	SF_SINGLE_TON(ClassName)
///
/// @brief	A macro that defines sf single ton.
///
/// @author	SkyFire
/// @date	2017/8/16
///
/// @param	ClassName	Name of the class.

#define SF_SINGLE_TON(ClassName)								\
ClassName(const ClassName&) = delete;							\
ClassName(ClassName&&) = delete;								\
ClassName& operator=(const ClassName&) = delete;				\
static ClassName* get_instance()								\
{																\
	static std::mutex init_mutex;								\
	static ClassName* instance__{ nullptr };					\
	if(instance__==nullptr){									\
		std::lock_guard<std::mutex> lck(init_mutex);			\
		if (instance__ == nullptr)								\
		{														\
			instance__ = new ClassName;							\
		}														\
	}															\
	return instance__;											\
}																\


