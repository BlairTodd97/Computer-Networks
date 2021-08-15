#include <iostream>
#include <vector>
#include <fstream>
#include <string>

using namespace std;



struct Neighbor{
    int cost = 0;
    int nextHop = -1;
    bool hasNext = false;
};


struct Node{
   int rounds = 0;
	std::vector<Neighbor> neighborTable;
};


int main(int argc, char** argv){   
   

  //prepare input files 
     std::string inFile = "";
        

  if(argc < 2){
    cout << "please put input file in command line " << endl;
    exit(-1);
    }    
    
   
    if(argc == 2){
    inFile = argv[1];
    }

    if(argc > 2){
    cout << "only pass input file " << endl;
    exit(-1);
    }
   
    
    
   std::ifstream topology(inFile);
   
   // create node network and counter variables
   std::vector<Node> network;
   int nodeA = 0;
   int nodeB = 0;
   int link = 0;
   int lastNode = 0; 
   int messages = 0;

    

   // read in from file and fill vectors
   while(topology >> nodeA >> nodeB >> link){
      int max = 0;

      if(nodeA > nodeB)
         max = nodeA;

      else max = nodeB;

     if(max >= network.size())
        network.resize(max+1);

     if(nodeB >= network.at(nodeA).neighborTable.size())
        network.at(nodeA).neighborTable.resize(nodeB+1);
     if(nodeA >= network.at(nodeB).neighborTable.size())
        network.at(nodeB).neighborTable.resize(nodeA+1);


     network.at(nodeA).neighborTable.at(nodeB).cost = link;
     network.at(nodeA).neighborTable.at(nodeB).nextHop = nodeB;
     network.at(nodeA).neighborTable.at(nodeB).hasNext = true;
     
     network.at(nodeB).neighborTable.at(nodeA).cost = link;
     network.at(nodeB).neighborTable.at(nodeA).nextHop = nodeB;
     network.at(nodeB).neighborTable.at(nodeA).hasNext = true;
}





    // prepare for DV updates
     bool updated = true;
     while(updated){
     updated = false;
     std::vector<Node> DV_table(network);

     for(int i = 0; i < network.size(); i++){
        Node tempNode = DV_table.at(i);
        // create and populate the DV table with network info
        for(int j = 0; j < tempNode.neighborTable.size(); j++){
           if(tempNode.neighborTable.at(j).hasNext){
              if(tempNode.neighborTable.size() > DV_table.at(j).neighborTable.size()){
                 network.at(j).neighborTable.resize(tempNode.neighborTable.size());
                 DV_table.at(j).neighborTable.resize(tempNode.neighborTable.size());
              }
         //scan through table and find better routes/link costs
         for(int links = 0; links < tempNode.neighborTable.size(); links++){
            if(i!=links && j != links && tempNode.neighborTable.at(links).nextHop != -1){
               int sum = tempNode.neighborTable.at(links).cost + tempNode.neighborTable.at(j).cost;
               if(sum < network.at(j).neighborTable.at(links).cost){
                  network.at(j).neighborTable.at(links).cost = sum;
                  network.at(j).neighborTable.at(links).nextHop = i;
                  lastNode = i;
                  updated = true;
                  
               }
             
           }
            
        }
       messages++;
     }
   }
 network.at(i).rounds++;
}
}
   int maxrounds = 0;
   for(int i =0; i < network.size(); i++){
      if(maxrounds < network.at(i).rounds)
         maxrounds = network.at(i).rounds;
   }


//output 
   cout << "node list: " ;
   for(int i = 0; i < network.size(); i++){
   cout << i << " ";
   }
   cout << "\nnetwork converged at round: " << maxrounds << endl;
   cout << "last node changed: " << lastNode  << endl;
   cout << "number messages per round: " << messages << endl;
   cout << "number of messages until convergence: " << (messages * maxrounds) << endl;    
   for(int i = 0; i < network.size(); i++){
      cout << "Node " << i << " table" << endl;
      for(int j = 0; j < network.at(i).neighborTable.size(); j++){
         if(network.at(i).neighborTable.at(j).nextHop != -1)
         cout << j << " | " << network.at(i).neighborTable.at(j).cost << " | " << network.at(i).neighborTable.at(j).nextHop << endl;
     }
   }
  

    int searchNode = 0;
    if(inFile == "topology1.txt"){
        searchNode = 3;
        cout << "Sending packet from node 0 to node 3." << endl;
    }

    if(inFile == "topology2.txt"){
       searchNode = 7;
       cout << "Sending packet from node 0 to node 7." << endl;
    } 
   
    if(inFile == "topology3.txt"){
       searchNode = 23;
       cout << "Sending packet from node 0 to node 23." << endl;
    }
     
     int from = -1;
     
     
        
     for(int i = 0; i < network.size(); i++){
        for(int j = 0; j < network.at(i).neighborTable.size(); j++){
          if(network.at(i).neighborTable.at(j).nextHop == searchNode && network.at(i).neighborTable.at(j).nextHop != from){
             cout << "taking edge: " << i << " " << j << endl;
             from = i;
             i++;
    
          }
         }
       }

	             
   

return 0;
}
 


