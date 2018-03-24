#include "MyPSO.h"
void ANeuralNetwork::setNumNeurons(int i, int j, int k)
{
    in = i;
    hn = j;
    on = k;
    now = ((i*j)+(j*k));
}

void ANeuralNetwork::setNumParticles(int i)
{
    nop = i;
}

void ANeuralNetwork::setPSOConstants(float i, float j)
{
    c1 = i;
    c2 = j;
}

int ANeuralNetwork::getInpNeuron()
{
    return in;
}
int ANeuralNetwork::getHidNeuron()
{
    return hn;
}
int ANeuralNetwork::getOutNeuron()
{
    return on;
}
int ANeuralNetwork::getNumParticles()
{
    return nop;
}

int ANeuralNetwork::getNumWeights()
{
    return (in*hn+hn*on);
}

float ANeuralNetwork::getConstc1()
{
    return c1;
}
float ANeuralNetwork::getConstc2()
{
    return c2;
}


