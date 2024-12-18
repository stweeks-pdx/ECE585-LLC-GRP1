#ifndef _CACHE_H
#define _CACHE_H
#include "trace.h"
#include "defines.h"
#include <stdbool.h>
#include <stdint.h>
#include <math.h>
#define MASK2LSB 0x3

//MESI States
#define MODIFIED 'M'
#define EXCLUSIVE 'E'
#define SHARED 'S'
#define INVALID 'I'

//Address Presence/Snoop Results
#define MISS 0
#define HIT 1
#define HITM 2
#define NOHIT 3

//L1 Messages
#define GETLINE 0
#define	SENDLINE 1
#define	INVALIDATELINE 2
#define	EVICTLINE 3

//Bus Commands 
#define	READ 0
#define	WRITE 1
#define	INVALIDATE 2
#define	RWIM 3

//Trace Commands
#define L1DATAREAD 0
#define L1WRITE 1
#define L1INSTREAD 2
#define SNOOPEDREAD 3
#define SNOOPEDWRITE 4
#define SNOOPEDRWIM 5
#define SNOOPEDINVAL 6
#define CLEARCACHE 8
#define PRINTCACHE 9

typedef struct { 
	uint16_t tag;
	char state;
} way;

typedef struct { 
	way *myWay;
	uint8_t *plru;
} set;

typedef struct {
	set *cache;
} cacheStruct;

extern cacheStruct *LLC;

//*******************FUNCTION PROTOTYPES**********************************

/*
Cache: Accepts a parsed trace and responds appropriately

Arguments:
	request: custom data type Trace that contains a command and address
*/
void cache(Trace request);

/*
checkForPresence: Uses index and tag arguments to determine if an address is present in the cache or not

Arguments:
	tag: unsigned 16 bit integer that associates data to unique physical address
	index: unsigned 16 bit integer that points us to the correct set for our tag

	returns requestResult, either hit or miss
*/
int checkForPresence(uint16_t tag, uint16_t index);

/*
store: Uses index to find a place to store a particular tag. Uses command to broadcast proper snoop message on bus.

Arguments:
	tag: unsigned 16 bit interger that associated data to unique physical address
	index: unsigned 16 bit integer that points us to the correct set for our tag
	command: one of 9 commands for this cache simulation
	address: address to be broadcasted on bus if needed
*/
void store(uint16_t tag, uint16_t index, uint8_t command, uint32_t address);

/* 
updatePLRU: Updates the plru bits based on recent memory access

Arguments:
	plru[]: The desired plru array that needs updated
	index: The index of the way that is being accessed, should be 0-15
*/
void updatePLRU(uint8_t plru[], int index);

/*
setNotFull: Checks if a set has room

Arguments: 
	index: The set that we are checking

returns the index for the first empty way found, -1 if full.
*/
int setNotFull(uint16_t index);

/*
victimPLRU: Reads PLRU bits to find which way needs replaced on collision miss, then updates most recently accessed memory location

Arguments: 
	plru[]: The desired plru array used to point to eviction

Returns the index of the way that needs to be evicted
*/
int victimPLRU(uint8_t plru[]);

/*
busOperation: Broadcasts the command and address IAW the MESI protocol based on what the cache just did

Arguemnts:
	busOp: The operation that we are putting out on the bus
	address: The address that other caches need to check for
*/
void busOperation(int busOp, uint32_t address);

/*
getSnoopResult: Checks the trace to get the snoop result. For sim purposes, uses the 2LSB of the address.

Arguments:
	address: Address that we are acting on that needs snoop information for state changes

returns int based on 2LSB of address
*/
int getSnoopResult(uint32_t address);

/*
putSnoopResult: Tell other caches if we have an address or not

Arguments:
	address: The address that other caches are accessing
	message: Tells them if we have it, and if it is modified
*/
void putSnoopResult(uint32_t address, int message);

/*
messageToL1: Sends a message to L1 to maintain inclusivity

Arguments:
	message: Telling L1 what to do with an address
	address: What address L1 needs to act on
*/
void messageToL1(int message, uint32_t address);

/*
getState: Gets the MESI state of a given way

Arguments:
	index: Tells us which set in the cache to access
	way: Which way in the set we need to access

Returns the MESI state of the requested cache line
*/
char getState(uint16_t index, uint16_t tag);

/*
displayTraceResult: Displays the cache's usage statistic for a given simulation
*/
void displayTraceResult(void);

/*
resetCache: Invalidates all cache lines and resets all plrus
*/
void resetCache(void);

/*
printCache: Prints contents and state of each valid cache line without ending simulation
*/
void printCache(void);

/*
updateState: updates the MESI state of a given way

Arguments:
	index: Points us to the correct set in the cache
	way: The index for the specific cache line we are updating. Should be 0-15
	command: What command we are performing
	result: Snoop results from the bus
*/
void updateState(uint16_t index, int way, uint8_t command, int result, uint16_t tag, uint32_t address);

/*
findWay: Gives the index of the way that matches the given tag

Arguments:
  index: Points us to the correct set in the cache
  tag: the tag that we want to find in the set

Returns the index of the way that holds the tag that matches the one we give
*/
int findWay(uint16_t index, uint16_t tag);

/*
  cacheInit: Initializes the cache by setting all the ways in every set to INVALID
*/
cacheStruct *cacheInit(void);

/*
cacheDestroy: Destroys the cache

Arguments:
  LLC: LLC cache pointer
*/
void cacheDestroy(cacheStruct *LLC);

void plruInit(void);
#endif
