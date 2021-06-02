#include <iostream>
#include <cstring>
#include <fstream>

using namespace std;

class rule{
public:
    char trmal;
    char *deduce;
    bool pos;


    rule(){
        pos = false;
    }
    ~rule(){
        if(pos == true) {
            delete(deduce);
            //cout<<"delete a exist rules"<<endl;
        }
        //else cout<<"delete a null rules"<<endl;
    }
    void setRule(char a,char *b){
        //make sure that both parameters are legal!
        if(a == '\0'){//我们默认此处如果a不为空，则b也会不为空。因为输入的规则必须如此。
            cout<<"setRule error!"<<endl;
            return;
        }
        trmal = a;
        deduce = new char[strlen(b)];//strlen不包括\0
        strcpy(deduce,b);
        pos = true;
    }
    void print(){
        if (pos == true){
            cout<<trmal<<"->"<<deduce;
        }
        else {cout<<"the rule is NULL."<<endl;}
    }
};

class ruleTable{
    //we make sure that VT is a char(which means that id should be replaced by 'i') ,and VN is a capital.
    //and as we focus on operator grammar , we supose the VT is {+,*,(,),i,$}  
    rule table[10];
    char VN[10];
    char VT[10];
    int ntable;
    int nVN;
    int nVT;
    bool firstVT[10][10];
    bool lastVT[10][10];
    bool legal;
    //we set 0 for no relation,1 for >,2 for < ,4 for = ,else for error;
    int priTable[10][10];

public:
    ruleTable(){
        //initialization
        for(int i =0;i<10;i++){
            for(int j = 0 ;j<10;j++){
                firstVT[i][j] = false;
                lastVT[i][j] = false;
                priTable[i][j] = 0;
            }
        }
        legal = false;
        nVN = 0;
        nVT = 0;
        ntable = 0;
        VT[nVT++] = '$';
    }
    void analyzation(){
        
        this->initialFirstVT();
        this->initialLastVT();
        this->initialPriTable();
        
        
    }
    void printVN(){
        cout<<"the VN is:";
        for(int i =0;i<nVN;i++){
            cout<<VN[i]<<" ";
        }
        cout<<endl;
    }
    void printVT(){
        cout<<"the VT is:";
        for(int i =0;i<nVT;i++){
            cout<<VT[i]<<" ";
        }
        cout<<endl;
    }

    void initialFirstVT(){
        //first scan ,we set the most obivios firstVT
        for(int i=0;i<ntable;i++){
            for(int j = 0;table[i].deduce[j]!='\0';j++){
                int n = findVT(table[i].deduce[j]);
                if(n >= 0){
                    firstVT[findVN(table[i].trmal)][n] = true;
                    //cout<<"ntable is"<<ntable<<endl;
                    break;
                }
            }
        }
        //second scan , we fullfill all the firstVT
        bool changed ;
        do{
            changed = false;
            for(int i=0;i<ntable;i++){
                    int m = findVN(table[i].trmal);
                    int n = findVN(table[i].deduce[0]);
                    //cout<<"n = "<<n<<endl;
                    //如果产生式第一个字母为VN，那么将这个VN的firstVT赋给该产生式左边VN的firstVT
                    if(n>=0){
                        //cout<<"@3"<<endl;
                        if(VN[n] == table[i].trmal) continue;
                        else{
                            for(int k=0;k<nVT;k++)
                                if(!firstVT[m][k]  && firstVT[n][k]) {
                                    //cout<<" "<<VT[k];
                                    firstVT[m][k] = firstVT[n][k];
                                    changed =true;
                                }
                        }
                    }
            }
        }while (changed);
        
    }
    void initialLastVT(){
        //first scan ,we set the most obivios lastVT
        for(int i=0;i<ntable;i++){
            for(int j = ( strlen(table[i].deduce)-1 ) ; j>=0 ; j--){
                int n = findVT(table[i].deduce[j]);
                //cout<<"n ="<<n<<endl;
                if(n >= 0){
                    
                    lastVT[findVN(table[i].trmal)][n] = true;
                    break;
                }
            }
        }
        //second scan , we fullfill all the lastVT
        bool changed ;
        do{
            changed = false;
            for(int i=0;i<ntable;i++){
                    int m = findVN(table[i].trmal);
                    int n = findVN(table[i].deduce[strlen(table[i].deduce)-1]);
                    //cout<<"n = "<<n<<endl;
                    //如果产生式最后一个字母为VN，那么将这个VN的lastVT赋给该产生式左边VN的lastVT
                    if(n>=0){
                        //cout<<"@3"<<endl;
                        if(VN[n] == table[i].trmal) continue;
                        else{
                            for(int k=0;k<nVT;k++)
                                if(!lastVT[m][k]  && lastVT[n][k]) {
                                    //cout<<" "<<VT[k];
                                    lastVT[m][k] = lastVT[n][k];
                                    changed =true;
                                }
                        }
                    }
            }
        }while (changed);
    }
    void initialPriTable(){
        int len ;
        for(int i=0;i<ntable;i++){
            len = strlen(table[i].deduce);
            for(int j=0; j<len-1;j++){
                int m = findVT(table[i].deduce[j]);
                int n = findVT(table[i].deduce[j+1]);
                if((m>=0) && (n>=0))
                    priTable[m][n] += 4;
                if(j<len-2){
                    int p = findVT(table[i].deduce[j+2]);
                    if(m>=0 && n<0 && p>=0) priTable[m][p] += 4;
                }
                if((m>=0) && n<0)
                    for(int k=0;k<nVT;k++){
                        if(firstVT[findVN(table[i].deduce[j+1])][k] == true){
                            priTable[m][k] += 2;
                        }
                    }
                if(m<0 && n>=0)
                    for(int k=0;k<nVT;k++){
                        if(lastVT[findVN(table[i].deduce[j])][k] == true){
                            priTable[k][n] +=1;
                        }
                    }
            }
        }
        for(int i=0;i<nVT;i++){
            if(firstVT[0][i]){
                priTable[findVT('$')][i] += 2;
            }
            if(lastVT[0][i]){
                priTable[i][findVT('$')] += 1;
            }
        }
        priTable[findVT('$')][findVT('$')] +=4;

        //judge legal or not
        legal = true;
        for(int i=0;i<nVT;i++){
            for(int j=0;j<nVT;j++)
            switch (priTable[i][j])
            {
            case 0:break;
            case 1:break;
            case 2:break;
            case 4:break;
            default:
            legal = false;
                break;
            }
        }
    }
    void printPritable(){
        cout<<"the pritable :"<<endl;
        for(int i=0;i<nVT;i++){cout<<'\t'<<VT[i]<<'\t';}
        cout<<'\n';
        for(int i=0;i<nVT;i++){
            cout<<VT[i]<<":";
            for(int j=0;j<nVT;j++){
                cout<<"\t"<<priTable[i][j]<<"\t";
            }
            cout<<'\n';
        }
    }
    void printFirstVT(){
        cout<<"firstVT:"<<endl;
        for(int i=0;i<nVN;i++){
            cout<<VN[i]<<":";
            for(int j=0;j<nVT;j++){
                if(firstVT[i][j] == true) cout<<VT[j]<<" ";
            }
            cout<<endl;
        }
    }
    void printLastVT(){
        cout<<"lastVT:"<<endl;
        for(int i=0;i<nVN;i++){
            cout<<VN[i]<<":";
            for(int j=0;j<nVT;j++){
                if(lastVT[i][j] == true) cout<<VT[j]<<" ";
            }
            cout<<endl;
        }}
    void printAll(){
        printVN();
        printVT();
        printTable();
        printFirstVT();
        printLastVT();
        printPritable();
    }
    void printTable(){
        for(int i=0;i<ntable;i++){
            table[i].print();
            cout<<endl;
        }
    }
    int findVN(char a){
        //got it return sequence,else return 0
        for(int i =0;i<nVN;i++){
            if(VN[i] == a) return i;
        }
        return -1;
    }
    int findVT(char a){
        //got it return sequence,else return 0
        for(int i =0;i<nVT;i++){
            if(VT[i] == a) return i;
        }
        return -1;
    }

