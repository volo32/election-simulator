 #include <stdio.h>
    #include <stdlib.h>
#include <string.h>
#include "elections.h"


 struct district districts[56];
 struct party parties[5];
 struct parliament parliament;


/* Free all voters and stations in a district */
void free_district_memory(int index) {
    struct station *station = districts[index].stations;
    while (station) {
        struct voter *voter = station->voters;
        while (voter) {
            struct voter *temp_voter = voter;
            voter = voter->next;
            free(temp_voter);
        }
        struct station *temp_station = station;
        station = station->next;
        free(temp_station);
    }
    districts[index].stations = NULL;
}

/* Free all candidates in a district */
void free_candidates_memory(int index) {
    struct candidate *cand = districts[index].candidates;
    while (cand) {
        struct candidate *temp_cand = cand;
        cand = cand->next;
        free(temp_cand);
    }
    districts[index].candidates = NULL;
}

/* Free all members of the parliament */
void free_parliament_members(void) {
    struct candidate *cand = parliament.members;
    while (cand) {
        struct candidate *temp_cand = cand;
        cand = cand->next;
        free(temp_cand);
    }
    parliament.members = NULL;
}

/* Announce Elections - Event A */
void announce_elections(void) {
    memset(districts, 0, sizeof(districts));
    memset(parties, 0, sizeof(parties));
    free_parliament_members();
}

/* Create District - Event D */
int create_district(int did, int seats) {
    for (int i = 0; i < 56; i++) {
        if (districts[i].did == 0) {
            districts[i].did = did;
            districts[i].seats = seats;
            districts[i].stations = NULL;
            districts[i].candidates = NULL;
                        /* success*/

            return 0;
        }
    }
    /*failure*/
    return 1;
}

/* Create Station - Event S */
int create_station(int sid, int did) {
    for (int i = 0; i < 56; i++) {
        if (districts[i].did == did) {
            struct station *new_station = malloc(sizeof(struct station));
            if (!new_station){
           /*failure*/

            return 1;
            }
            new_station->sid = sid;
            new_station->registered = 0;
            new_station->voters = NULL;
            new_station->next = districts[i].stations;
            districts[i].stations = new_station;
            /* success*/
            return 0;
        }
    }
        /*failure*/

    return 1;
}

/* Create Party - Event P */
void create_party(int pid) {
    for (int i = 0; i < 5; i++) {
        if (parties[i].pid == 0) {
            parties[i].pid = pid;
            parties[i].nelected = 0;
            parties[i].elected = NULL;
            return;
        }
    }
}


/* Register Candidate - Event C */
int register_candidate(int cid, int did, int pid) {
    if (cid == 0 || cid == 1){
    /*failure*/
     return 1;
}
    for (int i = 0; i < 5; i++) {
        if (parties[i].pid == pid) {
            for (int j = 0; j < 56; j++) {
                if (districts[j].did == did) {
                    struct candidate *new_candidate = malloc(sizeof(struct candidate));
                    if (!new_candidate){
    /*failure*/
                     return 1;
                }
                    new_candidate->cid = cid;
                    new_candidate->pid = pid;
                    new_candidate->votes = 0;
                    new_candidate->elected = 0;
                    new_candidate->next = districts[j].candidates;
                    districts[j].candidates = new_candidate;
                                /* success*/

                    return 0;
                }
            }
        }
    }
        /*failure*/

    return 1;
}

/* Register voter - Event R */
int register_voter(int vid, int did, int sid) {
    for (int i = 0; i < 56; i++) {
        if (districts[i].did == did) {
            struct station *station = districts[i].stations;
            while (station) {
                if (station->sid == sid) {
                    struct voter *new_voter = malloc(sizeof(struct voter));
                    if (!new_voter){
    /*failure*/

                    return 1;
                    }
                    new_voter->vid = vid;
                    new_voter->voted = 0;
                    new_voter->next = station->voters;
                    station->voters = new_voter;
                    station->registered++;
                                /* success*/

                    return 0;
                }
                station = station->next;
            }
        }
    }    /*failure*/

    return 1;
}

