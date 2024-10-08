#include <particlesystem/particlesystem.h>
#include <vector>
#include <random>

// Class Particle

glm::vec2& Particle::get_position() { return position; }
int& Particle::get_size() { return size; }
glm::vec4& Particle::get_color() { return color; }

void Particle::set_position(const glm::vec2& p) { position = p; }
void Particle::set_size(const int& s) { size = s; }
void Particle::set_color(const glm::vec4& c) { color = c; }
void Particle::set_endOfLife(const double& time) { endOfLife = time; }

bool Particle::is_alive(const double& time) { return endOfLife > time; }

void Particle::calculate_accelaration() {
    glm::vec2 allForces{0.0f, 0.0f};
    for (int i = 0; i < forces.size(); i++) {
        allForces += forces[i];
    }
    accelaration.x = allForces.x / mass;
    accelaration.y = allForces.y / mass;
}

void Particle::calculate_velocity(const double& dt) {
    velocity.x = velocity.x - accelaration.x * dt;
    velocity.y = velocity.y - accelaration.y * dt;
}

void Particle::calculate_position(const double& dt) {
    position.x = position.x - velocity.x * dt;
    position.y = position.y - velocity.y * dt;
}

void Particle::add_force(const glm::vec2& force) { forces.push_back(force); }

void Particle::clear_forces() { forces.clear(); }



//Class Emitter

glm::vec2& Emitter::get_position() { return position; }
int& Emitter::get_size() { return size; }
glm::vec4& Emitter::get_color() { return color; }
std::vector<Particle>& Emitter::get_particleList() { return particleList; }

void Emitter::set_position(const glm::vec2& p) { position = p; }
void Dot::set_directions(const std::tuple<int, int>& d) { 
    directions = d;
    set_directionSpread(); //has to change when user changes directions. Not so good looking code tho :/
}
void Dot::set_randomDistribution(const bool& trueOrFalse) { randomDistribution = trueOrFalse; }
void Emitter::set_amountToSpawn(const int& amount) { amountToSpawn = amount; }
void Emitter::set_spawnRate(const double& sr) { spawnRate = sr; }
void Emitter::set_power(const double& p) { power = p; }
void Emitter::set_size(const int& s) { size = s; }
void Emitter::set_color(const glm::vec4& c) { color = c;}
void Emitter::set_particleLifeTime(const double& lifeTime) {particleLifeTime = lifeTime; }
void Dot::set_directionSpread() {directionSpread = std::get<1>(directions) - std::get<0>(directions);}


Particle Emitter::create_particle(const double& time) { 
    // Create new particle
    Particle newParticle = Particle(); 

    //Set attributes in regards to the Emitter
    newParticle.set_position({position.x, position.y}); //Spawn position
    newParticle.set_endOfLife(time + particleLifeTime); //endOfLife timestamp

    return newParticle;
}


float Emitter::rnd() { return rand() / static_cast<float>(RAND_MAX); }

// Random float (-1,1)
float Emitter::srnd() { return rnd() * 2.0f - 1.0f; }





//Class Dot

void Dot::spawn_particles(const double& time) {

    frameCounter += spawnRate;
    if (frameCounter >= 1) {

        frameCounter = 0;

        int division = amountToSpawn -1;  //If 1 is not removed then the emitter wont spawn at correct angles
        if (directionSpread == 360) {
            division += 1;
        }

        for (int i = 0; i < amountToSpawn; i++) {

            // Create new particle
            Particle newParticle = create_particle(time);

            // Här räknas riktning ut. Borde kanske bli ny funktion som kallas calculate_direction?
            // Vi kan också flytta hela denna del till "create_particle". Kanske är snyggare
            // struktur då eftersom man antar att en partikel ska vara klar efter den är skapad?
            if (randomDistribution) {
                newParticle.add_force(
                    calculate_random_direction() *
                    power);  // Direction of force multiplied by the power from the emitter

            } else {
                glm::vec2 direction;
                int startDegree = std::get<0>(directions);
                float particleDegree = startDegree + (directionSpread / division) * i;
                float toRad =
                    particleDegree * 3.14159 / 180;  // omvanddling från grader till radianer
                direction.x = std::cos(toRad);
                direction.y = std::sin(toRad);
                newParticle.add_force(direction * power);  // Direction of force multiplied by the power from the emitter
            }

            particleList.push_back(newParticle);
        }
    
    }


}

void Emitter::move_particles(const double& dt) {
    for (int i = 0; i < particleList.size(); i++) {
        particleList[i].calculate_accelaration();
        particleList[i].calculate_velocity(dt);
        particleList[i].calculate_position(dt);
        particleList[i].clear_forces();
    }
}

void Emitter::remove_particles(const double& time) {
    int lastDead = 0;
    for (int i = 0; i < particleList.size(); i++) {
        if (particleList[i].is_alive(time)) {
            lastDead = i - 1;
            break;
        }
    }

    if (lastDead > 0) {
        particleList.erase(particleList.begin(),
                                  particleList.begin() + lastDead);
    }
}

glm::vec2 Dot::calculate_random_direction() {
    glm::vec2 directionNormalized;
    double randomNumber =
        rand() % directionSpread +
        std::get<0>(
            directions);  // Random nummer mellan första värdet till nästa värde i directions
    randomNumber = randomNumber * 3.14159 / 180;  // omvanddling från grader till radianer
    directionNormalized.x = std::cos(randomNumber);
    directionNormalized.y = std::sin(randomNumber);
    return directionNormalized;  // mellan -1 o 1
}




