//include definition file
#include "dataReader.h"
#include <iostream>
#include <fstream>
#include <string>
#include <math.h>
#include <cstring>
#include <climits>
#include <algorithm>

using namespace std;

/*******************************************************************
* Destructor
********************************************************************/
dataReader::~dataReader()
{
    //clear data
    //for (int i=0; i < (int) data.size(); i++ ) delete data[i];
    data.clear();
}
/*******************************************************************
* Pre-Process the csv file of input data
********************************************************************/
void dataReader:: setN(int num)
{
    N = num;
}
int dataReader:: getN()
{
    return N;
}

bool dataReader::preProcessFile( const char* filename)
{
    //open file for reading
    fstream inputFile;
    ofstream fp;
    inputFile.open(filename, ios::in);
    fp.open("inputfile.csv", ios::app);
    cout << "Input File Open: " << filename << "\nPre-Processing Started."<< endl;
    int linecount=1;
    if ( inputFile.is_open() )
    {
        string line = "";
        char* target="0\0";
        char* previous="0\0";
        //read data
        getline(inputFile, line); //skipping first line.
        getline(inputFile, line);
        //for(int i=0; i<N; i++)
        processFirstNLine(fp, line, target, previous); //Processing First N Line for Target Output.
        while ( !inputFile.eof() )
        {
            getline(inputFile, line);
            //process line
            if (line.length() > 2 ) processLineToCSV(fp, line, target, previous);
        }

        cout << "Input File Closed: " << filename << "\nPre-Processing Complete."<< endl;

        //close file
        inputFile.close();
        fp.close();
        return true;
    }
    else
    {
        cout << "Error Opening Input File: " << filename << endl;
        return false;
    }
}

/*******************************************************************
* Processes a single line to Pre-Processed File from Original File
********************************************************************/

void dataReader::processFirstNLine(ofstream &fp, string &line, char* &target, char* &previous)
{
    //store inputs
    char* cstr = new char[line.size()+1];
    char* t;
    strcpy(cstr, line.c_str());
    //tokenise
    int i = 0;
    t=strtok (cstr,",");
    while ( i < 6 )
    {
        if(i==0)
        {
            t = strtok(NULL,",");
            i++;
            continue;
        }
        else if ( i < 5 )
        {
            if(i==4) target=t;
        }
        else
        {
            previous=target;
            vecnum.insert(vecnum.begin(),atof(previous));
            //cout<<target<<endl;
        }

        //move token onwards
        t = strtok(NULL,",");
        i++;
    }
}


void dataReader::processLineToCSV(ofstream &fp, string &line, char* &target, char* &previous)
{
    //store inputs
    char* cstr = new char[line.size()+1];
    char* t;
    strcpy(cstr, line.c_str());
    //tokenise
    int i = 0;
    t=strtok (cstr,",");
    while ( i < 7 )
    {
        if(i==0)
        {
            t = strtok(NULL,",");
            i++;
            continue;
        }
        else if ( i < 5 )
        {
            fp<<t<<",";
            if(i==4) target=t;
        }
        else if(i==5)
        {
            vecnum.insert(vecnum.begin(),atof(target));
        }
        /*else if(i==6)
        {
            double dum = ema[countEMA]+(alpha5*(atof(target)-ema[countEMA]));
            ema.push_back(dum);
            countEMA++;
            fp<<ema[countEMA]<<",";
        }
        else if(i==7)
        {
            double dum = ema10[countEMA10]+(alpha10*(atof(target)-ema10[countEMA10]));
            ema10.push_back(dum);
            countEMA10++;
            fp<<ema10[countEMA10]<<",";
        }
        else if(i==8)
        {
            cmax = max(cmax,atof(target));
            cmin = min(cmin,atof(target));
            double dif = (cmax-cmin);
            if(dif==0) dif=1;
            double sol = (cmax-(atof(target)))*100/dif;
            fp<<sol<<",";
        }
        */
        else
        {
            fp<<previous<<"\n";
            previous=target;
            //cout<<target<<endl;
        }

        //move token onwards
        t = strtok(NULL,",");
        i++;
    }
}

