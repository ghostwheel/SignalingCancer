// main2.cpp
//
//  main.cpp
//  Cancer3D
//
//  Created by Leonardo Ona Bubach on 04/11/2015.
//  Copyright © 2015 Leonardo Ona Bubach. All rights reserved.
//

// includes
#include <fstream>
#include <iostream>
#include "random.h" // for rnd::uniform()
#include "random.h" // for rnd::sample_1
#include <vector>
#include <vector>
#include <vector>

// Parameters
using namespace std;





//PARAMETERS

const double b=1.0;                         //Benefit
const double c=0.01;                        //Cost
//const int R=30;                           //Radius
const int Rmin=0;                           //Min Radius in a loop
const int Rmax=15;                          //Max Radius in a loop
const int Rstep=2;                          //R step in a loop
const int nR= ((Rmax-Rmin)/Rstep)+1;        //Number of data per Radius
const int L=7;                              //Range of local competition L
const int iSize=30;
const int jSize=30;

const int PopSize = iSize * jSize;
//const int first1=PopSize/2;               //Position of the unique mutant signalling cell in the begining of the simulation

const int nRuns=1000;                        //Number of runs


int MapVM[iSize][jSize]= {{ 0 }} ;

// Functions
// IntegerRandom
//The function "IntegerRandom" returns a random integer from min (inclusive) to max (exclusive): [min,max). When called with one argument gives [0,max)
int IntegerRandom(const int max, const int min=0)
{
  return static_cast<int>( rnd::uniform()*(max-min)+min );
}

// Sum
int Sum(const int *pnArray, const int nLength)
{
    int val =0;
    for ( int i=0; i<nLength; i++)
        val += pnArray[i];
    return val;
}

// SumArr2
int SumArr2(const bool pnArray[][jSize], const int nLength, const int mLength)
{
    int val =0;
    for (int i=0; i<nLength; i++){
        for (int j=0; j<mLength; j++) {
            val+=pnArray[i][j];
        }
    }
    return val;
}

// WithinCompartment
int WithinCompartment( int i, int j, int k)
{
  return (0 <= i) && (i < iSize ) &&
         (0 <= j) && (i < jSize )   ;
}

// MatMappingInitialization
void MatMappingInitialization(int (&MapVM)[iSize][jSize]){
    
    int index=0;
    for (int i=0; i<iSize; i++) {
        for (int j=0; j<jSize; j++) {
                MapVM[i][j] = index;
                index++;
            }
        
    }
}

// MatToVet
int MatToVet(const int ik, const int jk)
{
    int ret=0;
    int counn=0;
    for (int i=0; i<iSize; i++) {
        for (int j=0; j<jSize; j++) {
            if (i==ik && j==jk) {
                ret= counn;
            }
            counn++;
        }
    }
    return ret;
}


// VetToMat
int VetToMat(const int valueFromVector, const int ZeroOrOne)
{
  int MapVM[PopSize][3]={{ 0 }};
    
    int count2=-1;
    int count3=0;
    
    for (int i=0; i<PopSize; i++) {
        count2++;
        for (int j=0; j<3; j++) {
            if (j==0) {
                MapVM[i][j]=count2;
            } else
                if (j==1){
                    MapVM[i][j]= count3%iSize;
                    if (j==1 && count2%jSize == (jSize-1)) {
                        count3++;
                    }
                }
                else
                    if (j==2) {
                        MapVM[i][j]= count2%jSize;
                    }
        }
    }
    return MapVM[valueFromVector][ZeroOrOne+1];
}


