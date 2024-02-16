#include <iostream>
#include <iomanip>
#include <fstream>
#include <sstream>
#include <cassert>
#include <vector>
#include <cmath>

#include<limits>

#include <map>
#include "cell.h"
#include "net.h"
#include "partitioner.h"
using namespace std;

////////////////////////////////////////////////////////
void Partitioner::parseInput(fstream& inFile)
{

    string str;
    srand( (unsigned)time(NULL));
    float die =0;
    // Set balance factor
    inFile >> str;
    _bFactor = stod(str);

    int randompart = 0;
    int mypart=0;

    // Set up whole circuit
    while (inFile >> str) 
    {
        if (str == "NET") 
        {
            string netName, cellName, tmpCellName = "";
            inFile >> netName;
            int netId = _netNum;
            

            _netArray.push_back(new Net(netId));

            while (inFile >> cellName) 
            {
                if (cellName == ";") 
                {
                    tmpCellName="";
                    break;
                }
                else 
                {
                    // a newly seen cell
                    if (_cellName2Id.count(cellName) == 0) 
                    {
                        //die = (float) rand()/RAND_MAX;
                        //if(die>0.5)
                        //{
                        //    randompart=1;
                        //}else
                        //{
                        //    randompart=0;
                        //}



                        ////////////////////////////////////////////////////
                        int cellId = _cellNum;
                        _cellArray.push_back(new Cell(cellName ,randompart, cellId));

                        
                        _cellName2Id[cellName] = cellId;

                        _cellArray[cellId]->addNet(netId);
                        _cellArray[cellId]->incPinNum();

                        mypart =  _cellArray[cellId]->getPart();
                        _netArray[netId]->incPartCount(mypart);

                        _netArray[netId]->addCell(cellId);
                        ++_cellNum;

                        incPartSize(mypart);

                        tmpCellName = cellName;
                    }
                    // an existed cell
                    else 
                    {
                        if (cellName != tmpCellName) 
                        {
                            assert(_cellName2Id.count(cellName) == 1);

                            int cellId = _cellName2Id[cellName];
                            _cellArray[cellId]->addNet(netId);
                            _cellArray[cellId]->incPinNum();
                            mypart =  _cellArray[cellId]->getPart();
                            _netArray[netId]->incPartCount(mypart);
                            _netArray[netId]->addCell(cellId);
                            tmpCellName = cellName;
                        }
                    }
                }
            }

            //die = (float) rand()/RAND_MAX;
            //if(die>0.5)
            //{
            //    randompart=1;
            //}else
            //{
            //    randompart=0;
            //}



            randompart=1-randompart;
            ++_netNum;
        }
    }
    return;
}
////////////////////////////////////////////////////////////////


