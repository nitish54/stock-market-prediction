#include <iostream>
#include <iomanip>
#include <tgmath.h>
#include <cmath>
#include <ctime>
#include <cstdlib>
#include <cstring>
#include "MyPSO.h"
#include "dataReader.h"
using namespace std;

PSOTraining::PSOTraining(ANeuralNetwork an, dataReader d) ///Constructor initialization
{
    in = an.getInpNeuron();
    hn = an.getHidNeuron();
    on = an.getOutNeuron();
    nop = an.getNumParticles();
    now = (in*hn)+(hn*on);
    c1 = an.getConstc1();
    c2 = an.getConstc2();
    MAX = d.getMAXValue();
    particle = new double*[nop];
    pbest = new double*[nop];
    velocity = new double*[nop];
    for(int i = 0; i<nop; i++)
    {
        particle[i] = new double[now];
        pbest[i] = new double[now];
        velocity[i] = new double[now];
    }
    gbest = new double[now];
    pbestfval = new double[nop];
    fvalue = new double[nop];
    tmp = new double[hn];
}

PSOTraining::~PSOTraining() ///Destructor for saving memory
{
    delete[] gbest;
    delete[] pbestfval;
    delete[] fvalue;
    delete[] tmp;
    for(int i = 0; i<nop; i++)
    {
        delete[] particle[i];
        delete[] pbest[i];
        delete[] velocity[i];
    }
    delete[] particle;
    delete[] pbest;
    delete[] velocity;
}

int PSOTraining :: setNoOfSample(dataReader d)  ///Setting number of samples.
{
    nosample = d.getNumTrainingSets();
    cout<<nosample<<"  "<<nop<<"  "<<now<<endl;
    return 1;
}

int PSOTraining :: getNoOfSample()
{
    return nosample;
}

void PSOTraining :: clear()
{
    for(int i=0; i<nop; i++)
    {
        gbest[i]=pbestfval[i]=fvalue[i]=0;
    }
    for(int i=0; i<hn; i++)  tmp[i]=0;
    for(int i = 0; i<nop; i++)
    {
        for(int j=0; j<now; j++)
        {
            particle[i][j]=pbest[i][j]=velocity[i][j]=0;
        }
    }
}

void PSOTraining::assignIndexToGbest(double *gbest, double *arr)
{
    for(int i=0; i<now; i++)
    {
        gbest[i] = arr[i];
    }
}

void PSOTraining::assignIndexToPbest(double *pbest, double *arr)
{
    for(int i=0; i<now; i++)
    {
        pbest[i] = arr[i];
    }
}

double PSOTraining :: fv(double *a, vector<double*> data)  ///For checking the fitness value
{
    int k;
    total=0;
    for(int p=0; p<nosample; p++)
    {
        memset(tmp, 0, sizeof(double)*hn);
        intd=0;
        k=0;
        for(int i=0; i<hn; i++)
        {
            for(int j=0; j<in; j++)
            {
                tmp[i] += (((data[p][j])/MAX)*a[k++]);
            }
            tmp[i] = (1 / (1 + exp(-1*tmp[i])));
        }
        for(int i=0; i<hn; i++)
        {
            intd += (tmp[i]*a[k++]);
        }
        intd = (1 / (1 + exp(-1*intd)));
        total += (((data[p][6]/MAX)-intd)*((data[p][6]/MAX)-intd));
    }
    //cout<<data[0]->pattern[0]-data[0]->target[0]<<" ";
    return (total/nosample);
}

int PSOTraining :: findMinErrorFitnessValue(double *fvalue, int nop)
{
    double min = fvalue[0];
    int ind = 0;
    for(int i=1; i<nop; i++)
    {
        if(fvalue[i]<min)
        {
            ind = i;
        }
    }
    return ind;
}

void PSOTraining::calculateVelocity(double *vel, double *arr,double *gbest, double *pbest, int itrn)
{
    RandomValues rv;
    for(int i=0; i<now; i++)
    {
        we = ((weight-0.4)*(Imax-itrn)/Imax)+0.4;
        c1 = (c1max - c1min)*((Imax-itrn)/Imax)+c1min;
        c2 = (c2max - c2min)*((Imax-itrn)/Imax)+c2min;
        vel[i] = we*vel[i] + ((c1*rv.random(0,1)*(pbest[i]-arr[i]))+(c2*rv.random(0,1)*(gbest[i]-arr[i])));
    }
}

void PSOTraining::updateParticlePosition(double *particle, double *velocity)
{
    for(int i=0; i<now; i++)
    {
        particle[i] += velocity[i];
    }
}

double PSOTraining::checkForPbestI(double *particle, double pre, double *pbest, vector<double*> data)
{
    double tmp = fv(particle,data);
    if(tmp < pre)
    {
        pre = tmp;
        for(int i=0; i<now; i++)
        {
            pbest[i] = particle[i];
        }
    }
    return pre;
}

