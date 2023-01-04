
#ifndef META_MACROS_H
#define META_MACROS_H

/////////////////////////////////////////////////
////////////////// TYPE INFO ////////////////////
/////////////////////////////////////////////////

#define TYPEOF(X)	Meta::Typeof<X>()
#define TYPEOFV(X)	Meta::Typeof(X)

#define META_REBINDER_TYPE_INFO() friend struct Meta::TypeInfo::Rebinder<this_t>

#define META_TYPE_BINDER_SPECIALIZATION(TYPE)                                                                          \
	template<>                                                                                                         \
	struct TypeInfo::Rebinder<TYPE>: TypeInfo::Binder<TYPE>

#define META_TYPE_BINDER_SPECIALIZATION_TEMPLATE(TYPE, ...)                                                            \
	template<>                                                                                                         \
	struct TypeInfo::Rebinder<TYPE<__VA_ARGS__>>: TypeInfo::Binder<TYPE<__VA_ARGS__>>

#define META_TYPE_BINDER_BODY(TYPE)                                                                                    \
	using this_t							 = TYPE;                                                                   \
	static constexpr id_t		ID	 = Hash::Fnv1A(#TYPE);                                                 \
	static constexpr eastl::string_view NAME = (char*)#TYPE;

#define META_TYPE_BINDER_BODY_TEMPLATE(TYPE, TYPE_NAME, ...)                                                           \
	using this_t							 = TYPE<__VA_ARGS__>;                                                      \
	static constexpr id_t		ID	 = Hash::Fnv1A(TYPE_NAME);                                             \
	static constexpr eastl::string_view NAME = (char*)TYPE_NAME;

#define META_TYPE_BINDER_OPERATIONS_CUSTOM(DEFAULT_CTOR, MOVE_CTOR, COPY_CTOR, MOVE_ASSIGN, COPY_ASSIGN, DTOR,         \
										   TO_STRING)                                                                  \
	static void Operations(TypeInfo::OperationBody& body)                                                              \
	{                                                                                                                  \
		auto& l_default_args_tuple = *static_cast<eastl::tuple<void*, void*>*>(body.args_tuple);                       \
		(void)l_default_args_tuple;	\
		switch (body.type)                                                                                             \
		{                                                                                                              \
		case TypeInfo::eDefaultCtor: {                                                                                 \
			DEFAULT_CTOR                                                                                               \
			return;                                                                                                    \
		}                                                                                                              \
		case TypeInfo::eMoveCtor: {                                                                                    \
			MOVE_CTOR                                                                                                  \
			return;                                                                                                    \
		}                                                                                                              \
		case TypeInfo::eCopyCtor: {                                                                                    \
			COPY_CTOR                                                                                                  \
			return;                                                                                                    \
		}                                                                                                              \
		case TypeInfo::eMoveAssign: {                                                                                  \
			MOVE_ASSIGN                                                                                                \
			return;                                                                                                    \
		}                                                                                                              \
		case TypeInfo::eCopyAssign: {                                                                                  \
			COPY_ASSIGN                                                                                                \
			return;                                                                                                    \
		}                                                                                                              \
		case TypeInfo::eDtor: {                                                                                        \
			DTOR return;                                                                                               \
		}                                                                                                              \
		case TypeInfo::eToString: {                                                                                    \
			TO_STRING                                                                                                  \
			return;                                                                                                    \
		}                                                                                                              \
		}                                                                                                              \
	}

