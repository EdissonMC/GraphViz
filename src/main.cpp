/*
    THIS PROGRAM RESOLVE THE Assignment 1: CS106L GraphViz FROM CS106L STANFORD COURSE
    THIS CODE IMPLEMENT Fruchterman-Reingold algorithm
*/


#include <iostream>
#include <fstream>
#include <cmath>
#include <string>
#include "SimpleGraph.h"

using namespace std;



//const double PI = 3.14159265358979323;

const double K_REPEL = 1e-3;
const double K_ATTRACT = 1e-3;


void Welcome();
void LoadGraphFromFile(SimpleGraph &g, ifstream& file);
void InitNodePositions(SimpleGraph& s);
void TuningMyGraph(SimpleGraph&);
void promptTypeGraph(ifstream & myFile);

int main() {


    /* initialize random seed: */
     srand (time(NULL));

    cout << "Starting.... GraphViz!" << endl;

    // CREATING OBJETC FOR THE GRAPH
    SimpleGraph myGraph;

    // LOADING FILE WITH DATA NODES -> CHECK res FOLDER
    ifstream myFile;
    promptTypeGraph(myFile);


    LoadGraphFromFile(myGraph, myFile);
    InitNodePositions(myGraph);
    InitGraphVisualizer(myGraph);
    DrawGraph(myGraph);

    double duration = 15;
    time_t startTime = time(NULL);

    while ( difftime(time(NULL), startTime) < duration ) {

        TuningMyGraph(myGraph);
        DrawGraph(myGraph);
    }

    return 0;
}





void promptTypeGraph(ifstream & myFile){
    string fileName="";
    cout << "File Name:>" ;
    while (getline(cin, fileName)) {
          myFile.open(fileName); //3grid //cube
         if(myFile.fail()) {
             cout << "Please reenter valid file name>" << endl;

         }else{
             break;
         }
    }
}





/*LoadGraphFromFile*/
void LoadGraphFromFile(SimpleGraph &g, ifstream& file) {
    cout << "\n>LoadGraphFromFile...." << endl;

    // READING THE NUMBERS OF NODES
    int n;
    file >> n;

    // RESIZING THE SIZE OF THE NODE VECTORS
    g.nodes.resize(n);

    // DEFING TEMPORAL VARIABLES FOR THE INDEX OF THE NODE
    size_t n1, n2;

    // FILLING THE EDGE WITH DATA FROM FILE
    while (file >> n1 >> n2)
    {

         g.edges.push_back( Edge{.start = n1, .end = n2} );
    }

    cout << "n/>Finish LoadGraphFromFile...." << endl;
}
// END LoadGraphFromFile()







/*InitNodePositions*/
void InitNodePositions(SimpleGraph& g) {
    size_t n = g.nodes.size();

    // INITIALIZATION NODE POSITION WITH RANDOM NUMBERS
    for (size_t i = 0; i < n; i++) {

        g.nodes[i].x = rand() % 7; // cos(2*PI*i/n);
        g.nodes[i].y = rand() % 7 ;// sin(2*PI*i/n);
          std::cout <<"asignando puntos aleatorios a los nodos " <<g.nodes[i].x <<" : "<< g.nodes[i].y <<std::endl;
    }


     // INITIALIZATION NODE POSITIONS SPACED APART ON THE UNIT CIRCLE
    /*
    for (size_t i = 0; i < n; i++) {

        g.nodes[i].x =  cos(2*PI*i/n);
        g.nodes[i].y =  sin(2*PI*i/n);
    }
    */

}







/* TuningMyGraph */
void TuningMyGraph(SimpleGraph& g) {

    vector<Node> nodoDeltas;
    nodoDeltas.resize(g.nodes.size());

    double x0=0, y0=0;
    double x1=0, y1=0;
    double Frepel=0, Fattract=0;
    double disX=0, disY=0;
    double theta=0;
    size_t totalEdges = g.edges.size();
    size_t totalNodes = g.nodes.size();

    // <---COMPUTING  REPULSIVE FORCES---->

    //for1
    for (size_t currentNodeItr = 0; currentNodeItr < totalNodes ; ++currentNodeItr ){

       // GET POINT FIRTS NODE OR NODE_A
        Node nodeA =g.nodes[ currentNodeItr ];
        x0 = nodeA.x;
        y0 = nodeA.y;

        //for2
        for (size_t itrNodes = currentNodeItr + 1  ;  itrNodes < totalNodes ; ++itrNodes ){

            // GET POINT SECOND NODE OR NODE_B
            Node nodeB =g.nodes[ currentNodeItr ];
            x1= nodeB.x;
            y1= nodeB.y;

            disX = x1-x0;
            disY = y1-y0;


            if ( ((disY > 0.00005) || (disY < -0.00005))  &&  ( (disX > 0.00005) || (disX < -0.00005))  ){

                // COMPUTING  REPULSIVE FORCES
                Frepel  =  K_REPEL / (   sqrt( pow(disX, 2)  +  pow(disY, 2) )   );

                theta = atan2(disY, disX);

                // NODO_A
                nodoDeltas[currentNodeItr].x -= Frepel * cos(theta); // deltaX0
                nodoDeltas[currentNodeItr].y -= Frepel * sin(theta); // deltaY0

                // NODO_B
                nodoDeltas[itrNodes].x += Frepel * cos(theta); // deltaX1
                nodoDeltas[itrNodes].y += Frepel * sin(theta); // deltaY1


            }//end if

          } //end for2

    }//end for1






   // < --- COMPUTING   ATTRACTIVE FORCES ---------------"

    for (size_t i = 0; i < totalEdges  ; i++) {


            Edge currentEdge = g.edges[i];
            Node nodeA= g.nodes[  currentEdge.start ];
            Node nodeB= g.nodes[  currentEdge.end ];

            // GET POINT FIRTS_NODE OR NODE_A
            x0= nodeA.x;
            y0= nodeA.y;

            // GET POINT SECOND_NODE OR NODE_B
            x1= nodeB.x;
            y1= nodeB.y;

            // COMPUTING DISTANCES BETWEEN NODE_A AND NODE_B
            disX = x1 - x0;  // X1 - X0
            disY = y1 - y0;  // Y1 - Y0

            // COMPUTING  ATTRACTIVE FORCES
            Fattract  =  K_ATTRACT * (  pow(disY, 2)   +   pow(disX, 2)  );

            theta = atan2(disY, disX);

            // UPDATING DELTAS DX0 DY0
            nodoDeltas[  g.edges[i].start ].x  += Fattract * cos(theta); // deltaX0;
            nodoDeltas[   g.edges[i].start ].y  += Fattract * sin(theta); // deltaY0;
            nodoDeltas[   g.edges[i].end   ].x  -= Fattract * cos(theta); // deltaX1;
            nodoDeltas[   g.edges[i].end   ].y  -= Fattract * sin(theta); // deltaY1;


     }//END For




    // <-------MOVING NODES ACCORDING TO THE FORCES ------>
    for (size_t i = 0; i < totalNodes; i++) {

         // UPDATING POSITION (X,Y) NODE
        g.nodes[i].x += nodoDeltas[i].x ;
        g.nodes[i].y += nodoDeltas[i].y;

    }



}// END TuningMyGraph


void Welcome() {
    cout << "Welcome to CS106L GraphViz!" << endl;
    cout << "This program uses a force-directed graph layout algorithm" << endl;
    cout << "to render sleek, snazzy pictures of various graphs." << endl;
    cout << endl;
}