// ChooseAnElement
int ChooseAnElement(const bool StateArray[iSize][jSize], const int R)
{
  // ChooseAnElement: initialise fitness array (MatrixFitness)
      //**************** Initialise Fitness Array ********************************
      double MatrixFitness[iSize][jSize] = {{ 0 }}; 

  // Calculate fitness
  #ifdef CUMULATIVE
    // Calculate fitness cumulative
    // set fitness to 1
    for (int i=0; i<iSize; i++){
      for (int j=0; j<jSize; j++) {
        MatrixFitness[i][j] = 1;
      }
    }

    // for every cell of type 1 add b to all neighbours
        //**************** This Part A: Cumulative Model ****************************
        for (int i=0; i<iSize; i++) {
            for (int j=0; j<jSize; j++) {
                if (StateArray[i][j]==1) {
                    for (int k = -R; k <= R ; k++) {
                        for (int l = -R ; l <= R ; l++) {
                            if ((i+k)>=0 && (i+k)<iSize && (j+l)>=0 && (j+l)<jSize) {
                              MatrixFitness[i+k][j+l] += b ; // was * StateArray[i][j]. but it is 1.
                            }
                        }
                    }
                }
            }
        }
    

    // subtract c from every cell of type 1
         for (int i=0; i<iSize; i++){
            for (int j=0; j<jSize; j++) {
                if (StateArray[i][j]==1) {
                  MatrixFitness[i][j]+= -(c);
                }
            }
        }
     


  #else
    // Calculate fitness non cumulative
    // set fitness to 1
    for (int i=0; i<iSize; i++){
      for (int j=0; j<jSize; j++) {
        MatrixFitness[i][j] = 1;
      }
    }

    // for every cell of type 1 set all neighbours to 1+b
       for (int i=0; i<iSize; i++) {
            for (int j=0; j<jSize; j++) {
                if (StateArray[i][j]==1) {
                    for (int k = -R; k <= R ; k++) {
                        for (int l = -R ; l <= R ; l++) {
                            if ((i+k)>=0 && (i+k)<iSize && (j+l)>=0 && (j+l)<jSize) {
                              MatrixFitness[i+k][j+l] = 1 + b; // was b, but should be 1+b.
                            }
                        }
                    }
                }
            }
        }
    

    // subtract c from every cell of type 1
         for (int i=0; i<iSize; i++){
            for (int j=0; j<jSize; j++) {
                if (StateArray[i][j]==1) {
                  MatrixFitness[i][j]+= -(c);
                }
            }
        }
     


  #endif

  // Choose random cell
  double FitnessArray[PopSize] = {0};
  // transform fitness matrix to fitness array
     //**************** Transforming Matrix Fitness to Array Fitness*************
      int counter=0;
      for (int i=0; i<iSize; i++) {
          for (int j=0; j<jSize; j++) {
              FitnessArray [counter]= MatrixFitness[i][j];
              counter++;
          }
      }
  

  // calculate (non normalised) CDF of fitness array
      //******************** Cumulative vector **************************
      double FitnessCDF[PopSize]={0};
      for (int i=0; i<PopSize; i++){FitnessCDF[i]  =FitnessArray[i];}
      for (int i=1; i<PopSize; i++){FitnessCDF[i] += FitnessCDF[i-1];}
  

  // sample 1 random cell using CDF
      //******************** Return an Element weighted sampled **************************
      return rnd::sample_1( FitnessCDF, PopSize);


}

// NeighbourWithinL
int NeighbourWithinL(const int addresoffocal)
{
   int a[2]={0,0};
    do{
        a[0] = VetToMat(addresoffocal,0) - L + IntegerRandom(2*L+1);
        a[1] = VetToMat(addresoffocal,1) - L + IntegerRandom(2*L+1);
    } while ((a[0] == VetToMat(addresoffocal,0) && a[1] == VetToMat(addresoffocal,1))
             || a[0]<0         || a[1]<0
             || a[0]>(iSize-1) || a[1]>(jSize-1));
    return MatToVet( a[0], a[1]);
}

