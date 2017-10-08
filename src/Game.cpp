#include <iostream>

#include "Game.h"

namespace tjg {

    Game::Game() :
            space(cpSpaceNew()),
            resource_manager("media"),
            entity_factory(resource_manager, sprite_render_system, space),
            window(sf::VideoMode(800, 600, 32), "Game", sf::Style::Titlebar | sf::Style::Close) {

        window.setVerticalSyncEnabled(true);

        cpSpaceSetGravity(space, cpv(0, 0));

    }

    Game::~Game() {
        cpSpaceFree(space);
    }

    void Game::Run() {
        // Load some fonts and a texture sheet
        auto avenir_bold = resource_manager.LoadFont("Avenir-Bold.ttf");
        auto lcd_regular = resource_manager.LoadFont("LCD-Regular.ttf");

        auto texture_sheet = resource_manager.LoadTexture("texturesheet.png");


        // Create boundary walls using the entity factory.
        // TODO: make the Walls use sprites vice line segments, and add them to the SpriteRenderSystem
        entities.push_back(entity_factory.MakeWall(sf::Vector2f(-500, -500), sf::Vector2f(500, -500)));
        entities.push_back(entity_factory.MakeWall(sf::Vector2f(500, -500), sf::Vector2f(500, 500)));
        entities.push_back(entity_factory.MakeWall(sf::Vector2f(500, 500), sf::Vector2f(-500, 500)));
        entities.push_back(entity_factory.MakeWall(sf::Vector2f(-500, 500), sf::Vector2f(-500, -500)));

        tech17 = entity_factory.MakeTech17();

        sprite_render_system.AddEntity(entity_factory.MakeTiledBackground("background.png"), -100);

        sf::Sprite obstacle;
        obstacle.setTexture(*texture_sheet);
        obstacle.setTextureRect(sf::IntRect(1, 1, 160, 150));

        // Create many asteroids
        for (auto i = 0; i < 10; ++i) {
            auto character = std::make_shared<Entity>();
            character->AddComponent<Location>();
            character->AddComponent<Sprite>(obstacle);
            character->AddComponent<DynamicBody>(
                    space,
                    sf::Vector2f(floor(i / 4) * 100 + 50, 100 * (i % 4)),
                    3,
                    sf::Vector2f(obstacle.getGlobalBounds().width, obstacle.getGlobalBounds().height));
            entities.push_back(character);
            sprite_render_system.AddEntity(character);
        }


        info.setFont(*avenir_bold);
        info.setStyle(sf::Text::Bold);
        info.setCharacterSize(24);

        camera.setCenter(0, 0);
        camera.setSize(1600, 1200);

        while (window.isOpen()) {
            Draw();
            Update();
            HandleEvents();
        }

    }

    void Game::HandleEvents() {
        sf::Event event;
        while (window.pollEvent(event)) {
            switch (event.type) {
                case sf::Event::Closed:
                    window.close();
                    break;
                case sf::Event::KeyPressed: {

                    switch (event.key.code) {
                        case sf::Keyboard::Escape:
                            window.close();
                            break;
                        case sf::Keyboard::F1:
                            show_info = !show_info;
                            break;

                        default:
                            break;
                    }
                    break;
                }
                default:
                    break;
            }
        }
    }

    void Game::Update() {
        sf::Time elapsed = clock.restart();


        // Temporary/Example control system.
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::Left))
            cpBodyApplyForceAtWorldPoint(tech17->GetComponent<DynamicBody>()->GetBody(), cpv(-1000, 0), cpv(0, 0));
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::Right))
            cpBodyApplyForceAtWorldPoint(tech17->GetComponent<DynamicBody>()->GetBody(), cpv(1000, 0), cpv(0, 0));
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::Up))
            cpBodyApplyForceAtWorldPoint(tech17->GetComponent<DynamicBody>()->GetBody(), cpv(0, -1000), cpv(0, 0));
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::Down))
            cpBodyApplyForceAtWorldPoint(tech17->GetComponent<DynamicBody>()->GetBody(), cpv(0, 1000), cpv(0, 0));


        //
        // Update all of the entities' components
        //
        for (auto entity : entities) {
            auto rigidbody = entity->GetComponent<DynamicBody>();
            if (rigidbody) rigidbody->Update();
        }

        tech17->GetComponent<DynamicBody>()->Update();
        tech17->ForEachChild([](std::shared_ptr<Entity> child) {
            auto r = child->GetComponent<DynamicBody>();
            if (r) r->Update();
        });


        //
        // Example of moving the camera location
        //
        camera.setCenter(camera.getCenter() * 0.99f + tech17->GetComponent<Location>()->position * 0.01f);

        // Perform the physical simulation based on SFML timer elapsed time.
        cpSpaceStep(space, elapsed.asSeconds() * 2);
    }

    void Game::Draw() {
        window.clear(sf::Color(50, 50, 50, 255));
        window.setView(camera);

        // Drawing to the camera view
        for (auto &entity : entities) {
            auto lineComponent = entity->GetComponent<Line>();
            if (lineComponent) lineComponent->Render(window);

            auto rectComponent = entity->GetComponent<RectangleShape>();
            if (rectComponent) rectComponent->Render(window);
        }

        sprite_render_system.render(window);

        tech17->ForEachChild([&](std::shared_ptr<Entity> child) {
            auto r = child->GetComponent<RectangleShape>();
            if (r) {
                r->Render(window);
            }
        });

        // Drawing that should take place separate from the "camera" should go below here.
        window.setView(window.getDefaultView());

        if (show_info) {
            info.setString(std::to_string(fps) + " FPS");
            window.draw(info);
        }

        window.display();

        // Count FPS
        frames_drawn++;
        if (fps_clock.getElapsedTime().asSeconds() > 1.0f) {
            fps_clock.restart();
            fps = frames_drawn;
            frames_drawn = 0;
        }
    }
}