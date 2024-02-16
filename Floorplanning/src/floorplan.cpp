#include <fstream>
#include <iostream>
#include <cstdlib>
#include <list>
#include <vector>
#include <cmath>
#include <random>
#include <stack>
#include <algorithm>
#include "floorplan.h"
#include "module.h"
#include "bstar.h"

using namespace std;

void Floorplan::readblock(fstream& bf) 
{
    string ss="";
    bf>>ss;

    if(ss == "Outline:") 
    {
        bf >> ss;
        borderW = stoi(ss);

        bf >> ss;
        borderH = stoi(ss);
    }
    ///////////////////////////////////////////////
    bf>>ss;
    if(ss=="NumBlocks:")
    {
        bf>>ss;
        num_blk=stoi(ss);
    }
    ////////////////////////////////////////////////////
    bf>>ss;
    if(ss=="NumTerminals:") 
    {
        bf>>ss;
        num_term= stoi(ss);
    }
    ///////////////////////////////////////////////////////
    int termid=0;
    int blkid=0;

    string blkname="";
    size_t xcoo=0;
    size_t ycoo=0;

    string ystr="";


    while(bf>>blkname) 
    {
        bf>>ss;

        if(ss=="terminal")
        {
            bf>>ss;
            xcoo =stoi(ss);
            bf>>ss;
            ycoo =stoi(ss);

            Terminal *myterm = new Terminal(blkname,xcoo,ycoo);
            NameToTermId.insert({blkname, termid});
            TermBuff.push_back(myterm);

            termid+=1;
        }else
        {
            bf>>ystr;

            NameToBlkId.insert({blkname,blkid});
            size_t ww=stoi(ss);
            size_t hh=stoi(ystr);
            size_t temp=0;
            if(ww<hh)
            {
                temp=ww;
                ww=hh;
                hh=temp;
            }


            Block *myblk = new Block(blkname, ww, hh, blkid);

            BlockBuff.push_back(myblk);
            blkid++;
        }
    }
}
/////////////////////////////////////////////////

void Floorplan::readnet(fstream& nf)
{   
    int deg=0;
    string nam="";
    string s2="";
    nf>>s2;

    if(s2=="NumNets:") 
    {
        nf>>s2;
        num_net=stoi(s2);
    }

    while(nf>>s2)
    {
        if(s2=="NetDegree:")
        {
            
            nf>>s2;
            deg=stoi(s2);

            Net* mynet = new Net();

            for(int i=0;i<deg;i++)
            {
                nf >>nam;
                auto check = NameToTermId.find(nam);
                if(check!=NameToTermId.end())
                {
                    mynet->addTerm(TermBuff[check->second]);
                }else
                {
                    check = NameToBlkId.find(nam);
                    mynet->addTerm(BlockBuff[check->second]);
                }

            }
            NetBuff.push_back(mynet);
        }
    }
}
////////////////////////////////////////////

int Floorplan::checkfeasible(Node* pblkn, Node* myblkn, int pos,bool force=false)
{   
    //pos:0left 1right


    int result=0;
    size_t tx1=0;
    size_t tx2=0;
    size_t ty1=0;
    size_t ty2=0;
    
    bool condition1=false;
    bool condition2=false;


    if(myblkn!=nullptr)
    {


    



    //0:success 1:fail
    Block *myblk=BlockBuff[myblkn->getid()];
    

    if(pblkn==nullptr)
    {
        if(seeOK(myblk))
        {
            result =0;
        }else
        {
            myblk->rotate();
            if(seeOK(myblk))
            {
                result=2;
            }else
            {
                myblk->rotate();
                result=1;
            }
        }

        cout<<'a'<<endl;
    }else
    {

        cout<<"b"<<endl;
        //
        Block *pblk = BlockBuff[pblkn->getid()];
        if(pos==0)
        {
            tx1=pblk->getX2();
        }else
        {
            tx1=pblk->getX1();
        }

        tx2=tx1 + myblk->getWidth();


        for(auto& u: upline)
        {

            if(u->x >=tx1)
            {
                condition1=true;
            }else
            {
                condition1=false;
            }

            if(u->x <tx2)
            {
                condition2=true;
            }else
            {
                condition2=false;
            }
            ////////////////////////////////////////////////

            if((condition1) && (condition2))
            {
                if((u->y >ty1))
                {
                    ty1 = u->y;
                }
            }else if(!condition2)
            {
                break;
            }

        }
        /////////////////////////////
        ty2 = ty1+myblk->getHeight();

        if(((tx2<=borderW) && (ty2<=borderH)) || (force))
        {
            if(pos==0)
            {
                pblkn->setL(myblkn);
            }else
            {
                pblkn->setR(myblkn);
            }
            myblk->setPos(tx1,ty1,tx2,ty2);
            updateupline(myblk);

            result=0;
        }else
        {
            myblk->rotate();
            tx2=tx1+myblk->getWidth();
            ty1=0;

            for(auto& u: upline)
            {

                if(u->x >=tx1)
                {
                    condition1=true;
                }else
                {
                    condition1=false;
                }

                if(u->x <tx2)
                {
                    condition2=true;
                }else
                {
                    condition2=false;
                }
                ////////////////////////////////////////////////

                if((condition1) && (condition2))
                {
                    if((u->y >ty1))
                    {
                        ty1 = u->y;
                    }
                }else if(!condition2)
                {
                    break;
                }

            }
            ///////////////////////
            ty2 =ty1+myblk->getHeight();

            if((tx2<=borderW) && (ty2<=borderH))
            {
                if(pos==0)
                {
                    pblkn->setL(myblkn);
                }else
                {
                    pblkn->setR(myblkn);
                }
                myblk->setPos(tx1,ty1,tx2,ty2);
                updateupline(myblk);
                
                result =2;
            }else
            {
                myblk->rotate();
                result=1;
            }

        }

    }


    }


    return result;
}

