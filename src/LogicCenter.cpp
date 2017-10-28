
#include "LogicCenter.h"

namespace tjg {
    LogicCenter::LogicCenter(ResourceManager &resource_manager) :
            resource_manager(resource_manager),
            physics_system(),
            collision_center(physics_system.GetSpace()),
            entity_factory(resource_manager, physics_system) {
    }

    void LogicCenter::Initialize() {
        tech17 = entity_factory.MakeTech17();
        control_center.AddEntity(tech17);

        entrance = entity_factory.MakeEntrance(sf::Vector2f(0, 0));
        exit = entity_factory.MakeExit(sf::Vector2f(-1400, -200));

        event_manager.RegisterListener<ReachedExit>([&](ReachedExit &event){
            (void)event;
            game_state = State::WON;
        });

        event_manager.RegisterListener<HitWall>([&](HitWall &event){
            (void)event;
            std::cout << "TECH17 just died." << std::endl;
            game_state = State::FAILED;
        });

        event_manager.RegisterListener<TimeExpired>([&](TimeExpired &event) {
            (void)event;
            std::cout << "Time's up! Consider to restart this level." << std::endl;
            game_state = State::FAILED;
        });

        // Create boundary walls using the entity factory.
        auto top_wall = entity_factory.MakeWall(sf::Vector2f(-1540, -600), sf::Vector2f(540, -600), 80);
        auto bottom_wall = entity_factory.MakeWall(sf::Vector2f(-1540, 600), sf::Vector2f(540, 600), 80);
        auto left_wall = entity_factory.MakeWall(sf::Vector2f(-1500, -600), sf::Vector2f(-1500, 600), 80);
        auto right_wall = entity_factory.MakeWall(sf::Vector2f(500, -600), sf::Vector2f(500, 600), 80);

        // Add the walls to the entities vector.
        walls.push_back(top_wall);
        walls.push_back(bottom_wall);
        walls.push_back(left_wall);
        walls.push_back(right_wall);

        // Create fans and add them to the fans vector.
        // Top fan above span
        fans.push_back(entity_factory.MakeFan(sf::Vector2f(0, -600), sf::Vector2f(0, 600), 200, 250.0f, 0.f));
        // Bottom fan below spawn
        fans.push_back(entity_factory.MakeFan(sf::Vector2f(0, 600), sf::Vector2f(0, -600), 200, 250.0f, 0.f));
        // Top fan to right of spawn.
        fans.push_back(entity_factory.MakeFan(sf::Vector2f(300, -600), sf::Vector2f(300, 600), 200, 250.0f, 0.f));
        // Right fan below spawn.
        fans.push_back(entity_factory.MakeFan(sf::Vector2f(500, 400), sf::Vector2f(-1500, 400), 200, 250.0f, 50.f));
        // Bottom fan near exit.
        fans.push_back(entity_factory.MakeFan(sf::Vector2f(-1000, 600), sf::Vector2f(-1000, -600), 200, 250.0f, 0.f));
        // Top fan near exit.
        fans.push_back(entity_factory.MakeFan(sf::Vector2f(-1300, -600), sf::Vector2f(-1300, 600), 200, 300.0f, 0.f));

        // Create a collision center handler that will fire a HitWall event when TECH17 hits a wall.
        collision_center.AddHandler(
            CollisionGroup::TECH17,
            CollisionGroup::WALL,
            [&](cpArbiter *arb, cpSpace *space) {
                (void)arb;
                (void)space;
                event_manager.Fire<HitWall>();
            }
        );

        // Check if Tech17's chest overlaps with the exit door
        collision_center.AddHandler(
            CollisionGroup::TECH17,
            CollisionGroup::EXIT,
            [&](cpArbiter *arb, cpSpace *space) {
                (void)arb;
                (void)space;
                event_manager.Fire<ReachedExit>();
            }
        );

    }

    void LogicCenter::Update(const sf::Time elapsed) {

        physics_system.Update(elapsed);

        // Countdown timer - start counting. The reason of fairness, do not start to count during initialization.  The current countdown timer is not implemented as an entity. Since there is only one unphysical timer for player and we are using the hybrid event system, I think it might be okay this way.                  
        time_countdown = countdown_clock.getElapsedTime();
        remaining_seconds = max_countdown > time_countdown.asSeconds() ? static_cast<unsigned int>(max_countdown - time_countdown.asSeconds()) : 0;        
      
        if (!remaining_seconds){            
            event_manager.Fire<TimeExpired>();
        }

    }


    void LogicCenter::Reset() {
        physics_system.Reset();
        collision_center.Reset(physics_system.GetSpace());
        game_state = State::PLAYING;
        remaining_seconds = max_countdown;
        countdown_clock.restart();
    }


    State LogicCenter::GetGameState() {
        return game_state;
    }

    std::shared_ptr<Entity> LogicCenter::GetTech17() {
        return tech17;
    }

    std::vector<std::shared_ptr<Entity>>& LogicCenter::GetWalls() {
        return walls;
    }

    std::shared_ptr<Entity> LogicCenter::GetEntrance() {
        return entrance;
    }

    std::shared_ptr<Entity> LogicCenter::GetExit() {
        return exit;
    }

    std::vector<std::shared_ptr<Entity>>& LogicCenter::GetFans() {
        return fans;
    }

    EntityFactory& LogicCenter::GetEntityFactory() {
        return entity_factory;
    }

    ControlCenter& LogicCenter::GetControlCenter() {
        return control_center;
    }

    unsigned int LogicCenter::GetRemainingSeconds() {
        return remaining_seconds;
    }
}
