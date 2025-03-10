#include "Application.h"

Application::Application():
	scene(), display(scene)
{

}

void Application::eventLoop()
{
	sf::Event e;


	sf::Vector2i mousePos;
	bool moving = false;
	while (display.window().isOpen())
	{
		while (display.window().pollEvent(e))
		{
			switch (e.type)
			{
			case sf::Event::Closed:
				display.window().close();
				break;
			case sf::Event::Resized:
				display.resize();
				break;
			case sf::Event::MouseWheelMoved:
				if (sf::Keyboard::isKeyPressed(sf::Keyboard::LControl))
					display.zoom(e.mouseWheel.delta);
				else
					scene.scrollSelectedTile(e.mouseWheel.delta);
				break;
			case sf::Event::MouseButtonPressed:
				if (e.key.code == sf::Mouse::Right)
				{
					scene.rightClick(display.ptc(sf::Mouse::getPosition(display.window())));
				}
				else if (e.key.code == sf::Mouse::Left)
				{
					if (sf::Keyboard::isKeyPressed(sf::Keyboard::LControl))
						scene.leftClick(display.ptc(sf::Mouse::getPosition(display.window())));
					else
						moving = true;
				}
				break;
			case sf::Event::MouseMoved:
				if (moving && sf::Mouse::isButtonPressed(sf::Mouse::Left))
				{
					auto mouseOffset = mousePos - sf::Mouse::getPosition(display.window());
					display.move(mouseOffset);
				}
				mousePos = sf::Mouse::getPosition(display.window());
				break;
			case sf::Event::KeyPressed:
				switch (e.key.code)
				{
				case sf::Keyboard::LControl:
					moving = false;
				default: break;
				}
				break;
			}
		}
		display.print();
		scene.tick();
	}
}
