#include <SFML\Graphics.hpp>
#include <iostream>
#include <random>
#include <vector>
#include <ctime>

const int KPOINTS = 3;
std::clock_t start;
double clockDuration;

class KPoints;
class Pixels
{
public:
	void setXY(int _x, int _y);
	void setRGB(sf::Color _color);
	int getX();
	int getY();
	int getR();
	int getG();
	int getB();
	int distanceToKPoint(KPoints _kPoint);

private:
	int x, y; //Position in image
	int r, g, b; //Colors/position in 3D graph
};

class KPoints
{
public:
	KPoints();
	void newPixel(int _pixelIndex);
	void clearPixels();
	void recalculatePosition(Pixels *_pixel);
	void colorImage(sf::Image &image, int _imageSizeX);
	int getR();
	int getG();
	int getB();
	void reservePixels(int _n);

private:
	int r, g, b;
	std::vector<int> kPixels;
};



int main()
{
	srand(time(0));
	sf::Image inputImage, outputImage;
	if (!inputImage.loadFromFile("D:\\Images\\holdon.jpg")) { return -1; }
	sf::Vector2u imageSize;
	imageSize = inputImage.getSize();
	outputImage.create(imageSize.x, imageSize.y);

	KPoints *kPoint = new KPoints[KPOINTS];
	Pixels *pixel = new Pixels[imageSize.x * imageSize.y];
	for (unsigned int i = 0; i < imageSize.x * imageSize.y; i++) //Initialize pixels
	{
		pixel[i].setXY(i % imageSize.x, i / imageSize.x);
		pixel[i].setRGB(inputImage.getPixel(i % imageSize.x, i / imageSize.x));
	}
	for (unsigned int i = 0; i < imageSize.x * imageSize.y; i++)
	{
		outputImage.setPixel(i % imageSize.x, i / imageSize.x, sf::Color(pixel[i].getR(), pixel[i].getG(), pixel[i].getB()));
	}
	sf::Texture texture;
	texture.loadFromImage(outputImage);
	sf::Sprite sprite(texture);

	sf::RenderWindow window(sf::VideoMode(imageSize.x, imageSize.y), "window");
	while (window.isOpen())
	{
		sf::Event event;
		while (window.pollEvent(event))
		{
			if (event.type == sf::Event::Closed)
			{
				window.close();
			}
			if (event.type == sf::Event::KeyPressed)
			{
				if (event.key.code == sf::Keyboard::Return)
				{
					for (int i = 0; i < KPOINTS; i++)
					{
						kPoint[i].reservePixels(imageSize.x * imageSize.y);
					}
					start = std::clock();
					for (int i = 0; i < (int)(imageSize.x * imageSize.y); i++)
					{
						int closest = INT_MAX;
						int closestPoint = 0;
						for (int j = 0; j < KPOINTS; j++)
						{
							int distance = pixel[i].distanceToKPoint(kPoint[j]);
							if (distance < closest)
							{
								closest = distance;
								closestPoint = j;
							}
						}
						kPoint[closestPoint].newPixel(i);
						if (i == 10000)
						{
							clockDuration = (std::clock() - start) / (double)CLOCKS_PER_SEC;
							std::cout << "Spent " << clockDuration << " on 10000 pixels\n";
						}
						if (i == 100000)
						{
							clockDuration = (std::clock() - start) / (double)CLOCKS_PER_SEC;
							std::cout << "Spent " << clockDuration << " on 100000 pixels\n";
						}
					}
					clockDuration = (std::clock() - start) / (double)CLOCKS_PER_SEC;
					std::cout << "Spent " << clockDuration << " to complete the image (" << imageSize.x *imageSize.y << " pixels)\n";
					for (int i = 0; i < KPOINTS; i++)
					{
						kPoint[i].recalculatePosition(pixel);
						kPoint[i].colorImage(outputImage, imageSize.x);
					}
					texture.loadFromImage(outputImage);
					for (int i = 0; i < KPOINTS; i++)
					{
						kPoint[i].clearPixels();
					}
				}
			}
		}
		window.clear();
		window.draw(sprite);
		window.display();
	}
	return 0;
}

void Pixels::setXY(int _x, int _y)
{
	x = _x;
	y = _y;
}

void Pixels::setRGB(sf::Color _color)
{
	r = _color.r;
	g = _color.g;
	b = _color.b;
}

int Pixels::getX()
{
	return x;
}

int Pixels::getY()
{
	return y;
}

int Pixels::getR()
{
	return r;
}

int Pixels::getG()
{
	return g;
}

int Pixels::getB()
{
	return b;
}

int Pixels::distanceToKPoint(KPoints _kPoint)
{
	return ((r - _kPoint.getR())*(r - _kPoint.getR())
		+ (g - _kPoint.getG())*(g - _kPoint.getG())
		+ (b - _kPoint.getB())*(b - _kPoint.getB()));
}


KPoints::KPoints()
{
	//Initialize position
	r = rand() % 255;
	g = rand() % 255;
	b = rand() % 255;
}

void KPoints::newPixel(int _pixelIndex)
{
	kPixels.push_back(_pixelIndex);
}

void KPoints::clearPixels()
{
	kPixels.clear();
}

void KPoints::recalculatePosition(Pixels *_pixel)
{
	float temp = 0;
	for (int i = 0; i < kPixels.size(); i++)
	{
		temp = temp + (_pixel[kPixels[i]].getR() - temp) / (i + 1);
	}
	r = (int)temp;
	temp = 0;
	for (int i = 0; i < kPixels.size(); i++)
	{
		temp = temp + (_pixel[kPixels[i]].getG() - temp) / (i + 1);
	}
	g = (int)temp;
	temp = 0;
	for (int i = 0; i < kPixels.size(); i++)
	{
		temp = temp + (_pixel[kPixels[i]].getB() - temp) / (i + 1);
	}
	b = (int)temp;
}

void KPoints::colorImage(sf::Image &_image, int _imageSizeX)
{
	for (int i = 0; i < kPixels.size(); i++)
	{
		_image.setPixel(kPixels[i] % _imageSizeX, kPixels[i] / _imageSizeX, sf::Color(r, g, b));
	}
}

int KPoints::getR()
{
	return r;
}

int KPoints::getG()
{
	return g;
}

int KPoints::getB()
{
	return b;
}

void KPoints::reservePixels(int _n)
{
	kPixels.reserve(_n);
}