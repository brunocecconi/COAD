
#ifndef CORE_ANY_H
#define CORE_ANY_H

#include "Meta/TypeInfo.h"
#include "Core/Assert.h"

#include <EASTL/any.h>

namespace Detail
{

// utility to switch between exceptions and asserts
inline void DoBadAnyCast()
{
	ENFORCE_MSG(false, "Bad any cast");

	// NOTE(rparolin): CRASH!
	// You crashed here because you requested a type that was not contained in the object.
	// We choose to intentionally crash here instead of returning invalid data to the calling
	// code which could cause hard to track down bugs.
	*((volatile int*)0) = 0xDEADC0DE;
}

template<typename T, typename... Args>
void* DefaultConstruct(Args&&... args)
{
	auto* l_mem = Allocators::default_t(DEBUG_NAME_VAL("Meta")).allocate(sizeof(T), alignof(T), 0);
	return ::new (l_mem) T(eastl::forward<Args>(args)...);
}

template<typename T>
void DefaultDestroy(T* p)
{
	p->~T();

	Allocators::default_t(DEBUG_NAME_VAL("Meta")).deallocate(static_cast<void*>(p), sizeof(T));
}

} // namespace Detail

class Any
{
	//////////////////////////////////////////////////////////////////////////////////////////
	// storage_operation
	//
	// operations supported by the storage handler
	//
	enum class StorageOperation
	{
		eGet,
		eDestroy,
		eCopy,
		eMove,
		eTypeInfo
	};

	//////////////////////////////////////////////////////////////////////////////////////////
	// storage
	//
	// the underlying storage type which enables the switching between objects stored in
	// the heap and objects stored within the any type.
	//
	union storage {
		typedef eastl::aligned_storage_t<4 * sizeof(void*), eastl::alignment_of<void*>::value> internal_storage_t;

		void*			   external_storage = nullptr;
		internal_storage_t internal_storage;
	};

	//////////////////////////////////////////////////////////////////////////////////////////
	// use_internal_storage
	//
	// determines when the "local buffer optimization" is used
	//
	template<typename T>
	using use_internal_storage_t =
		eastl::bool_constant<eastl::is_nothrow_move_constructible<T>::value && (sizeof(T) <= sizeof(storage)) &&
							 (eastl::alignment_of<storage>::value % eastl::alignment_of<T>::value == 0)>;

	//////////////////////////////////////////////////////////////////////////////////////////
	// non-member friend functions
	//
	template<class ValueType>
	friend const ValueType* AnyCast(const Any* any) EA_NOEXCEPT;
	template<class ValueType>
	friend ValueType* AnyCast(Any* any) EA_NOEXCEPT;
	template<class ValueType>
	friend ValueType AnyCast(const Any& operand);
	template<class ValueType>
	friend ValueType AnyCast(Any& operand);
	template<class ValueType>
	friend ValueType AnyCast(Any&& operand);

	// Adding Unsafe any cast operations
	template<class ValueType>
	friend const ValueType* UnsafeAnyCast(const Any* any) EA_NOEXCEPT;
	template<class ValueType>
	friend ValueType* UnsafeAnyCast(Any* any) EA_NOEXCEPT;

	//////////////////////////////////////////////////////////////////////////////////////////
	// internal storage handler
	//
	template<typename T>
	struct StorageHandlerInternal
	{
		template<typename V>
		static void Construct(storage& s, V&& v)
		{
			::new (&s.internal_storage) T(eastl::forward<V>(v));
		}

		template<typename... Args>
		static void ConstructInplace(storage& s, Args... args)
		{
			::new (&s.internal_storage) T(eastl::forward<Args>(args)...);
		}

		template<class NT, class U, class... Args>
		static void ConstructInplace(storage& s, std::initializer_list<U> il, Args&&... args)
		{
			::new (&s.internal_storage) NT(il, eastl::forward<Args>(args)...);
		}

		static void Destroy(Any& ref_any)
		{
			T& t = *static_cast<T*>(static_cast<void*>(&ref_any.storage_.internal_storage));
			EA_UNUSED(t);
			t.~T();

			ref_any.handle_ = nullptr;
		}

