#include <iostream>
#include <vector>
#include <cstdlib>
#include <cstring>
#include <string>
#include <ctime>
#include <fstream>
#include <tgmath.h>
#include "MyPSO.h"
#include "dataReader.h"
using namespace std;
/******************************
hn is number of hidden layers
in is number of input layers
interval is for interval of days, here on the per day basis.
now is for number of weights required as a whole in the neural network.
Weights is calculated as [(in*hn)+(hn*on)].
*******************************/
const int hn = 30, in = 6, interval=1, now = 210;

/************************
Function: CheckForOutput()
Checking output after training where a[] is the trained weight.
m[] is the output parameters.
MAX is for normalization which is for maximum element extracted from the input file.
*************************/
double CheckForOutput(double *a, double *m, double MAX)
{
    double tmp[hn], intd=0;
    memset(tmp, 0, sizeof(double)*hn);
    int k=0;
    for(int i=0; i<hn; i++)
    {
        for(int j=0; j<in; j++)
        {
            tmp[i] += ((m[j]/MAX)*a[k++]);
        }
        tmp[i] = (1 / (1 + exp(-1*tmp[i])));
    }
    for(int i=0; i<hn; i++)
    {
        intd += (tmp[i]*a[k++]);
    }
    intd = (1 / (1 + exp(-1*intd)));
    return intd;
}

