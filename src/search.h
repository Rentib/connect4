#ifndef SEARCH_H_
#define SEARCH_H_

#include "position.h"

struct search_stack {
	int ply;
	u64 nodes;
	enum move *pv;
	enum move move;
	enum move killer;
};

void search(struct position *position);

#endif /* SEARCH_H_ */
