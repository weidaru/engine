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
namespace typeinfo { \
class TypeInfoRegistration##TypeName { \
private: \
	TypeInfoRegistration(); \
	static TypeInfoRegistration registration; \
}; \
} \

#define DefineTypeInfo(TypeName) \
TypeInfoRegistration##TypeName::registration; \
TypeInfoRegistration##TypeName::TypeInfoRegistration() \