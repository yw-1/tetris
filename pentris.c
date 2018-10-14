#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <SDL2/SDL.h>

int piece[18][5][3] = {
	{{0,1,0},{0,1,0},{0,1,0},{0,1,0},{0,1,0}},
	{{0,0,0},{0,0,0},{0,1,1},{1,1,0},{0,1,0}},
	{{0,0,0},{0,0,0},{1,1,0},{0,1,1},{0,1,0}},
	{{0,0,0},{0,1,0},{0,1,0},{0,1,0},{1,1,0}},
	{{0,0,0},{0,1,0},{0,1,0},{0,1,0},{0,1,1}},
	{{0,0,0},{0,0,0},{1,1,0},{1,1,0},{0,1,0}},
	{{0,0,0},{0,0,0},{0,1,1},{0,1,1},{0,1,0}},
	{{0,0,0},{0,1,0},{0,1,0},{1,1,0},{1,0,0}},
	{{0,0,0},{0,1,0},{0,1,0},{0,1,1},{0,0,1}},
	{{0,0,0},{0,0,0},{1,1,1},{0,1,0},{0,1,0}},
	{{0,0,0},{0,0,0},{0,0,0},{1,0,1},{1,1,1}},
	{{0,0,0},{0,0,0},{0,0,1},{0,0,1},{1,1,1}},
	{{0,0,0},{0,0,0},{0,0,1},{0,1,1},{1,1,0}},
	{{0,0,0},{0,0,0},{0,1,0},{1,1,1},{0,1,0}},
	{{0,0,0},{0,1,0},{1,1,0},{0,1,0},{0,1,0}},
	{{0,0,0},{0,1,0},{0,1,1},{0,1,0},{0,1,0}},
	{{0,0,0},{0,0,0},{0,1,1},{0,1,0},{1,1,0}},
	{{0,0,0},{0,0,0},{1,1,0},{0,1,0},{0,1,1}},
};

int piece_color[18][3] = {
	{255, 0, 0},
	{255, 85, 0},
	{255, 170, 0},
	{255, 255, 0},
	{170, 255, 0},
	{85, 255, 0},
	{0, 255, 0},
	{0, 255, 85},
	{0, 255, 170},
	{0, 255, 255},
	{0, 170, 255},
	{0, 85, 255},
	{0, 0, 255},
	{85, 0, 255},
	{170, 0, 255},
	{255, 0, 255},
	{255, 0, 170},
	{255, 0, 85},
};

int size = 30, sw = 15, sh = 30;

void rotate(int *xx, int *yy, int x, int y, int pn, int rot)
{
	int rot_new;

	switch (pn) {
	case 0:
		rot_new = rot % 2;
		break;
	default:
		rot_new = rot;
		break;
	}

	switch (rot_new) {
	case 0:
		*xx = x;
		*yy = y;
		break;
	case 1:
		if (pn == 0 || pn == 14 || pn == 15) {
			*xx = y - 1;
			*yy = 3 - x;
		} else {
			*xx = y - 2;
			*yy = 4 - x;
		}
		break;
	case 2:
		if (pn == 14 || pn == 15) {
			*xx = 2 - x;
			*yy = 4 - y;
		} else {
			*xx = 2 - x;
			*yy = 6 - y;
		}
		break;
	case 3:
		if (pn == 14 || pn == 15) {
			*xx = 3 - y;
			*yy = x + 1;
		} else {
			*xx = 4 - y;
			*yy = x + 2;
		}
		break;
	}
}

int valid(int *board, int pn, int px, int py, int rot)
{
	int xx, yy;

	for (int y=0; y<5; y++) {
		for (int x=0; x<3; x++) {
			rotate(&xx, &yy, x, y, pn, rot);

			if (py + yy >= 0 && piece[pn][y][x]) {
				if (px + xx >= sw || px + xx < 0) return 0;
				if (py + yy >= sh) return 0;
				if (py >= 0 && board[sw*(py + yy) + px + xx]) return 0;
			}
		}
	}

	return 1;
}

int place(int *board, int pn, int px, int py, int rot)
{
	int xx, yy, clear, line = 0;

	for (int y=0; y<5; y++) {
		for (int x=0; x<3; x++) {
			rotate(&xx, &yy, x, y, pn, rot);

			if (py + yy >= 0 && piece[pn][y][x]) board[sw*(py + yy) + px + xx] = pn + 1;
		}
	}

	//clear lines
	for (int y=py-6; y<py+6; y++) {
		if (y >= 0 && y < sh) {
			clear = 1;

			for (int x=0; x<sw; x++) {
				if (board[sw*y + x] == 0) {
					clear = 0;
					break;
				}
			}

			if (clear) {
				line++;
				for (yy=y; yy>0; yy--)
					for (int x=0; x<sw; x++) board[sw*yy + x] = board[sw*(yy-1) + x];
			}
		}
	}

	return line;
}

