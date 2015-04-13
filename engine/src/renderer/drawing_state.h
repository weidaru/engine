#ifndef DRAW_STATE_H_
#define DRAW_STATE_H_

namespace s2 {

class GraphicPipeline;

class DrawingState {
public:
	virtual ~DrawingState() {}
private:
	virtual void Dummy() = 0;			//Make this an interface.
};

}


#endif				//DRAW_STATE_H_