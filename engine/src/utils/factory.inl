#define RegistrationClassName(Prototype, Substance) Prototype##Substance##Registration
#define RegistrationObjName(Prototype, Substance) Prototype##Substance##RegistrationObj

#define DeclareRegistration(Prototype, Substance) \
class RegistrationClassName(Prototype, Substance) : public Registration<Substance> { \
public: \
	RegistrationClassName(Prototype, Substance)() {	\
	Factory<Substance>::Register(#Prototype, &RegistrationObjName(Prototype, Substance)); } \
	virtual Substance * Create(void *data); \
private: \
	static RegistrationClassName(Prototype, Substance) RegistrationObjName(Prototype, Substance); \
};

#define DefineRegistration(Prototype, Substance) \
RegistrationClassName(Prototype, Substance) RegistrationClassName(Prototype, Substance)::RegistrationObjName(Prototype, Substance); \
Component * RegistrationClassName(Prototype, Substance)::Create(void *data)