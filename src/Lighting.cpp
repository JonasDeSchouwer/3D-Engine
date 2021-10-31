#include "Lighting.h"

ColorArray::ColorArray(int width_, int height_)
{
    width = width_;
    height = height_;

    arr = new sf::Uint8**[height];
    for (int i=0; i<height; i++)
    {
        arr[i] = new sf::Uint8*[width];
        for (int j=0; j<width; j++)
        {
            arr[i][j] = new sf::Uint8[4];
        }
    }
}

ColorArray::~ColorArray()
{
    for (int i=0; i<height; i++)
    {
        for (int j=0; j<width; j++)
        {
            delete[] arr[i][j];
        }
        delete[] arr[i];
    }
    delete[] arr;
}

ColorVector::ColorVector(sf::Uint8 r, sf::Uint8 g, sf::Uint8 b, sf::Uint8 a, float x, float y, float z)
{
    color = new sf::Uint8[4];
    color[0] = r;
    color[1] = g;
    color[2] = b;
    color[3] = a;

    dir_incidence << x, y, z;
}
ColorVector::ColorVector(sf::Color c, float x, float y, float z)
{
    color = new sf::Uint8[4];
    color[0] = c.r;
    color[1] = c.g;
    color[2] = c.b;
    color[3] = c.a;

    dir_incidence << x, y, z;
}

ColorVector::~ColorVector()
{
    delete[] color;
}


LightSource::LightSource()
{
    //ctor
}

LightSource::~LightSource()
{
    //dtor
}
