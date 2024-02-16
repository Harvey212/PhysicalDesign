#ifndef FLOORPLAN_H
#define FLOORPLAN_H

#include <vector>
#include <list>
#include "bstar.h"
#include <map>
#include <string>
#include "module.h"

using namespace std;

struct Coo
{
    size_t x;
    size_t y;
};


class Floorplan
{
    public:
        Floorplan(double alpha, std::fstream& blockfile, std::fstream& netfile):
        _alpha(alpha),root(nullptr),Anorm(1.0),Wnorm(1.0),num_net(0),num_term(0),num_blk(0),borderW(0.0),borderH(0.0),_ratio(1.0)
        ,penaltyA(0.0),penaltyR(0.0),penaltyW(0.0),Proot(nullptr),Broot(nullptr),bcost(0.0)
        {

            readblock(blockfile);
            readnet(netfile);
            _ratio = (double)borderW/(double)borderH;
        }
        ~Floorplan(){}

        void readblock(std::fstream& bf);
        void readnet(std::fstream& nf);
        void plan();
        void ini();
        int checkfeasible(Node* pblkn, Node* myblkn, int pos, bool force);
        bool seeOK(Block* bk);
        void updateupline(Block* bk2);
        void delete_blk(Block* bk3);
        void sw(Block *a, Block *b);
        void put(Block *bk4);
        void ops();
        void update(); 


        void calcnorm();


        double calc()
        {
            double cost=0.0;

            if((Block::getMaxX() <= borderW) && (Block::getMaxY() <=borderH))
            {
                penaltyA =0.0;
            }else
            {
                penaltyA =2.0;
            }

            if((Block::getMaxX() <= borderW) && (Block::getMaxY() <=borderH))
            {
                penaltyR =0.0;
            }else
            {
                penaltyR =2.0;
            }

            if((Block::getMaxX() <= borderW) && (Block::getMaxY() <=borderH))
            {
                penaltyW =1.0;
            }else
            {
                penaltyW =0.0;
            }

            double val1 = 0.0;
            double val2= 0.0;
            double cost1 =0.0;
            double cost2 =0.0;
            double cost3 =0.0;

            for(int i=0;i<NetBuff.size();i++)
            {
                val1+=((NetBuff[i])->calcHPWL());
            }

            ////////////////////////////////////////////
            //val2=abs( ((double)Block::getMaxX()/Block::getMaxY()) - _ratio);


            //cost1=penaltyW*(1-_alpha)*(val1/Wnorm);
            //cost2=(_alpha+penaltyA)*Block::getMaxX()*Block::getMaxY()*(1/Anorm);//
            //cost3=penaltyR*val2;

            

            cost1 = _alpha*Block::getMaxX()*Block::getMaxY()*(1/Anorm);
            cost2 = (1-_alpha)*(val1/Wnorm);

            cost = cost1+cost2;//+cost3;
            return cost;

        };

        void uPT()
        {

            

            if(root!=nullptr)
            {
                for(int i=0;i<BlockBuff.size();i++)
                {
                    (BlockBuff[i])->UP();
                }

                Proot =(BlockBuff[root->getid()])->getPN();
            }

        };


        void rPT()
        {

            if(Proot!=nullptr)
            {
                for(int i=0;i<BlockBuff.size();i++)
                {
                    (BlockBuff[i])->RP();
                }
                root =(BlockBuff[Proot->getid()])->getnode();
            }

            
        };

        void uBT()
        {


            if(root!=nullptr)
            {
                for(int i=0;i<BlockBuff.size();i++)
                {
                    (BlockBuff[i])->UB();
                }

                Broot =(BlockBuff[root->getid()])->getBN();
            }
            
        };


        void rBT()
        {

            if(Broot!=nullptr)
            {
                for(int i=0;i<BlockBuff.size();i++)
                {
                    (BlockBuff[i])->RB();
                }

                root =(BlockBuff[Broot->getid()])->getnode();
            }
            
        };


        void SA();
        void myprint(fstream& out_log, double exe_time);

    private:
        double _alpha;
        list<Coo*> upline;
        Node *root;
        map<string ,int> NameToBlkId;
        map<string ,int> NameToTermId;
        vector<Net*> NetBuff;
        vector<Terminal*> TermBuff;
        vector<Block*> BlockBuff;

        double Anorm;
        double Wnorm;

        int num_net;
        int num_term;
        int num_blk;

        int borderW;
        int borderH;

        double _ratio;

        double penaltyA;
        double penaltyR;
        double penaltyW;
        ///////////////////////////////////
        Node* Proot;
        Node* Broot;
        double bcost;




};






#endif