		static void* HandlerFunc(const StorageOperation op, const Any* this_value, Any* other)
		{
			switch (op)
			{
			case StorageOperation::eGet: {
				ASSERT(this_value);
				return (void*)(&this_value->storage_.internal_storage);
			}
			break;

			case StorageOperation::eDestroy: {
				ASSERT(this_value);
				Destroy(const_cast<Any&>(*this_value));
			}
			break;

			case StorageOperation::eCopy: {
				ASSERT(this_value);
				ASSERT(other);
				Construct(other->storage_, *(T*)(&this_value->storage_.internal_storage));
			}
			break;

			case StorageOperation::eMove: {
				ASSERT(this_value);
				ASSERT(other);
				Construct(other->storage_, eastl::move(*(T*)(&this_value->storage_.internal_storage)));
				Destroy(const_cast<Any&>(*this_value));
			}
			break;

			case StorageOperation::eTypeInfo: {
				return (void*)&Meta::Typeof<T>();
			}
			break;

			default: {
				ENFORCE_MSG(false, "unknown storage operation\n");
			}
			break;
			};

			return nullptr;
		}
	};

	//////////////////////////////////////////////////////////////////////////////////////////
	// external storage handler
	//
	template<typename T>
	struct StorageHandlerExternal
	{
		template<typename V>
		static void Construct(storage& s, V&& v)
		{
			s.external_storage = Detail::DefaultConstruct<T>(eastl::forward<V>(v));
		}

		template<typename... Args>
		static void ConstructInplace(storage& s, Args... args)
		{
			s.external_storage = Detail::DefaultConstruct<T>(eastl::forward<Args>(args)...);
		}

		template<class NT, class U, class... Args>
		static void ConstructInplace(storage& s, std::initializer_list<U> il, Args&&... args)
		{
			s.external_storage = Detail::DefaultConstruct<NT>(il, eastl::forward<Args>(args)...);
		}

		static void Destroy(Any& ref_any)
		{
			Detail::DefaultDestroy(static_cast<T*>(ref_any.storage_.external_storage));

			ref_any.handle_ = nullptr;
		}

		static void* HandlerFunc(const StorageOperation op, const Any* this_value, Any* other)
		{
			switch (op)
			{
			case StorageOperation::eGet: {
				ENFORCE(this_value);
				ENFORCE(this_value->storage_.external_storage);
				return static_cast<void*>(this_value->storage_.external_storage);
			}
			break;

			case StorageOperation::eDestroy: {
				ENFORCE(this_value);
				Destroy(*const_cast<Any*>(this_value));
			}
			break;

			case StorageOperation::eCopy: {
				ENFORCE(this_value);
				ENFORCE(other);
				construct(other->storage_, *static_cast<T*>(this_value->storage_.external_storage));
			}
			break;

			case StorageOperation::eMove: {
				ENFORCE(this_value);
				ENFORCE(other);
				construct(other->storage_, eastl::move(*(T*)(this_value->storage_.external_storage)));
				Destroy(const_cast<Any&>(*this_value));
			}
			break;

			case StorageOperation::eTypeInfo: {
				return const_cast<void*>(&Meta::Typeof<T>());
			}
			break;

			default: {
				ENFORCE_MSG(false, "Unknown storage operation.");
			}
			break;
			};

			return nullptr;
		}
	};

	//////////////////////////////////////////////////////////////////////////////////////////
	// storage_handler_ptr
	//
	// defines the function signature of the storage handler that both the internal and
	// external storage handlers must implement to retrieve the underlying type of the any
	// object.
	//
	using storage_handler_ptr_t = void* (*)(StorageOperation, const Any*, Any*);

	//////////////////////////////////////////////////////////////////////////////////////////
	// storage_handler
	//
	// based on the specified type T we select the appropriate underlying storage handler
	// based on the 'use_internal_storage' trait.
	//
	template<typename T>
	using storage_handler_t = typename eastl::conditional<use_internal_storage_t<T>::value, StorageHandlerInternal<T>,
														  StorageHandlerExternal<T>>::type;

	//////////////////////////////////////////////////////////////////////////////////////////
	// data layout
	//
	storage				  storage_;
	storage_handler_ptr_t handle_;

public:
#ifndef EA_COMPILER_GNUC
	// TODO(rparolin):  renable constexpr for GCC
	EA_CONSTEXPR
#endif
	Any() EA_NOEXCEPT: storage_(), handle_(nullptr)
	{
	}

	Any(const Any& other) : handle_(nullptr)
	{
		if (other.handle_)
		{
			// NOTE(rparolin): You can not simply copy the underlying
			// storage because it could hold a pointer to an object on the
			// heap which breaks the copy semantics of the language.
			other.handle_(StorageOperation::eCopy, &other, this);
			handle_ = other.handle_;
		}
	}

