#include <cstdlib>
#include "evac.h"
#include "EvacRunner.h"

using namespace std;

int RatioCmp(const void *ptr, const void *ptr2)
{
    if(((EvacInfo*)ptr)->ratio > ((EvacInfo*)ptr2)->ratio)

        return -1;

    else

    if(((EvacInfo*)ptr)->ratio < ((EvacInfo*)ptr2)->ratio)

        return 1;

    else

        return 0;
}

Evac::Evac(City *citie, int numCitie, int numRoads) : numCities(numCitie)
{

    cities = new CityInfo[numCities];

    edge = new RoadEdge[numRoads * 2];

    usedRoads = new int[numRoads * 5]; // each road max 5

    visitedIDs = new int[numCities];

    int id, numEdges;

    for(int i = 0; i < numCitie; i++)
    {

        id = citie[i].ID;

        cities[id].evacPeople = 0;

        cities[id].maxPeople = citie[i].population;

        cities[id].numEdges = numEdges = citie[i].roadCount;

        cities[id].edgeID = new int[numEdges];

        int roadID;

        for(int j = 0; j < numEdges; j++)
        {

            cities[id].edgeID[j] = roadID = citie[i].roads[j].ID;

            edge[roadID].destCityID = citie[i].roads[j].destinationCityID;

            edge[roadID].weight =  citie[i].roads[j].peoplePerHour;

        }
    }

} // Evac()


void Evac::evacuate(int *evacIDs, int numEvacs, EvacRoute *evacRoutes,
    int &routeCount)
{

    hour = 1;

    routeCount = 0;

    evacQ = new int[numCities];

    front = back = 0;

    evacCities = new EvacInfo[numEvacs];

    int id;

    for(int i = 0; i < numEvacs; i++)
    {

        id = evacIDs[i];

        cities[id].known = true;

        cities[id].deadEnd = false;

        cities[id].evacuatedCity = true;

        cities[id].depth = 1;

        evacQ[back] = id;

        evacCities[i].ID = id;

        back++;

    } // enqueue evac cities

    bool peopleRemaining = true;

    visitedSize = 0;

    while(peopleRemaining)
    {

        int back2 = back, ID, destCityID;

        for(int i = 0; i < back; i++)

            cities[evacQ[i]].deadEnd = false;

        while(front < back2)
        {

            ID = evacQ[front++];

            for(int j = 0; j < cities[ID].numEdges;)
            {
                destCityID = edge[cities[ID].edgeID[j]].destCityID;

                if(!cities[destCityID].known)
                {
                    evacQ[back++] = destCityID;

                    cities[ID].depth = hour + 1;

                    cities[destCityID].known = true;

                }

                if(cities[destCityID].depth != 0 && cities[destCityID].depth < hour - 1)
                {

                    cities[ID].numEdges--;

                    cities[ID].edgeID[j] = cities[ID].edgeID[cities[ID].numEdges];

                    cities[ID].edgeID[cities[ID].numEdges] = cities[ID].edgeID[j];

                } // remove edge
                else

                  j++;

            } // for connected edges

        } // while queue not end


        int capacity;

        if(peopleRemaining)
        {
              //mark false if done
              peopleRemaining = false;

              for(int i = 0; i < numEvacs; i++)
              {

                  int ID = evacCities[i].ID;

                  if(cities[ID].evacPeople < cities[ID].maxPeople)
                  {

                      peopleRemaining = true;

                      capacity = 0;

                      for(int j = 0; j < cities[ID].numEdges; j++)
                      {

                          if(edge[cities[ID].edgeID[j]].weight < cities[edge[cities[ID].edgeID[j]].destCityID].maxPeople)

                                capacity += edge[cities[ID].edgeID[j]].weight;
                          else

                                capacity += cities[edge[cities[ID].edgeID[j]].destCityID].maxPeople;

                      } // calculate each edge capacity

                      evacCities[i].ratio = (cities[ID].maxPeople - cities[ID].evacPeople) / capacity;

                  } // not done

              } // for remaining cities

              if(peopleRemaining)

                  qsort(evacCities, numEvacs, sizeof(EvacInfo), RatioCmp); // sort by max ratio

        }

        usedCount = 0;

        for(int i = 0; i < numEvacs; i++)
        {

            int cityID = evacCities[i].ID;

            int total = 0, difference;

            int needed = cities[cityID].maxPeople - cities[cityID].evacPeople;

            while(visitedSize > 0) // mark backroad not visited

                cities[visitedIDs[--visitedSize]].visited = false;


            visitedIDs[visitedSize++] = cityID;

            cities[cityID].visited = true;

            int edgeID, maxFlow;

            for(int i = 0; i < cities[cityID].numEdges && total < needed; i++)
            {
                edgeID = cities[cityID].edgeID[i];

                maxFlow = edge[edgeID].weight - edge[edgeID].used;

                difference = needed - total;

                if(maxFlow > difference)

                    maxFlow = difference;

                maxFlow = dfs(edge[edgeID].destCityID, maxFlow, cityID);

                edge[edgeID].used += maxFlow;

                total += maxFlow;

                if(maxFlow > 0)

                    usedRoads[usedCount++] = edgeID;

            }

            cities[cityID].evacPeople += total;

        }

        for(int i = 0; i < numCities; i++)

            cities[i].visited = 0;

        for(int i = 0; i < usedCount; i++)
        {

            evacRoutes[routeCount].roadID = usedRoads[i];

            evacRoutes[routeCount].numPeople = edge[usedRoads[i]].used;

            evacRoutes[routeCount++].time = hour;

            edge[usedRoads[i]].used = 0;

            cities[edge[usedRoads[i]].destCityID].visited = 0;
        }
          hour++;
    }

} // evacuate


int Evac::dfs(int cityID, int needed, int sourceCityID)
{

    if(cities[cityID].visited)

      return 0;

    else
    {

        cities[cityID].visited = true;

        visitedIDs[visitedSize++] = cityID;

    } // store as visited

    int total = 0, difference;

    if(!cities[cityID].evacuatedCity) // if not marked evac then return amount
    {

        difference = cities[cityID].maxPeople - cities[cityID].evacPeople;

        if(needed <= difference)
        {
            cities[cityID].evacPeople += needed;

            return needed;
        }
        else
        {
            total = difference;

            cities[cityID].evacPeople = cities[cityID].maxPeople;
        }
    }

    if(cities[cityID].deadEnd)

        return total;

    int maxFlow;

    for(int i = 0; i < cities[cityID].numEdges && total < needed; i++)
    {
        if(edge[cities[cityID].edgeID[i]].destCityID == sourceCityID)

            continue;

        maxFlow = edge[cities[cityID].edgeID[i]].weight - edge[cities[cityID].edgeID[i]].used;

        if(maxFlow > needed - total)

            maxFlow = needed - total;

        maxFlow = dfs(edge[cities[cityID].edgeID[i]].destCityID, maxFlow, cityID);

        edge[cities[cityID].edgeID[i]].used += maxFlow;

        total += maxFlow;

        if(maxFlow > 0)

            usedRoads[usedCount++] = cities[cityID].edgeID[i];
    }
    return total;
}
