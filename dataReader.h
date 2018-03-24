/*******************************************************************
* CSV Data File Reader and Training Set Creator
* ------------------------------------------------------------------
* Bobby Anguelov - takinginitiative.wordpress.com (2008)
* MSN & email: banguelov@cs.up.ac.za
********************************************************************/

#ifndef _DATAREADER
#define _DATAREADER

//include standard header files
#include <vector>
#include <string>
#include <climits>
#include <fstream>
using namespace std;

//data reader class
class dataReader
{

//private members
//----------------------------------------------------------------------------------------------------------------
private:

	//data storage
	vector<double*> data;
	vector<double> ema;
	vector<double> vecnum;
	vector<double> vecsum;
	vector<double> ema10;
	vector<double*>::iterator it;
	vector<double*> targetinp;
	vector<double*> trainingset;
	int nInputs;
	int nTargets;
	int count=0;
	int countEMA=0;
    int countEMA10=0;
	//total number of dataSets
	int numTrainingSets;
	int trainingDataEndIndex;
	int noOfData;
	long double sum=0;
	int noOfLine=2;
	float alpha5=0.33;
    float alpha10=0.182;
    int sumsize;
    int N;

//public methods
//----------------------------------------------------------------------------------------------------------------
public:
    double MAX = 0;
    double MIN = INT_MAX;
    double cmax=0;
    double cmin=INT_MAX;
	dataReader(){}
	~dataReader();

	bool preProcessFile( const char* filename);
	bool loadDataFile( const char* filename, int nI, int nT );
	int getNumTrainingSets();
	int getTotalDataSize();
	void setNumTrainingSets(int);
	double getMAXValue();
	double getMINValue();
	vector<double*> getTargetInput();
	int noOfTrainingData();
	void setN(int);
	int getN();
	void setNoOfTrainingData(int);
	void createDataSet(int);

	vector<double*> getTrainingDataSet(int);
	vector<double*>& getAllDataEntries();

//private methods
//----------------------------------------------------------------------------------------------------------------
private:
	double* processLine(string &line, int check );
	void processFirstNLine(ofstream &fp, string &line , char* &target, char* &previous);
	void processLineToCSV(ofstream &fp, string &line , char* &target, char* &previous );
};

#endif
