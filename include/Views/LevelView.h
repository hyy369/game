#ifndef GAME_LEVELVIEW_H
#define GAME_LEVELVIEW_H

#include <SFML/Graphics.hpp>

#include "Constants.h"
#include "LogicCenter.h"
#include "Systems/DialogueSystem.h"
#include "Systems/SpriteRenderSystem.h"
#include "Systems/ParticleSystem.h"
#include "Systems/PhysicsParticleSystem.h"
#include "View.h"

namespace tjg {

    class LevelView : public View {
    private:

        LogicCenter &logic_center;
        sf::View camera;

        void CheckKeys(const sf::Time &elapsed);

        // Sprite render systems.
        SpriteRenderSystem main_render_system;
        SpriteRenderSystem statusbar_render_system;

        PhysicsParticleSystem dust_particle_system;
        ParticleSystem jetpack_flame_system;

        // FPS display
        sf::Clock fps_clock;
        int fps = 0;
        int frames_drawn = 0;
        sf::Text info;
        bool show_info = false;

        // Status bar pieces.
        sf::RectangleShape statusbar_background;
        sf::RectangleShape fuel_tank_background;
        sf::RectangleShape oxygen_tank_background;
        sf::RectangleShape dialog_background;
        float statusbar_element_height;
        float statusbar_x_padding;
        float statusbar_y_padding;
        sf::Vector2f trackers_initial_size;
        sf::Vector2f dialog_initial_size;

        // Status bar methods.
        void InitializeStatusBar();
        void RenderStatusBarBackground();
        void UpdateStatusBarTrackers();

        // Dialog box pieces.
        DialogueSystem dialogue_system;

        // Dialog box methods.
        void InitializeDialogueSystem(std::vector<Dialogue> &dialogues, std::shared_ptr<sf::Font> font);

    public:
        // Constructor
        explicit LevelView(ResourceManager &resource_manager, sf::RenderWindow &window, LogicCenter &logic_center);

        // Methods
        void Initialize();
        void Render();

        ViewSwitch HandleWindowEvents(sf::Event event) override;
        void Update(const sf::Time &elapsed);
    };

}

#endif //GAME_LEVELVIEW_H