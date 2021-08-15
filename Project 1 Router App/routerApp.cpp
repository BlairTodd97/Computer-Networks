#include <stdio.h>
#include <iostream>
#include <fstream>
#include <vector>
#include <string>

using namespace std;

struct routingTable{
   int cost; 
   int nextHop;
   bool hasNeigh;
};


struct Node{
   int rounds;
   int lastChanged;
   int messages;
 //  int updates = 0;
  // int id;
 //  int neighbor;
   vector<routingTable> neighborTable;
//   Node() = default;
   };




int main( int argc, char* argv[] ){

   ifstream infile("../topology1.txt");

  vector<Node> network;
   int tempSize = 0;
   int nodeA = 0;
   int nodeB = 0;
   int link = 0;



   while(infile >> nodeA >> nodeB >> link){
        if(nodeA > nodeB)
          tempSize = nodeA;
        else
           tempSize = nodeB;

   if(tempSize > network.size())
      network.resize(tempSize + 1);

   if(nodeA >= network.at(nodeB).neighborTable.size())
      network.at(nodeB).neighborTable.resize(nodeA+1);

   if(nodeB >= network.at(nodeA).neighborTable.size())
      network.at(nodeA).neighborTable.resize(nodeB+1);  



//populate
   network.at(nodeA).neighborTable.at(nodeB).cost = link;
   network.at(nodeB).neighborTable.at(nodeA).cost = link;

   network.at(nodeA).neighborTable.at(nodeB).nextHop = nodeB;
   network.at(nodeB).neighborTable.at(nodeA).nextHop = nodeA;

   network.at(nodeA).neighborTable.at(nodeB).hasNeigh = true;
   network.at(nodeB).neighborTable.at(nodeA).hasNeigh = true;
     

   }




//  DV_Packet list;
//   int sum = 0;
   bool stable = true;
   while(stable){
      stable = false;
     std::vector<Node> shareTable(network);
      for(int i = 0; i < network.size(); i++){
         Node temp = shareTable.at(i);
         for(int j = 0; j < temp.neighborTable.size(); j++){
            if(temp.neighborTable.at(j).hasNeigh){
               if(temp.neighborTable.size() > shareTable.at(j).neighborTable.size()){
                  network.at(j).neighborTable.resize(temp.neighborTable.size());
                  shareTable.at(j).neighborTable.resize(temp.neighborTable.size());
               }

               for(int links = 0; links < temp.neighborTable.size(); links++){
                  if(links != i && links != j && temp.neighborTable.at(j).nextHop != -1){
                 int sum = temp.neighborTable.at(links).cost + temp.neighborTable.at(j).cost;
                  if(network.at(j).neighborTable.at(links).cost > sum){
                     network.at(j).neighborTable.at(links).cost = sum;
                     network.at(j).neighborTable.at(links).nextHop = i;
                     stable = true;
             //   list.rounds++;
             //       list.lastChanged = j;
                  }
                   
               }
              } 
             }
          //   list.rounds++;
           }
           network.at(i).rounds++;
          } 
         }
        



  cout << "node list is: [ ";
  for(int i = 0; i < network.size(); i++){
     cout << "'" << network.at(i) << "' '";
  }
  cout << "]";
  
  cout << "network converged in round: " << network.rounds;
  cout << "last node changed: " << network.lastChanged << endl;
  cout << "num messages per round: " << network.messages << endl;
  cout << "number of messages sent until convergence: " << networks.rounds * network.messages << endl;
  
  for(int i = 0; i < network.size(); i++){
     cout << "Node " << i << "'s routing table" << endl;
     for(int j = 0; j < network.at(i).neighborTable.size(); j++){
        cout << network.at(i).neighborTable.at(j) << " | " << network.at(i).neighborTable.at(j).cost << " | "  << network.at(i).neighborTable.at(j).nextHop << endl;
     }
  }

  







 
   return 0;                     
 }  