	Any(Any&& other) EA_NOEXCEPT: handle_(nullptr)
	{
		if (other.handle_)
		{
			// NOTE(rparolin): You can not simply move the underlying
			// storage because because the storage class has effectively
			// type erased user type so we have to defer to the handler
			// function to get the type back and pass on the move request.
			handle_ = eastl::move(other.handle_);
			other.handle_(StorageOperation::eMove, &other, this);
		}
	}

	~Any()
	{
		Reset();
	}

	template<class ValueType>
	Any(ValueType&& value,
		typename eastl::enable_if<!eastl::is_same<typename eastl::decay<ValueType>::type, Any>::value>::type* = 0)
	{
		typedef eastl::decay_t<ValueType> DecayedValueType;
		static_assert(eastl::is_copy_constructible<DecayedValueType>::value, "ValueType must be copy-constructible");
		storage_handler_t<DecayedValueType>::Construct(storage_, eastl::forward<ValueType>(value));
		handle_ = &storage_handler_t<DecayedValueType>::HandlerFunc;
	}

	template<class T, class... Args>
	explicit Any(eastl::in_place_type_t<T>, Args&&... args)
	{
		typedef storage_handler_t<eastl::decay_t<T>> storage_handler_t;
		static_assert(eastl::is_constructible<T, Args...>::value, "T must be constructible with Args...");

		storage_handler_t::construct_inplace(storage_, eastl::forward<Args>(args)...);
		handle_ = &storage_handler_t::HandlerFunc;
	}

	template<class T, class U, class... Args>
	explicit Any(eastl::in_place_type_t<T>, std::initializer_list<U> il, Args&&... args,
				 typename eastl::enable_if<eastl::is_constructible<T, std::initializer_list<U>&, Args...>::value,
										   void>::type* = 0)
	{
		typedef storage_handler_t<eastl::decay_t<T>> storage_handler_t;

		storage_handler_t::construct_inplace(storage_, il, eastl::forward<Args>(args)...);
		handle_ = &storage_handler_t::HandlerFunc;
	}

	// 20.7.3.2, assignments
	template<class ValueType>
	Any& operator=(ValueType&& value)
	{
		static_assert(eastl::is_copy_constructible<eastl::decay_t<ValueType>>::value,
					  "ValueType must be copy-constructible");
		any(eastl::forward<ValueType>(value)).swap(*this);
		return *this;
	}

	Any& operator=(const Any& other)
	{
		Any(other).Swap(*this);
		return *this;
	}

	Any& operator=(Any&& other) EA_NOEXCEPT
	{
		Any(eastl::move(other)).Swap(*this);
		return *this;
	}

	// 20.7.3.3, modifiers
	template<class T, class... Args>
	void Emplace(Args&&... args)
	{
		typedef storage_handler_t<eastl::decay_t<T>> StorageHandlerT;
		static_assert(eastl::is_constructible<T, Args...>::value, "T must be constructible with Args...");

		Reset();
		StorageHandlerT::construct_inplace(storage_, eastl::forward<Args>(args)...);
		handle_ = &StorageHandlerT::HandlerFunc;
	}

	template<class NT, class U, class... Args>
	typename eastl::enable_if<eastl::is_constructible<NT, std::initializer_list<U>&, Args...>::value, void>::type
	Emplace(std::initializer_list<U> il, Args&&... args)
	{
		typedef storage_handler_t<eastl::decay_t<NT>> StorageHandlerT;

		Reset();
		StorageHandlerT::construct_inplace(storage_, il, eastl::forward<Args>(args)...);
		handle_ = &StorageHandlerT::HandlerFunc;
		return {};
	}

	void Reset() const EA_NOEXCEPT
	{
		if (handle_)
			handle_(StorageOperation::eDestroy, this, nullptr);
	}

	void Swap(Any& other) EA_NOEXCEPT
	{
		if (this == &other)
			return;

		if (handle_ && other.handle_)
		{
			Any tmp;
			tmp.handle_ = other.handle_;
			other.handle_(StorageOperation::eMove, &other, &tmp);

			other.handle_ = handle_;
			handle_(StorageOperation::eMove, this, &other);

			handle_ = tmp.handle_;
			tmp.handle_(StorageOperation::eMove, &tmp, this);
		}
		else if (handle_ == nullptr && other.handle_)
		{
			eastl::swap(handle_, other.handle_);
			handle_(StorageOperation::eMove, &other, this);
		}
		else if (handle_ && other.handle_ == nullptr)
		{
			eastl::swap(handle_, other.handle_);
			other.handle_(StorageOperation::eMove, this, &other);
		}
		// else if (handle_ == nullptr && other.handle_ == nullptr)
		//{
		//      // nothing to swap
		// }
	}

