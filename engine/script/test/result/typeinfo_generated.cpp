#include <utility>
#include "utils/type_info.h"
#include "utils/type_info_helper.h"

//All the necessary struct declaration
struct Bar {
	double a;
	Foo * foo_ptr;
	Foo &foo_ref;
};

struct Foo {
	char a;
	int b;
	Bar bar;
};

namespace s2 {
//TypeInfo for primitive
DefinePrimitive(bool)
DefinePrimitive(int)
DefinePrimitive(unsigned int)
DefinePrimitive(unsigned char)
DefinePrimitive(char)
DefinePrimitive(double)
DefinePrimitive(float)

//TypeInfo for pointer and reference
namespace internal {
struct PointerRegistration {
	static PointerRegistration registration;

	PointerRegistration() {
		TypeInfoManager::GetSingleton()->Create( 
			pointer_typename, 
			sizeof(char *), 
			TypeInfo::Members()); 
	}
};
PointerRegistration PointerRegistration::registration;

struct Dummy {
	int &a;

	Dummy(int new_a) : a(new_a) {}
};

struct ReferenceRegistration {
	static ReferenceRegistration registration;

	ReferenceRegistration() {
		TypeInfoManager::GetSingleton()->Create( 
			reference_typename, 
			sizeof(Dummy), 
			TypeInfo::Members()); 
	}
};
ReferenceRegistration ReferenceRegistration::registration;

}


//TypeInfo for user defined type
DeclareTypeInfo(Bar)
DefineTypeInfo(Bar) {
	TypeInfo::Members members;
	{
		TypeInfo::Member member;
		member.type_name="double";
		member.name="a";
		member.offset=offset(Bar,a);
		members.push_back(member)
	{
	{
		TypeInfo::Member member;
		member.type_name="pointer";
		member.name="foo_ptr";
		member.offset=offset(Bar,foo_ptr);
		members.push_back(member)
	{
	{
		TypeInfo::Member member;
		member.type_name="reference";
		member.name="foo_ref";
		member.offset=offset(Bar,foo_ref);
		members.push_back(member)
	{
	TypeInfoManager::GetSingleton()->Create(TypeInfoBind<Bar>::GetName(), sizeof(Bar), members);
}

DeclareTypeInfo(Foo)
DefineTypeInfo(Foo) {
	TypeInfo::Members members;
	{
		TypeInfo::Member member;
		member.type_name="char";
		member.name="a";
		member.offset=offset(Foo,a);
		members.push_back(member)
	{
	{
		TypeInfo::Member member;
		member.type_name="int";
		member.name="b";
		member.offset=offset(Foo,b);
		members.push_back(member)
	{
	{
		TypeInfo::Member member;
		member.type_name="Bar";
		member.name="bar";
		member.offset=offset(Foo,bar);
		members.push_back(member)
	{
	TypeInfoManager::GetSingleton()->Create(TypeInfoBind<Foo>::GetName(), sizeof(Foo), members);
}

}
