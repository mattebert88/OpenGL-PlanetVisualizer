#ifndef PLANETS_H
#define PLANETS_H

#include <vector>
using namespace std;

class Planets
{
public:
    Planets();
    vector<float> xPositions;
    vector<float> yPositions;
    vector<float> Mass;
    vector<bool> Fixed;

    void NextTimeInterval(float Time, bool Constrain);
    void AddPoint(float xPos, float yPos, float xVel, float yVel, float mass, bool fixed);
    void RemoveAllPlanets();

private:
    vector<float> xVelocity;
    vector<float> yVelocity;
};

#endif // PLANETS_H
