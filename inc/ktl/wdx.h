#pragma once

#include <wdf.h>
#include <utility>
#include <memory>
#include <cstdint>
#include <new>
#include "code_annotation.h"

namespace ktl
{
	// WDF does not call our object's constructor for us, so we need to call it.
	template<typename T>
	class wdx_object_context
	{
		alignas(T) std::uint8_t buffer[sizeof(T)] = {};

		T * val()
		{
			return static_cast<T*>(static_cast<void*>(buffer));
		}

		T const * val() const
		{
			return static_cast<T const*>(static_cast<void const*>(buffer));
		}
	public:

		wdx_object_context() { }

		// This object cannot be moved or copied because it doesn't know if its
		// contents have been initialized, so is unable to determine if the 
		// destination buffer, or the local buffer, is initialized or not.
		// However it IS assumed that by the time the destructor is called, init() has been called
		wdx_object_context(wdx_object_context const & rhs) = delete;
		wdx_object_context(wdx_object_context && rhs) = delete;

		wdx_object_context& operator=(wdx_object_context const & rhs) = delete;
		wdx_object_context& operator=(wdx_object_context && rhs) = delete;

		~wdx_object_context()
		{
			std::destroy_at(val());
		}

		// This should be called exactly once after the context has been allocated
		template<typename... Args>
		void init(Args&&... args)
		{
			new (buffer) T(std::forward<Args>(args)...);
		}

		T * operator->()
		{
			return val();
		}

		T const * operator->() const
		{
			return val();
		}

		T & operator*()
		{
			return *val();
		}

		T const & operator*() const
		{
			return *val();
		}
	};
}

#define WDX_INIT_CONTEXT(_object, _contexttype, ...) \
    (WdfObjectGetTypedContext(_object, WDX_ ## _contexttype))->init(__VA_ARGS__)

#define WDX_DECLARE_CASTING_FUNCTION(_contextType, _cType, _castingFunction) \
	__forceinline _contextType & _castingFunction(_In_ WDFOBJECT handle) \
	{ \
		return **(WdfObjectGetTypedContext(handle, _cType));\
	}

#define WDX_DECLARE_CONTEXT_TYPE(_contextType) \
	WDX_DECLARE_CONTEXT_TYPE_WITH_NAME(_contextType, WdxObjectGet_ ## _contextType)

#define WDX_DECLARE_CONTEXT_TYPE_WITH_NAME(_contextType, _castingFunction) \
	using WDX_ ## _contextType = ktl::wdx_object_context<_contextType>; \
	WDF_DECLARE_CONTEXT_TYPE(WDX_ ## _contextType); \
	WDX_DECLARE_CASTING_FUNCTION(_contextType, WDX_ ## _contextType, _castingFunction)


#define WDX_OBJECT_ATTRIBUTES_SET_CONTEXT_TYPE(_attributes, _contexttype) \
	WDF_OBJECT_ATTRIBUTES_SET_CONTEXT_TYPE(_attributes, WDX_ ## _contexttype); \
	(_attributes)->EvtDestroyCallback = [](_In_ WDFOBJECT handle) NONPAGED { \
		std::destroy_at(WdfObjectGetTypedContext(handle, WDX_ ## _contexttype)); \
	}

#define WDX_OBJECT_ATTRIBUTES_INIT_CONTEXT_TYPE(_attributes, _contextType) \
	WDF_OBJECT_ATTRIBUTES_INIT_CONTEXT_TYPE(_attributes, WDX_ ## _contextType)