// Replacement
void Replacement( bool (&MatrixSt)[iSize][jSize], 
                  const int focal_Vet, const int replace_Vet) 
{
  //    call_count++; I moved this out
    
    // If the focal and replace are different, do the replacement
  int focal[2] = { VetToMat( focal_Vet  , 0),
                   VetToMat( focal_Vet  , 1) } ;
  int replace[2] = { VetToMat( replace_Vet, 0),
                     VetToMat( replace_Vet, 1) } ;
  bool focal_type =   (MatrixSt)[ focal[0]   ][ focal[1]   ] ;
  bool replace_type = (MatrixSt)[ replace[0] ][ replace[1] ] ;
  
  if( focal_type != replace_type) {
        if ( focal_type == 1) {
          
          // Calculate number of immediate neighbours of type 1 for each cell of type 0
          vector<int> VectorZeroAddress;
          vector<int> VectorNOnes;
          
          // Find places in MatrixSt that are 0 and have 
          // neighbours that are 1.
          // Store the place and how many neighbours are 1 in VectorNOnes.
          // neighbourhood is -1, 0, +1.
          for (int i=0; i<iSize; i++) {
            for (int j=0; j<jSize; j++) {
              if (MatrixSt[i][j]==0) {
                int counter2=0;
                for (int k=-1+i; k<2+i; k++) {
                  for (int l=-1+j; l<2+j; l++) {
                    if (k>=0 && k < iSize && l>=0 && l<jSize && MatrixSt[k][l]==1)
                      counter2++;
                  }
                }
                if (counter2 !=0) {
                  VectorZeroAddress.push_back(MatToVet(i,j));
                  VectorNOnes.push_back(counter2);
                }
              }
            }
           }
          
          //############################
          

          // Find the cells with the maximal number of neighbours of type 1
          vector<int> VectorAddresMaximum;
          
          // Among VectorNOnes, find those that are equal to the maximum,
          // and put those on VectorAddresMaximum.
          // Notice that max_element is computed every step of the loop,
          // though it could be computed just once.
          for (unsigned int i=0; i < (VectorZeroAddress.size()); i++) {
            if ( VectorNOnes[i] == *max_element( begin( VectorNOnes), end( VectorNOnes) ) ) {
              VectorAddresMaximum.push_back( VectorZeroAddress[ i]);
            }
          }
                      

          
           // choose one of them, those with the maximal #of neighbours diff from them.
           int choo= VectorAddresMaximum[ IntegerRandom(static_cast<int>(VectorAddresMaximum.size()))];

           // Finally, set that one to 1, we replicated.
           MatrixSt[VetToMat(choo, 0)][VetToMat(choo, 1)] = 1 ;
            
           // Erase the vectors. ### why is this needed? Won't they be erased on their own?
           VectorZeroAddress.erase (VectorZeroAddress.begin(), VectorZeroAddress.begin()+VectorZeroAddress.size());
           VectorNOnes.erase (VectorNOnes.begin(), VectorNOnes.begin()+VectorNOnes.size());
           VectorAddresMaximum.erase (VectorAddresMaximum.begin(), VectorAddresMaximum.begin()+VectorAddresMaximum.size());
        } else { // if the focal isn't 1, i.e. if the focal is 0.
          // Calculate number of immediate neighbours of type 0 for each cell of type 1
          vector<int> VectorOneAddress;
          vector<int> VectorNZeros;
          
          // Find places in MatrixSt that are 1 and have 
          // neighbours that are 0.
          // Store the place and how many neighbours are 0 in VectorNZeros.
          // neighbourhood is -1, 0, +1.
          
          for (int i=0; i<iSize; i++) {
            for (int j=0; j<jSize; j++) {
              if (MatrixSt[i][j]==1) {
                int counter2=0;
                for (int k=-1+i; k<2+i; k++) {
                  for (int l=-1+j; l<2+j; l++) {
                    if (k>=0 && k < iSize && l>=0 && l<jSize && MatrixSt[k][l]==0)
                      counter2++;
                  }
                }
                if (counter2 !=0) {
                  VectorOneAddress.push_back(MatToVet(i,j));
                  VectorNZeros.push_back(counter2);
                }
              }
            }
           }
          
          //############################
          

          // Find the cells with the maximal number of neighbours of type 0
          vector<int> VectorAddresMaximum;
          
          // Among VectorNZeros, find those that are equal to the maximum,
          // and put those on VectorAddresMaximum.
          // Notice that max_element is computed every step of the loop,
          // though it could be computed just once.
          
          for (unsigned int i=0; i< (VectorOneAddress.size()); i++) {
            if( VectorNZeros[i] == *max_element( begin(VectorNZeros), end(VectorNZeros) )) {
              VectorAddresMaximum.push_back( VectorOneAddress[ i] );
            }
          }
                      


          // chose a random maximal cell
          int choo=VectorAddresMaximum[IntegerRandom(static_cast<int>(VectorAddresMaximum.size()))];

          // set it to 0, i.e. to the value of the focal
            MatrixSt[VetToMat(choo, 0)][VetToMat(choo, 1)] = 0 ;
            
            
            VectorOneAddress.erase (VectorOneAddress.begin(), VectorOneAddress.begin()+VectorOneAddress.size());
            VectorNZeros.erase (VectorNZeros.begin(), VectorNZeros.begin()+VectorNZeros.size());
            VectorAddresMaximum.erase (VectorAddresMaximum.begin(), VectorAddresMaximum.begin()+VectorAddresMaximum.size());
            
            
            
        }
        
    }
    
}


