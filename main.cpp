// Copyright (C) 2022 Jacob Allen. Released under the MIT license.

#include <iostream>
#include <SDL.h>
#undef main
#include <thread>
#include <vector>

#include <string>
#include <map>

#include <complex>

#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>
#define STB_IMAGE_WRITE_IMPLEMENTATION
#include <stb_image_write.h>

int ScreenWidth = 640;
int ScreenHeight = 480;

using namespace std;

struct pixel {
	uint8_t r = 0;
	uint8_t g = 0;
	uint8_t b = 0;
	uint8_t a = 0;
};

pixel pixelFromImage(unsigned char* image, int imageW, int imageH, int x, int y)
{
	unsigned long long cords = (std::min(y, imageH) * imageW + std::min(x, imageW)) * 4;
	return {
		(uint8_t)image[cords + 0],
		(uint8_t)image[cords + 1],
		(uint8_t)image[cords + 2],
		(uint8_t)image[cords + 3]
	};
}

void pixelToImage(unsigned char* image, int imageW, int imageH, pixel pix, int x, int y)
{
	unsigned long long cords = (std::min(y, imageH) * imageW + std::min(x, imageW)) * 4;
	image[cords + 0] = pix.r;
	image[cords + 1] = pix.g;
	image[cords + 2] = pix.b;
	image[cords + 3] = pix.a;
}

void threadedRenderCopy(unsigned char* picture, void* pixels, int frame, int w, int h, int minX, int minY, int maxX, int maxY)
{
	for (int y = minY; y < maxY; y++)
	{
		for (int x = minX; x < maxX; x++)
		{
			pixel pix = pixelFromImage(picture, w, h, x + (sin((float)y + frame) * 5.0f), y);
			//std::cout << pix->r << ", " << pix->g << ", " << pix->b << ", " << pix->a << ", " << std::endl;
			pixelToImage((unsigned char*)pixels, w, h, pix, x, y);
		}
	}
}

const int MAX_ITER = 100;
int RE_START = -2;
int RE_END = 1;
int IM_START = -1;
int IM_END = 1;

int mandelbrot(float c)
{
	float z = 0;
	int n = 0;
	while (abs(z) <= 2 && n < MAX_ITER)
	{
		z = z * z + c;
		n += 1;
	}
	return n;
}

class vec3 {
public:
	union {
		struct {
			float x, y, z;
		};
		struct {
			float r, g, b;
		};
	};
	vec3(float xi, float yi, float zi)
	{
		x = xi;
		y = yi;
		z = zi;
	}
	vec3(float xyzi)
	{
		x = xyzi;
		y = xyzi;
		z = xyzi;
	}
};
class vec2 {
public:
	union {
		struct {
			float x, y;
		};
		struct {
			float r, g;
		};
	};
	vec2(float xi, float yi)
	{
		x = xi;
		y = yi;
	}
	vec2(float xyi)
	{
		x = xyi;
		y = xyi;
	}
};

unsigned char* picture;

bool isOpen = true;

inline unsigned int AsInt(float f)
{
	return *(unsigned int*)&f;
}

inline float AsFloat(unsigned int i)
{
	return *(float*)&i;
}

inline float NegateFloat(float f)
{
	return AsFloat(AsInt(f) ^ 0x80000000);
}

#define RADIANS 0.017453292519943295

//const int zoom = 100;
const float brightness = 0.975;
float fScale = 1.25;

float cosRange(float degrees, float range, float minimum) {
	return (((1.0 + cos(degrees * RADIANS)) * 0.5) * range) + minimum;
}

//vec3 render(int x, int y)
//{
//	float time = SDL_GetTicks() * 0.005;
//	vec2 p = vec2((float)x / ScreenWidth, (float)y / ScreenHeight);
//	float ct = cosRange(time * 5.0, 3.0, 1.1);
//	float xBoost = cosRange(time * 0.2, 5.0, 5.0);
//	float yBoost = cosRange(time * 0.1, 10.0, 5.0);
//
//	fScale = cosRange(time * 15.5, 1.25, 0.5);
//
//	for (int i = 1; i < zoom; i++) {
//		float _i = float(i);
//		vec2 newp = p;
//		newp.x += 0.25 / _i * sin(_i * p.y + time * cos(ct) * 0.5 / 20.0 + 0.005 * _i) * fScale + xBoost;
//		newp.y += 0.25 / _i * sin(_i * p.x + time * ct * 0.3 / 40.0 + 0.03 * float(i + 15)) * fScale + yBoost;
//		p = newp;
//	}
//
//	vec3 col = vec3(0.5 * sin(3.0 * p.x) + 0.5, 0.5 * sin(3.0 * p.y) + 0.5, sin(p.x + p.y));
//	col.r *= brightness;
//	col.g *= brightness;
//	col.b *= brightness;
//
//	return col;
//}

const int MAX_ITERATIONS = 100;

float zoom = 1.0f;

float camX = 0;
float camY = 0;

inline int value(int x, int y) {
	complex<float> point(((((float)x / ScreenWidth) - 0.5f) / zoom) + camX, ((((float)y / ScreenHeight) - 0.5) / zoom) + camY);
	unsigned int nb_iter = 0;
	complex<float> z(0, 0);
	while (abs(z) < 2 && nb_iter <= MAX_ITERATIONS * zoom) {
		z = z * z + point;
		nb_iter++;
	}
	if (nb_iter < MAX_ITERATIONS * zoom) return 255*nb_iter/33;
	else return 0;
}

