
#include <rendering/window.h>
#include <cmath>
#include <cstdlib>
#include <vector>
#include <iostream>

class Particle {
public:
    Particle()
        : position{0, 0}
        , endOfLife{1.0}
        , mass{1.0}
        , color{1, 0, 0, 0.5}
        , velocity{0.0f, 0.0f}
        , accelaration{0.0f, 0.0f}
        , size{5} {}

    bool is_alive(const double& time);

    void calculate_accelaration();

    void calculate_velocity(const double& dt);

    void calculate_position(const double& dt);

    void add_force(const glm::vec2& force);

    void clear_forces();

    glm::vec2& get_position();
    int& get_size();
    glm::vec4& get_color();

    void set_position(const glm::vec2& p);
    void set_size(const int& s);
    void set_color(const glm::vec4& c);
    void set_endOfLife(const double& time);

private:
    glm::vec2 position;
    glm::vec2 velocity;
    glm::vec2 accelaration;
    glm::vec4 color;
    double mass;
    double endOfLife;
    int size;

    std::vector<glm::vec2> forces{0};  // All current forces affecting the particle
};




class Emitter {
    public:

        void set_position(const glm::vec2& p);

        void set_amountToSpawn(const int& amount);
        void set_spawnRate(const double& sr);
        void set_power(const double& p);
        void set_size(const int& s);
        void set_color(const glm::vec4& c);
        void set_particleLifeTime(const double& lifeTime);

        std::vector<Particle>& get_particleList();

        Particle create_particle(const double& time);

        void move_particles(const double& dt);

        void remove_particles(const double& time);


        virtual void spawn_particles(const double& time) = 0;

        

        glm::vec2& get_position();

        int& get_size();

        glm::vec4& get_color();

        //int amount_to_spawn();

            // Simple random functions to get started
        // Random float (0,1)
        float rnd();

        // Random float (-1,1)
        float srnd();

        
    protected:

        std::vector<Particle> particleList;
        int amountToSpawn{1};  // How many particles to spawn
        float spawnRate{0.1};  // How often to spawn particles
        float power{50};
        float frameCounter{0};  // to keep track when it is time to create a particle
        glm::vec2 position{0, 0};
        int size{5};
        glm::vec4 color{0.5,0.9,0.3,0.5};
        float particleLifeTime{10};
        
};


class Dot : public Emitter {
public:    
    Dot( 
        const std::tuple<int, int> inDirection = {0, 360},
        const bool& randomInDistribution = true, 
        const int& inSize = 10, 
        const float& inPower = 80,
        const int inDirectionSpread = 0)
        : directions{inDirection}, 
        directionSpread{inDirectionSpread}, 
        randomDistribution{randomInDistribution}
    {
        randomDistribution = randomInDistribution;
        size = inSize;
        power = inPower;
        set_directionSpread();
    }

    void set_directions(const std::tuple<int, int>& d);
    void set_randomDistribution(const bool& trueOrFalse);
    glm::vec2 calculate_random_direction();

    void spawn_particles(const double& time) override;

private:

    // Functions
    void set_directionSpread();

    std::tuple<int, int> directions;
    int directionSpread;
    bool randomDistribution;
};


class Rectangular : public Emitter {

public:
    Rectangular(const float& width = 0.5,
                const float& height = 0.5,
                const int& inDirection = 180)
        : dimensions{width, height}, 
          direction{inDirection}{

        calculate_boundaries();
    }

    void spawn_particles(const double& time) override;

    void set_direction(const int& inDirection);
    void set_dimensions(const std::tuple<float, float>& inDimensions);

    glm::vec2 calculate_random_position();  // static för att den inte används av instanser

private:
    std::tuple<float, float> dimensions;  // width, height
    void calculate_boundaries();
    int direction;

    // Boundary limit
    float right;
    float left;
    float top;
    float bottom;
};




class Effect {
    public:
        //Effect(glm::vec2 pos = { 0, 0 }, float inPower = 1.0) 
        //   : position(pos), power(inPower) {}


        glm::vec2& get_position();

        void set_position(const glm::vec2& p);
        

        void apply_forces(std::vector<Particle>& particleList);

    protected:
        virtual void apply_force(Particle& particle) = 0;

    
        glm::vec2 position;


        /* Effect(glm::vec2 pos = {0, 0}, float radius = 0.1, float power = 1.0)
            : position(pos), radius(radius), power(power) {
        }

        void apply_force(Particle& particle);
        void apply_forces(std::vector<Particle>& particleList);

        void set_power(const float& p);
        void set_radius(const float& r);
        void set_position(const glm::vec2& p);


    private:
        glm::vec2 position;    // Position of the effect
        float radius;          // Radius of the effect
        float power;  // Power constant to scale the force of the effect

        */


};

class GravityWell : public Effect {
public:
        GravityWell(glm::vec2 pos = {0, 0}, float inPower = 1.0, float inRadius = 0.1) 
        {
        position = pos;
        power = inPower;
        radius = inRadius;
        }

        void set_radius(const float& radius);
        void set_power(const float& p);

private:

    float radius;
    float power;

    void apply_force(Particle& particle) override;
};

class Wind : public Effect {

    public:
    Wind(const glm::vec2& pos = {0, 0}, const float& width = 0.1,
             const float& height = 0.1, const glm::vec2& inForce = {0,0}) {
            position = pos;
            dimensions = {width, height};
            force = inForce;
            calculate_boundaries();
        }

        

    private:

        std::tuple<float, float> dimensions;

        void calculate_boundaries();

        float right;
        float left;
        float top;
        float bottom;

        glm::vec2 force;

        void apply_force(Particle& particle) override;
};



class Particlesystem {
    public:
        Particlesystem() {}

        void update(const double& time, const double& dt);

        void move_particles(const double& dt);

        void spawn_particles(const double& time);

        void remove_particles(const double& time);

        void apply_effects();

        void add_emitter(Emitter& e);
        void add_effect(Effect& e);

        //std::tuple<std::vector> export_particles();

        std::vector<Emitter*>& get_emitterList();
        std::vector<Effect*>& get_effectList();

    private:
        std::vector<Emitter*> emitterList;
        std::vector<Effect*> effectList;
};
