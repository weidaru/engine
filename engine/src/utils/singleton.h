#ifndef SINGLETON_H_
#define SINGLETON_H_

template <typename T>
class Singleton {
public:
	static T * GetSingleton() {
		static T data;
		return &data;
	}

	Singleton() {}
	virtual ~Singleton() {}

private:
	Singleton(const Singleton &);
	Singleton & operator=(const Singleton &);
};

#endif		//SINGLETON_H_