// Class Rectangular
void Rectangular::calculate_boundaries() {
    right = position.x + (std::get<0>(dimensions) / 2);
    left = position.x - (std::get<0>(dimensions) / 2);
    top = position.y + (std::get<1>(dimensions) / 2);
    bottom = position.y - (std::get<1>(dimensions) / 2);
}

void Rectangular::set_direction(const int& inDirection) { direction = inDirection; }
void Rectangular::set_dimensions(const std::tuple<float, float>& inDimensions) {
    dimensions = inDimensions;
    calculate_boundaries();
}

void Rectangular::spawn_particles(const double& time) {

    frameCounter += spawnRate;
    if (frameCounter >= 1) {

        frameCounter = 0;

        for (int i = 0; i < amountToSpawn; i++) {
            Particle p = create_particle(time);
            p.set_position(calculate_random_position());

            glm::vec2 directionVector;
            float toRad = direction * 3.14159 / 180;  // omvanddling från grader till radianer
            directionVector.x = std::cos(toRad);
            directionVector.y = std::sin(toRad);

            p.add_force(directionVector * power);
            particleList.push_back(p);
        }
    }
}

glm::vec2 Rectangular::calculate_random_position() {
    std::random_device rd;
    std::default_random_engine generator(rd());

    std::uniform_real_distribution<double> distributionX(left, right);
    double randomizedX = distributionX(generator);

    std::uniform_real_distribution<double> distributionY(bottom, top);
    double randomizedY = distributionY(generator);

    glm::vec2 randomized = {randomizedX, randomizedY};

    return randomized;
}







//Class Effect


glm::vec2& Effect::get_position() { return position; }

void Effect::set_position(const glm::vec2& p) { position = p; }

void Effect::apply_forces(std::vector<Particle>& particleList) {
    for (int i = 0; i < particleList.size(); i++) {
        apply_force(particleList[i]);
    }
}


//Class Effect : GravityWell

void GravityWell::apply_force(Particle& particle) {
    glm::vec2 dist_vec = particle.get_position() - position;
    float dist = std::sqrt((dist_vec.x * dist_vec.x) + (dist_vec.y * dist_vec.y));
    if (dist < radius) {

        // 1. Normalisera avståndsvektorn mellan partikel och effekt. 2. Skala kraften baserat på
        // hur procentuellt nära effekten partikeln är. 3. Multiplicera med kraften
        glm::vec2 effectForce = (dist_vec / dist) * (1 - (dist / radius)) * power;
        particle.add_force(effectForce);
        particle.set_color({0.6, 0.1, 0.9, 0.3});
    }
} 

void GravityWell::set_radius(const float& r) { radius = r; }

void GravityWell::set_power(const float& p) { power = p; }



//Class Effect : Wind

void Wind::calculate_boundaries() {
    right = position.x + (std::get<0>(dimensions) / 2);
    left = position.x - (std::get<0>(dimensions) / 2);
    top = position.y + (std::get<1>(dimensions) / 2);
    bottom = position.y - (std::get<1>(dimensions) / 2);
    std::cout << right << "\n";
    std::cout << left << "\n";
    std::cout << top << "\n";
    std::cout << bottom << "\n";
}


void Wind::apply_force(Particle& particle) {
    glm::vec2 particlePos = particle.get_position();

        //std::cout << "{" << particlePos.x << ", " << particlePos.y << "}          ";
    
    if (particlePos.x <= right && particlePos.x >= left && particlePos.y <= top && particlePos.y >= bottom) {
        
        particle.add_force(force);
        particle.set_color({0, 1.0, 0, 0.5});
    }


}




//Class Particlesystem

void Particlesystem::update(const double& time, const double& dt) { 
    remove_particles(time);
    spawn_particles(time);
    apply_effects();
    move_particles(dt);
}

void Particlesystem::move_particles(const double& dt) {
    for (int i = 0; i < emitterList.size(); i++) {
        emitterList[i]->move_particles(dt);
    }
}

void Particlesystem::spawn_particles(const double& time) {
    for (int i = 0; i < emitterList.size(); i++) {
        emitterList[i]->spawn_particles(time);
    }
}


void Particlesystem::remove_particles(const double& time) {
    for (int i = 0; i < emitterList.size(); i++) {
        emitterList[i]->remove_particles(time);
    }
}

void Particlesystem::apply_effects() {
    //Loop through all emitters
    for (int i = 0; i < emitterList.size(); i++) {

        //Loop through all effects for each emitter
        for (int i2 = 0; i2 < effectList.size(); i2++) {

            //Get the vector of particles from each emitter and call on effect function "apply_forces". All particles in range will get a new force. 
            std::vector<Particle>& emitterParticles = emitterList[i]->get_particleList(); //Note that the vector of particles created is a reference, so that the actiual vector in each emitter get modified.
            effectList[i2]->apply_forces(emitterParticles);
        }
    }
}

void Particlesystem::add_emitter(Emitter& e) { 
    Emitter* ptr = &e;
    emitterList.push_back(ptr);
}
void Particlesystem::add_effect(Effect& e) { 
    Effect* ptr = &e;
    effectList.push_back(ptr);
}

std::vector<Emitter*>& Particlesystem::get_emitterList() { 
    return emitterList; 
}
std::vector<Effect*>& Particlesystem::get_effectList() { 
    return effectList;
}












   




