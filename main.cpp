#include <iostream>
#include <deque>

#include <Eigen/Dense>
#include <SFML/Graphics.hpp>

#include "Object.h"
#include "Camera.h"
#include "timer.h"
#include "windows.h"
#include "Tests.h"
#include "Lighting.h"

void getCurrentDirectory()
{
    char pBuf[256];
    size_t len = sizeof(pBuf);
    int bytes = GetModuleFileName(NULL, pBuf, len);
    //std::cout << bytes ? bytes : -1;
    std::cout << pBuf << std::endl;
}

int main()
{
    Object O;
    //O.load("cube");
    Sphere S (100, 10, 8);
    std::deque<Object> myObjects;
    myObjects.push_back(S);

    int width = 1000;
    int height = 800;
    Camera C (width, height, 0,0,-500, myObjects);
    sf::RenderWindow window (sf::VideoMode(width,height), "3D");

    Timer myTimer;
    double dt = 0;

    while (window.isOpen())
    {
        sf::Event event;
        while (window.pollEvent(event))
        {
            if (event.type == sf::Event::Closed)
            {
                window.close();
            }
            else if (event.type == sf::Event::KeyPressed)
            {
                switch (event.key.code)
                {
                case sf::Keyboard::Escape:
                    window.close();
                    break;

                default:
                    break;
                }
            }
        }

        if (sf::Keyboard::isKeyPressed(sf::Keyboard::Up))
        {
            C.strafe(0,0,dt*C.speed);
        }
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::Down))
        {
            C.strafe(0,0,-dt*C.speed);
        }
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::Right))
        {
            C.strafe(-dt*C.speed,0,0);      //negative because the x-as points leftwards
        }
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::Left))
        {
            C.strafe(dt*C.speed,0,0);       //positive because the x-as points leftwards
        }
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::U))
        {
            C.strafe(0,dt*C.speed,0);
        }
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::D))
        {
            C.strafe(0,-dt*C.speed,0);
        }

        sf::Vector2i mouse_pos = sf::Mouse::getPosition(window);
        int dx = mouse_pos.x - width/2;
        int dy = mouse_pos.y - height/2;
        sf::Mouse::setPosition(sf::Vector2i(width/2, height/2), window);
        C.rotateAngle(0.001*dx, -0.001*dy);

        C.display();
        window.draw(sf::Sprite(C.render_texture.getTexture()));
        window.display();

        dt = myTimer.tick();
    }

    int n;
    std::cin >> n;
    return 0;
}