double PSOTraining::updateGbest(double *pbestfval, double **pbest, double *gbest)
{
    double min = pbestfval[0];
    int ind = 0;
    for(int i=1; i<nop; i++)
    {
        if(pbestfval[i]<min)
        {
            min = pbestfval[i];
            ind = i;
        }
    }
    for(int i=0; i<now; i++)
    {
        gbest[i] = pbest[ind][i];
    }
    return min;
}

double PSOTraining::BPTraining(double* (&a), int count, vector<double*> data)
{
    int flag=0,k;
    double delta_hidden[hn];
    memset(delta_hidden, 0, sizeof(double)*hn);
    double delta_out= 0;
    double theta_hidden[hn];
    RandomValues rv;
    for(int i=0; i<hn; i++) theta_hidden[i] = rv.random(0,1);
    double theta_out = 0.3;
    double sum_sq_error=0;
    double sum_delta =0;
    double error_out[nosample];
    memset(error_out, 0, sizeof(double)*nosample);
    float alpha = 0.2;
    memset(tmp, 0, sizeof(double)*hn);
    /* Algorithm Implemenatation */
    while(flag!=1)
    {
        sum_sq_error=0;
        for(int p=0; p<nosample; p++)
        {
            memset(tmp, 0, sizeof(double)*hn);
            intd=0;
            k=0;
            /* Calculating Outputs of Hidden layer */
            for(int i=0; i<hn; i++)
            {
                for(int j=0; j<in; j++)
                {
                    tmp[i] += (((data[p][j])/MAX)*a[k++]);
                }
                tmp[i] = (1 / (1 + exp(-1*tmp[i])));
            }

            /* Calculating Output of the Output layer */
            for(int i=0; i<hn; i++)
            {
                intd += (tmp[i]*a[k++]);
            }
            intd = (1 / (1 + exp(-1*intd)));
            /* Calculating Error at Output Layer */

            error_out[p] = (data[p][6]/MAX) - intd;

            /* Calculating Error gradient for Output Layer */

            delta_out = intd*(1-intd)*error_out[p];

            /* Updating Weights of Output Layer */
            for(int j=in*hn; j<now; j++)
            {
                a[j] = a[j] + (alpha*tmp[j-(in*hn)]*delta_out);
            }

            /* Calculating Error gradient for Hidden layer */
            for(int i=0; i<hn; i++)
            {
                sum_delta = sum_delta + (delta_out*a[i+(in*hn)]);
                delta_hidden[i] = tmp[i] * (1-tmp[i]) * sum_delta;
                sum_delta=0;
            }
            /* Updating Weights of Hidden Layer */
            k=0;
            int i=0;
            while(k<(in*hn))
            {
                for(int j=0; j<in; j++)
                {
                    a[k] = a[k++] + (alpha*(data[p][j]/MAX)*delta_hidden[i]);
                }
                i++;
            }
        }
        /* Checking Final Error */
        cout<<"Iteration #"<<count;
        for(int i=0; i<nosample; i++)
        {
            sum_sq_error += pow(error_out[i],2);
        }
        cout<<" :: ERROR: "<<sum_sq_error<<endl;
        if(sum_sq_error<=0.01 || count>=30000)
            flag=1;
        else
        {
            count = count+1;
        }
    }
}

double* PSOTraining::getGBest()
{
    return gbest;
}

double* PSOTraining::doTraining(vector<double*> data)
{
    itrn = 1;
    we=1;
    srand((unsigned int)time(0));
    double ch=0.0f;
    int check=0;
    RandomValues rv;
    rv.provideRandValues(particle, nop, now);
    rv.provideRandValues(velocity, nop, now);
    for(int i=0; i<nop; i++)
    {
        fvalue[i] = fv(particle[i],data);
        pbestfval[i] = fvalue[i];
    }
    int index = findMinErrorFitnessValue(fvalue,nop);   //finding index of best fitness value.
    gfvalue = fvalue[index];
    assignIndexToGbest(gbest, particle[index]);   //assigning to gbest.
    for(int i=0; i<nop; i++)
    {
        assignIndexToPbest(pbest[i], particle[i]);
    }
    assignIndexToPbest(pbest[index], particle[index]);
    cout<<"Iteration #0 :: GFValue: "<<gfvalue<<endl;
    while(gfvalue >=0.0001f)
    {
        cout<<"Iteration #"<<itrn++<<" :: ";
        itrn++;
        for(int i=0; i<nop; i++)
        {
            calculateVelocity(velocity[i], particle[i], gbest , pbest[i], itrn);
            updateParticlePosition(particle[i], velocity[i]);
            pbestfval[i] = checkForPbestI(particle[i], pbestfval[i], pbest[i], data);
        }
        gfvalue = updateGbest(pbestfval, pbest, gbest);
        cout<<"GFValue: "<<gfvalue<<endl;
        if(check>=400 || itrn >=Imax)
        {
            //for(int i=0; i<now; i++) cout<<gbest[i]<<" ";
            //system("pause");
            gfvalue = BPTraining(gbest,itrn,data);
            gfvalue = 0.00001;
            continue;
        }
        if(gfvalue!=ch)
        {
            ch=gfvalue;
            check=0;
        }
        else check++;
    }
    return gbest;
}