bool Floorplan::seeOK(Block* bk)
{

    bool res=false;




    if((bk->getWidth()<=borderW) && (bk->getHeight()<=borderH))
    {
        res=true;
        bk->setPos(0,0,bk->getWidth(),bk->getHeight());
        root=bk->getnode();
        updateupline(bk);  
    }else
    {
        res=false;
    }


    return res;
}

void Floorplan::updateupline(Block* bk2)
{

    size_t tx1 = bk2->getX1();

    size_t ty1 = bk2->getY1();
    size_t tx2 = bk2->getX2();
    
    size_t ty2 = bk2->getY2();

    size_t temp=0;
    size_t temp2=0;

    bool check1 =true;
    bool check2 =false;

    bool condition1=false;
    bool condition2=false;
    bool condition3=false;


    if (upline.empty())
    {
        Coo *co1 = new Coo {tx1,ty2};
        Coo *co2 = new Coo {tx2,0};

        upline.push_back(co1);
        upline.push_back(co2);
    }
    else 
    {
        for(auto u= upline.begin(); u!=upline.end();)
        {
            temp2 = (*u)->y;

            if((*u)->x >=tx1)
            {
                condition1 = true;
            }else
            {
                condition1 =false;
            }

            if((*u)->x <=tx2)
            {
                condition2 = true;
            }else
            {
                condition2 =false;
            }

            if((*u)->y <ty2)
            {
                condition3=true;
            }else
            {
                condition3=false;
            }
            ////////////////////////////
            if((condition1) && (condition2))
            {
                if(condition3)
                {
                    u = upline.erase(u);
                    if(!check2)
                    {
                        Coo *co3= new Coo {tx1,ty2};
                        upline.emplace(u, co3);
                        check2=true;
                    }


                }else
                {
                    check1=false;
                    break;
                }

            }else if(!condition2)
            {
                Coo *co4= new Coo {tx2,temp};
                upline.emplace(u, co4);

                check1=false;
                break;

            }else
            {
                u++;
            }

            temp = temp2;


        }
        
        if(check1)
        {
            Coo *co6= new Coo {tx2,0};
            upline.push_back(co6);
        }
        ////////////////////////////////////////////////   
    }
    //////////////////////////////////////
    if (Block::getMaxX()<tx2)
    {
        Block::setMaxX(tx2);
    }
    if(Block::getMaxY()<ty2)
    {
        Block::setMaxY(ty2);
    }

}
//////////////////////////////////////////////////////
void Floorplan::delete_blk(Block* bk3)
{
    srand((unsigned) time(NULL));

    int random = (rand() % 2);

    Node *nn = bk3->getnode();

    bool condition1=false;
    bool condition2=false;

    if(nn->getL()!=nullptr)
    {
        condition1 =true;
    }else
    {
        condition1=false;
    }

    if(nn->getR()!=nullptr)
    {
        condition2 =true;
    }else
    {
        condition2=false;
    }

    while(condition1 && condition2)
    {
        if(random==0)
        {
            sw(bk3,BlockBuff[nn->getL()->getid()]);
        }else
        {
            sw(bk3,BlockBuff[nn->getR()->getid()]);
        }
        random = (rand() % 2);

        if(nn->getL()!=nullptr)
        {
            condition1 =true;
        }else
        {
            condition1=false;
        }

        if(nn->getR()!=nullptr)
        {
            condition2 =true;
        }else
        {
            condition2=false;
        }
        /////////////////////////////////////
    }

    if (nn==root) 
    {
        if(nn->getL() != nullptr)
        {
            root = nn->getL();
        }else
        {
            root = nn->getR();
        }
    }
    nn->remove();
}

