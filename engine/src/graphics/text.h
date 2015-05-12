#ifndef TEXT_H_
#define TEXT_H_

#include "entity/component.h"
#include "utils/s2string.h"

namespace s2 {
class Text : public Component {
public:

private:
	s2string font_name;
	unsigned int font_size;
};

}


#endif			//TEXT_H_