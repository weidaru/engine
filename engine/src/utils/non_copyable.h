#ifndef NON_COPYABLE_H_
#define NON_COPYABLE_H_

class NonCopyable {
public:
	NonCopyable() {}

private:
	NonCopyable(const NonCopyable &);
	NonCopyable &operator=(const NonCopyable &);
};


#endif			//NON_COPYABLE_H_