void Partitioner::partition()
{
    double d =getBFactor();
    double G1min = ((1-d)/2) * _cellNum;

    ///////////////////////////////////////////////////////
    //meet initial balance condition
    //int ind=0;
    //int cpart=0;
    //int mypin=0;
    //vector<int> mylist=NULL;

    //while(getPartSize(0)<G1min)
    //{
    //    cpart = _cellArray[ind]->getPart();
    //    if(cpart==1)
    //    {
    //        _cellArray[ind]->move();
    //        incPartSize(0);
    //        decPartSize(1);
    //        mypin = _cellArray[ind]->getPinNum();
    //        mylist = _cellArray[ind]->getNetList();
    //        for(int i=0;i<mypin;i++)
    //        {
    //            _netArray[mylist[i]]->incPartCount(0);
    //            _netArray[mylist[i]]->decPartCount(1);
    //        }
    //    }
    //    ind++;
    //}
    /////////////////////////////////////////////////
    //////////////////////////////////////////////////////
    int total=0;
    int maxtotal=-1* numeric_limits<int>::infinity();
    int step=0;

    int target=0;
    int beststep=0;
    //vector<int> beststep;
    vector<int> history;
    
    //////////////////////////////////////////
    //vector<int> tempnets;
    //set<int> tempcells;

    int pinnumm=0;
    int cellnumm=0;
    int fromnumm=0;
    int tonumm=0;

    int frompp=0;
    int topp=0;
    //////////////////////////////////////////////////////////
    bool condition1=true;
    bool condition2=true;
    /////////////////////////////////////////////////////
    int tar=0;
    int see=0;
    //vector<int> templist;
    
    //int bestmove=0;
    int backstep = 0;
    bool sp1=false;
    bool sp2=false;
    bool sp3=false;
    bool sp4=false;
    //int maxx=0;

    //Node *n4;

    //you may need to find the second largest gain due to balance
    //for this question, we can see if it meets balance condiiton first, if ok then just find the best val of one side, if not
    // the other side

    while(condition1)
    {
        step=0;
        total=0;
        target=0;
        maxtotal=-1* numeric_limits<int>::infinity();
        //beststep.clear();
        history.clear();
        //tempnets.clear();
        //tempcells.clear();
        condition2=true;

        //bucketALV = -1* numeric_limits<int>::infinity();
        //bucketBLV = -1* numeric_limits<int>::infinity();
        //ALargestVal=-1* numeric_limits<int>::infinity();
        //BLargestVal=-1* numeric_limits<int>::infinity();

        //bucketAwhoL = 0;
        //bucketBwhoL = 0;
        reset();
        calIni();




        if((!bmapA.empty()) && (!bmapB.empty()))
        {

            if(bmapA.rbegin()->first > bmapB.rbegin()->first)
            {
                if(getPartSize(0)-1<G1min)
                {
                    target = ((bmapB.rbegin()->second)->getId());
                }else
                {
                    target = ((bmapA.rbegin()->second)->getId());
                }
            }else
            {
                if(getPartSize(1)-1<G1min)
                {
                    target = ((bmapA.rbegin()->second)->getId());
                }else
                {
                    target = ((bmapB.rbegin()->second)->getId());
                }
            }
        }else if(bmapA.empty())
        {
            target = ((bmapB.rbegin()->second)->getId());
        }else
        {
            target = ((bmapA.rbegin()->second)->getId());
        }

        /////////////////////////////////
        history.push_back(target);
        _cellArray[target]->lock();
        total += _cellArray[target]->getGain();
        step+=1;

        if(total>maxtotal)
        {
            maxtotal = total;
            beststep = step;
            //beststep.clear();
            //beststep.push_back(step); 
        }//else if(total==maxtotal)
        //{
        //    beststep = step;
            //beststep.push_back(step); 
        //}
        //////////////////////////////////

        
        while(condition2)
        {

            //tempnets = _cellArray[target]->getNetList();
            pinnumm=_cellArray[target]->getPinNum();
            frompp=_cellArray[target]->getPart();
            topp=1-frompp;

            //maxx = _cellArray[target]->getGain();

            //n4 = _cellArray[target]->getNode();

            mydelete(_cellArray[target]->getNode());
            

            //if(frompp==0)
            //{
                

            //    bmapA[maxx].erase(target);
            //    if(bmapA[maxx].empty())
            //    {
            //        bmapA.erase(maxx);
            //    }

            //}else
            //{
            //    bmapB[maxx].erase(target);
            //    if(bmapB[maxx].empty())
            //    {
            //        bmapB.erase(maxx);
            //    }
            //}


            for(int i=0;i<pinnumm;i++)
            {
                fromnumm = _netArray[(_cellArray[target]->getNetList())[i]]->getPartCount(frompp);
                tonumm = _netArray[(_cellArray[target]->getNetList())[i]]->getPartCount(topp);

                //_netArray[(_cellArray[target]->getNetList())[i]]->deleteunlock(target);
                //tempcells=(_netArray[(_cellArray[target]->getNetList())[i]]->gettempunlock());

                //cellnumm = tempcells.size();
                int vall=0;
                int parr=0;
                
                if(tonumm==0)
                {
                    sp1=true;
                }else
                {
                    sp1=false;
                }

                if(tonumm==1)
                {
                    sp2=true;
                }else
                {
                    sp2=false;
                }

                if(fromnumm-1 == 0)
                {
                    sp3=true;        
                }else
                {
                    sp3=false;
                }

                if(fromnumm-1==1)
                {
                    sp4=true;
                }else
                {
                    sp4=false;
                }

                if(sp1 || sp2 || sp3 || sp4)
                {

                    //if(!(_netArray[(_cellArray[target]->getNetList())[i]]->gettempunlock()).empty())
                    //{
                    //for(const auto &m : _netArray[(_cellArray[target]->getNetList())[i]]->gettempunlock())
                    for(int m=0;m<_netArray[(_cellArray[target]->getNetList())[i]]->getcellnum();m++)
                    {
                        Cell *mycellg = _cellArray[(_netArray[(_cellArray[target]->getNetList())[i]]->getCellList())[m]];
                        if(!mycellg->getLock())
                        {

                            vall = mycellg->getGain();
                            parr = mycellg->getPart();
                            //n4 = _cellArray[m]->getNode();

                            mydelete(mycellg->getNode());
                        
                            //if(parr==0)
                            //{
                            //    bmapA[vall].erase(m);
                            //    if(bmapA[vall].empty())
                            //    {
                            //        bmapA.erase(vall);
                            //    }
                            //}else{
                            //    bmapB[vall].erase(m);
                            //    if(bmapB[vall].empty())
                            //    {
                            //        bmapB.erase(vall);
                            //    }
                            //}

                            //////////////////////////
                            if(sp1)
                            {
                                mycellg->incGain();
                            }

                            if(sp2)
                            {
                                if(parr==topp)
                                {
                                    mycellg->decGain();
                                }
                            }
                            ////////////////////////////
                            if(sp3)
                            {
                                mycellg->decGain();
                        
                            }

                            if(sp4)
                            {
                                if(parr==frompp)
                                {
                                mycellg->incGain();
                                }
                            }

                            myinsert(mycellg->getNode());
                            //vall = _cellArray[m]->getGain();
                            //if(parr==0)
                            //{
                            //    bmapA[vall].insert(m);

                            //}else
                            //{
                            //    bmapB[vall].insert(m);
                            //}

                        }

                    }
                }

                //}
                ///////////////////////////////
                _netArray[(_cellArray[target]->getNetList())[i]]->incPartCount(topp);
                _netArray[(_cellArray[target]->getNetList())[i]]->decPartCount(frompp);

            }

            _cellArray[target]->move();
            incPartSize(topp);
            decPartSize(frompp);
            /////////////////////////////////////////
            //bucketALV = -1* numeric_limits<int>::infinity();
            //bucketBLV = -1* numeric_limits<int>::infinity();

            //bucketAwhoL = 0;
            //bucketBwhoL = 0;

            //for(int i=0;i<_cellNum;i++)
            //{
            //    if(!_cellArray[i]->getLock())
            //    {

            //        frompp=_cellArray[i]->gettempPart();
                    
            //        if(frompp==0)
            //        {
            //            if(_cellArray[i]->getGain()>bucketALV)
            //            {
            //                bucketALV = _cellArray[i]->getGain();
            //                bucketAwhoL = i;
            //            }
            //        }else
            //        {
            //            if(_cellArray[i]->getGain()>bucketBLV)
            //            {
            //                bucketBLV = _cellArray[i]->getGain();
            //                bucketBwhoL = i;
            //            }

            //        }

            //    }
            //}

            ///////////////////////////////////
            if((!bmapA.empty()) && (!bmapB.empty()))
            {

                if(bmapA.rbegin()->first > bmapB.rbegin()->first)
                {
                    if(getPartSize(0)-1<G1min)
                    {

                        target = ((bmapB.rbegin()->second)->getId()); //bucketBwhoL;
                    }else
                    {
                        target = ((bmapA.rbegin()->second)->getId());
                    }
                }else
                {
                    if(getPartSize(1)-1<G1min)
                    {
                        target = ((bmapA.rbegin()->second)->getId());
                    }else
                    {
                        target = ((bmapB.rbegin()->second)->getId());
                    }
                }

            }else if(!bmapA.empty())
            {
                target = ((bmapA.rbegin()->second)->getId());
            }else
            {
                target = ((bmapB.rbegin()->second)->getId());
            }

            //////////////////////////////////
            history.push_back(target);
            _cellArray[target]->lock();
            total += _cellArray[target]->getGain();
            step+=1;

            if(total>maxtotal)
            {
                maxtotal = total;
                beststep = step;
                //beststep.clear();
                //beststep.push_back(step); 
            }//else if(total==maxtotal)
            //{
                //beststep.push_back(step);
            //    beststep = step;
            //}

            if(step==_cellNum)
            {
                condition2=false;
                
                pinnumm=_cellArray[target]->getPinNum();
                frompp=_cellArray[target]->getPart();
                topp=1-frompp;

                for(int w=0;w<pinnumm;w++)
                {
                    _netArray[(_cellArray[target]->getNetList())[w]]->incPartCount(topp);
                    _netArray[(_cellArray[target]->getNetList())[w]]->decPartCount(frompp);
                }
                
                _cellArray[target]->move();
                mydelete(_cellArray[target]->getNode());
                incPartSize(topp);
                decPartSize(frompp);

            }

        }
        //////////////////////////////////////////////////////


        //////////////////////////////////////////////////////
        cout << " maxtotal: " << maxtotal << endl;
        condition1 = false;
        


        if((getPartSize(0)>=int(G1min))  && (getPartSize(1)>=int(G1min)) && !(maxtotal>0) )
        {
            condition1 =false;
        }else
        {
            condition1=true;
        }
        
        
        
        //if(maxtotal>0)
        //{
        //    condition1=true;
        //}

        //////////////////////////////////////////////////////
        //int ASize = getPartSize(0);
        //int BSize = getPartSize(1);
        //int tempASize =ASize;
        //int tempBSize =BSize;
        //int bestdiff = 1;
        //int diff=0;
        //int ratio=0;
        //int bestmove=0;
        //int far=0;
        //for(int i=0;i<beststep.size();i++)
        //{

        //    far = beststep[i]; 
        //    tempASize =ASize;
        //    tempBSize =BSize;

        //    for(int j=0;j<far;j++)
        //    {
        //        if(_cellArray[history[j]]->getPart()==0)
        //        {
        //            tempBSize+=1;
        //            tempASize-=1;

        //        }else
        //        {
        //            tempASize+=1;
        //            tempBSize-=1;
        //        }
        //    }

        //    if(tempASize>tempBSize)
        //    {
        //        ratio = tempASize/(tempASize+tempBSize);
        //    }else
        //    {
        //        ratio = tempBSize/(tempASize+tempBSize);
        //    }

        //    diff = ratio-0.5;

        //    if(diff<bestdiff)
        //    {
        //        bestdiff=diff;
        //        bestmove = beststep[i];
        //    }
 
        //}
        if(condition1)
        {
            //cout<<beststep<<endl;
            backstep = _cellNum - beststep +1;
        }else
        {
            backstep = _cellNum+1;
        }


        for(int k=1;k<backstep;k++)
        {

            //tar=0;
            //see=0;
            //templist.clear();
            tar = history[_cellNum-k];
            see = _cellArray[tar]->getPart();
            if(see==0)
            {
                incPartSize(1);
                decPartSize(0);

            }else
            {
                incPartSize(0);
                decPartSize(1);
            }

            _cellArray[tar]->move();
            //templist = (_cellArray[tar]->getNetList());
            for(int j=0;j<_cellArray[tar]->getPinNum();j++)
            {
                if(see==0)
                {
                    _netArray[(_cellArray[tar]->getNetList())[j]]->incPartCount(1);
                    _netArray[(_cellArray[tar]->getNetList())[j]]->decPartCount(0);
                }else
                {
                    _netArray[(_cellArray[tar]->getNetList())[j]]->incPartCount(0);
                    _netArray[(_cellArray[tar]->getNetList())[j]]->decPartCount(1);
                }
                
            }
            

        }

        


    }

    ///////////////////////////////////////
    _cutSize=0;

    for(int i=0;i<_netArray.size();i++)
    {
        if((_netArray[i]->getPartCount(0)!=0) && (_netArray[i]->getPartCount(1)!=0))
        {
            _cutSize+=1;
        }
    }
    ///////////////////////////////////////
    

}


