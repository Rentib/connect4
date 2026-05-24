#ifndef SEARCH_H_
#define SEARCH_H_

#include "position.h"

struct search_stack {
	int ply;
	enum move move;
	enum move *pv;
	u64 nodes;
};

void search(struct position *position);

#endif /* SEARCH_H_ */
