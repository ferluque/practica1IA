#include <iostream>
#include <vector>

using namespace std;

int main () {
    vector<vector<pair<int,int>>> casillasTerreno(8);
      for (int i=0; i<8; ++i)
        casillasTerreno.push_back(vector<pair<int,int>>(16));
      
      // Norte
      vector<pair<int,int>> posiciones = {
          pair<int,int>(0,0), 
          pair<int,int>(-1,-1), pair<int,int>(-1,0), pair<int,int>(-1,0), 
          pair<int,int>(-2,-2), pair<int,int>(-2,-1), pair<int,int>(-2,0), pair<int,int>(-2,1), pair<int,int>(-2,2), 
          pair<int,int>(-3,-3), pair<int,int>(-3,-2), pair<int,int>(-3,-1), pair<int,int>(-3,0), pair<int,int>(-3,1), pair<int,int>(-3,2), pair<int,int>(-3,3)
      };    
      casillasTerreno[0] = posiciones;
          
      // Noreste
      posiciones = {
          pair<int,int>(0,0), 
          pair<int,int>(-1,0), pair<int,int>(-1,1), pair<int,int>(0,1), 
          pair<int,int>(-2,0), pair<int,int>(-2,1), pair<int,int>(-2,2), pair<int,int>(-1,2), pair<int,int>(0,2), 
          pair<int,int>(-3,0), pair<int,int>(-3,1), pair<int,int>(-3,2), pair<int,int>(-3,3), pair<int,int>(-2,3), pair<int,int>(-1,3), pair<int,int>(0,3)
      };    
      casillasTerreno[1] = posiciones;

      // Este
      posiciones = {
          pair<int,int>(0,0), 
          pair<int,int>(-1,1), pair<int,int>(0,1), pair<int,int>(1,1), 
          pair<int,int>(-2,2), pair<int,int>(-1,2), pair<int,int>(0,2), pair<int,int>(1,2), pair<int,int>(2,2), 
          pair<int,int>(-3,3), pair<int,int>(-2,3), pair<int,int>(-1,3), pair<int,int>(0,3), pair<int,int>(1,3), pair<int,int>(2,3), pair<int,int>(3,3)
      };    
      casillasTerreno[2] = posiciones;

      // Sureste
      posiciones = {
          pair<int,int>(0,0), 
          pair<int,int>(0,1), pair<int,int>(1,1), pair<int,int>(1,0), 
          pair<int,int>(0,2), pair<int,int>(1,2), pair<int,int>(2,2), pair<int,int>(2,1), pair<int,int>(2,0), 
          pair<int,int>(0,3), pair<int,int>(1,3), pair<int,int>(2,3), pair<int,int>(3,3), pair<int,int>(3,2), pair<int,int>(3,1), pair<int,int>(3,0)
      };    
      casillasTerreno[3] = posiciones;

      // Sur
      posiciones = {
          pair<int,int>(0,0), 
          pair<int,int>(1,1), pair<int,int>(1,0), pair<int,int>(1,-1), 
          pair<int,int>(2,2), pair<int,int>(2,1), pair<int,int>(2,0), pair<int,int>(2,-1), pair<int,int>(2,-2), 
          pair<int,int>(3,3), pair<int,int>(3,2), pair<int,int>(3,1), pair<int,int>(3,0), pair<int,int>(3,-1), pair<int,int>(3,-2), pair<int,int>(3,-3)
      };    
      casillasTerreno[4] = posiciones;

      // Sin hacer: Suroeste
      posiciones = {
          pair<int,int>(0,0), 
          pair<int,int>(1,0), pair<int,int>(1,-1), pair<int,int>(0,-1), 
          pair<int,int>(2,0), pair<int,int>(2,-1), pair<int,int>(2,-2), pair<int,int>(1,-2), pair<int,int>(0,-2), 
          pair<int,int>(3,0), pair<int,int>(3,-1), pair<int,int>(3,-2), pair<int,int>(3,-3), pair<int,int>(2,-3), pair<int,int>(1,-3), pair<int,int>(0,-3)
      };    
      casillasTerreno[5] = posiciones;


      // Oeste
      posiciones = {
          pair<int,int>(0,0), 
          pair<int,int>(-1,-1), pair<int,int>(0,-1), pair<int,int>(1,-1), 
          pair<int,int>(-2,-2), pair<int,int>(-1,-2), pair<int,int>(0,-2), pair<int,int>(1,-2), pair<int,int>(2,-2), 
          pair<int,int>(-3,-3), pair<int,int>(-2,-3), pair<int,int>(-1,-3), pair<int,int>(0,-3), pair<int,int>(1,-3), pair<int,int>(2,-3), pair<int,int>(3,-3)
      };    
      casillasTerreno[6] = posiciones;

      // Sin hacer : Noroeste
      posiciones = {
          pair<int,int>(0,0), 
          pair<int,int>(0,-1), pair<int,int>(-1,-1), pair<int,int>(-1,0), 
          pair<int,int>(0,-2), pair<int,int>(-1,-2), pair<int,int>(-2,-2), pair<int,int>(-2,-1), pair<int,int>(-2,0), 
          pair<int,int>(0,-3), pair<int,int>(-1,-3), pair<int,int>(-2,-3), pair<int,int>(-3,-3), pair<int,int>(-3,-2), pair<int,int>(-3,-1), pair<int,int>(-3,0)
      };    
      casillasTerreno[7] = posiciones;
}