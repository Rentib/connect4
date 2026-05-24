#include <ctype.h>
#include <stdio.h>
#include <string.h>

#include "connect4.h"
#include "position.h"
#include "search.h"
#include "transposition.h"

static bool running;

static void readline(char *input)
{
	int c;
	while (isspace(c = getchar())) {}
	for (; c != '\n' && c != EOF; c = getchar())
		*input++ = c;
	*input = '\0';
}

static void position_(struct position *pos, char *fmt)
{
	enum move move_list[FILES], *last, *m;

	for (; *fmt; fmt++) {
		last = pos_moves(pos, move_list);
		for (m = move_list; m != last; m++) {
			if (!strncmp(fmt, MOVE_STR(*m), 1))
				break;
		}
		if (m == last) {
			printf("Invalid move: \'%s\'.\n", fmt);
			return;
		}
		pos_do_move(pos, *m);
	}
}

int main(void)
{
	char cmd[1024] = {0};
	struct position position, *pos = &position;

	printf("connect4 " VERSION " by Stanislaw Bitner\n");

	setbuf(stdin, nullptr);
	setbuf(stdout, nullptr);

	pos_init();
	tt_init(256);

	pos_reset(pos);
	for (running = true; running;) {
		readline(cmd);
		if (!*cmd)
			break;
		position_(pos, cmd);
		pos_print(pos);
		search(pos);
	}

	tt_free();

	return 0;
}
