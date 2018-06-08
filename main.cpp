#include <SFML/Graphics.hpp>

const int TILE_SIZE = 13;
const int TILE_MARGIN = 1;
const int FIELD_WIDTH = 50;
const int FIELD_HEIGHT = 50;
const float STEP = 1 / 6.f;

int main() {
	// SFML variables
	sf::RenderWindow wnd(sf::VideoMode(1024, 800), "Game of Life", sf::Style::Titlebar | sf::Style::Close);
	sf::Event event;
	sf::Clock clock;

	// Game of Life variables
	sf::RectangleShape shape;
	sf::VertexBuffer lines(sf::PrimitiveType::Lines, sf::VertexBuffer::Usage::Static);
	bool field[FIELD_WIDTH][FIELD_HEIGHT] = { false };
	bool fieldCopy[FIELD_WIDTH][FIELD_HEIGHT] = { false };
	float timeAcc = 0.0f;
	bool simulate = false;
	
	// initialize the tile shape
	shape.setSize(sf::Vector2f(TILE_SIZE, TILE_SIZE));
	shape.setFillColor(sf::Color::Cyan);

	// fill vertex buffer with lines
	std::vector<sf::Vertex> verts(FIELD_WIDTH * 2 + FIELD_HEIGHT * 2);
	lines.create(FIELD_WIDTH * 2 + FIELD_HEIGHT * 2);
	
	// vertical lines
	for (int x = 0; x < FIELD_WIDTH; x++) {
		verts[x * 2 + 1].position.x = verts[x * 2].position.x = (TILE_SIZE + TILE_MARGIN * 2 + 1) * x;
		verts[x * 2].position.y = 2;
		verts[x * 2 + 1].position.y = FIELD_HEIGHT * (TILE_SIZE + 2 * TILE_MARGIN + 1) - 2;
		verts[x * 2 + 1].color = verts[x * 2].color = sf::Color(20, 20, 20);
	}

	// horizontal lines
	int vertsOffset = FIELD_WIDTH * 2;
	for (int y = 0; y < FIELD_HEIGHT; y++) {
		verts[vertsOffset + y * 2 + 1].position.y = verts[vertsOffset + y * 2].position.y = (TILE_SIZE + TILE_MARGIN * 2 + 1) * y;
		verts[vertsOffset + y * 2].position.x = 2;
		verts[vertsOffset + y * 2 + 1].position.x = FIELD_WIDTH * (TILE_SIZE + 2 * TILE_MARGIN + 1) - 2;
		verts[vertsOffset + y * 2 + 1].color = verts[vertsOffset + y * 2].color = sf::Color(20, 20, 20);
	}

	// upload data
	lines.update(verts.data());

	while (wnd.isOpen()) {
		while (wnd.pollEvent(event)) {
			if (event.type == sf::Event::Closed)
				wnd.close();
			else if (event.type == sf::Event::KeyPressed) {
				if (event.key.code == sf::Keyboard::R) { // reset everything
					simulate = false;
					timeAcc = 0;
					for (int x = 0; x < FIELD_WIDTH; x++)
						for (int y = 0; y < FIELD_HEIGHT; y++)
							field[y][x] = false;
				}
				else if (event.key.code == sf::Keyboard::Space)
					simulate = !simulate;
			}
		}

		// left click - place, right click - delete
		if (sf::Mouse::isButtonPressed(sf::Mouse::Button::Left) || sf::Mouse::isButtonPressed(sf::Mouse::Right)) {
			sf::Vector2i mpos = sf::Mouse::getPosition(wnd);

			int tx = mpos.x / (TILE_SIZE + TILE_MARGIN * 2 + 1);
			int ty = mpos.y / (TILE_SIZE + TILE_MARGIN * 2 + 1);

			if (!(tx < 0 || tx >= FIELD_WIDTH || ty < 0 || ty >= FIELD_HEIGHT))
				field[ty][tx] = sf::Mouse::isButtonPressed(sf::Mouse::Left);
		}


		timeAcc += clock.restart().asSeconds();
		if (timeAcc > STEP && simulate) { // step once in ${STEP} :D
			// save the state of the field
			for (int x = 0; x < FIELD_WIDTH; x++)
				for (int y = 0; y < FIELD_HEIGHT; y++)
					fieldCopy[y][x] = field[y][x];

			// apply rules
			for (int x = 0; x < FIELD_WIDTH; x++)
				for (int y = 0; y < FIELD_HEIGHT; y++) {
					// neighbor count
					int nCount = 0;
					for (int mx = -1; mx <= 1; mx++)
						for (int my = -1; my <= 1; my++)
							if (!((x + mx) < 0 || (x + mx) >= FIELD_WIDTH || (y + my) < 0 || (y + my) >= FIELD_HEIGHT))
								nCount += fieldCopy[y + my][x + mx] == true;

					// this cells state
					bool& cell = field[y][x];
					nCount -= cell;

					if (nCount == 0 && !cell)
						continue;


					// 1. Any live cell with fewer than two live neighbors dies, as if by under population.
					if (cell && nCount < 2)
						cell = false;

					// 2. Any live cell with more than three live neighbors dies, as if by overpopulation.
					if (cell && nCount > 3)
						cell = false;

					// 3. Any dead cell with exactly three live neighbors becomes a live cell, as if by reproduction.
					if (!cell && nCount == 3)
						cell = true;

					// 4. Any live cell with two or three live neighbors lives on to the next generation.
					// (do nothing)
				}
			timeAcc = 0;
		}




		wnd.clear();

		// this doesnt have good performance (lazy)
		for (int x = 0; x < FIELD_WIDTH; x++)
			for (int y = 0; y < FIELD_HEIGHT; y++) {
				if (!field[y][x]) continue; // skip empty tiles

				shape.setPosition(x*(TILE_SIZE + TILE_MARGIN * 2 + 1) + TILE_MARGIN, y*(TILE_SIZE + TILE_MARGIN * 2 + 1) + TILE_MARGIN);
				wnd.draw(shape);
			}

		// render lines
		wnd.draw(lines);

		wnd.display();
	}

	return 0;
}