/* Unregister voter - Event U */
int unregister_voter(int vid) {
    /*anazhthsh voter */
    for (int i = 0; i < 56; i++) {
        struct station *station = districts[i].stations;

        /* anazhthsh station sto district */
        while (station) {
            struct voter *voter = station->voters, *prev = NULL;

            /* anazhthsh voters sto station */
            while (voter) {
                if (voter->vid == vid) {
                    /* afairesh voter ap thn lista*/
                    if (prev) {
                        prev->next = voter->next;
                    } else {
                        station->voters = voter->next;
                    }

                    free(voter);

                    station->registered--;

                    /* success */
                    return 0;
                }

        
                prev = voter;
                voter = voter->next;
            }

           
            station = station->next;
        }
    }

    /* fail*/
    return 1;
}

/* Delete empty stations - Event E */
void delete_empty_stations(void) {
    for (int i = 0; i < 56; i++) {
        struct station *station = districts[i].stations, *prev = NULL;
        while (station) {
            if (station->registered == 0) {
                struct station *to_delete = station;
                if (prev) {
                    prev->next = station->next;
                } else {
                    districts[i].stations = station->next;
                }
                station = station->next;
                free(to_delete);
            } else {
                prev = station;
                station = station->next;
            }
        }
    }
}

/* Vote - Event V */
int vote(int vid, int sid, int cid) {
    struct district *district = NULL; /* pointer sto district poy psaxnoume*/
    struct station *station = NULL;  /* Pointer sto station poy psaxnoume*/

    /* anazhthsh zhtoumenou station k district */
    for (int i = 0; i < 56; i++) {
        station = districts[i].stations;
        while (station) {
            if (station->sid == sid) {
                district = &districts[i];
                break;
            }
            station = station->next;
        }
        if (district){
        break;
        }
    }

    if (!district) {
        /*failure*/
        return 1;
    }
    /* anazhthsh voter sthn lista */
    struct voter *voter = station->voters;
    while (voter) {
        if (voter->vid == vid) break;
        voter = voter->next;
    }

    if (!voter || voter->voted) {
        /*failure*/
        return 1;
    }
    /* exei pshfhsei */
    voter->voted = 1;

    /* Locate to candidate */
    struct candidate *candidate = district->candidates;
    while (candidate) {
        if (candidate->cid == cid) break;
        candidate = candidate->next;
    }

    /* la8os candidate*/
    if (!candidate && cid != 0 && cid != 1) return 1;

    /* 0=leuka, 1= akura,alliws epituxia */
    if (cid == 0) district->blanks++;        
    else if (cid == 1) district->voids++;    
    else candidate->votes++;               

    return 0;
}
   
void count_votes(int did) {
    for (int i = 0; i < 56; i++) {
        if (districts[i].did == did) {
		/*komvos frouros*/
            struct candidate sentinel;
            sentinel.next = districts[i].candidates;

            struct candidate *prev = &sentinel;
            struct candidate *cand = sentinel.next;

            int seats_left = districts[i].seats;

            while (cand && seats_left > 0) {
                struct candidate *next_cand = cand->next;
                prev->next = next_cand;
                /*update ta parties*/
                for (int j = 0; j < 5; j++) {
                    if (parties[j].pid == cand->pid) {
                        cand->elected = 1;
                        parties[j].nelected++;
                        cand->next = parties[j].elected;
                        parties[j].elected = cand;
                        seats_left--;
                        break;
                    }
                }

                if (cand->elected == 0) {
                    prev->next = cand;
                    prev = cand;
                }

                cand = next_cand;
            }
    /* Update districts k dia8esimes 8eseis*/

            districts[i].candidates = sentinel.next;
            districts[i].allotted = districts[i].seats - seats_left;

            return;
        }
    }
}

