#include "Application.h"

Application::Application():
	scene(), display(scene)
{

}

void Application::eventLoop()
{
	sf::Event e;

	while (display.window().isOpen())
	{
		while (display.window().pollEvent(e))
		{
			if (e.type == sf::Event::Closed)
				display.window().close();
			if (e.type == sf::Event::MouseWheelMoved)
			{
				if (sf::Keyboard::isKeyPressed(sf::Keyboard::LControl))
					display.zoom(e.mouseWheel.delta);
				else
					scene.scrollSelectedTile(e.mouseWheel.delta);
			}
			if (e.type == sf::Event::MouseButtonPressed)
			{
				if (e.key.code == sf::Mouse::Right)
				{
					scene.rightClick(display.ptc(sf::Mouse::getPosition(display.window())));
				}
				else if (e.key.code == sf::Mouse::Left)
				{
					scene.leftClick(display.ptc(sf::Mouse::getPosition(display.window())));
				}
			}
		}
		display.print();
		scene.tick();
	}
}