void Partitioner::myinsert(Node *n1)
{
   
    int mypart = _cellArray[n1->getId()]->getPart();
    int myval = _cellArray[n1->getId()]->getGain();



    if(mypart==0)
    {
        auto check = bmapA.insert({myval,n1});

        if(! check.second)
        {
            
            Node *exist = check.first->second;

            //exist->setPrev(n1);
            //n1->setNext(exist);
            //bmapA[myval]=n1;


            ///////////////////////////////////////////////
            if(exist->getNext())
            {
                exist->getNext()->setPrev(n1);
            }

            n1->setNext(exist->getNext());
            n1->setPrev(exist);

            exist->setNext(n1);
            /////////////////////////////////////////////////////////
        }
    }else
    {
        auto check = bmapB.insert({myval,n1});

        if(! check.second)
        {
            Node *exist = check.first->second;
            
            //exist->setPrev(n1);
            //n1->setNext(exist);
            //bmapB[myval]=n1;


            if(exist->getNext())
            {
                exist->getNext()->setPrev(n1);
            }

            n1->setNext(exist->getNext());
            n1->setPrev(exist);

            exist->setNext(n1);
        }
    }

    


}


void Partitioner::mydelete(Node *n2)
{
    
    int mypart = _cellArray[n2->getId()]->getPart();
    int myval = _cellArray[n2->getId()]->getGain();

    if(n2->getPrev()==NULL)
    {
        if(n2->getNext()==NULL)
        {
            if(mypart==0)
            {
                bmapA.erase(myval);
            }else
            {
                bmapB.erase(myval);
            }
        }else
        {
            if(mypart==0)
            {
                bmapA[myval]=n2->getNext();
                n2->getNext()->setPrev(NULL);
            }else
            {
                bmapB[myval]=n2->getNext();
                n2->getNext()->setPrev(NULL);
            }
        }
    }

    n2->remove();

}