/* Form Government - Event G */
void form_government(void) {
    int top_party_index = -1;
    int max_elected_seats = 0;

    /*komma me perissoteres 8eseis*/
    for (int i = 0; i < 5; i++) {
        if (parties[i].nelected > max_elected_seats) {
            max_elected_seats = parties[i].nelected;
            top_party_index = i;
        }
    }

    if (top_party_index == -1) {
        printf("No party found for government formation.\n");
        return;
    }

    for (int i = 0; i < 56; i++) {
        int remaining_seats = districts[i].seats - districts[i].allotted;
        struct candidate *cand = districts[i].candidates;

        /* dia8esh remaining 8esewn*/
        while (cand && remaining_seats > 0) {
            if (!cand->elected && cand->pid == parties[top_party_index].pid) {
                cand->elected = 1;
                parties[top_party_index].nelected++;

                /* vazoume to candidate sthn lista */
                cand->next = parties[top_party_index].elected;
                parties[top_party_index].elected = cand;

                remaining_seats--;
            }
            cand = cand->next;
        }

        /* dia8esh remaining seats se candidates ap ola ta party*/
        cand = districts[i].candidates;
        while (cand && remaining_seats > 0) {
            if (!cand->elected) {
                cand->elected = 1;
                for (int j = 0; j < 5; j++) {
                    if (parties[j].pid == cand->pid) {
                        parties[j].nelected++;
                        cand->next = parties[j].elected;
                        parties[j].elected = cand;
                        break;
                    }
                }
                remaining_seats--;
            }
            cand = cand->next;
        }

        districts[i].allotted = districts[i].seats - remaining_seats;
    }

    printf("G\nSeats =\n");
    for (int i = 0; i < 56; i++) {
        struct candidate *cand = districts[i].candidates;
        while (cand) {
            if (cand->elected) {
                printf("%d %d %d\n", districts[i].did, cand->cid, cand->votes);
            }
            cand = cand->next;
        }
    }
    printf("DONE\n");
}

/* Form Parliament - Event N */
void form_parliament(void) {
    struct candidate *sorted_list = NULL;
    for (int i = 0; i < 5; i++) {
        struct candidate *cand = parties[i].elected;
        while (cand) {
            struct candidate *new_member = malloc(sizeof(struct candidate));
            if (!new_member) return;
            *new_member = *cand;

            /* Insert into sorted list */
            if (!sorted_list || new_member->votes > sorted_list->votes) {
                new_member->next = sorted_list;
                sorted_list = new_member;
            } else {
                struct candidate *current = sorted_list;
                while (current->next && current->next->votes >= new_member->votes) {
                    current = current->next;
                }
                new_member->next = current->next;
                current->next = new_member;
            }

            cand = cand->next;
        }
    }
    parliament.members = sorted_list;
}

/* Print District - Event I */
void print_district(int did) {
    struct district *district = NULL;

    for (int i = 0; i < 56; i++) {
        if (districts[i].did == did) {
            district = &districts[i];
            break;
        }
    }

    if (!district) {
        printf("District ID %d not found.\n", did);
        return;
    }

    printf("District ID: %d\nSeats: %d\nAllotted: %d\nBlanks: %d\nVoids: %d\n",
           district->did, district->seats, district->allotted,
           district->blanks, district->voids);

    printf("Stations:\n");
    struct station *station = district->stations;
    while (station) {
        printf("  Station ID: %d, Registered: %d\n", station->sid, station->registered);
        station = station->next;
    }

    printf("Candidates:\n");
    struct candidate *cand = district->candidates;
    while (cand) {
        printf("  Candidate ID: %d, Votes: %d\n", cand->cid, cand->votes);
        cand = cand->next;
    }
}

/* Print Station - Event J */
void print_station(int sid, int did) {
    for (int i = 0; i < 56; i++) {
        if (districts[i].did == did) {
            struct station *station = districts[i].stations;
            while (station) {
                if (station->sid == sid) {
                    printf("Station ID: %d, Registered voters: %d\n", sid, station->registered);
                    struct voter *voter = station->voters;
                    while (voter) {
                        printf("  Voter ID: %d, Voted: %s\n", voter->vid, voter->voted ? "Yes" : "No");
                        voter = voter->next;
                    }
                    return;
                }
                station = station->next;
            }
        }
    }
    printf("Station ID %d not found in District %d.\n", sid, did);
}

/* Print Party - Event K */
void print_party(int pid) {
    printf("Party ID: %d\n", pid);
    for (int i = 0; i < 5; i++) {
        if (parties[i].pid == pid) {
            struct candidate *cand = parties[i].elected;
            if (!cand) {
                printf("No elected members.\n");
                return;
            }
            printf("Elected Members:\n");
            while (cand) {
                printf("  Candidate ID: %d, Votes: %d\n", cand->cid, cand->votes);
                cand = cand->next;
            }
            return;
        }
    }
    printf("Party ID %d not found.\n", pid);
}

void print_parliament(void) {
    struct candidate *cand = parliament.members;
    printf("Parliament Members:\n");
    while (cand) {
        printf("  Candidate ID: %d, Votes: %d\n", cand->cid, cand->votes);
        cand = cand->next;
    }
}

