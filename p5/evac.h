// Author: Sean Davis
#ifndef evacH
#define evacH

#include "EvacRunner.h"

class CityInfo
{
public:
      bool known, deadEnd, evacuatedCity;
      int *edgeID;
      int maxPeople;
      int evacPeople;
      char numEdges;
      int depth;
      bool visited;
      CityInfo():known(false), deadEnd(true), evacuatedCity(false), evacPeople(0), visited(false) {}
};

class RoadEdge
{
public:
      int destCityID;
      int weight;
      int used;
      RoadEdge():used(0) {}
};

class EvacInfo
{
public:
      int ID;
      float ratio;
};


class Evac
{
public:

    CityInfo *cities;
    RoadEdge *edge;
    EvacInfo *evacCities;
    int numCities;
    int hour;
    int *evacQ, back, front;
    int *visitedIDs, visitedSize;
    int *usedRoads, usedCount;
	/*
    City* cities;;
    int numCities;
    MaxFlow *maxflow; */
    /*
    int numRoads;
    int evacID;
    int currentCity;
    int timer; //need to set it equal to 1*/
	int dfs(int cityID, int needed, int sourceCityID);
   	Evac(City *cities, int numCities, int numRoads);
  	void evacuate(int *evacIDs, int numEvacs, EvacRoute *evacRoutes, int &routeCount); // student sets evacRoutes and routeCount

//    void Graph(int numCities);
//    void bfs(int *evacIDs, int numEvacs);
}; // class Evac



#endif
