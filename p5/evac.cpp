#include <cstdlib>
#include "evac.h"
#include "EvacRunner.h"

using namespace std;

int compare(const void *a, const void *b)
{
    if(((EvacRoute4*)a)->numPeople > ((EvacRoute4*)b)->numPeople) {return -1;}
    else if(((EvacRoute4*)a)->numPeople < ((EvacRoute4*)b)->numPeople) {return 1;}
    else {return 0;}
}

int Evac::dfs(int cityID, int required, int source)
{
    int sum = 0;
    int capacityMax = 0;

    if(cities[cityID].visited == false)
    {
        cities[cityID].visited = true;
        visitedIDs[visitedSize++] = cityID;
    }
    else
    {
        return 0;
    } // store as visited


    if(cities[cityID].cityEvac == false) // if not marked evac then return amount
    {
        if(cities[cityID].population - cities[cityID].evacuees <= required)
        {
            sum = cities[cityID].population - cities[cityID].evacuees;
            cities[cityID].evacuees = cities[cityID].population;
        }
        else
        {
            cities[cityID].evacuees = cities[cityID].evacuees + required;
            return required;
        }
    }

    int index = 0;

    while((index < cities[cityID].roadCount) && (sum < required))
    {

        capacityMax = roads[cities[cityID].edgeID[index]].peoplePerHour - roads[cities[cityID].edgeID[index]].occupied;
        if(capacityMax > required - sum)
        {
            capacityMax = required - sum;
        }
        capacityMax = dfs(roads[cities[cityID].edgeID[index]].destinationCityID, capacityMax, cityID);
        roads[cities[cityID].edgeID[index]].occupied = roads[cities[cityID].edgeID[index]].occupied + capacityMax;
        sum = sum + capacityMax;
        if(0 < capacityMax)
        {
            roadsOccupied[count++] = cities[cityID].edgeID[index];
        }
        index++;
    }
    return sum;
}

Evac::Evac(City *citie, int numCitie, int numRoads) : numCities(numCitie)
{
    roadsOccupied = new int[numRoads * 5]; // each road max 5
    visitedIDs = new int[numCities];
    numCities = numCitie;
    cities = new City4[numCities];
    roads = new Road4[numRoads * 2];

    int index = 0;
    while (index < numCitie)
    {
        cities[citie[index].ID].evacuees = 0;
        cities[citie[index].ID].population = citie[index].population;
        cities[citie[index].ID].roadCount = citie[index].roadCount;
        cities[citie[index].ID].edgeID = new int[citie[index].roadCount];

        int index2 = 0;
        while(index2 < citie[index].roadCount)
        {
            cities[citie[index].ID].edgeID[index2] = citie[index].roads[index2].ID;
            roads[citie[index].roads[index2].ID].destinationCityID = citie[index].roads[index2].destinationCityID;
            roads[citie[index].roads[index2].ID].peoplePerHour =  citie[index].roads[index2].peoplePerHour;
            index2++;
        }
        index ++;
    }

} // Evac()


void Evac::evacuate(int *evacIDs, int numEvacs, EvacRoute *evacRoutes,
    int &routeCount)
{
    int capacity;
    routeCount = 0;
    queueF = 0;
    queueB = 0;
    timer = 1;
    queue = new int[numCities];
    evacCities = new EvacRoute4[numEvacs];
    bool evacMore = true;
    visitedSize = 0;

    int index = 0;
    while(index < numEvacs)
    {
        cities[evacIDs[index]].cityEvac = true;
        queue[queueB] = evacIDs[index];
        evacCities[index].ID = evacIDs[index];
        queueB++;
        index++;
    } // enqueue evac cities

    while(evacMore == true)
    {
        int ID = 0;
        int queueB2 = queueB;

        while(queueF < queueB2)
        {
            int x = 0;
            ID = queue[queueF++];
            while(x < cities[ID].roadCount)
            {

                int time2 = timer - 1;

                if(time2 > 1)
                {
                   cities[ID].roadCount--;
                   cities[ID].edgeID[cities[ID].roadCount] = cities[ID].edgeID[x];
                   cities[ID].edgeID[x] = cities[ID].edgeID[cities[ID].roadCount];
                } // remove edge
                else {
                  x++;
                }
            } // for connected edges
        } // while queue not end
        if(evacMore == true)
        {
              int index = 0; //mark false if done
              evacMore = false;
              while(index < numEvacs)
              {
                  if(cities[evacCities[index].ID].evacuees < cities[evacCities[index].ID].population)
                  {
                      capacity = 0;
                      int index2 = 0;
                      evacMore = true;
                      while(index2 < cities[evacCities[index].ID].roadCount)
                      {
                          if(roads[cities[evacCities[index].ID].edgeID[index2]].peoplePerHour > cities[roads[cities[evacCities[index].ID].edgeID[index2]].destinationCityID].population){
                                capacity += cities[roads[cities[evacCities[index].ID].edgeID[index2]].destinationCityID].population;
                                index2++;
                          }
                          else {
                                capacity += roads[cities[evacCities[index].ID].edgeID[index2]].peoplePerHour;
                                index2++;
                          }

                      } // calculate each edge capacity
                      evacCities[index].numPeople = (cities[evacCities[index].ID].population - cities[evacCities[index].ID].evacuees) / capacity;
                  }
                  index ++;
              } // while remaining cities
              if(evacMore == true)
              {
                  qsort(evacCities, numEvacs, sizeof(EvacRoute4), compare); // sort by max ratio
              }
        }
        count = 0;
        int index = 0;
        while(index < numEvacs)
        {
            int sum = 0;
            int required = cities[evacCities[index].ID].population - cities[evacCities[index].ID].evacuees;
            while(visitedSize > 0)
            { // mark backroad not visited
                cities[visitedIDs[--visitedSize]].visited = false;
            }
            cities[evacCities[index].ID].visited = true;
            int capacityMax;
            int index2 = 0;
            visitedIDs[visitedSize++] = evacCities[index].ID;
            while(index2 < cities[evacCities[index].ID].roadCount && sum < required)
            {
                capacityMax = roads[cities[evacCities[index].ID].edgeID[index2]].peoplePerHour - roads[cities[evacCities[index].ID].edgeID[index2]].occupied;
                if(capacityMax > (required - sum))
                {
                    capacityMax = required - sum;
                }
                capacityMax = dfs(roads[cities[evacCities[index].ID].edgeID[index2]].destinationCityID, capacityMax, evacCities[index].ID);
                roads[cities[evacCities[index].ID].edgeID[index2]].occupied = roads[cities[evacCities[index].ID].edgeID[index2]].occupied + capacityMax;
                sum = sum + capacityMax;
                if(capacityMax > 0)
                {
                    roadsOccupied[count++] = cities[evacCities[index].ID].edgeID[index2];
                }
                index2++;
            }
            cities[evacCities[index].ID].evacuees = cities[evacCities[index].ID].evacuees + sum;
            index++;
        }
        index = 0;
        while(index<numCities)
        {
            cities[index].visited = false;
            index++;
        }

        index = 0;
        while(index < count)
        {
            evacRoutes[routeCount].numPeople = roads[roadsOccupied[index]].occupied;
            evacRoutes[routeCount].roadID = roadsOccupied[index];
            cities[roads[roadsOccupied[index]].destinationCityID].visited = false;
            roads[roadsOccupied[index]].occupied = 0;
            evacRoutes[routeCount++].time = timer;
            index++;
        }
          timer++;
    }

} // evacuate
