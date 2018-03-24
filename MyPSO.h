#ifndef _MYPSO_H_
#define _MYPSO_H_
#include <iostream>
#include "dataReader.h"
using namespace std;
class RandomValues
{
public:
    double random(int, int);
    void provideRandValues(double **, int, int);
};

class ANeuralNetwork
{
private:
    int in, hn, on, now, nop;
    float c1,c2;
public:
    void setNumNeurons(int, int, int);
    void setNumParticles(int);
    void setPSOConstants(float, float);
    int getInpNeuron();
    int getHidNeuron();
    int getOutNeuron();
    int getNumParticles();
    int getNumWeights();
    float getConstc1();
    float getConstc2();
};

class PSOTraining
{
protected:
    int in, hn, on, now, nop;
    const int Imax = 1000;
    const double c1min=0.05, c2min=0.05, c1max = 3, c2max = 2;
    float c1,c2;
    double **particle, **pbest, **velocity, *gbest, gfvalue, *pbestfval, *fvalue;
    int itrn=1, nosample;
    double weight=0.9, we;
    double MAX;
    vector<double*> data;
    double total=0, intd=0, *tmp;

public:
    PSOTraining(ANeuralNetwork, dataReader);
    ~PSOTraining();
    double fv(double *, vector<double*>);
    int findMinErrorFitnessValue(double *, int);
    void assignIndexToGbest(double *, double *);
    void assignIndexToPbest(double *, double *);
    void calculateVelocity(double *, double *,double *, double *, int);
    void updateParticlePosition(double *, double *);
    double checkForPbestI(double *, double, double *, vector<double*>);
    double updateGbest(double *, double **, double *);
    double* doTraining(vector<double*>);
    int setNoOfSample(dataReader);
    int getNoOfSample();
    double* getGBest();
    void clear();
    double BPTraining(double* &,int, vector<double*>);
};

#endif
