#include "trafficSimulator.h"

// optional TODO: Put your new function declarations here
int maxInt(int a, int b);

/* printNames
 * input: none
 * output: none
 
 * Prints names of the students who worked on this solution
 * REMINDER: You may only work with one other cs2124 student
 */
void printNames() {
    /* TODO : Fill in you and your partner's names (or N/A if you worked
     * individually) */
    printf("\nThis solution was completed by:\n");
    printf("Vivian Tran\n");
    printf("NA\n\n");
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
     */

     int numNodes;
     int numEdges;
     int i;
     int j;
     int to;
     int from;
     int length;
     int greenOn;
     int greenOff;
     int reset;
     int timeStep;


     // Read and add edges
     fscanf( pFile, "%d", &numNodes );
     fscanf( pFile, "%d", &numEdges );

     /*
     * Each road can be stored in a `Road` struct (see `road.h`).
     * You'll want to store all of these `Road` structs in a single array.
     * This is important to track the order that roads were added to your graph.
     * These are also the order they are processed in each time step.  You can
     * store this in the arrayOfRoads variable in your TrafficData struct.
     */

     // Malloc traffic data
     TrafficData* myTrafficData = ( TrafficData* )malloc( 1 * sizeof(TrafficData) );

     // Malloc the arrayOfRoads
     Road **roadArray = ( Road** )malloc( numEdges * sizeof(Road*) );

     // Store the array of roads in traffic data
     myTrafficData->arrayOfRoads = roadArray;

     /*
     * You'll add edges to a graph to represent each road using `setEdge` (see
     * `graph.c`).  The vertices of the graph represent intersections.  The
     * graph will do the work of finding shortest paths (see
     * `getNextOnShortestPath` in graph.c and the longest shortest path problem
     * on Canvas).
     */

     //This is like our Graph* g;
     myTrafficData->roadGraph = createGraph(numNodes);

     // For loop to add nodes to the graph
     for( i = 0; i < numNodes; i++){
	     addVertex( myTrafficData->roadGraph, i );
     }

     // Loop to read and set edges
     for( i = 0; i < numEdges; i++){
	     if ( fscanf(pFile, "%d%d%d%d%d%d", &from, &to, &length, &greenOn, &greenOff, &reset) != 6 ){
		     printf("ERROR READING FILE\n");
		     exit(-1);
	     }
	     setEdge( myTrafficData->roadGraph, from, to, length );
	     Road* currRoad = createRoad( from, to, length, greenOn, greenOff, reset );
	     setEdgeData( myTrafficData->roadGraph, from, to, currRoad );
     }

     /*
     * It will also help to add your `Road` structs to your graph using
     * `setEdgeData` (see `graph.c`).   You can then retrieve that `Road`
     * struct by calling `getEdgeData`.  This will be helpful for finding roads
     * when cars pass through intersections.
     */
     /* Each event will be stored in an Event struct (see `event.h`).
     *
     * It's recommended to store all of the `Event` structs in a single priority
     * queue (using time step as the priority).  This allows you to quickly find
     * the next event to execute by time step (see `enqueueByPriority`,
     * `getFrontPriority`, and `dequeuePQ`).
     *
     * Each car is stored in a Car struct (see `car.h`).
     */

     //Create the priority queue within traffic data
     myTrafficData->eventQueue = createPQ();

     // READ IN THE EVENTS
     int numEvents;
     fscanf( pFile, "%d", &numEvents );

     int numCarsToAdd;
     myTrafficData->numCars = 0;
     int dest;
     for(i = 0; i < numEvents; i++){

	     //create a queue for each event
	     Queue *currQueue = createQueue();

	    fscanf( pFile, "%d%d%d", &from, &to, &timeStep );
	    fscanf( pFile, "%d", &numCarsToAdd );

	    for(j = 0; j < numCarsToAdd; j++){

		    fscanf( pFile, "%d", &dest );
		    // CAR IS MALLOC'D BE SURE TO FREE IT LATER WITH freeCar
		    Car* currCar = createCar(timeStep, from, to, dest);
		    //Put each car into the queue
		    enqueue(currQueue, currCar);
	    }
	    
	    //call createAddCarEvent to put queue into PQ
	    enqueueByPriority(myTrafficData->eventQueue, createAddCarEvent(timeStep, getEdgeData(myTrafficData->roadGraph,from,to), currQueue), timeStep);
     }

     // READ IN ACCIDENT EVENTS
     int numAccidents;
     int duration;
     fscanf( pFile, "%d", &numAccidents );
     for( i = 0; i < numAccidents; i++){
	     fscanf( pFile, "%d%d%d%d", &from, &to, &timeStep, &duration );
	     // Put everything into the same event queue
	     enqueueByPriority(myTrafficData->eventQueue, createAccidentEvent(timeStep, getEdgeData(myTrafficData->roadGraph,from,to)), timeStep);
	     enqueueByPriority(myTrafficData->eventQueue, createResolvedEvent((timeStep+duration), getEdgeData(myTrafficData->roadGraph,from,to)), (timeStep+duration) ); //I think this is correct?
     }

    /* close file */
    fclose(pFile);

    return myTrafficData; /* TODO: Replace this with your TrafficData pointer */
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
     int step = 0;
     int totalSteps = step;

     while( !isEmptyPQ(pTrafficData->eventQueue) && (pTrafficData->numCars>0) && (step<1000) ){

        /* Print the current step number */
	printf("Step: %d\n", step);

        /* Update the state of every traffic light */

	Road* currRoad;

	for(i = 0; i < pTrafficData->numRoads; i++){ //Update every road
		currRoad = pTrafficData->arrayOfRoads[i];

		// Don't need to worry about resetting, modulo takes care of those cases

		// If our current step == greenOn
		if(step % currRoad->lightCycleLength == currRoad->greenStartTime){
			currRoad->currentLightState = GREEN_LIGHT;
		}

		// If our current step == greenOff
		if(step % currRoad->lightCycleLength == currRoad->greenEndTime){
			currRoad->currentLightState = RED_LIGHT;
		}
	}


        /* Loop on events associated with this time step */
	Event* currEvent;

	while( !isEmptyPQ(pTrafficData->eventQueue) )
        {
		// Only dequeue if there is an event on this timeStep
		// Get front priority 
		// If it's equal to the current timestep then dequeue
		if ( getFrontPriority(pTrafficData->eventQueue) == step ){
			currEvent = dequeuePQ(pTrafficData->eventQueue);
		}

            /* If ADD_CAR_EVENT, use mergeQueues from queue.h to add the queue
             * of cars to the queue of the road associated with the event */
	     	if( currEvent->eventCode == ADD_CAR_EVENT ){

			// Takes the contents of one queue and puts it into a different queued
			Road* roadWithEvent = currEvent->pRoad;	
			mergeQueues( roadWithEvent->waitingCars, currEvent->pCarQueue );

		}

            /* If ROAD_ACCIDENT_EVENT, add one to the numAccidents of the road
             * associated with the event */
	     // get this value from the current event
	     	if( currEvent->eventCode == ROAD_ACCIDENT_EVENT ){
			Road* roadWithEvent = currEvent->pRoad;
			roadWithEvent->numAccidents += 1; //access the road and then num accidents?
		}

            /* If ROAD_RESOLVED_EVENT, subtract one from the numAccidents of the
             * road associated with the event */
	     // from current event to get this value to subtract
	     	if( currEvent->eventCode == ROAD_RESOLVED_EVENT ){
			Road* roadWithEvent = currEvent->pRoad;
			roadWithEvent->numAccidents -= 1;
		}
        }

	 /* Print the contents of each road (use the provided function
         * printRoadContents) */
	for(i = 0; i < pTrafficData->numRoads; i++){
		printRoadContents(pTrafficData->arrayOfRoads[i]);

	}


        /* For each road, try to move waiting cars onto the end of that road if // ANOTHER LOOP
         * possible (remember to check that numAccidents==0 for the road) */ // Check the waiting queue, and if can be placed then put the car
	 for(i = 0; i < pTrafficData->numRoads; i++){
		 currRoad = pTrafficData->arrayOfRoads[i];
		 if( currRoad->numAccidents == 0 ){
			 // Check if it's like NULL or something?
			 //currRoad->waitingCars
		 //Check waiting queue, if can be placed then put the car
		 }
	 }

        /* For each road, try to move cars, which haven't already moved, forward
         * one space on every road (remember to check that numAccidents==0 for
         * the road)
         */

        /* For each road, try to move cars, which haven't already moved, through
         * the next intersection (remember to check that numAccidents==0 for
         * both roads)
         */
	 step++;
	 pTrafficData->numCars -= 1;//not necessary just testing
    }

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

     free(pTrafficData->arrayOfRoads);
     freePQ(pTrafficData->eventQueue);
     freeGraph(pTrafficData->roadGraph);
     free(pTrafficData);

}

int maxInt(int a, int b) {
    if (a > b) return a;
    return b;
}
