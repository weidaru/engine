/**
 * These macros help to declare and define TypeInfo.
 * Use it only if you know what you are doing.
 * In general case, TypeInfo initialization is handled by script through annotation tag, for example
 * //[[TypeInfo]]//
 * 	struct Foo {
 * 		int a;
 *		float b;
 * 	}
 * This allows the script to generate type info for run-time. See more in the script/generate_type_info.lua
 */

#define StringifyInternal(Name)	#Name
#define Stringify(Name) StringifyInternal(Name)

#define DeclareTypeInfo(TypeName) \
template <> \
struct TypeInfoBind<TypeName> { \
public: \
	static s2::s2string GetName(); \
private: \
	TypeInfoBind(); \
	TypeInfoBind(const TypeInfoBind &); \
	TypeInfoBind & operator=(const TypeInfoBind &); \
	static TypeInfoBind registration; \
};

#define DefineTypeInfo(TypeName) \
s2::s2string s2::TypeInfoBind<TypeName>::GetName() { \
	return Stringify(TypeName); \
} \
s2::TypeInfoBind<TypeName> s2::TypeInfoBind<TypeName>::registration; \
s2::TypeInfoBind<TypeName>::TypeInfoBind() 