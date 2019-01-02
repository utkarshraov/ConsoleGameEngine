#pragma once
#include "ConsoleGameEngine.h"
using namespace std;

class PerlinNoise : public ConsoleGameEngine
{
private:
	float *noiseSeed1D = nullptr;
	float *perlinNoise1D = nullptr;
	float *noiseSeed2D = nullptr;
	float *perlinNoise2D = nullptr;
	int outputSize = 256;
	int outputWidth = 256;
	int outputHeight = 256;
	int octaveCount = 1;
	float scalingBias = 2.0f;

	int mode = 1;
public:
	virtual bool Start()
	{
		outputSize = windowWidth;
		outputHeight = windowHeight;
		outputWidth = windowWidth;
		noiseSeed1D = new float[outputSize];
		perlinNoise1D = new float[outputSize];
		noiseSeed2D = new float[outputHeight * outputWidth];
		perlinNoise2D = new float[outputHeight * outputWidth];

		for (int i = 0; i < outputSize; i++) noiseSeed1D[i] = (float)rand() / (float)RAND_MAX;

		for (int i = 0; i < outputWidth * outputHeight; i++) noiseSeed2D[i] = (float)rand() / (float)RAND_MAX;

		return true;
	}

	virtual bool Update(float elapsedTime)
	{
		if (keys[VK_SPACE].onReleased)
			octaveCount++;
		if (octaveCount == 9)
			octaveCount = 1;
		if(keys[L'Z'].onReleased)
			for (int i = 0; i < outputSize; i++) noiseSeed1D[i] = (float)rand() / (float)RAND_MAX;
		if (keys[L'Q'].onReleased)
			scalingBias += 0.2f;
		if (keys[L'A'].onReleased)
			scalingBias -= 0.2f;
		if (scalingBias < 0.2f)
			scalingBias = 0.2f;

		if (keys[L'1'].onReleased)
			mode = 1;
		if (keys[L'2'].onReleased)
			mode = 2;
		if (keys[L'3'].onReleased)
			mode = 3;
		Fill(0, 0, windowWidth, windowHeight, ' ');

		if (mode == 1)
		{
			if (keys[L'Z'].onReleased)
				for (int i = 0; i < outputSize; i++) noiseSeed1D[i] = (float)rand() / (float)RAND_MAX;
			PerlinNoise1D(outputSize, noiseSeed1D, octaveCount, scalingBias, perlinNoise1D);

			for (int x = 0; x < outputSize; x++)
			{
				int f = -(perlinNoise1D[x] * (float)windowHeight / 2.0f) + (float)windowHeight / 2.0f;
				for (int y = f; y < windowHeight / 2; y++)
					Draw(x, y, PIXEL_SOLID, FG_BLUE);
			}
		}
		else if (mode == 2)
		{
			if (keys[L'Z'].onReleased)
				for (int i = 0; i < outputWidth * outputHeight; i++) noiseSeed2D[i] = (float)rand() / (float)RAND_MAX;
			PerlinNoise2D(outputWidth, outputHeight, noiseSeed2D, octaveCount, scalingBias, perlinNoise2D);

			for (int x = 0; x < outputWidth; x++)
			{
				for (int y = 0; y < outputHeight; y++)
				{
					short bgColour, fgColour;
					wchar_t sym;
					int pixelBW = (int)(perlinNoise2D[y*outputWidth + x] * 16.0f);
					switch (pixelBW)
					{
					case 0: bgColour = BG_DARK_BLUE; fgColour = FG_DARK_BLUE; sym = PIXEL_SOLID; break;

					case 1: bgColour = BG_DARK_BLUE; fgColour = FG_BLUE; sym = PIXEL_QUARTER; break;
					case 2: bgColour = BG_DARK_BLUE; fgColour = FG_BLUE; sym = PIXEL_HALF; break;
					case 3: bgColour = BG_DARK_BLUE; fgColour = FG_BLUE; sym = PIXEL_THREEQUARTERS; break;
					case 4: bgColour = BG_DARK_BLUE; fgColour = FG_BLUE; sym = PIXEL_SOLID; break;

					case 5: bgColour = BG_BLUE; fgColour = FG_GREEN; sym = PIXEL_QUARTER; break;
					case 6: bgColour = BG_BLUE; fgColour = FG_GREEN; sym = PIXEL_HALF; break;
					case 7: bgColour = BG_BLUE; fgColour = FG_GREEN; sym = PIXEL_THREEQUARTERS; break;
					case 8: bgColour = BG_BLUE; fgColour = FG_GREEN; sym = PIXEL_SOLID; break;

					case 9:  bgColour = BG_GREEN; fgColour = FG_DARK_GREY; sym = PIXEL_QUARTER; break;
					case 10: bgColour = BG_GREEN; fgColour = FG_DARK_GREY; sym = PIXEL_HALF; break;
					case 11: bgColour = BG_GREEN; fgColour = FG_DARK_GREY; sym = PIXEL_THREEQUARTERS; break;
					case 12: bgColour = BG_GREEN; fgColour = FG_DARK_GREY; sym = PIXEL_SOLID; break;

					case 13: bgColour = BG_DARK_GREY; fgColour = FG_WHITE; sym = PIXEL_QUARTER; break;
					case 14: bgColour = BG_DARK_GREY; fgColour = FG_WHITE; sym = PIXEL_HALF; break;
					case 15: bgColour = BG_DARK_GREY; fgColour = FG_WHITE; sym = PIXEL_THREEQUARTERS; break;
					case 16: bgColour = BG_DARK_GREY; fgColour = FG_WHITE; sym = PIXEL_SOLID; break;
					}
					Draw(x, y, sym, fgColour | bgColour);
				}
			}
			
		}
		else if (mode == 3)
		{
			if (keys[L'Z'].onReleased)
				for (int i = 0; i < outputWidth * outputHeight; i++) noiseSeed2D[i] = (float)rand() / (float)RAND_MAX;
			PerlinNoise2D(outputWidth, outputHeight, noiseSeed2D, octaveCount, scalingBias, perlinNoise2D);

			for (int x = 0; x < outputWidth; x++)
			{
				for (int y = 0; y < outputHeight; y++)
				{
					short bgColour, fgColour;
					wchar_t sym;
					int pixelBW = (int)(perlinNoise2D[y*outputWidth + x] * 12.0f);
					switch (pixelBW)
					{
					case 0: bgColour = BG_BLACK; fgColour = FG_BLACK; sym = PIXEL_SOLID; break;

					case 1: bgColour = BG_BLACK; fgColour = FG_DARK_GREY; sym = PIXEL_QUARTER; break;
					case 2: bgColour = BG_BLACK; fgColour = FG_DARK_GREY; sym = PIXEL_HALF; break;
					case 3: bgColour = BG_BLACK; fgColour = FG_DARK_GREY; sym = PIXEL_THREEQUARTERS; break;
					case 4: bgColour = BG_BLACK; fgColour = FG_DARK_GREY; sym = PIXEL_SOLID; break;

					case 5: bgColour = BG_DARK_GREY; fgColour = FG_GREY; sym = PIXEL_QUARTER; break;
					case 6: bgColour = BG_DARK_GREY; fgColour = FG_GREY; sym = PIXEL_HALF; break;
					case 7: bgColour = BG_DARK_GREY; fgColour = FG_GREY; sym = PIXEL_THREEQUARTERS; break;
					case 8: bgColour = BG_DARK_GREY; fgColour = FG_GREY; sym = PIXEL_SOLID; break;

					case 9:  bgColour = BG_GREY; fgColour = FG_WHITE; sym = PIXEL_QUARTER; break;
					case 10: bgColour = BG_GREY; fgColour = FG_WHITE; sym = PIXEL_HALF; break;
					case 11: bgColour = BG_GREY; fgColour = FG_WHITE; sym = PIXEL_THREEQUARTERS; break;
					case 12: bgColour = BG_GREY; fgColour = FG_WHITE; sym = PIXEL_SOLID; break;
					}
					Draw(x, y, sym, fgColour | bgColour);
				}
			}

		}

		return true;
	}