void Partitioner::calIni()
{
    //vector<int> templist;
    int temppin=0;

    int fromm=0;
    int too=0;
    
    //int val=0;
    //bmapA;
    //bmapB;
    //ALargestVal;
    //BLargestVal;
    //Node *n3;




    
    //for(int i=0;i<_netNum;i++)
    //{
    //    _netArray[i]->reset();

    //}

    
    for(int i=0;i<_cellNum;i++)
    {
        _cellArray[i]->reset();
        temppin = _cellArray[i]->getPinNum();
        //templist = _cellArray[i]->getNetList();
        fromm = _cellArray[i]->getPart();
        too = 1-fromm;

        for(int j=0;j<temppin;j++)
        {

            
            if(_netArray[(_cellArray[i]->getNetList())[j]]->getPartCount(fromm)==1)
            {
                _cellArray[i]->incGain();
            }

            if(_netArray[(_cellArray[i]->getNetList())[j]]->getPartCount(too)==0)
            {
                _cellArray[i]->decGain();
            }
        }

        /////////////////////////////////////////
        //val = _cellArray[i]->getGain();
        //n3 = _cellArray[i]->getNode();

        myinsert(_cellArray[i]->getNode());



        //if(fromm==0)
        //{
            //if(val>ALargestVal)
            //{
            //    ALargestVal=val;
            //}
            
        //    myinsert(n3);
            //bmapA[val].insert(i);
            //if(_cellArray[i]->getGain()>bucketALV)
            //{
            //    bucketALV = _cellArray[i]->getGain();
            //    bucketAwhoL = i;
            //}
        //}else
        //{

            //if(val>BLargestVal)
            //{
            //    BLargestVal = val;
            //}
        //    myinsert(n3);
            //bmapB[val].insert(i);

            //if(_cellArray[i]->getGain()>bucketBLV)
            //{
            //    bucketBLV = _cellArray[i]->getGain();
            //    bucketBwhoL = i;
            //}
        //}
        /////////////////////////

    }

}