	// 20.7.3.4, observers
	NODISCARD bool HasValue() const EA_NOEXCEPT
	{
		return handle_ != nullptr;
	}

	NODISCARD const Meta::TypeInfo& Type() const EA_NOEXCEPT
	{
		if (handle_)
		{
			const auto* type_info = handle_(StorageOperation::eTypeInfo, this, nullptr);
			return *static_cast<const Meta::TypeInfo*>(type_info);
		}
		else
		{
			return Meta::TypeInfo::None();
		}
	}
};

//////////////////////////////////////////////////////////////////////////////////////////
// 20.7.4, non-member functions
//
inline void Swap(Any& rhs, Any& lhs) EA_NOEXCEPT
{
	rhs.Swap(lhs);
}

//////////////////////////////////////////////////////////////////////////////////////////
// 20.7.4, The non-member any_cast functions provide type-safe access to the contained object.
//
template<class ValueType>
ValueType AnyCast(const Any& operand)
{
	static_assert(eastl::is_reference<ValueType>::value || eastl::is_copy_constructible<ValueType>::value,
				  "ValueType must be a reference or copy constructible");

	auto* p = AnyCast<typename eastl::add_const<typename eastl::remove_reference<ValueType>::type>::type>(&operand);

	if (p == nullptr)
		Detail::DoBadAnyCast();

	return *p;
}

template<class ValueType>
ValueType AnyCast(Any& operand)
{
	static_assert(eastl::is_reference<ValueType>::value || eastl::is_copy_constructible<ValueType>::value,
				  "ValueType must be a reference or copy constructible");

	auto* p = AnyCast<typename eastl::remove_reference<ValueType>::type>(&operand);

	if (p == nullptr)
		Detail::DoBadAnyCast();

	return *p;
}

template<class ValueType>
ValueType AnyCast(Any&& operand)
{
	static_assert(eastl::is_reference<ValueType>::value || eastl::is_copy_constructible<ValueType>::value,
				  "ValueType must be a reference or copy constructible");

	auto* p = AnyCast<typename eastl::remove_reference<ValueType>::type>(&operand);

	if (p == nullptr)
		eastl::Internal::DoBadAnyCast();

	return *p;
}

// NOTE(rparolin): The runtime type check was commented out because in DLL builds the templated function pointer
// value will be different -- completely breaking the validation mechanism.  Due to the fact that eastl::any uses
// type erasure we can't refresh (on copy/move) the cached function pointer to the internal handler function because
// we don't statically know the type.
template<class ValueType>
const ValueType* AnyCast(const Any* any) EA_NOEXCEPT
{
	return (any && any->handle_ EASTL_IF_NOT_DLL(== &Any::storage_handler_t<eastl::decay_t<ValueType>>::HandlerFunc) &&
			any->Type() == Meta::Typeof<typename eastl::remove_reference<ValueType>::type>())
			   ? static_cast<const ValueType*>(any->handle_(Any::StorageOperation::eGet, any, nullptr))
			   : nullptr;
}

template<class ValueType>
ValueType* AnyCast(Any* any) EA_NOEXCEPT
{
	return (any && any->handle_ EASTL_IF_NOT_DLL(== &Any::storage_handler_t<eastl::decay_t<ValueType>>::HandlerFunc) &&
			any->Type() == Meta::Typeof<typename eastl::remove_reference<ValueType>::type>())
			   ? static_cast<ValueType*>(any->handle_(Any::StorageOperation::eGet, any, nullptr))
			   : nullptr;
}

// Unsafe operations - use with caution
template<class ValueType>
const ValueType* UnsafeAnyCast(const Any* any) EA_NOEXCEPT
{
	return UnsafeAnyCast<ValueType>(const_cast<Any*>(any));
}

template<class ValueType>
ValueType* UnsafeAnyCast(Any* any) EA_NOEXCEPT
{
	return static_cast<ValueType*>(any->handle_(Any::StorageOperation::eGet, any, nullptr));
}

//////////////////////////////////////////////////////////////////////////////////////////
// make_any
//
template<class T, class... Args>
Any MakeAny(Args&&... args)
{
	return Any(eastl::in_place<T>, eastl::forward<Args>(args)...);
}

template<class T, class U, class... Args>
Any MakeAny(std::initializer_list<U> il, Args&&... args)
{
	return Any(eastl::in_place<T>, il, eastl::forward<Args>(args)...);
}

#endif