	void PerlinNoise1D(int count, float *seed, int octaves, float bias,float *output)
	{
		for (int i = 0; i < count; i++)
		{
			float noise = 0.0f;
			float scale = 1.0f;
			float scaleAccumalator = 0.0f;
			for (int o = 0; o < octaves; o++)
			{
				int pitch = count >> o;
				int sample1 = (i / pitch) * pitch; //leverage integer division
				int sample2 = (sample1 + pitch) % count;

				float blend = (float)(i - sample1) / (float)pitch;
				float sample = (1.0f - blend) * seed[sample1] + blend * seed[sample2];
				noise += sample * scale;
				scaleAccumalator += scale;
				scale = scale / bias;
			}
			output[i] = noise / scaleAccumalator;
		}
	}

	void PerlinNoise2D(int width, int height, float *seed, int octaves, float bias, float *output)
	{
		for (int i = 0; i < width; i++)
		{
			for (int j = 0; j < height; j++) {
				float noise = 0.0f;
				float scale = 1.0f;
				float scaleAccumalator = 0.0f;
				for (int o = 0; o < octaves; o++)
				{
					int pitch = width >> o;
					int samplex1 = (i / pitch) * pitch;
					int sampley1 = (j / pitch) * pitch;//leverage integer division

					int samplex2 = (samplex1 + pitch) % width;
					int sampley2 = (sampley1 + pitch) % width;

					float blendX = (float)(i - samplex1) / (float)pitch;
					float blendY = (float)(j - sampley1) / (float)pitch;

					float sampleT = (1.0f - blendX) * seed[sampley1 * width + samplex1] + blendX * seed[sampley1 * width + samplex1];
					float sampleB = (1.0f - blendX) * seed[sampley2 * width + samplex1] + blendX * seed[sampley2 * width + samplex1];

					noise += (blendY * (sampleB - sampleT) + sampleT) * scale;
					scaleAccumalator += scale;
					scale = scale / bias;
				}
				output[j*width + i] = noise / scaleAccumalator;
			}
		}
	}
};