/************************
Function: printResults()
Generates an formated output file that can be run on matlab to generate graphs.
*************************/
void printResults(double *result, double *targetop, int *no, int num, int nosample, double MAX, double MIN, string str)
{
    cout<<"#########################::Final Results::#########################\n";
    fstream print;
    string res="";
    print.open("Figure.m", ios::out);
    double diff, acc, sumacc=0;
    for(int i=0; i<((nosample-num)/interval); i++)
    {
        diff = targetop[i]-result[i];
        acc = (diff/targetop[i]);
        if(acc<0) acc = (acc*(-1));
        sumacc += acc;
        cout<<"Output: "<<result[i]<<" :: Target: "<<targetop[i]<<"->Difference: "<<diff<<"-->Accuracy: "<<100-(acc*100)<<"%"<<endl;
    }
    acc = sumacc/(((nosample-num)/interval));
    double finacc = 100-(acc*100);
    cout<<"     Final Accuracy:  "<<finacc<<"%"<<endl;
    print<<"y = [";
    for(int i=0; i<((nosample-num)/interval); i++)
    {
        print<<result[i]<<" ";
    }
    print<<"];"<<endl;
    print<<"x = [";
    for(int i=0; i<((nosample-num)/interval); i++)
    {
        print<<no[i]<<" ";
    }
    print<<"];"<<endl;
    print<<"Y = [";
    for(int i=0; i<((nosample-num)/interval); i++)
    {
        print<<targetop[i]<<" ";
    }
    print<<"];"<<endl;
    print<<"X = [";
    for(int i=0; i<((nosample-num)/interval); i++)
    {
        print<<no[i]<<" ";
    }
    print<<"];"<<endl;
    print<<"axh = gca;"<<endl;
    print<<"plot(x,y,'r--',X,Y,'b-','LineWidth',2.5);"<<endl;
    print<<"set(gca,'XLim',["<<(num-10)<<" "<<(nosample+10)<<"],'YLim',["<<MIN<<" "<<(MAX+10)<<"]);\n";
    //print<<"axis(["<<(num-50)<<" "<<(nosample+50)<<" "<<(MIN-100)<<" "<<(MAX+100)<<"]);"<<endl;
    print<<"xlabel('Working Days');"<<endl;
    print<<"ylabel('Closing Prices of the next day');"<<endl;
    print<<"title('Stock Market Prediction using PSO-BP Trained ANN');"<<endl;
    print<<"txstr(1) = {'o : Predicted Value'};"<<endl;
    print<<"txstr(2) = {'* : Target Value'};"<<endl;
    print<<"txstr(3) = {''};"<<endl;
    print<<"uicontrol('Style','text','HorizontalAlignment','left','FontSize', 10,'position',[1111 90 120 40],'String',txstr);"<<endl;
    print<<"grid on;\n";
    print<<"set(axh,'GridLineStyle','-');\n";
    print<<"grid minor;\n";
    print<<"hgsave('nitish');\n";
    print<<"h=hgload('nitish');\n";
    res = res+str;
    res = res+"_Acc_";
    char op[100];
    sprintf(op,"%lf",finacc);
    res = res+op;
    string name = res;
    res = res+".png";
    print<<"saveas(h,'"<<res<<"');";
    print.close();
    name = "COPY Figure.m Data_SMA_EMA_five\\" + name;
    name = name + ".txt";
    system(name.c_str());
    //system("pause");
    system("matlab -r Figure");
    //char b = (char)(7);
    //printf("%c%c%c\n",b,b,b);
}
/************************
Function: main()
Responsibilities:
=> Preprocessing of the input file through dataReader.cpp.
=> Load processed input file with number of input and output neurons.
=> Create a NeuralNetwork object and setting number of neurons, Here (6,30,1).
=> Set number of Particles for the PSO algorithm, Here its 30.
=> Set PSO constants c1 and c2 be 3 and 2 respectively.
*************************/
int main(int argc, char *argv[])
{
    srand((unsigned int)time(0));
    dataReader d;
    d.setN(5);
    d.preProcessFile("SBI_daily_241113_240514.csv");  ///Give the path of the input file to be processed.
    d.loadDataFile("inputfile.csv",6,1);
    vector<double*> data;
    double MAX = d.getMAXValue();
    double MIN = d.getMINValue();
    int nosample = d.getTotalDataSize();
    vector<double*> target;
    ANeuralNetwork p;
    p.setNumNeurons(6,30,1);
    p.setNumParticles(30);
    p.setPSOConstants(3,2);
    system("echo *******Data Retrieval complete*******");
    system("pause");
    system("erase inputfile.csv");
    //system("pause");
    d.setNoOfTrainingData(nosample-40);  ///Setting Number of training data.
    int num = d.noOfTrainingData();
    PSOTraining pt(p,d); ///Object of PSOTraining Class to perform training.
    double output;
    double *result = new double[((nosample-num)/interval)+2];  ///Generated output.
    double *targetop = new double[((nosample-num)/interval)+2]; ///Correct Output from file.
    int *no = new int[((nosample-num)/interval)+2]; ///Total number of testing days.

//    data = d.getTrainingDataSet(nosample);
//    int m = pt.setNoOfSample(d);
//    double *gbest = pt.doTraining(data);

    /******************
    For each output day the neural network is trained for the past say 500 number of days
    and repeated again and again till the last testing day.
    *******************/
    for(int i=0; i<((nosample-num)/interval); i++)
    {
        cout<<((nosample-num)/interval)<<" "<<i<<endl;
        data = d.getTrainingDataSet(num+(interval*i));
        int m = pt.setNoOfSample(d);
        no[i] = (num+(interval*i));
        double *gbest = pt.doTraining(data);
        //double *gbest = pt.getGBest();
        target = d.getTargetInput();
        cout<<"Target::\n";
        for(int i=0; i<in; i++)
        {
            cout<<"inp "<<i<<" : "<<target[0][i]<<endl;
        }
        //for(int i=0; i<now; i++) cout<<gbest[i]<<" ";
        //system("pause");
        output = CheckForOutput(gbest, target[0], MAX);
        result[i]=(output*MAX);
        targetop[i]=target[0][6];
        cout<<"Output: "<<result[i]<<" :: Target: "<<targetop[i]<<endl;
        pt.clear();
        //system("pause");
    }
    printResults(result, targetop, no, num, nosample, MAX, MIN, "SBI_daily_241113_240514.csv");
    delete[] result;
    delete[] targetop;
    return 0;
}