void Floorplan::sw(Block *a, Block *b)
{
    if(a->getnode()==root)
    {
        root = b->getnode();
    }else if(b->getnode() == root)
    {
        root = a->getnode();
    }
    
    Node::swap(a->getnode(), b->getnode());
}
///////////////////////////////////////////////////////////
void Floorplan::put(Block *bk4)
{
    srand((unsigned) time(NULL));
    int random2 = (rand() % num_blk);
    int random3 = (rand() % 2);

    Node* nn = bk4->getnode();

    ////////////////////////////////////////////
    while(random2 ==nn->getid())
    {
        random2 = (rand() % num_blk);
    }

    /////////////////////////////////////////////////////
    int choice=0;

    if(random3==0)
    {
        choice=0;
    }else
    {
        choice=1;
    }

    ////////////////////////////////
    if(nn->getid()==random2)
    {
        root->setP(nn);
        if(root->getL()!=nullptr)
        {
            root->getL()->setP(nn);
        }
        nn->setR(root);
        nn->setL(root->getL());
        root->setL(nullptr);
        root = nn;
    }else
    {
        Node* temp = (BlockBuff[random2])->getnode();
        if(choice==0)
        {
            if(temp->getL() !=nullptr)
            {
                temp->getL()->setP(nn);
                nn->setL(temp->getL());
            }
        }else
        {
            if(temp->getR()!=nullptr)
            {
                temp->getR()->setP(nn);
                nn->setR(temp->getR());
            }
        }

        nn->setP(temp);
        if(choice==0)
        {
            temp->setL(nn);
        }else
        {
            temp->setR(nn);
        }
    }
    ////////////////////////////////
}
//////////////////////////////////////////////
void Floorplan::ops()
{

    srand((unsigned) time(NULL));
    int random2 = (rand() % num_blk);
    int random3 = (rand() % num_blk);

    while(random2==random3)
    {
        random3 = (rand() % num_blk);
    }

    int random4 = (rand() % 2);

    switch(random4)
    {
        case 0:
            (BlockBuff[random2])->rotate();
            break;
        case 1:
            sw(BlockBuff[random2], BlockBuff[random3]);
            break;
        case 2:
            delete_blk(BlockBuff[random2]);
            put(BlockBuff[random2]);
            break;
            
    }
}
///////////////////////////////////////////////////
void Floorplan::update() 
{

    stack<Node *> pump;

    upline.clear();
    
    Block::setMaxY(0);
    Block::setMaxX(0);
    //////////////////////////////////////////
    if (root!=nullptr) 
    {
        pump.push(root);
    }


    bool check=false;
    if(pump.empty())
    {
        check=true;
    }

    while(!check)
    {
        //////////////////////////////////////
        Node *blkn = pump.top();
        Block *blk =BlockBuff[blkn->getid()];
        
        pump.pop();

        if( blkn->getR()!= nullptr )
        {
            pump.push(blkn->getR());
        }

        if( blkn->getL()!= nullptr )
        {
            pump.push(blkn->getL());
        }

        if(blkn->getP() !=nullptr)
        {
            size_t tx1=0;
            size_t tx2=0;
            size_t ty1=0;
            size_t ty2=0;

            bool condition1 =false;
            bool condition2 =false;

            Block *pblk = BlockBuff[blkn->getP()->getid()];
            if(blkn->getP()->getL()==blkn)
            {
                tx1=pblk->getX2();
            }else
            {
                tx1=pblk->getX1();
            }

            tx2 = tx1 + blk->getWidth();
            ////////////////////////////////////////////
            size_t sasa=0;
            //ty1=0;

            for(auto& u: upline)
            {

                if(u->x >=tx1)
                {
                    condition1=true;
                }else
                {
                    condition1=false;
                }

                if(u->x <tx2)
                {
                    condition2=true;
                }else
                {
                    condition2=false;
                }
                ////////////////////////////////////////////////

                if((condition1) && (condition2))
                {
                    if((u->y >sasa))
                    {
                        sasa = u->y;
                    }
                }else if(!condition2)
                {
                    break;
                }

            }
            ty1=sasa;
            ///////////////////////
            ty2 =ty1+blk->getHeight();
            blk->setPos(tx1,ty1,tx2,ty2);

        }else
        {
            blk->setPos(0,0,blk->getWidth(),blk->getHeight());
        }
        updateupline(blk);
        /////////////////////////
        if(pump.empty())
        {
            check=true;
        }
    }
   
}




