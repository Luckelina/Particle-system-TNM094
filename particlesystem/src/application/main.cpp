#include <tracy/Tracy.hpp>
#include <rendering/window.h>
#include <particlesystem/particlesystem.h>

#include <cmath>
#include <cstdlib>
#include <vector>

#include <fmt/format.h>

// Simple random functions to get started
// Random float (0,1)
float rnd() { return rand() / static_cast<float>(RAND_MAX); }

// Random float (-1,1)
float srnd() { return rnd() * 2.0f - 1.0f; }

int main(int, char**) try {
    rendering::Window window("Particle System v0.0.1 pre-release alpha", 850, 850);


    //Variables to use in loop
    double prevTime = 0.0;
    float speed = 1.0f;
    float idk = 1.0f;
    bool running = true;

    //Containers to store information about each particle to draw.
    std::vector<glm::vec2> positions;
    std::vector<float> sizes;
    std::vector<glm::vec4> colors;


    //Create your particle system, then create and add Emitters and Effects to it.
    Particlesystem ps;

    /* Emitter e1 = Emitter();
    e1.set_position({-0.5, 0.0});
    e1.set_particleLifeTime(5.0);
    e1.set_power(80);
    e1.set_randomDistribution(true);
    e1.set_spawnRate(0.05);  // Whole numbers or decimals
    ps.add_emitter(e1);*/

    Dot e2 = Dot();
    e2.set_size(10);
    //e2.set_position({0.5, 0.0});
    //e2.set_particleLifeTime(8.0);
    //e2.set_power(80);
    e2.set_randomDistribution(true);
    ////e2.set_directions({10, 171});
    e2.set_amountToSpawn(1);
    ////e2.set_spaceBetweenParticles(180);
    //e2.set_spawnRate(0.3); //Whole numbers or decimals
    ps.add_emitter(e2);

    
    Rectangular r1 = Rectangular();
    r1.set_position({0.0, 0.9});
    r1.set_dimensions({2.0, 0.1});
    r1.set_direction(270);
    
    ps.add_emitter(r1);

    //Wind w = Wind({0.5, 0.0}, 0.4, 1, {-2, 0});
    //ps.add_effect(w);
    //
    //GravityWell gw = GravityWell({0, 0.5}, -2, 0.3);
    //ps.add_effect(gw);



    /* Effect ef = Effect({0.0, 0.5});
    ef.set_power(-5);
    ef.set_radius(0.5);
    ps.add_effect(ef);*/
    

    float copydt = 0;
    while (running) {

        positions.clear();
        sizes.clear();
        colors.clear();

        window.beginFrame();

        

        const double t = window.time();
        const double dt = t - prevTime;
        prevTime = t;

        

        
        ps.update(t, copydt); //This will update the particle system!


        //Exports all particles attributes to different vector lists

        std::vector<Effect*> effects = ps.get_effectList();
        for (int i = 0; i < effects.size(); i++) {
            //Rita ut effekterna
        }

        std::vector<Emitter*>& allEmitters = ps.get_emitterList();
        for (int i = 0; i < allEmitters.size(); i++) {
            positions.push_back(allEmitters[i]->get_position());
            sizes.push_back(allEmitters[i]->get_size());
            colors.push_back(allEmitters[i]->get_color());

            std::vector<Particle>& emitterParticles = allEmitters[i]->get_particleList();
            for (int i2 = 0; i2 < emitterParticles.size(); i2++) {

                Particle& currentParticle = emitterParticles[i2];
                positions.push_back(currentParticle.get_position());
                sizes.push_back(currentParticle.get_size());
                colors.push_back(currentParticle.get_color());

            }
        }
        


        // Create some global smooth rocking motion
        const glm::vec2 vel =
            glm::vec2{static_cast<float>(std::cos(t)), -static_cast<float>(std::abs(std::sin(t)))} *
            0.2f;

        {
            ZoneScopedN("Update particles");

            // Simulation dt may differ from actual dt based on the simulation speed
            const float sim_dt = static_cast<float>(dt) * speed;

            copydt = static_cast<float>(dt) * speed;
        }

        // Clear screen with color
        window.clear({0, 0, 0, 1});

        // Draw particles

        window.drawPoints(positions, sizes, colors);

        // UI
        {
            window.beginGuiWindow("UI");
            window.text("I'm text!");
            window.sliderFloat("Speed", speed, 0.001f, 10.0f);
            window.sliderFloat("test", idk, 0.001f, 10.0f);
            if (window.button("Close application")) {
                running = false;
            }
            window.endGuiWindow();
        }

        window.endFrame();
        running = running && !window.shouldClose();
    }

    return EXIT_SUCCESS;
} catch (const std::exception& e) {
    fmt::print("{}", e.what());
    return EXIT_FAILURE;
}
