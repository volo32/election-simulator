/*
 * ============================================
 * file: voting.h
 * @Author Myron Tsatsarakis (myrontsa@csd.uoc.gr)
 * @Version 26/11/2024
 *
 * @e-mail hy240@csd.uoc.gr
 *
 * @brief Structure and function declarations
 *        for CS240 Project Phase 2,
 *        Winter Semester 2024-2025
 * ============================================
 */

#ifndef VOTING_H
#define VOTING_H

#include <stdbool.h>

/* Struct for MinHeapNode */
typedef struct MinHeapNode {
    struct Candidate* candidate;
    int votes;
} MinHeapNode;

/* Function Declarations */
void EventAnnounceElections(int maxStationsCount, int maxSid);
void EventCreateDistrict(int did, int seats);
void EventCreateStation(int sid, int did);
void EventRegisterVoter(int vid, int sid);
void EventRegisterCandidate(int cid, int pid, int did);
void EventVote(int vid, int sid, int cid, int pid);
void EventCountVotes(int did);
void EventFormParliament(void);
void EventPrintDistrict(int did);
void EventPrintStation(int sid);
void EventPrintParty(int pid);
void EventPrintParliament(void);
void EventBonusUnregisterVoter(int vid, int sid);
void EventBonusFreeMemory(void);

/* Helper Function Declarations */
void MinHeapify(MinHeapNode heap[], int i, int size);
void InsertMinHeap(MinHeapNode heap[], struct Candidate* candidate, int* size);
void ReplaceRoot(MinHeapNode heap[], struct Candidate* candidate, int size);
void TraverseCandidates(struct Candidate* root, MinHeapNode heap[], int* size, int maxSize);
void ElectPartyCandidatesInDistrict(int pid, int did, int seats);

struct Voter* DeleteVoterFromTree(struct Voter* root, int vid, int* registeredCount);
struct Voter* FindMin(struct Voter* root);
void PrintVotersInOrder(struct Voter* root);
void FreeVotersTree(struct Voter* root);

void FreeCandidatesTree(struct Candidate* root);
void PrintCandidatesInOrder(struct Candidate* root);

#endif /* VOTING_H */