#define META_TYPE_BINDER_OPERATION_NOT_IMPL(TYPE)	\
	ENFORCE_MSG(false, "Operation " #TYPE " not implemented");

#define META_TYPE_BINDER_NO_OPERATIONS()                                                                               \
	META_TYPE_BINDER_OPERATIONS_CUSTOM(META_TYPE_BINDER_OPERATION_NOT_IMPL(eDefaultCtor),	\
	META_TYPE_BINDER_OPERATION_NOT_IMPL(eMoveCtor),	\
	META_TYPE_BINDER_OPERATION_NOT_IMPL(eCopyCtor),	\
	META_TYPE_BINDER_OPERATION_NOT_IMPL(eMoveAssign),	\
	META_TYPE_BINDER_OPERATION_NOT_IMPL(eCopyAssign),	\
	META_TYPE_BINDER_OPERATION_NOT_IMPL(eDtor),	\
	META_TYPE_BINDER_OPERATION_NOT_IMPL(eToString)	\
	)

#define META_TYPE_BINDER_DEFAULT_OPERATION_DEFAULT_CTOR() new (eastl::get<0>(l_default_args_tuple)) this_t{};

#define META_TYPE_BINDER_DEFAULT_OPERATION_MOVE_CTOR()                                                                 \
	new (eastl::get<0>(l_default_args_tuple))                                                                          \
		this_t{eastl::move(*static_cast<this_t*>(eastl::get<1>(l_default_args_tuple)))};

#define META_TYPE_BINDER_DEFAULT_OPERATION_COPY_CTOR()                                                                 \
	new (eastl::get<0>(l_default_args_tuple)) this_t{*static_cast<this_t*>(eastl::get<1>(l_default_args_tuple))};

#define META_TYPE_BINDER_DEFAULT_OPERATION_MOVE_ASSIGN()                                                               \
	*static_cast<this_t*>(eastl::get<0>(l_default_args_tuple)) =                                                       \
		eastl::move(*static_cast<this_t*>(eastl::get<1>(l_default_args_tuple)));

#define META_TYPE_BINDER_DEFAULT_OPERATION_COPY_ASSIGN()                                                               \
	*static_cast<this_t*>(eastl::get<0>(l_default_args_tuple)) =                                                       \
		*static_cast<this_t*>(eastl::get<1>(l_default_args_tuple));

#define META_TYPE_BINDER_DEFAULT_OPERATION_DTOR()                                                                      \
	eastl::destroy_at(static_cast<this_t*>(eastl::get<0>(l_default_args_tuple)));

#define META_TYPE_BINDER_DEFAULT_OPERATION_TO_STRING()                                                                 \
	auto& l_to_string_args_tuple = *static_cast<eastl::tuple<eastl::string*, void*, uint64_t>*>(body.args_tuple);        \
	*eastl::get<0>(l_to_string_args_tuple) =                                                                           \
		Algorithm::ToString(*static_cast<this_t*>(eastl::get<1>(l_to_string_args_tuple)), {DEBUG_NAME("Meta")},        \
							eastl::get<2>(l_to_string_args_tuple));

#define META_TYPE_BINDER_DEFAULT_OPERATIONS()                                                                          \
	META_TYPE_BINDER_OPERATIONS_CUSTOM(                                                                                \
		META_TYPE_BINDER_DEFAULT_OPERATION_DEFAULT_CTOR(), META_TYPE_BINDER_DEFAULT_OPERATION_MOVE_CTOR(),             \
		META_TYPE_BINDER_DEFAULT_OPERATION_COPY_CTOR(), META_TYPE_BINDER_DEFAULT_OPERATION_MOVE_ASSIGN(),              \
		META_TYPE_BINDER_DEFAULT_OPERATION_COPY_ASSIGN(), META_TYPE_BINDER_DEFAULT_OPERATION_DTOR(),                   \
		META_TYPE_BINDER_DEFAULT_OPERATION_TO_STRING())

#define META_TYPE_BINDER_DEFAULT(TYPE)                                                                                 \
	namespace Meta                                                                                                     \
	{                                                                                                                  \
	META_TYPE_BINDER_SPECIALIZATION(TYPE){META_TYPE_BINDER_BODY(TYPE) META_TYPE_BINDER_DEFAULT_OPERATIONS()};          \
	}

#define META_TYPE_BINDER_DEFAULT_NO_TO_STRING(TYPE)                                                                    \
	namespace Meta                                                                                                     \
	{                                                                                                                  \
	META_TYPE_BINDER_SPECIALIZATION(TYPE){META_TYPE_BINDER_BODY(TYPE) META_TYPE_BINDER_OPERATIONS_CUSTOM(              \
		META_TYPE_BINDER_DEFAULT_OPERATION_DEFAULT_CTOR(), META_TYPE_BINDER_DEFAULT_OPERATION_MOVE_CTOR(),             \
		META_TYPE_BINDER_DEFAULT_OPERATION_COPY_CTOR(), META_TYPE_BINDER_DEFAULT_OPERATION_MOVE_ASSIGN(),              \
		META_TYPE_BINDER_DEFAULT_OPERATION_COPY_ASSIGN(), META_TYPE_BINDER_DEFAULT_OPERATION_DTOR(), EMPTY)};          \
	}

#define META_TYPE_BINDER_DEFAULT_TEMPLATE(TYPE, TYPE_NAME, ...)                                                        \
	namespace Meta                                                                                                     \
	{                                                                                                                  \
	META_TYPE_BINDER_SPECIALIZATION_TEMPLATE(TYPE, __VA_ARGS__){                                                       \
		META_TYPE_BINDER_BODY_TEMPLATE(TYPE, TYPE_NAME, __VA_ARGS__) META_TYPE_BINDER_DEFAULT_OPERATIONS()};           \
	}

#define META_TYPE_BINDER_BEGIN(NAME)                                                                                   \
	namespace Meta                                                                                                     \
	{                                                                                                                  \
	template<>                                                                                                         \
	struct TypeInfo::Rebinder<NAME>: Binder<NAME>                                                                      \
	{                                                                                                                  \
		META_TYPE_BINDER_BODY(NAME)

#define META_TYPE_BINDER_END()                                                                                         \
	}                                                                                                                  \
	;                                                                                                                  \
	}

#define META_TYPE_AUTO_REGISTER_NS(NAME, SYMBOL)	\
struct SYMBOL##TypeAutoRegister	\
{	\
	SYMBOL##TypeAutoRegister(){ Meta::Typeof<NAME>(); }	\
}; static SYMBOL##TypeAutoRegister g##SYMBOL##TypeAutoRegister

#define META_TYPE_AUTO_REGISTER(NAME)	\
struct NAME##TypeAutoRegister	\
{	\
	NAME##TypeAutoRegister(){ Meta::Typeof<NAME>(); }	\
}; static NAME##TypeAutoRegister g##NAME##TypeAutoRegister

/////////////////////////////////////////////////
////////////////// CTOR INFO ////////////////////
/////////////////////////////////////////////////

#define META_REBINDER_CTOR(INDEX)	\
public:                                                                                                                \
	static constexpr auto Ctor_##INDEX##_ID = "Ctor_"#INDEX##_fnv1a;                                                                    \
                                                                                                                       \
private:                                                                                                               \
	friend struct Meta::CtorInfo::Rebinder<Ctor_##INDEX##_ID>

#define META_CTOR_INFO_BINDER(OWNER, INDEX, OPTIONAL_ARGS, ...)                                                        \
	template<> struct CtorInfo::Rebinder<OWNER::Ctor_##INDEX##_ID>                                                                \
		: Binder<OWNER::Ctor_##INDEX##_ID, OWNER(__VA_ARGS__), OPTIONAL_ARGS>

#define META_CTOR_INFO_BINDER_INVOKE_CUSTOM(...)                                                                       \
	static void Invoke(body_t& body)                                                                                   \
	{                                                                                                                  \
		__VA_ARGS__                                                                                                    \
	}

#define META_CTOR_INFO_BINDER_INVOKE_DEFAULT_ARGS0()                                                                   \
	META_CTOR_INFO_BINDER_INVOKE_CUSTOM(if (body.args_tuple_size == 0) { new (body.memory) owner_t{}; })

#define META_CTOR_INFO_BINDER_INVOKE_DEFAULT_ARGS1()                                                                   \
	META_CTOR_INFO_BINDER_INVOKE_CUSTOM(auto l_body = body_adapter_t{body}; if (body.args_tuple_size == 1) {           \
		new (body.memory) owner_t{l_body.Get<0>()};                                                                    \
	})

#define META_CTOR_INFO_BINDER_INVOKE_DEFAULT_ARGS2()                                                                   \
	META_CTOR_INFO_BINDER_INVOKE_CUSTOM(                                                                               \
		auto l_body = body_adapter_t{body}; if (body.args_tuple_size == 1) {                                           \
			new (body.memory) owner_t{l_body.Get<0>()};                                                                \
			return;                                                                                                    \
		} if (body.args_tuple_size == 2) {                                                                             \
			new (body.memory) owner_t{l_body.Get<0>(), l_body.Get<1>()};                                               \
		})

#define META_CTOR_INFO_BINDER_INVOKE_DEFAULT_ARGS3()                                                                   \
	META_CTOR_INFO_BINDER_INVOKE_CUSTOM(                                                                               \
		auto l_body = body_adapter_t{body}; if (body.args_tuple_size == 1) {                                           \
			new (body.memory) owner_t{l_body.Get<0>()};                                                                \
			return;                                                                                                    \
		} if (body.args_tuple_size == 2) {                                                                             \
			new (body.memory) owner_t{l_body.Get<0>(), l_body.Get<1>()};                                               \
			return;                                                                                                    \
		} if (body.args_tuple_size == 3) {                                                                             \
			new (body.memory) owner_t{l_body.Get<0>(), l_body.Get<1>(), l_body.Get<2>()};                              \
		})

#define META_CTOR_INFO_BINDER_INVOKE_DEFAULT_ARGS4()                                                                   \
	META_CTOR_INFO_BINDER_INVOKE_CUSTOM(                                                                               \
		auto l_body = body_adapter_t{body}; if (body.args_tuple_size == 1) {                                           \
			new (body.memory) owner_t{l_body.Get<0>()};                                                                \
			return;                                                                                                    \
		} if (body.args_tuple_size == 2) {                                                                             \
			new (body.memory) owner_t{l_body.Get<0>(), l_body.Get<1>()};                                               \
			return;                                                                                                    \
		} if (body.args_tuple_size == 3) {                                                                             \
			new (body.memory) owner_t{l_body.Get<0>(), l_body.Get<1>(), l_body.Get<2>()};                              \
			return;                                                                                                    \
		} if (body.args_tuple_size == 4) {                                                                             \
			new (body.memory) owner_t{l_body.Get<0>(), l_body.Get<1>(), l_body.Get<2>(), l_body.Get<3>()};             \
		})

#define META_CTOR_INFO_BINDER_INVOKE_DEFAULT_ARGS5()                                                                   \
	META_CTOR_INFO_BINDER_INVOKE_CUSTOM(                                                                               \
		auto l_body = body_adapter_t{body}; if (body.args_tuple_size == 1) {                                           \
			new (body.memory) owner_t{l_body.Get<0>()};                                                                \
			return;                                                                                                    \
		} if (body.args_tuple_size == 2) {                                                                             \
			new (body.memory) owner_t{l_body.Get<0>(), l_body.Get<1>()};                                               \
			return;                                                                                                    \
		} if (body.args_tuple_size == 3) {                                                                             \
			new (body.memory) owner_t{l_body.Get<0>(), l_body.Get<1>(), l_body.Get<2>()};                              \
			return;                                                                                                    \
		} if (body.args_tuple_size == 4) {                                                                             \
			new (body.memory) owner_t{l_body.Get<0>(), l_body.Get<1>(), l_body.Get<2>(), l_body.Get<3>()};             \
			return;                                                                                                    \
		} if (body.args_tuple_size == 5) {                                                                             \
			new (body.memory)                                                                                          \
				owner_t{l_body.Get<0>(), l_body.Get<1>(), l_body.Get<2>(), l_body.Get<3>(), l_body.Get<4>()};          \
		})

#define META_CTOR_INFO_BINDER_INVOKE_DEFAULT_ARGS6()                                                                   \
	META_CTOR_INFO_BINDER_INVOKE_CUSTOM(                                                                               \
		auto l_body = body_adapter_t{body}; if (body.args_tuple_size == 1) {                                           \
			new (body.memory) owner_t{l_body.Get<0>()};                                                                \
			return;                                                                                                    \
		} if (body.args_tuple_size == 2) {                                                                             \
			new (body.memory) owner_t{l_body.Get<0>(), l_body.Get<1>()};                                               \
			return;                                                                                                    \
		} if (body.args_tuple_size == 3) {                                                                             \
			new (body.memory) owner_t{l_body.Get<0>(), l_body.Get<1>(), l_body.Get<2>()};                              \
			return;                                                                                                    \
		} if (body.args_tuple_size == 4) {                                                                             \
			new (body.memory) owner_t{l_body.Get<0>(), l_body.Get<1>(), l_body.Get<2>(), l_body.Get<3>()};             \
			return;                                                                                                    \
		} if (body.args_tuple_size == 5) {                                                                             \
			new (body.memory)                                                                                          \
				owner_t{l_body.Get<0>(), l_body.Get<1>(), l_body.Get<2>(), l_body.Get<3>(), l_body.Get<4>()};          \
			return;                                                                                                    \
		} if (body.args_tuple_size == 6) {                                                                             \
			new (body.memory) owner_t{l_body.Get<0>(), l_body.Get<1>(), l_body.Get<2>(),                               \
									  l_body.Get<3>(), l_body.Get<4>(), l_body.Get<5>()};                              \
		})

#define META_CTOR_INFO_BINDER_INVOKE_DEFAULT_ARGS7()                                                                   \
	META_CTOR_INFO_BINDER_INVOKE_CUSTOM(                                                                               \
		auto l_body = body_adapter_t{body}; if (body.args_tuple_size == 1) {                                           \
			new (body.memory) owner_t{l_body.Get<0>()};                                                                \
			return;                                                                                                    \
		} if (body.args_tuple_size == 2) {                                                                             \
			new (body.memory) owner_t{l_body.Get<0>(), l_body.Get<1>()};                                               \
			return;                                                                                                    \
		} if (body.args_tuple_size == 3) {                                                                             \
			new (body.memory) owner_t{l_body.Get<0>(), l_body.Get<1>(), l_body.Get<2>()};                              \
			return;                                                                                                    \
		} if (body.args_tuple_size == 4) {                                                                             \
			new (body.memory) owner_t{l_body.Get<0>(), l_body.Get<1>(), l_body.Get<2>(), l_body.Get<3>()};             \
			return;                                                                                                    \
		} if (body.args_tuple_size == 5) {                                                                             \
			new (body.memory)                                                                                          \
				owner_t{l_body.Get<0>(), l_body.Get<1>(), l_body.Get<2>(), l_body.Get<3>(), l_body.Get<4>()};          \
			return;                                                                                                    \
		} if (body.args_tuple_size == 6) {                                                                             \
			new (body.memory) owner_t{l_body.Get<0>(), l_body.Get<1>(), l_body.Get<2>(),                               \
									  l_body.Get<3>(), l_body.Get<4>(), l_body.Get<5>()};                              \
			return;                                                                                                    \
		} if (body.args_tuple_size == 7) {                                                                             \
			new (body.memory) owner_t{l_body.Get<0>(), l_body.Get<1>(), l_body.Get<2>(), l_body.Get<3>(),              \
									  l_body.Get<4>(), l_body.Get<5>(), l_body.Get<6>()};                              \
		})

#define META_CTOR_INFO_BINDER_INVOKE_DEFAULT_ARGS8()                                                                   \
	META_CTOR_INFO_BINDER_INVOKE_CUSTOM(                                                                               \
		auto l_body = body_adapter_t{body}; if (body.args_tuple_size == 1) {                                           \
			new (body.memory) owner_t{l_body.Get<0>()};                                                                \
			return;                                                                                                    \
		} if (body.args_tuple_size == 2) {                                                                             \
			new (body.memory) owner_t{l_body.Get<0>(), l_body.Get<1>()};                                               \
			return;                                                                                                    \
		} if (body.args_tuple_size == 3) {                                                                             \
			new (body.memory) owner_t{l_body.Get<0>(), l_body.Get<1>(), l_body.Get<2>()};                              \
			return;                                                                                                    \
		} if (body.args_tuple_size == 4) {                                                                             \
			new (body.memory) owner_t{l_body.Get<0>(), l_body.Get<1>(), l_body.Get<2>(), l_body.Get<3>()};             \
			return;                                                                                                    \
		} if (body.args_tuple_size == 5) {                                                                             \
			new (body.memory)                                                                                          \
				owner_t{l_body.Get<0>(), l_body.Get<1>(), l_body.Get<2>(), l_body.Get<3>(), l_body.Get<4>()};          \
			return;                                                                                                    \
		} if (body.args_tuple_size == 6) {                                                                             \
			new (body.memory) owner_t{l_body.Get<0>(), l_body.Get<1>(), l_body.Get<2>(),                               \
									  l_body.Get<3>(), l_body.Get<4>(), l_body.Get<5>()};                              \
			return;                                                                                                    \
		} if (body.args_tuple_size == 7) {                                                                             \
			new (body.memory) owner_t{l_body.Get<0>(), l_body.Get<1>(), l_body.Get<2>(), l_body.Get<3>(),              \
									  l_body.Get<4>(), l_body.Get<5>(), l_body.Get<6>()};                              \
			return;                                                                                                    \
		} if (body.args_tuple_size == 8) {                                                                             \
			new (body.memory) owner_t{l_body.Get<0>(), l_body.Get<1>(), l_body.Get<2>(), l_body.Get<3>(),              \
									  l_body.Get<4>(), l_body.Get<5>(), l_body.Get<6>(), l_body.Get<7>()};             \
		})

#define META_CTOR_INFO_BINDER_DEFAULT(OWNER, INDEX, OPTIONAL_ARGS, ...)                                                \
	namespace Meta                                                                                                     \
	{                                                                                                                  \
	META_CTOR_INFO_BINDER(OWNER, INDEX, OPTIONAL_ARGS, __VA_ARGS__)                                                    \
	{                                                                                                                  \
		META_CTOR_INFO_BINDER_INVOKE_DEFAULT_ARGS0();                                                                  \
	};                                                                                                                 \
	}

/////////////////////////////////////////////////
////////////////// PROPERTY INFO ////////////////
/////////////////////////////////////////////////

#define META_REBINDER_PROPERTY(OWNER, PROPERTY)                                                                        \
public:                                                                                                                \
	static constexpr auto Property_##PROPERTY##_ID = #OWNER "::" #PROPERTY##_fnv1a;                                    \
                                                                                                                       \
private:                                                                                                               \
	friend struct Meta::PropertyInfo::Rebinder<Property_##PROPERTY##_ID>

#define META_PROPERTY_INFO_BINDER(OWNER, TYPE, NAME_SYMBOL, FLAGS)                                                     \
	template<>                                                                                                         \
	struct Meta::PropertyInfo::Rebinder<OWNER::Property_##NAME_SYMBOL##_ID>                                            \
		: Binder<OWNER::Property_##NAME_SYMBOL##_ID, OWNER, TYPE, FLAGS>

#define META_PROPERTY_INFO_BINDER_BODY(OWNER, NAME_SYMBOL)                                                             \
	static constexpr char NAME[64]	 = {#OWNER "::" #NAME_SYMBOL};                                                     \
	static constexpr auto MEMBER_PTR = &OWNER::NAME_SYMBOL;

#define META_PROPERTY_INFO_BINDER_SET_CUSTOM(BODY)                                                                     \
	static void Set(const body_t& body)                                                                                \
	{                                                                                                                  \
		if constexpr (FLAGS & (Meta::PropertyInfo::eWriteOnly | Meta::PropertyInfo::eReadWrite))                       \
		{                                                                                                              \
			BODY                                                                                                       \
		}                                                                                                              \
	}

#define META_PROPERTY_INFO_BINDER_GET_CUSTOM(BODY)                                                                     \
	static void* Get(const body_t& body)                                                                               \
	{                                                                                                                  \
		if constexpr (FLAGS & (Meta::PropertyInfo::eReadOnly | Meta::PropertyInfo::eReadWrite))                        \
		{                                                                                                              \
			BODY                                                                                                       \
		}                                                                                                              \
		else                                                                                                           \
		{                                                                                                              \
			return nullptr;                                                                                            \
		}                                                                                                              \
	}

#define META_PROPERTY_INFO_BINDER_SET_DEFAULT()                                                                        \
	META_PROPERTY_INFO_BINDER_SET_CUSTOM(                                                                              \
		if constexpr (FLAGS & (Meta::PropertyInfo::eWriteOnly | Meta::PropertyInfo::eReadWrite)) {                     \
			static_cast<owner_t*>(body.owner)->*MEMBER_PTR = *static_cast<type_t*>(body.value);                        \
		})

#define META_PROPERTY_INFO_BINDER_GET_DEFAULT()                                                                        \
	META_PROPERTY_INFO_BINDER_GET_CUSTOM(return &(static_cast<owner_t*>(body.owner)->*MEMBER_PTR);)

#define META_PROPERTY_INFO_BINDER_DEFAULT(OWNER, TYPE, NAME_SYMBOL, FLAGS)                                             \
	namespace Meta                                                                                                     \
	{                                                                                                                  \
	META_PROPERTY_INFO_BINDER(OWNER, TYPE, NAME_SYMBOL, FLAGS)                                                         \
	{                                                                                                                  \
		META_PROPERTY_INFO_BINDER_BODY(OWNER, NAME_SYMBOL);                                                            \
		META_PROPERTY_INFO_BINDER_SET_DEFAULT();                                                                       \
		META_PROPERTY_INFO_BINDER_GET_DEFAULT();                                                                       \
	};                                                                                                                 \
	}

/////////////////////////////////////////////////
////////////////// METHOD INFO //////////////////
/////////////////////////////////////////////////

#define META_REBINDER_METHOD(OWNER, METHOD)                                                                            \
public:                                                                                                                \
	static constexpr auto Method_##METHOD##_ID = #OWNER "::" #METHOD##_fnv1a;                                          \
                                                                                                                       \
private:                                                                                                               \
	friend struct Meta::MethodInfo::Rebinder<Method_##METHOD##_ID>

#define META_METHOD_INFO_BINDER(OWNER, NAME_SYMBOL, RETURN_TYPE, OPTIONAL_ARGS, FLAGS, ...)                            \
	template<>                                                                                                         \
	struct Meta::MethodInfo::Rebinder<OWNER::Method_##NAME_SYMBOL##_ID>                                                \
		: MethodInfo::Binder<OWNER::Method_##NAME_SYMBOL##_ID, OWNER, RETURN_TYPE, TypeTraits::TypeList<__VA_ARGS__>,  \
							 OPTIONAL_ARGS, FLAGS>

#define META_METHOD_INFO_BINDER_BODY(OWNER, NAME_SYMBOL)                                                               \
	static constexpr char NAME[64]	 = {#OWNER "::" #NAME_SYMBOL};                                                     \
	static constexpr auto MEMBER_PTR = &OWNER::NAME_SYMBOL

#define META_METHOD_INFO_BINDER_INVOKE_CUSTOM(...)                                                                     \
	static Meta::Value Invoke(body_t& body)                                                                            \
	{                                                                                                                  \
		body_adapter_t l_body{body};                                                                                   \
		if constexpr (FLAGS & Meta::MethodInfo::eCallable)                                                             \
		{                                                                                                              \
			__VA_ARGS__                                                                                                \
		}                                                                                                              \
		else                                                                                                           \
		{                                                                                                              \
			return {};                                                                                                 \
		}                                                                                                              \
	}

#define META_METHOD_INFO_BINDER_INVOKE_DEFAULT_ARGS0()                                                                 \
	META_METHOD_INFO_BINDER_INVOKE_CUSTOM(                                                                             \
		if (l_body.args_tuple_size == 0) { return (l_body.owner.*MEMBER_PTR)(); } else { return {}; })

#define META_METHOD_INFO_BINDER_INVOKE_DEFAULT_ARGS1()                                                                 \
	META_METHOD_INFO_BINDER_INVOKE_CUSTOM(                                                                             \
		if (l_body.args_tuple_size == 1) { return (l_body.owner.*MEMBER_PTR)(l_body.Get<0>()); } else { return {}; })

#define META_METHOD_INFO_BINDER_INVOKE_DEFAULT_ARGS2()                                                                 \
	META_METHOD_INFO_BINDER_INVOKE_CUSTOM(                                                                             \
		if (l_body.args_tuple_size == 2) {                                                                             \
			return (l_body.owner.*MEMBER_PTR)(l_body.Get<0>(), l_body.Get<1>());                                       \
		} else { return {}; })

#define META_METHOD_INFO_BINDER_INVOKE_DEFAULT_ARGS3()                                                                 \
	META_METHOD_INFO_BINDER_INVOKE_CUSTOM(                                                                             \
		if (l_body.args_tuple_size == 3) {                                                                             \
			return (l_body.owner.*MEMBER_PTR)(l_body.Get<0>(), l_body.Get<1>(), l_body.Get<2>());                      \
		} else { return {}; })

#define META_METHOD_INFO_BINDER_INVOKE_DEFAULT_ARGS4()                                                                 \
	META_METHOD_INFO_BINDER_INVOKE_CUSTOM(                                                                             \
		if (l_body.args_tuple_size == 4) {                                                                             \
			return (l_body.owner.*MEMBER_PTR)(l_body.Get<0>(), l_body.Get<1>(), l_body.Get<2>(), l_body.Get<3>());     \
		} else { return {}; })

#define META_METHOD_INFO_BINDER_INVOKE_DEFAULT_ARGS5()                                                                 \
	META_METHOD_INFO_BINDER_INVOKE_CUSTOM(                                                                             \
		if (l_body.args_tuple_size == 5) {                                                                             \
			return (l_body.owner.*MEMBER_PTR)(l_body.Get<0>(), l_body.Get<1>(), l_body.Get<2>(), l_body.Get<3>(),      \
											  l_body.Get<4>());                                                        \
		} else { return {}; })

#define META_METHOD_INFO_BINDER_INVOKE_DEFAULT_ARGS6()                                                                 \
	META_METHOD_INFO_BINDER_INVOKE_CUSTOM(                                                                             \
		if (l_body.args_tuple_size == 6) {                                                                             \
			return (l_body.owner.*MEMBER_PTR)(l_body.Get<0>(), l_body.Get<1>(), l_body.Get<2>(), l_body.Get<3>(),      \
											  l_body.Get<4>(), l_body.Get<5>());                                       \
		} else { return {}; })

#define META_METHOD_INFO_BINDER_INVOKE_DEFAULT_ARGS7()                                                                 \
	META_METHOD_INFO_BINDER_INVOKE_CUSTOM(                                                                             \
		if (l_body.args_tuple_size == 7) {                                                                             \
			return (l_body.owner.*MEMBER_PTR)(l_body.Get<0>(), l_body.Get<1>(), l_body.Get<2>(), l_body.Get<3>(),      \
											  l_body.Get<4>(), l_body.Get<5>(), l_body.Get<6>());                      \
		} else { return {}; })

#define META_METHOD_INFO_BINDER_INVOKE_DEFAULT_ARGS8()                                                                 \
	META_METHOD_INFO_BINDER_INVOKE_CUSTOM(                                                                             \
		if (l_body.args_tuple_size == 8) {                                                                             \
			return (l_body.owner.*MEMBER_PTR)(l_body.Get<0>(), l_body.Get<1>(), l_body.Get<2>(), l_body.Get<3>(),      \
											  l_body.Get<4>(), l_body.Get<5>(), l_body.Get<6>(), l_body.Get<7>());     \
		} else { return {}; })

#define META_METHOD_INFO_BINDER_DEFAULT(OWNER, NAME_SYMBOL, RETURN_TYPE, OPTIONAL_ARGS, FLAGS, INVOKE_MACRO, ...)      \
	namespace Meta                                                                                                     \
	{                                                                                                                  \
	META_METHOD_INFO_BINDER(OWNER, NAME_SYMBOL, RETURN_TYPE, OPTIONAL_ARGS, FLAGS, __VA_ARGS__)                        \
	{                                                                                                                  \
		META_METHOD_INFO_BINDER_BODY(OWNER, NAME_SYMBOL);                                                              \
		INVOKE_MACRO();                                                                                                \
	};                                                                                                                 \
	}

/////////////////////////////////////////////////
////////////////// CLASS INFO ///////////////////
/////////////////////////////////////////////////

#define CLASSOF(X)	Meta::Classof<X>()
#define CLASSOFV(X)	Meta::Classof(X)

#define META_CLASS_INFO_BEGIN(FLAGS)                                                                                   \
	namespace Meta                                                                                                     \
	{                                                                                                                  \
	template<>                                                                                                         \
		struct ClassInfo::Rebinder<META_CLASS_INFO_TARGET>: ClassInfo::Binder < META_CLASS_INFO_TARGET,

#define CTOR_BINDER(NAME)	  CtorInfo::Rebinder<META_CLASS_INFO_TARGET::Ctor_##NAME##_ID>
#define PROPERTY_BINDER(NAME) PropertyInfo::Rebinder<META_CLASS_INFO_TARGET::Property_##NAME##_ID>
#define METHOD_BINDER(NAME)	  MethodInfo::Rebinder<META_CLASS_INFO_TARGET::Method_##NAME##_ID>

#define BINDERS(...) TypeTraits::TypeList<__VA_ARGS__>

#define META_CLASS_INFO_END()                                                                                          \
	> {};                                                                                                        \
	}

#define META_CLASS_AUTO_REGISTER(NAME)	\
struct NAME##ClassAutoRegister	\
{	\
	NAME##ClassAutoRegister(){ Meta::Classof<NAME>(); }	\
};  static NAME##ClassAutoRegister g##NAME##ClassAutoRegister

/////////////////////////////////////////////////
////////////////// ENUM INFO ////////////////////
/////////////////////////////////////////////////

#define ENUMOF(X)	Meta::Enumof<X>()
#define ENUMOFV(X)	Meta::Enumof(X)

#define META_ENUM_INFO_BINDER_BEGIN(FLAGS)                                                                             \
	namespace Meta                                                                                                     \
	{                                                                                                                  \
	template<>                                                                                                         \
	struct EnumInfo::Rebinder<META_ENUM_INFO_TARGET>: Binder<META_ENUM_INFO_TARGET, FLAGS>                             \
	{                                                                                                                  \
		static auto Pairs()                                                                                            \
		{                                                                                                              \
			return eastl::vector<eastl::pair<eastl::string_view, int64_t>>                                               \
			{                                                                                                          \
				{

#define ENUM_VALUE(V)                                                                                                  \
	{                                                                                                                  \
#V, (int64_t)META_ENUM_INFO_TARGET::V                                                                                                          \
	}

#define META_ENUM_INFO_BINDER_END()                                                                                    \
	}                                                                                                                  \
	}                                                                                                                  \
	;                                                                                                                  \
	}                                                                                                                  \
	}                                                                                                                  \
	;                                                                                                                  \
	}

#define META_ENUM_AUTO_REGISTER(NAME)	\
struct NAME##EnumAutoRegister	\
{	\
	NAME##EnumAutoRegister(){ Meta::Enumof<NAME>(); }	\
};  static NAME##EnumAutoRegister g##NAME##EnumAutoRegister

#define META_ENUM_AUTO_REGISTER_CUSTOM(ENUM_TYPE, NAME)	\
struct NAME##EnumAutoRegister	\
{	\
	NAME##EnumAutoRegister(){ Meta::Enumof<ENUM_TYPE>(); }	\
};  static NAME##EnumAutoRegister g##NAME##EnumAutoRegister

#endif
