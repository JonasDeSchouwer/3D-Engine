#ifndef LIGHTING_H
#define LIGHTING_H

#include <SFML/Graphics.hpp>
#include <Eigen/Core>

class ColorArray
{
public:
    int width, height;
    sf::Uint8*** arr;

    ColorArray(int width_, int height_);
    virtual ~ColorArray();
};

class ColorVector
{
    /*
    A class containing a color and a vector,
    can be used by light sources to encode the lighting in a particular point in space
    */
public:
    sf::Uint8* color;           //array containing the r,g,b,a values (each in sf::Uint8 format)
    Eigen::RowVector3f dir_incidence; //vector oriented towards the light source

    ColorVector(sf::Uint8 r, sf::Uint8 g, sf::Uint8 b, sf::Uint8 a, float x, float y, float z);
    ColorVector(sf::Color c, float x, float y, float z);
    virtual ~ColorVector();
};


class LightSource
{
public:
    LightSource();
    virtual ~LightSource();

protected:

private:
};

#endif // LIGHTING_H