/////////////////////////////////////////////////////////////////////////////



////////////////////////////////////////////////////////////////////////////////////

void Partitioner::printSummary() const
{
    cout << endl;
    cout << "==================== Summary ====================" << endl;
    cout << " Cutsize: " << _cutSize << endl;
    cout << " Total cell number: " << _cellNum << endl;
    cout << " Total net number:  " << _netNum << endl;
    cout << " Cell Number of partition A: " << _partSize[0] << endl;
    cout << " Cell Number of partition B: " << _partSize[1] << endl;
    cout << "=================================================" << endl;
    cout << endl;
    return;
}

//void Partitioner::reportNet() const
//{
//    cout << "Number of nets: " << _netNum << endl;
//    for (size_t i = 0, end_i = _netArray.size(); i < end_i; ++i) {
//        cout << setw(8) << _netArray[i]->getName() << ": ";
//        vector<int> cellList = _netArray[i]->getCellList();
//        for (size_t j = 0, end_j = cellList.size(); j < end_j; ++j) {
//            cout << setw(8) << _cellArray[cellList[j]]->getName() << " ";
//        }
//        cout << endl;
//    }
//    return;
//}

//void Partitioner::reportCell() const
//{
//    cout << "Number of cells: " << _cellNum << endl;
//    for (size_t i = 0, end_i = _cellArray.size(); i < end_i; ++i) {
//        cout << setw(8) << _cellArray[i]->getName() << ": ";
//        vector<int> netList = _cellArray[i]->getNetList();
//        for (size_t j = 0, end_j = netList.size(); j < end_j; ++j) {
//            cout << setw(8) << _netArray[netList[j]]->getName() << " ";
//        }
//        cout << endl;
//    }
//    return;
//}

void Partitioner::writeResult(fstream& outFile)
{
    stringstream buff;
    buff << _cutSize;
    outFile << "Cutsize = " << buff.str() << '\n';
    buff.str("");
    buff << _partSize[0];
    outFile << "G1 " << buff.str() << '\n';
    for (size_t i = 0, end = _cellArray.size(); i < end; ++i) {
        if (_cellArray[i]->getPart() == 0) {
            outFile << _cellArray[i]->getName() << " ";
        }
    }
    outFile << ";\n";
    buff.str("");
    buff << _partSize[1];
    outFile << "G2 " << buff.str() << '\n';
    for (size_t i = 0, end = _cellArray.size(); i < end; ++i) {
        if (_cellArray[i]->getPart() == 1) {
            outFile << _cellArray[i]->getName() << " ";
        }
    }
    outFile << ";\n";
    return;
}

void Partitioner::clear()
{
    for (size_t i = 0, end = _cellArray.size(); i < end; ++i) 
    {
        delete _cellArray[i];
    }
    
    for (size_t i = 0, end = _netArray.size(); i < end; ++i) 
    {
        delete _netArray[i];
    }
    return;
}