    char* cut(char* line,int a ,int b){
        char result [b-a];
        for(int i = 0; i<b-a-1;i++){
            result[i] = line[a+1+i];
        }
        result[b-a-1]='\0';
        return result; 
    }

    void setTable(char * filename){
        //in:the grammar file path
        //function:we read each deduce and put it into right place
        FILE *fp;
        char *buf = new char[1024];
        fp = fopen(filename,"r");
        if(fp == NULL) {
            cout<<"set table error!";exit(2);
            }
        for(;fgets(buf,1024,fp) != NULL;){
            //attention:buf[end] is \0
            //clean is the string kick out the \t\r\n.
            char clean[1024];
            int nclean = 0;
            for(int i = 0 ;buf[i] != '\0';i++){
                if(buf[i]=='\t' || buf[i]==' ' || buf[i]=='\n') continue;
                clean[nclean++] = buf[i];
            }
            clean[nclean] = '\0';

            //add the VN to table VN
            if(findVN(clean[0]) == -1)VN[nVN++] = clean[0];

            //to help devide deduce ,we need j and k to locate it
            int j=2;
            int k=0;
            for(int i = 3 ;clean[i-1]!='\0';i++){
                if(clean[i] == '|' || clean[i] == '\0') {
                    k>j ? j=i:k=i;
                    table[ntable++].setRule(clean[0],cut(clean,min(j,k),i));
                }
                else if(clean[i]>64 &&clean[i]<91);
                else if(findVT(clean[i]) == -1)
                    VT[nVT++] = clean[i];
                
            }
        }
    }

    void drawTable(char * outfile){
        ofstream out("out.txt");
        if(legal == true){
        out<<"the pritable :"<<endl;
        out<<"\t";
        for(int i=0;i<nVT;i++){out<<'\t'<<VT[i]<<'\t';}
        out<<'\n';
        for(int i=0;i<nVT;i++){
            out<<VT[i]<<":";
            for(int j=0;j<nVT;j++){
                out<<"\t";
                switch(priTable[i][j]){
                    case 0: out<<"\t";break;
                    case 1: out<<"\t>";break;
                    case 2: out<<"\t<";break;
                    case 4: out<<"\t=";break;
                    default:break;
                }
            }
            out<<'\n';
        }
        }
        else {out<<"this grammar is not operator priority grammar";}
    }

    
};


int main(){
    char* outfile = "out.txt";
    ruleTable a;
    a.setTable("in.txt");
    a.analyzation();

    //use this to debug 
    //a.printAll(); 

    a.drawTable(outfile);
    cout<<"program end successfully"<<endl;
    return 1;
}