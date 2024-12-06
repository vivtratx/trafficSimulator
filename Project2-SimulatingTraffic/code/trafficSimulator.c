#include "trafficSimulator.h"

// optional TODO: Put your new function declarations here
int maxInt(int a, int b);

/* printNames
 * input: none
 * output: none
 *
 * Prints names of the students who worked on this solution
 * REMINDER: You may only work with one other cs2124 student
 */
void printNames() {
    /* TODO : Fill in you and your partner's names (or N/A if you worked
     * individually) */
    printf("\nThis solution was completed by:\n");
    printf("Trae Ramirez>\n");
    printf("Vivian Tran\n");
}

/* createTrafficData
 * input: char* filename of file to read
 * output: TrafficData* which stores the data of this road network
 *
 * Create a new TrafficData variable and read in the traffic data from the file
 * whose name is stored in char* filename
 */
TrafficData* createTrafficData(char* filename) {
    /* open the file */
    FILE* pFile = fopen(filename, "r");

    if (pFile == NULL) {
        printf("ERROR - the file \"%s\" was not found!\n", filename);
        exit(-1);
    }

    /* Functions I used in my solution:
     * misc: malloc, exit, fscanf, printf, fopen, fclose
     * graph.h: createGraph, setEdge, setEdgeData
     * priorityQueue.h: createPQ, enqueueByPriority
     * queue.h: createQueue, enqueue
     * road.h: createRoad
     * car.h: createCar
     * event.h: createAddCarEvent, createAccidentEvent, createResolvedEvent
     */

    /* TODO: read in all the data in pFile */

    /* HINTs:
     * use fscanf( pFile, "%d", &<your int variable here> ) to read an int
     * from pFile
     *
     * Each road can be stored in a `Road` struct (see `road.h`).
     *
     * You'll want to store all of these `Road` structs in a single array.
     * This is important to track the order that roads were added to your graph.
     * These are also the order they are processed in each time step.  You can
     * store this in the arrayOfRoads variable in your TrafficData struct.
     *
     * You'll add edges to a graph to represent each road using `setEdge` (see
     * `graph.c`).  The vertices of the graph represent intersections.  The
     * graph will do the work of finding shortest paths (see
     * `getNextOnShortestPath` in graph.c and the longest shortest path problem
     * on Canvas).
     *
     * It will also help to add your `Road` structs to your graph using
     * `setEdgeData` (see `graph.c`).   You can then retrieve that `Road`
     * struct by calling `getEdgeData`.  This will be helpful for finding roads
     * when cars pass through intersections.
     *
     * Each event will be stored in an Event struct (see `event.h`).
     *
     * It's recommended to store all of the `Event` structs in a single priority
     * queue (using time step as the priority).  This allows you to quickly find
     * the next event to execute by time step (see `enqueueByPriority`,
     * `getFrontPriority`, and `dequeuePQ`).
     *
     * Each car is stored in a Car struct (see `car.h`).
     */

    /* close file */
    int numNodes, numEdges, i, j, to, from, length, greenOn, greenOff, rest, timeSteps;

    /*Read the edges*/
    fscanf(pFile, "%d %d", &numNodes, &numEdges);

    /*Malloc the traffic data and check for errors*/
    TrafficData* newTrafficData = (TrafficData*)malloc(sizeof(TrafficData));
    if (newTrafficData == NULL) {
        printf("ERROR - malloc failed to allocate memory for TrafficData\n");
        exit(-1);
    }

    /*Storing */
    newTrafficData->arrayOfRoads = (Road**)malloc(numEdges * sizeof(Road*));
    newTrafficData->numRoads = 0;

    newTrafficData->roadGraph = createGraph(numNodes);

    for( i = 0; i < numEdges; i++){
	    if ( fscanf(pFile, "%d %d %d    %d %d %d", &from, &to, &length, &greenOn, &greenOff, &rest) != 6 ){
		    printf("ERROR READING FILE\n");
		    exit(-1);
	    }
	    Road* currRoad = createRoad( from, to, length, greenOn, greenOff, rest );

	    newTrafficData->arrayOfRoads[i] = currRoad;
        setEdge( newTrafficData->roadGraph, currRoad->from, currRoad->to, length );
        setEdgeData( newTrafficData->roadGraph, from, to, currRoad );
	    newTrafficData->numRoads++;
    }

    newTrafficData->eventQueue = createPQ();

    int numEvent;
    fscanf(pFile, "%d", &numEvent);

    int carsAdd;
    newTrafficData->numCars = 0;
    int destionation;
    for (i = 0; i < numEvent; i++){
        Queue *curntQueue = createQueue();

        fscanf(pFile, "%d%d%d", &from, &to, &timeSteps);
        fscanf(pFile, "%d", &carsAdd);

        for (j = 0; j < carsAdd; j++){
            fscanf(pFile, "%d", &destionation);
            Car* newCar = createCar( timeSteps, from, to, destionation );
            enqueue( curntQueue, newCar );
            newTrafficData->numCars++;
        }

        enqueueByPriority( newTrafficData->eventQueue, createAddCarEvent(timeSteps, getEdgeData(newTrafficData->roadGraph,from, to), curntQueue), timeSteps );
    }

    int numAccident;
    int dur;
    fscanf(pFile, "%d", &numAccident);

    for (i = 0; i < numAccident; i++){
        fscanf(pFile, "%d%d%d%d", &from, &to, &timeSteps, &dur);
        enqueueByPriority( newTrafficData->eventQueue, createAccidentEvent(timeSteps, getEdgeData(newTrafficData->roadGraph,from, to)), timeSteps );
        enqueueByPriority(newTrafficData->eventQueue, createResolvedEvent((timeSteps + dur), getEdgeData(newTrafficData->roadGraph, from, to)), (timeSteps + dur));
 
    }



    fclose(pFile);

    return newTrafficData; /* TODO: Replace this with your TrafficData pointer */
}

