HY240 Election Simulation System
Course: HY240 - Data Structures
Institution: University of Crete
Academic Year: 2024–2025

Project Overview
This project implements a simplified simulation of the Greek electoral system as part of the HY240 course. It handles elections across 56 districts using static and dynamic data structures. It supports creation of districts and stations, registration of voters and candidates, voting, seat allocation, and final parliament formation.

Data Structures Used
Fixed-size arrays for districts and parties

Hash table with chaining for voting stations

Complete binary trees for voters

Binary search trees for candidates

Min-heaps to select top candidates per party

Sorted linked list to represent the final parliament

Functional Overview
A: Initialize all structures and global parameters

D: Create district

S: Add voting station to the hash table

R: Register voter to a full binary tree

C: Register candidate to BST

V: Voter casts vote (valid, blank, invalid)

M: Count votes, assign seats per party, elect candidates

N: Build final parliament list from elected candidates

I, J, K, L: Print district, station, party, or parliament info

BU: Unregister a voter

BF: Free all allocated memory

How to Compile and Run
make
./main testfiles/testfile01.txt
File Structure
less

Notes
The simulation logic does not follow real election law

No external libraries are used

Memory management is manual; all structures must be freed

Full tree balance is maintained during voter insertions/deletions