int main(int argc, char *argv[])
{
	SDL_Window *window;
	SDL_Renderer *renderer;
	SDL_Event event;

	int end = 0, redraw = 1;
	int key, shift;

	int speed = 20, line = 0;
	int pause = 0, put = 0;
	int game = 0;
	int startx = (sw - 3)/2;
	int pn, px, py, rot;

	int xx, yy, n;

	char title[255];
	snprintf(title, 255, "lines=%d,speed=%d", line, speed);

	if (argc == 4) {
		sw = atoi(argv[1]);
		sh = atoi(argv[2]);
		size = atoi(argv[3]);

		if (sw < 4) sw = 4;
		if (sh < 2) sh = 2;
		if (size < 1) size = 1;

		startx = (sw - 4)/2;
	}

	int *board = malloc(sw*sh*sizeof(int));
	memset(board, 0, sw*sh*sizeof(int));

	SDL_Init(SDL_INIT_EVERYTHING);
	window = SDL_CreateWindow(title, SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED,
			sw*size, sh*size, 0);
	renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);

	SDL_Rect box;

	srand(time(NULL));

	pn = rand() % 18;
	px = startx;
	py = -2;
	rot = 0;

	int t = SDL_GetTicks();

	while (!end) {
		if (SDL_GetTicks() - t > 3*(101-speed) && !pause) {
			if (valid(board, pn, px, py + 1, rot))
				py++;
			else put = 1;

			t = SDL_GetTicks();
			redraw = 1;
		}

		if (put) {
			if (valid(board, pn, px, py, rot)) {
				n = place(board, pn, px, py, rot);
				if (n) {
					line += n;
					snprintf(title, 255, "lines=%d,speed=%d", line, speed);
					SDL_SetWindowTitle(window, title);
				}
			}

			pn = rand() % 18;
			px = startx;
			py = -2;
			rot = 0;

			put = 0;
			redraw = 1;
			
			//check if lost
			if (!valid(board, pn, px, py, rot)) {
				printf("game %d: %d lines\n", ++game, line);
				memset(board, 0, sw*sh*sizeof(int));
				line = 0;
				snprintf(title, 255, "lines=%d,speed=%d", line, speed);
				SDL_SetWindowTitle(window, title);
			}
		}

		if (redraw) {
			SDL_SetRenderDrawColor(renderer, 0, 0, 0, 0);
			SDL_RenderClear(renderer);

			box.w = size - 1;
			box.h = size - 1;

			SDL_SetRenderDrawColor(renderer, 50, 50, 50, 0);
			SDL_RenderDrawRect(renderer, &box);

			for (int y=0; y<sh; y++) {
				for (int x=0; x<sw; x++) {
					box.x = x*size;
					box.y = y*size;

					if (board[sw*y + x] != 0) {
						SDL_SetRenderDrawColor(renderer,
								piece_color[board[sw*y + x] - 1][0],
								piece_color[board[sw*y + x] - 1][1],
								piece_color[board[sw*y + x] - 1][2], 0);
						SDL_RenderFillRect(renderer, &box);
						SDL_SetRenderDrawColor(renderer, 50, 50, 50, 0);
					} else {
						SDL_RenderDrawRect(renderer, &box);
					}
				}
			}

			for (int y=0; y<5; y++) {
				for (int x=0; x<3; x++) {
					rotate(&xx, &yy, x, y, pn, rot);

					box.x = (px + xx)*size;
					box.y = (py + yy)*size;

					if (piece[pn][y][x]) {
						SDL_SetRenderDrawColor(renderer, piece_color[pn][0],
								piece_color[pn][1], piece_color[pn][2], 0);
						SDL_RenderFillRect(renderer, &box);
					}
				}
			}

			SDL_RenderPresent(renderer);
			redraw = 0;
		}

		while (SDL_PollEvent(&event)) {
			switch (event.type) {
			case SDL_QUIT:
				end = 1;
				break;
			case SDL_WINDOWEVENT:
				SDL_RenderPresent(renderer);
				break;
			case SDL_KEYDOWN:
				key = event.key.keysym.sym;
				shift = event.key.keysym.mod == 0x1002 || event.key.keysym.mod == 0x1001;

				switch (key) {
				case SDLK_ESCAPE:
					end = 1;
					break;
				case SDLK_UP:
					if (valid(board, pn, px, py, (rot + 1) % 4))
						rot = (rot + 1) % 4;
					pause = 0;
					break;
				case SDLK_LEFT:
					if (shift) {
						for (xx=0; xx<sw; xx++)
							if (!valid(board, pn, px - xx, py, rot)) break;
						px = px - xx + 1;
					} else {
						if (valid(board, pn, px - 1, py, rot)) px--;
					}
					pause = 0;
					break;
				case SDLK_RIGHT:
					if (shift) {
						for (xx=px; xx<sw; xx++)
							if (!valid(board, pn, xx, py, rot)) break;
						px = xx - 1; 
					} else {
						if (valid(board, pn, px + 1, py, rot)) px++;
					}
					pause = 0;
					break;
				case SDLK_DOWN: case SDLK_SPACE:
					if (shift || key == SDLK_SPACE) {
						for (yy=py; yy<sh; yy++)
							if (!valid(board, pn, px, yy, rot)) break;
						py = yy - 1;
						put = 1;
					} else {
						if (valid(board, pn, px, py + 1, rot)) py++;
					}
					pause = 0;
					break;
				case SDLK_p:
					pause = !pause;
					break;
				case SDLK_RETURN:
					printf("game %d: %d lines\n", ++game, line);
					pn = rand() % 18;
					px = startx;
					py = -2;
					rot = 0;
					put = 0;
					redraw = 1;
					memset(board, 0, sw*sh*sizeof(int));
					line = 0;
					snprintf(title, 255, "lines=%d,speed=%d", line, speed);
					SDL_SetWindowTitle(window, title);
					break;
				case SDLK_KP_PLUS: case SDLK_PLUS: case SDLK_EQUALS:
					if (speed < 100) {
						speed++;
						snprintf(title, 255, "lines=%d,speed=%d", line, speed);
						SDL_SetWindowTitle(window, title);
					}
					break;
				case SDLK_KP_MINUS: case SDLK_MINUS:
					if (speed > 1) {
						speed--;
						snprintf(title, 255, "lines=%d,speed=%d", line, speed);
						SDL_SetWindowTitle(window, title);
					}
					break;
				}

				redraw = 1;
				break;
			}
			//SDL_FlushEvent(event.type);
		}
	}

	free(board);
	SDL_DestroyRenderer(renderer);
	SDL_DestroyWindow(window);
	SDL_Quit();

	return 0;
}

