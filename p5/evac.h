// Author: Sean Davis
#ifndef evacH
#define evacH

#include "EvacRunner.h"

class Road4
{
public:
      int destinationCityID;
      int peoplePerHour;
      int occupied;
};


class City4
{
public:
      int *edgeID;
      int population;
      int evacuees;
      int roadCount;
      bool visited;
      bool cityEvac;
};

class EvacRoute4
{
public:
      int ID;
      float numPeople;
};


class Evac
{
public:

    City4 *cities;
    Road4 *roads;
    EvacRoute4 *evacCities;
    int numCities;
    int timer;
    int *queue;
    int queueB;
    int queueF;
    int *visitedIDs;
    int visitedSize;
    int *roadsOccupied;
    int count;

	int dfs(int cityID, int needed, int source);
   	Evac(City *cities, int numCities, int numRoads);
  	void evacuate(int *evacIDs, int numEvacs, EvacRoute *evacRoutes, int &routeCount); // student sets evacRoutes and routeCount

}; // class Evac



#endif
