#include "planets.h"
#include <cmath>
#include <iostream>

Planets::Planets()
{

}

void Planets::AddPoint(float xPos, float yPos, float xVel, float yVel, float mass, bool fixed)
{
    xPositions.push_back(xPos);
    yPositions.push_back(yPos);

    xVelocity.push_back(xVel);
    yVelocity.push_back(yVel);

    Mass.push_back(mass);

    Fixed.push_back(fixed);

}

void Planets::NextTimeInterval(float Time)
{
    float CurrentX = 0;
    float CurrentY = 0;
    vector<float> xPositionsNew;
    vector<float> yPositionsNew;
    vector<float> xVelocityNew;
    vector<float> yVelocityNew;


    //go through each planet and do a calc to move it.
    // v = v0 + a*t
    for(unsigned int i = 0; i < xPositions.size(); i++)
    {
        CurrentX = xPositions[i];
        CurrentY = yPositions[i];
        float ForceX = 0;
        float ForceY = 0;

        for (unsigned int j = 0; j < xPositions.size();j++)
        {
            if(i != j)
            {
                float distance = sqrt((CurrentX - xPositions[j]) * (CurrentX - xPositions[j]) + (CurrentY - yPositions[j]) * (CurrentY - yPositions[j]));
                //cout << "Distance is " << distance << endl;

                float Force = Mass[i] * Mass[j] / distance;
                //cout << "Force is " << Force << endl;

                ForceX = ForceX + (Force/distance * (xPositions[j] - CurrentX)) * .1;
                ForceY = ForceY + (Force/distance * (yPositions[j] - CurrentY)) * .1;

            }
        }
        //cout << "Accel is " << ForceX/Mass[i] << endl;
        if (Fixed[i])
        {
            xVelocityNew.push_back(0);
            yVelocityNew.push_back(0);
            xPositionsNew.push_back(xPositions[i]);
            yPositionsNew.push_back(yPositions[i]);

        }
        else
        {
            xVelocityNew.push_back(ForceX/Mass[i] * Time + xVelocity[i]);
            yVelocityNew.push_back(ForceY/Mass[i] * Time + yVelocity[i]);
            xPositionsNew.push_back(xPositions[i] + .5 * (xVelocityNew[i] + xVelocity[i])* Time);
            yPositionsNew.push_back(yPositions[i] + .5 * (yVelocityNew[i] + yVelocity[i])* Time);
        }

    }
    xPositions = xPositionsNew;
    yPositions = yPositionsNew;
    xVelocity = xVelocityNew;
    yVelocity = yVelocityNew;

}

void Planets::RemoveAllPlanets()
{
    xPositions.clear();
    yPositions.clear();

    xVelocity.clear();
    yVelocity.clear();

    Mass.clear();

    Fixed.clear();

}