pixel render(int x, int y)
{
	pixel col;

	col.r = value(x, y);
	col.a = 255;
	return col;
}

const unsigned int timeLimit = 20000;

void renderFudge(int xStart, int yStart, int xEnd, int yEnd)
{
	while (isOpen)
	{
		unsigned int start = SDL_GetTicks();
		for (unsigned int y = yStart; y < yEnd; y++)
		{
			if (SDL_GetTicks() - start > timeLimit) break;
			for (unsigned int x = xStart; x < xEnd; x++)
			{
				pixel col = render(x, y);
				pixelToImage(picture, ScreenWidth, ScreenHeight, col, x, y);
			}
		}
	}
}

int main()
{
	SDL_Window* window = NULL;

	SDL_Surface* surface = NULL;

	SDL_Renderer* renderer = NULL;

	SDL_Init(SDL_INIT_VIDEO);

	window = SDL_CreateWindow("CPU Brot Plot by Jacob Allen", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, ScreenWidth, ScreenHeight, SDL_WINDOW_SHOWN | SDL_WINDOW_RESIZABLE);

	surface = SDL_GetWindowSurface(window);

	renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);

	int w = ScreenWidth, h = ScreenHeight, c = 4;
	picture = (unsigned char*)malloc(w * h * c); //stbi_load("pic.jpg", &w, &h, &c, 4);

	void* pixels;
	int pitch;

	SDL_Texture* sdlbuff = SDL_CreateTexture(renderer, SDL_PIXELFORMAT_RGBA32, SDL_TEXTUREACCESS_STREAMING, w, h);

	//std::thread renderThreadf(renderFudge, 0, 0, ScreenWidth / 2, ScreenHeight / 2);
	//std::thread renderThreadg(renderFudge, ScreenWidth / 2, 0, ScreenWidth, ScreenHeight / 2);
	//std::thread renderThreadh(renderFudge, 0, ScreenHeight / 2, ScreenWidth / 2, ScreenHeight);
	//std::thread renderThreadi(renderFudge, ScreenWidth / 2, ScreenHeight / 2, ScreenWidth, ScreenHeight);

	auto cpuCount = std::thread::hardware_concurrency();
	//auto cpuCount = 1;
	if (cpuCount <= 0)
	{
		cpuCount = 1;
	}

	std::vector<std::thread> threads;

	for (int i = 0; i < cpuCount; i++)
	{
		int startX = (ScreenWidth / cpuCount) * i;
		threads.push_back(std::thread(renderFudge, startX, 0, startX + (ScreenWidth / cpuCount), ScreenHeight));
	}

	int frame = 0;

	while (isOpen)
	{
		frame++;
		int mx, my;
		SDL_GetMouseState(&mx, &my);
		//for (unsigned int x = 0; x < w; x++)
		//{
		//	for (unsigned int y = 0; y < h; y++)
		//	{
		//		pixelToImage(picture, w, h, { (uint8_t)(x + frame * 5), (uint8_t)(y + frame * 5), 0}, x, y);
		//	}
		//}
		//pixelToImage(picture, w, h, { 0, 32, 0 }, (w / 2) + (sin(frame) * 50), (h / 2) + (cos(frame) * 50));

		float move = zoom * 0.1;

		bool screenshot = false;

		//std::cout << frame << std::endl;
		SDL_Event ev;
		while (SDL_PollEvent(&ev) != 0)
		{
			switch (ev.type)
			{
			case SDL_QUIT:
				isOpen = false;
				break;
			case SDL_KEYDOWN:
				switch (ev.key.keysym.sym)
				{
				case SDLK_z:
					zoom *= 1.1;
					break;
				case SDLK_x:
					zoom *= 0.9;
					break;
				case SDLK_UP:
					camY -= move;
					break;
				case SDLK_DOWN:
					camY += move;
					break;
				case SDLK_RIGHT:
					camX += move;
					break;
				case SDLK_LEFT:
					camX -= move;
					break;
				case SDLK_F2:
					screenshot = true;
				}
				break;
			}
		}
		SDL_RenderClear(renderer);
		SDL_LockTexture(sdlbuff, NULL, &pixels, &pitch);

		memcpy(pixels, picture, w * h * 4);

		if (screenshot)
		{
			stbi_write_png((std::string("frame_") + std::to_string(frame) + std::string("_screenshot.png")).c_str(), w, h, 4, picture, w * 4);
			screenshot = false;
		}

		SDL_UnlockTexture(sdlbuff);
		SDL_RenderCopy(renderer, sdlbuff, NULL, NULL);

		SDL_RenderPresent(renderer);

		//std::vector<std::thread> threadVec;

		//int threads = 10;

		//for (int yt = 0; yt < threads; yt++)
		//{
		//	for (int xt = 0; xt < threads; xt++)
		//	{
		//		threadVec.push_back(std::thread(threadedRenderCopy, picture, pixels, frame, w, h, w / (xt / threads), h / (yt / threads), w / ((xt + 1) / threads), h / ((yt + 1) / threads)));
		//	}
		//}

		//for (int t = 0; t < threadVec.size(); t++)
		//{
		//	threadVec[t].join();
		//}

		//memcpy(pixels, picture, w * h * 4);
	}
	for (int i = 0; i < threads.size(); i++)
	{
		threads[i].join();
	}

	return 0;
}