/* trafficSimulator
 * input: char* name of file to read
 * output: N/A
 *
 * Simulate the road network in the given TrafficData variable
 */
void trafficSimulator(TrafficData* pTrafficData) {
    /* TODO: complete this function */

    /* Functions I used in my solution:
     * misc: printf, maxInt
     * queue.h: isEmpty, mergeQueues, dequeue
     * priorityQueue.h: isEmptyPQ, getFrontPriority, dequeuePQ
     * event.h: freeEvent
     * road.h: printRoadContents
     * graph.h: getNextOnShortestPath, getEdgeData
     * car.h: freeCar
     */

    /* Loop until all events processed and all cars reached destination
     * You can also assume all test data will finish in <1000 steps
     * This fact can be used to help you avoid infinite loops while completing
     * the project */
    int i = 0;
    int j = 0;
    int steps = 0;
    int max = 0;
    double average = 0;
    int initialNumCars = pTrafficData->numCars;

    while (!isEmptyPQ(pTrafficData->eventQueue) || pTrafficData->numCars > 0)
    {
        /* Print the current step number */
        printf("STEP %d\n", steps);
        /* Update the state of every traffic light */
        for (i = 0; i < pTrafficData->numRoads; i++)
        /* Loop on events associated with this time step */
        {
            Road* curntRoad = pTrafficData->arrayOfRoads[i];

            int cycle = steps % curntRoad->lightCycleLength;

            if (cycle >= curntRoad->greenStartTime && cycle < curntRoad->greenEndTime){
                curntRoad->currentLightState = GREEN_LIGHT;
            }
            else{
                curntRoad->currentLightState = RED_LIGHT;
            }
            
        }

        while (!isEmptyPQ(pTrafficData->eventQueue) && steps == getFrontPriority(pTrafficData->eventQueue))
        {
            Event* curntEvent = dequeuePQ(pTrafficData->eventQueue);

            if (curntEvent->eventCode == ADD_CAR_EVENT){
                mergeQueues(curntEvent->pRoad->waitingCars, curntEvent->pCarQueue);
                printf("ADD_CAR_EVENT - Cars enqueued on road from %d to %d\n", curntEvent->pRoad->from, curntEvent->pRoad->to);
            }
            else if (curntEvent->eventCode == ROAD_ACCIDENT_EVENT){
                curntEvent->pRoad->numAccidents+= 1;
                printf("ROAD_ACCIDENT_EVENT - Accident on road from %d to %d\n", curntEvent->pRoad->from, curntEvent->pRoad->to);
            }
            else if (curntEvent->eventCode == ROAD_RESOLVED_EVENT){
                curntEvent->pRoad->numAccidents-= 1;
                printf("ROAD_RESOLVED_EVENT - Accident resolved on road from %d to %d\n", curntEvent->pRoad->from, curntEvent->pRoad->to);
            }
            freeEvent(curntEvent);
        }

        for(i = 0; i < pTrafficData->numRoads; i++){
            printRoadContents(pTrafficData->arrayOfRoads[i]);
        }

        for (i = 0; i < pTrafficData->numRoads; i++){
            Road* curntRoad = pTrafficData->arrayOfRoads[i];
            for( j = 0; j < curntRoad -> length; j++){
                if (curntRoad->roadContents[j] != NULL){
                    curntRoad->roadContents[j]->moved = false;
                }
            }
        }

        for (i = 0; i < pTrafficData->numRoads; i++){
            Road* curntRoad = pTrafficData->arrayOfRoads[i];

            if (!isEmpty(curntRoad->waitingCars) && curntRoad->roadContents[curntRoad->length - 1] == NULL && curntRoad->numAccidents == 0){
                if (curntRoad->roadContents[curntRoad->length - 1] == NULL){
                    Car* curntCar = dequeue(curntRoad->waitingCars);
                    curntRoad->roadContents[curntRoad->length - 1] = curntCar;
                    curntCar->moved = true;
                    printf("Car moved from waiting to road from %d to %d\n", curntRoad->from, curntRoad->to);
                }
            }
        }

        for (i = 0; i < pTrafficData->numRoads; i++){
            Road* curntRoad = pTrafficData->arrayOfRoads[i];

            for(j = (curntRoad->length - 1); j > 0; j--){
                Car *curntCar = curntRoad->roadContents[j];
                if (curntCar != NULL && curntCar->moved == false && curntRoad->numAccidents == 0){
                    if (curntRoad->roadContents[j - 1] == NULL){
                        curntRoad->roadContents[j - 1] = curntCar;
                        curntRoad->roadContents[j] = NULL;
                        curntCar->moved = true;
                    }
                }
            }
        }

        for (i = 0; i < pTrafficData->numRoads; i++){
            int Intersection = 0;
            Road* curntRoad = pTrafficData->arrayOfRoads[i];
            Car* frontCar = curntRoad->roadContents[0];

            if ((frontCar != NULL) && (frontCar->moved == false) && (curntRoad->currentLightState == GREEN_LIGHT)){
                if (frontCar -> destination != curntRoad->to){
                    getNextOnShortestPath(pTrafficData->roadGraph, curntRoad->to, frontCar->destination, &Intersection);
                    Road* nextRoad = getEdgeData(pTrafficData->roadGraph, curntRoad->to, Intersection);
                    printf("Next intersection is: %d\n", Intersection);
                    if (nextRoad -> numAccidents == 0 && nextRoad->roadContents[nextRoad->length - 1] == NULL){
                        nextRoad->roadContents[nextRoad->length - 1] = frontCar;
                        frontCar->moved = true;
                        frontCar->next = nextRoad->to;
                        curntRoad->roadContents[0] = NULL;
                }
            }
            else {
                int time = (steps - frontCar->stepAdded)+1;
                printf("Car successfully traveled from %d to %d in %d time steps\n", frontCar->origin, frontCar->destination, time);
                average = average + time;

                max = maxInt(max, time);

                curntRoad->roadContents[0] = NULL;
                freeCar(frontCar);
                pTrafficData->numCars--;

            }
                
        }
    }

    printf("\n");
    steps++;

    }   

    average = average / initialNumCars;
    printf("Average time to reach destination: %.2f\n", average);
    printf("Max time to reach destination: %d\n", max); 

        /* If ADD_CAR_EVENT, use mergeQueues from queue.h to add the queue
             * of cars to the queue of the road associated with the event */

            /* If ROAD_ACCIDENT_EVENT, add one to the numAccidents of the road
             * associated with the event */

            /* If ROAD_RESOLVED_EVENT, subtract one from the numAccidents of the
             * road associated with the event */

        /* Print the contents of each road (use the provided function
         * printRoadContents) */

        /* For each road, try to move waiting cars onto the end of that road if
         * possible (remember to check that numAccidents==0 for the road) */

        /* For each road, try to move cars, which haven't already moved, forward
         * one space on every road (remember to check that numAccidents==0 for
         * the road)
         */

        /* For each road, try to move cars, which haven't already moved, through
         * the next intersection (remember to check that numAccidents==0 for
         * both roads)
         */
    

    /* After the loop finishes print the average and max number of steps it took
     * for a car to reach its destination */
}

/* freeTrafficData
 * input: TrafficData* pTrafficData
 * output: N/A
 *
 * Free the data in the TrafficData variable
 */
void freeTrafficData(TrafficData* pTrafficData) {
    /* TODO: complete this function */

    /* Functions I used in my solution:
     * misc: free
     * road.h: freeRoad
     * priorityQueue.h: freePQ
     * graph.h: freeGraph
     */

    int i = 0; 
    freePQ(pTrafficData->eventQueue);
    for (i = 0; i < pTrafficData->numRoads; i++){
        freeRoad(pTrafficData->arrayOfRoads[i]);
    }

    free(pTrafficData->arrayOfRoads);
    freeGraph(pTrafficData->roadGraph);
    free(pTrafficData);
}

int maxInt(int a, int b) {
    if (a > b) return a;
    return b;
}