/*******************************************************************
* Loads the csv file of input data
********************************************************************/
bool dataReader::loadDataFile( const char* filename, int nI, int nT )
{
    //clear any previous data
    for (int i=0; i < (int) data.size(); i++ ) delete data[i];
    data.clear();
    //Calculating sum of closing prices for Indicators
    vecsum.push_back(0);
    for (int i=0; i < (int) vecnum.size(); i++ )
    {
        sum = vecsum[i]+vecnum[i];
        vecsum.push_back(sum);
        //cout<<vecsum[i]<<" , ";
    }
    //calculating EMA through vecsum
    ema.push_back((vecsum[N]-vecsum[0])/N);
    for (int i=0; i < (int) vecnum.size(); i++ )
    {
        double val = ((2/(double)(1+N))*(vecnum[N+i]-ema[i]))+ema[i];
        ema.push_back(val);
        //cout<<ema[i]<<" , ";
    }

    //set number of inputs and outputs
    nInputs = nI;
    nTargets = nT;

    //open file for reading
    fstream inputFile;
    inputFile.open(filename, ios::in);
    double* pat;
    int ch=0;
    if ( inputFile.is_open() )
    {
        string line = "";
        int check=(int)vecsum.size()-2;
        //read data
        while ( !inputFile.eof() )
        {
            getline(inputFile, line);

            //process line
            if (line.length() > 2 && check>=N)
            {
                pat = processLine(line,check);
                it = data.begin();
                data.insert(it, pat );
            }
            check--;
        }

        cout << "Input File: " << filename << "\nRead Complete: " << data.size() << " Patterns Loaded"  << endl;

        //close file
        inputFile.close();

        return true;
    }
    else
    {
        cout << "Error Opening Input File: " << filename << endl;
        return false;
    }
}

/*******************************************************************
* Processes a single line from the data file
********************************************************************/
double* dataReader::processLine( string &line, int check )
{
    //create new pattern and target
    double* pattern = new double[nInputs+nTargets];

    //store inputs
    char* cstr = new char[line.size()+1];
    char* t;
    strcpy(cstr, line.c_str());

    //tokenise
    int i = 0;
    t=strtok (cstr,",");

    while ( t!=NULL && i < (nInputs + nTargets) )
    {
        if ( i < nInputs-2 )
        {
            pattern[i] = atof(t);
            if(pattern[i]>MAX) MAX = pattern[i];
            //if ( i < nInputs-1 )
            MIN = min(MIN,pattern[i]);
        }
        else
        {
            pattern[i++] = (vecsum[check]-vecsum[check-N])/N;
            MIN = min(MIN,pattern[i-1]);
            pattern[i++] = ema[check-N];
            MIN = min(MIN,pattern[i-1]);
            pattern[i] = atof(t);
            if(pattern[i]>MAX) MAX = pattern[i];
            MIN = min(MIN,pattern[i]);
        }

        //move token onwards
        t = strtok(NULL,",");
        i++;
    }
    //add to records
    return pattern;
//      data.push_back(new dataEntry( pattern, target ) );
}

/*******************************************************************
* Returns number of data sets and max value among them
********************************************************************/
void dataReader::setNumTrainingSets(int num)
{
    numTrainingSets = num;
    //cout<<"numTrainingSets : "<<numTrainingSets<<endl;
}

int dataReader::getNumTrainingSets()
{
    return numTrainingSets;
}

double dataReader::getMAXValue()
{
    return MAX;
}

double dataReader::getMINValue()
{
    return MIN;
}

int dataReader::getTotalDataSize()
{
    return data.size();
}
/*******************************************************************
* Get data set created by creation approach
********************************************************************/
vector<double*> dataReader::getTrainingDataSet(int num)
{
    trainingset.clear();
    targetinp.clear();
    createDataSet(num);
    return trainingset;
}
/*******************************************************************
* Get all data entries loaded
********************************************************************/
vector<double*>& dataReader::getAllDataEntries()
{
    return data;
}


/*******************************************************************
* Create a data set (all the entries)
********************************************************************/
void dataReader::setNoOfTrainingData(int num)
{
    noOfData = num;
}

int dataReader::noOfTrainingData()
{
    return noOfData;
}

void dataReader::createDataSet(int trainingDataEndIndex)
{
    //training set
    if(trainingDataEndIndex>(data.size()-1))
    {
        trainingDataEndIndex = (data.size()-1);
    }
    setNumTrainingSets(noOfData);
    for ( int i = (trainingDataEndIndex-noOfData); i < trainingDataEndIndex; i++ ) trainingset.push_back( data[i] );
    targetinp.push_back(data[trainingDataEndIndex]);
    //checking the training data;
    /*
    ofstream ch;
    ch.open("input.csv", ios::app);
    for(int i=0; i<trainingset.size(); i++)
    {
        for(int j=0 ; j<6; j++)
        {
            ch<<trainingset[i][j]<<",";
        }
        ch<<endl;
    }
    cout<<"nits\n";
    */
}

vector<double*> dataReader :: getTargetInput()
{
    return targetinp;
}