int main() {
   ofstream outputfile ("output.csv");
   
   // main program inits
       MatMappingInitialization(MapVM);

   
   // main loop stats init
   double fixProbR[ nR ] ={ 0.0 };
   double avFixTimeR[ nR ]={ 0.0 };

  
   // main loop
   for (int R=Rmin; R <= Rmax ; R=R+Rstep) {
   
     int whoWon[nRuns]={0};
     int fixTime[nRuns]={0};
           
     for (int i=0; i<nRuns; i++){
       // main loop single run
         // initialise compartment and introduce single mutant
         bool MatrixState [ iSize   ][ jSize   ] = {{ 0 }} ;
         MatrixState [ iSize/2 ][ jSize/2 ]   =   1   ;   // Mutant

         static unsigned int call_count = 0;
         do {
           // Single moran step
           // Choose *reproducer* according to fitness
           int reproducer = ChooseAnElement( MatrixState, R);

           // Choose random cell within range L *to be replaced*
           int to_be_replaced = NeighbourWithinL( reproducer ) ;

           // Find cell of the same type as *to be replaced*, having maximum neigbours of same type as the *reproducer*, and replace it instead
             Replacement( MatrixState, reproducer, to_be_replaced);


           call_count++ ;
         } while( SumArr2(MatrixState, iSize, jSize ) != 0     && 
                  SumArr2(MatrixState, iSize, jSize ) != PopSize    );
                // Notice that here SumArr3 is called twice. Also, in a single step, only one replacement takes
                // place, and yet the 6x6x6 sum is calculated again (twice)

       // record who won and fixation time
       whoWon[i]  = MatrixState[1][1] ;
       fixTime[i] = call_count;

     }
     
      // collect stats and print results
      //****The following vector save cases where 1 reach fixation*****
              
      int OnlyCasesOneFixTime[nRuns]={0};
      
      int Counter1=0;
      
      for (int i=0; i<nRuns; i++) {
        if(whoWon[i]==1){ // mutant fixed
          Counter1++;
          OnlyCasesOneFixTime[i]=fixTime[i];
        }
       } // End of the For loop for Runs values
      //****************************************************************
      
      fixProbR[   (R-Rmin) / Rstep ]= (double(Sum( whoWon,              nRuns))/nRuns);
      avFixTimeR[ (R-Rmin) / Rstep ]= (double(Sum( OnlyCasesOneFixTime, nRuns))/Counter1)/PopSize;
      
      
      cout << endl;
      cout << endl;
      
      
      cout << "Radius:"<< endl;
      for (int i= Rmin; i <= Rmax; i= i+Rstep) {
        cout << i << " ";
      }
      
      cout << endl;
      cout << endl;
      
      
      
      cout << "Fix Prob dep R:"<< endl;
      for (int i=0; i<=(Rmax-Rmin)/Rstep; i++) {
        cout << fixProbR[i] << " ";
       }
      
      cout << endl;
      cout << endl;
      
      cout << "Av Fix Time dep R:"<< endl;
      for (int i=0; i<=(Rmax-Rmin)/Rstep; i++) {
        cout << avFixTimeR[i] << " ";
      }
      
      
      
      cout << endl;
      cout << endl;
      cout << endl;
      
      
      
      for (int i=0; i<=(Rmax-Rmin)/Rstep; i++)  {
        outputfile << i << "," << fixProbR[i] << "," << avFixTimeR[i] << endl;
      }

   }  


   outputfile.close();
    
   return 0;
}