/////////////////////////////////////////////////////
void Floorplan::ini()
{

    list<Block*> inib(BlockBuff.begin(), BlockBuff.end());
    inib.sort([](Block *a,Block *b) { return a->getWidth() > b->getWidth();}); 

    int check=0;


    check = checkfeasible(nullptr, (inib.front())->getnode(),0);
    inib.pop_front();

    Node *prev=nullptr;
    Node *floorth = root;
    
    bool con=false;

    if(inib.empty())
    {
        con=true;
    }

    while(!con)
    {
        if(floorth==nullptr)
        {
            floorth=inib.front()->getnode();
            check = checkfeasible(prev,floorth, 1, true);//
            inib.pop_front();
        }

        auto currn = floorth;

        auto u =inib.begin();
        while(u != inib.end())
        {

            if((*u)!=nullptr)
            {
                check = checkfeasible(currn, (*u)->getnode(),0);
                if(check!=1)
                {
                    currn = (*u)->getnode();
                    u=inib.erase(u);
                }else
                {
                    u++;
                }
            }else
            {
                u++;
            }
        }
        

        prev=floorth;
        floorth= floorth->getR();

        if(inib.empty())
        {
            con=true;
        }
    }

    uBT();
    update();
    calcnorm();
    rBT();
}
////////////////////////////////////////////////////////
void Floorplan::calcnorm()
{
    int count=60;
    Anorm=1.0;
    Wnorm=1.0;

    double qq=0.0;

   for(int i=0;i<count;i++)
   {
        ops();
        update();

        qq=(double)(Block::getMaxX() * Block::getMaxY());
        
        Anorm += qq;
        
        double val1=0.0;
        for(int j=0;j<NetBuff.size();j++)
        {
            val1 += ((NetBuff[j])->calcHPWL());
        }
        
        Wnorm+=val1;

   }

    Anorm=Anorm/count;
    Wnorm=Wnorm/count;
}


////////////////////////////////////////////////////
void Floorplan::SA() 
{   
    double heat=1.0;
    int tim = num_blk*3;
    double rate = 0.99;
    double frozen = 0.01;
    
    //////////////////////////////////////////////////////
    int tim2=0;

    double  T=0;
    T =heat;

    uniform_real_distribution<double> unif(0.0,1.0);
    std::default_random_engine re;
    double a_random_double = unif(re);
    double myc=0.0;

    myc=calc();

    double c2=0.0;
    double diff=0.0;
    double pcost = 0.0;
    pcost =bcost;

    bool con1=false;

    while(T>frozen) 
    {
        rBT();

        for (int i=0;i<tim; i++)
        {
            ops();
            update();

            c2 = calc();
            diff=c2-pcost;
            a_random_double = unif(re);

            if(diff<=0) 
            {
                pcost=c2;
                uPT();

                if(c2<myc)
                {
                    myc=c2;
                    uBT();
                }
            }else if (a_random_double< exp(-diff/T))
            {
                pcost=c2;
                uPT();
            }else
            {
                rPT();
            }
        }
        /////////////////////////////////////
        if((Block::getMaxX()<=borderW) && (Block::getMaxY()<=borderH))
        {
            con1=true;
        }else
        {
            con1=false;
        }

        if (!con1 && (tim2<5))
        {
            T = heat;
            tim2+=1;
        }else{
            T= T*rate;
        }
    }

    rBT();
    update();
}

/////////////////////////////////////////////////////
void Floorplan::plan() 
{
    ini();
    SA();
}

//////////////////////////////////////////////////////////
void Floorplan::myprint(fstream& out_log, double exe_time)
{
    double area=0.0;
    area = Block::getMaxX()*Block::getMaxY();
    
    double val1=0.0;
    for(int j=0;j<NetBuff.size();j++)
    {
        val1+=((NetBuff[j])->calcHPWL());
    }
    
    out_log << _alpha*area + (1-_alpha)*val1 << endl;
    out_log << val1 << endl;
    out_log << area << endl;
    out_log << Block::getMaxX() << " " << Block::getMaxY() << endl;
    out_log << exe_time << endl;
    for(auto& block : BlockBuff)
    {
        out_log << block->getName() << " "
                << block->getX1() << " "
                << block->getY1() << " "
                << block->getX2() << " "
                << block->getY2() << endl;
    }
}