/**
 * These macros help to declare and define TypeInfo.
 * Use it only if you know what you are doing.
 */
 
 #ifndef TYPE_INFO_HELPER_H_
 #define TYPE_INFO_HELPER_H_

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

#define DefinePrimitive(Name) \
DeclareTypeInfo(Name) \
DefineTypeInfo(Name) { \
	TypeInfoManager::GetSingleton()->Create( \
			TypeInfoBind<Name>::GetName(), \
			sizeof(Name), \
			TypeInfo::Members()); \
}

#define offset(type,member) &((type *)(0))->member

#endif 			//TYPE_INFO_HELPER_H_
