#include "voting.h"
#include <string.h>
#include <stdlib.h>
#include <stdbool.h>
#include <stdio.h>

#ifdef DEBUG_PRINTS_ENABLED
#define DebugPrint(...) printf(__VA_ARGS__);
#else
#define DebugPrint(...)
#endif

#define PRIMES_SZ 1024
#define DISTRICTS_SZ 56
#define PARTIES_SZ 5

typedef struct District District;
typedef struct Station Station;
typedef struct Voter Voter;
typedef struct Party Party;
typedef struct Candidate Candidate;
typedef struct ElectedCandidate ElectedCandidate;

struct District {
    int did;
    int seats;
    int blanks;
    int invalids;
    int partyVotes[PARTIES_SZ];
};

struct Station {
    int sid;
    int did;
    int registered;
    Voter* voters;
    Station* next;
};

struct Voter {
    int vid;
    bool voted;
    Voter* parent;
    Voter* lc;
    Voter* rc;
};

struct Party {
    int pid;
    int electedCount;
    Candidate* candidates;
};

struct Candidate {
    int cid;
    int did;
    int votes;
    bool isElected;
    Candidate* lc;
    Candidate* rc;
};

struct ElectedCandidate {
    int cid;
    int did;
    int pid;
    ElectedCandidate* next;
};

District Districts[DISTRICTS_SZ];
Station** StationsHT;
Party Parties[PARTIES_SZ];
ElectedCandidate* Parliament;

const int DefaultDid = -1;
const int BlankDid = -1;
const int InvalidDid = -2;

const int Primes[PRIMES_SZ] = {
    0, 1, 2, 3, 5, 7, 11, 13, 17, 19, 23, 29, 31, 37, 41, 43, 47, 53, 59, 61, 67, 71, 73, 79, 83, 89, 97, 101, 103, 107, 109, 113, 127, 131, 137, 139, 149, 151, 157, 163, 167, 173, 179, 181, 191, 193, 197, 199, 211, 223, 227, 229, 233, 239, 241, 251, 257, 263, 269, 271, 277, 281, 283, 293, 307, 311, 313, 317, 331, 337, 347, 349, 353, 359, 367, 373, 379, 383, 389, 397, 401, 409, 419, 421, 431, 433, 439, 443, 449, 457, 461, 463, 467, 479, 487, 491, 499, 503, 509, 521, 523, 541, 547, 557, 563, 569, 571, 577, 587, 593, 599, 601, 607, 613, 617, 619, 631, 641, 643, 647, 653, 659, 661, 673, 677, 683, 691, 701, 709, 719, 727, 733, 739, 743, 751, 757, 761, 769, 773, 787, 797, 809, 811, 821, 823, 827, 829, 839, 853, 857, 859, 863, 877, 881, 883, 887, 907, 911, 919, 929, 937, 941, 947, 953, 967, 971, 977, 983, 991, 997, 1009, 1013, 1019, 1021, 1031, 1033, 1039, 1049, 1051, 1061, 1063, 1069, 1087, 1091, 1093, 1097, 1103, 1109, 1117, 1123, 1129, 1151, 1153, 1163, 1171, 1181, 1187, 1193, 1201, 1213, 1217, 1223, 1229, 1231, 1237, 1249, 1259, 1277, 1279, 1283, 1289, 1291, 1297, 1301, 1303, 1307, 1319, 1321, 1327, 1361, 1367, 1373, 1381, 1399, 1409, 1423, 1427, 1429, 1433, 1439, 1447, 1451, 1453, 1459, 1471, 1481, 1483, 1487, 1489, 1493, 1499, 1511, 1523, 1531, 1543, 1549, 1553, 1559, 1567, 1571, 1579, 1583, 1597, 1601, 1607, 1609, 1613, 1619, 1621, 1627, 1637, 1657, 1663, 1667, 1669, 1693, 1697, 1699, 1709, 1721, 1723, 1733, 1741, 1747, 1753, 1759, 1777, 1783, 1787, 1789, 1801, 1811, 1823, 1831, 1847, 1861, 1867, 1871, 1873, 1877, 1879, 1889, 1901, 1907, 1913, 1931, 1933, 1949, 1951, 1973, 1979, 1987, 1993, 1997, 1999, 2003, 2011, 2017, 2027, 2029, 2039, 2053, 2063, 2069, 2081, 2083, 2087, 2089, 2099, 2111, 2113, 2129, 2131, 2137, 2141, 2143, 2153, 2161, 2179, 2203, 2207, 2213, 2221, 2237, 2239, 2243, 2251, 2267, 2269, 2273, 2281, 2287, 2293, 2297, 2309, 2311, 2333, 2339, 2341, 2347, 2351, 2357, 2371, 2377, 2381, 2383, 2389, 2393, 2399, 2411, 2417, 2423, 2437, 2441, 2447, 2459, 2467, 2473, 2477, 2503, 2521, 2531, 2539, 2543, 2549, 2551, 2557, 2579, 2591, 2593, 2609, 2617, 2621, 2633, 2647, 2657, 2659, 2663, 2671, 2677, 2683, 2687, 2689, 2693, 2699, 2707, 2711, 2713, 2719, 2729, 2731, 2741, 2749, 2753, 2767, 2777, 2789, 2791, 2797, 2801, 2803, 2819, 2833, 2837, 2843, 2851, 2857, 2861, 2879, 2887, 2897, 2903, 2909, 2917, 2927, 2939, 2953, 2957, 2963, 2969, 2971, 2999, 3001, 3011, 3019, 3023, 3037, 3041, 3049, 3061, 3067, 3079, 3083, 3089, 3109, 3119, 3121, 3137, 3163, 3167, 3169, 3181, 3187, 3191, 3203, 3209, 3217, 3221, 3229, 3251, 3253, 3257, 3259, 3271, 3299, 3301, 3307, 3313, 3319, 3323, 3329, 3331, 3343, 3347, 3359, 3361, 3371, 3373, 3389, 3391, 3407, 3413, 3433, 3449, 3457, 3461, 3463, 3467, 3469, 3491, 3499, 3511, 3517, 3527, 3529, 3533, 3539, 3541, 3547, 3557, 3559, 3571, 3581, 3583, 3593, 3607, 3613, 3617, 3623, 3631, 3637, 3643, 3659, 3671, 3673, 3677, 3691, 3697, 3701, 3709, 3719, 3727, 3733, 3739, 3761, 3767, 3769, 3779, 3793, 3797, 3803, 3821, 3823, 3833, 3847, 3851, 3853, 3863, 3877, 3881, 3889, 3907, 3911, 3917, 3919, 3923, 3929, 3931, 3943, 3947, 3967, 3989, 4001, 4003, 4007, 4013, 4019, 4021, 4027, 4049, 4051, 4057, 4073, 4079, 4091, 4093, 4099, 4111, 4127, 4129, 4133, 4139, 4153, 4157, 4159, 4177, 4201, 4211, 4217, 4219, 4229, 4231, 4241, 4243, 4253, 4259, 4261, 4271, 4273, 4283, 4289, 4297, 4327, 4337, 4339, 4349, 4357, 4363, 4373, 4391, 4397, 4409, 4421, 4423, 4441, 4447, 4451, 4457, 4463, 4481, 4483, 4493, 4507, 4513, 4517, 4519, 4523, 4547, 4549, 4561, 4567, 4583, 4591, 4597, 4603, 4621, 4637, 4639, 4643, 4649, 4651, 4657, 4663, 4673, 4679, 4691, 4703, 4721, 4723, 4729, 4733, 4751, 4759, 4783, 4787, 4789, 4793, 4799, 4801, 4813, 4817, 4831, 4861, 4871, 4877, 4889, 4903, 4909, 4919, 4931, 4933, 4937, 4943, 4951, 4957, 4967, 4969, 4973, 4987, 4993, 4999, 5003, 5009, 5011, 5021, 5023, 5039, 5051, 5059, 5077, 5081, 5087, 5099, 5101, 5107, 5113, 5119, 5147, 5153, 5167, 5171, 5179, 5189, 5197, 5209, 5227, 5231, 5233, 5237, 5261, 5273, 5279, 5281, 5297, 5303, 5309, 5323, 5333, 5347, 5351, 5381, 5387, 5393, 5399, 5407, 5413, 5417, 5419, 5431, 5437, 5441, 5443, 5449, 5471, 5477, 5479, 5483, 5501, 5503, 5507, 5519, 5521, 5527, 5531, 5557, 5563, 5569, 5573, 5581, 5591, 5623, 5639, 5641, 5647, 5651, 5653, 5657, 5659, 5669, 5683, 5689, 5693, 5701, 5711, 5717, 5737, 5741, 5743, 5749, 5779, 5783, 5791, 5801, 5807, 5813, 5821, 5827, 5839, 5843, 5849, 5851, 5857, 5861, 5867, 5869, 5879, 5881, 5897, 5903, 5923, 5927, 5939, 5953, 5981, 5987, 6007, 6011, 6029, 6037, 6043, 6047, 6053, 6067, 6073, 6079, 6089, 6091, 6101, 6113, 6121, 6131, 6133, 6143, 6151, 6163, 6173, 6197, 6199, 6203, 6211, 6217, 6221, 6229, 6247, 6257, 6263, 6269, 6271, 6277, 6287, 6299, 6301, 6311, 6317, 6323, 6329, 6337, 6343, 6353, 6359, 6361, 6367, 6373, 6379, 6389, 6397, 6421, 6427, 6449, 6451, 6469, 6473, 6481, 6491, 6521, 6529, 6547, 6551, 6553, 6563, 6569, 6571, 6577, 6581, 6599, 6607, 6619, 6637, 6653, 6659, 6661, 6673, 6679, 6689, 6691, 6701, 6703, 6709, 6719, 6733, 6737, 6761, 6763, 6779, 6781, 6791, 6793, 6803, 6823, 6827, 6829, 6833, 6841, 6857, 6863, 6869, 6871, 6883, 6899, 6907, 6911, 6917, 6947, 6949, 6959, 6961, 6967, 6971, 6977, 6983, 6991, 6997, 7001, 7013, 7019, 7027, 7039, 7043, 7057, 7069, 7079, 7103, 7109, 7121, 7127, 7129, 7151, 7159, 7177, 7187, 7193, 7207, 7211, 7213, 7219, 7229, 7237, 7243, 7247, 7253, 7283, 7297, 7307, 7309, 7321, 7331, 7333, 7349, 7351, 7369, 7393, 7411, 7417, 7433, 7451, 7457, 7459, 7477, 7481, 7487, 7489, 7499, 7507, 7517, 7523, 7529, 7537, 7541, 7547, 7549, 7559, 7561, 7573, 7577, 7583, 7589, 7591, 7603, 7607, 7621, 7639, 7643, 7649, 7669, 7673, 7681, 7687, 7691, 7699, 7703, 7717, 7723, 7727, 7741, 7753, 7757, 7759, 7789, 7793, 7817, 7823, 7829, 7841, 7853, 7867, 7873, 7877, 7879, 7883, 7901, 7907, 7919, 7927, 7933, 7937, 7949, 7951, 7963, 7993, 8009, 8011, 8017, 8039, 8053, 8059, 8069, 8081, 8087, 8089, 8093, 8101, 8111, 8117, 8123
};
int MaxStationsCount;
int MaxSid;
/* 
 *  InsertVoter: Eisagei enan neo psifoforo sto dentro me vasi to voter ID.
 * An den yparxei riza, dimiourgei kainourgio voter. 
 */
Voter* InsertVoter(Voter* root, int vid) {
    /* An i riza einai NULL, dimiourgoume neo voter kai epistrefoume ton komvo */
    if (root == NULL) {
        Voter* newVoter = (Voter*)malloc(sizeof(Voter));
        newVoter->vid = vid;      /*  voter ID */
        newVoter->voted = false;  /* Arxikopoiei oti den exei psifisei */
        newVoter->parent = NULL;  /* Den exei gonea */
        newVoter->lc = NULL;      /* Arxikopoiei lc  */
        newVoter->rc = NULL;      /* Arxikopoiei rc  */
        return newVoter;          /* Epistrefei ton neo voter */
    }

    /* An to voter ID einai mikrotero, paei sto aristero subtree */
    if (vid < root->vid) {
        root->lc = InsertVoter(root->lc, vid);
        root->lc->parent = root;  /* Orizei ton trexonta komvo os gonea */
    } 
    /* An to voter ID einai megalitero, paei sto dexi subtreee */
    else if (vid > root->vid) {
        root->rc = InsertVoter(root->rc, vid);
        root->rc->parent = root;  /* Orizei ton trexonta komvo ws gonea */
    }

    return root;  /* Epistrefei ti riza tou dentrou */
}

/* 
 * Synartisi FindDistrict: Psaxnei gia mia eklogiki perifereia vasei tou district ID.
 * Epistrefei pointer sti perifereia an vrethei, alliws NULL.
 */
District* FindDistrict(int did) {

    for (int i = 0; i < DISTRICTS_SZ; ++i) {
        if (Districts[i].did == did) {
            return &Districts[i]; /* Epistrefei pointer stin antistoixi perifereia */
        }
    }
    return NULL; /* Epistrefei NULL an den vrethei i perifereia */
}


void EventAnnounceElections(int parsedMaxStationsCount, int parsedMaxSid) {


    MaxStationsCount = parsedMaxStationsCount;
    MaxSid = parsedMaxSid;

    /* Arxikopoiei ton pinaka ton districts */
    for (int i = 0; i < DISTRICTS_SZ; ++i) {
        Districts[i].did = -1; /* default timh */
        Districts[i].seats = 0;
        Districts[i].blanks = 0;
        Districts[i].invalids = 0;
        memset(Districts[i].partyVotes, 0, sizeof(Districts[i].partyVotes));
    }

    /*  mnimi gia ton pinaka hashing ton stations */
    StationsHT = (Station**)malloc(MaxStationsCount * sizeof(Station*));
    if (StationsHT == NULL) {
        perror("Failed to allocate memory for StationsHT"); /* Memory fail */
        exit(EXIT_FAILURE);
    }

    for (int i = 0; i < MaxStationsCount; ++i) {
        StationsHT[i] = NULL; 
            }

    for (int i = 0; i < PARTIES_SZ; ++i) {
        Parties[i].pid = i;          /*  ID party */
        Parties[i].electedCount = 0;
        Parties[i].candidates = NULL; /* Empty List*/
    }

    Parliament = NULL;

    printf("Elections announced with MaxStationsCount=%d and MaxSid=%d\n", MaxStationsCount, MaxSid);
}


/* 
 * Synartisi EventCreateStation: Dimiourgei ena neo eklogiko tmima gia sygkekrimeni perifereia.
 * Dexetai os eisodo to station ID (sid) kai to district ID (did).
 */
void EventCreateStation(int sid, int did) {
    DebugPrint("S %d %d\n", sid, did);

    /* Elegxos an i perifereia einai egkiri */
    District* district = FindDistrict(did);
    if (district == NULL || district->seats <= 0) {
        printf("Error: District %d not found or invalid for station %d\n", did, sid);
        return;
    }

    /* Ypologismos timis katakermatismou gia to station ID */
    int h = sid % MaxStationsCount;

    /* Dimiourgia neou eklogikou tmimatos */
    Station* newStation = (Station*)malloc(sizeof(Station));
    if (newStation == NULL) {
        perror("Failed to allocate memory for new station");
        exit(EXIT_FAILURE);
    }

    /* Arxikopoiei ta dedomena tou neou station */
    newStation->sid = sid;
    newStation->did = did;
    newStation->registered = 0; /* Kenh lista me voters */
    newStation->voters = NULL;  
    newStation->next = StationsHT[h]; /* Pros8hkh stin arxi tis alisidas */
    StationsHT[h] = newStation;

    /* Ektypwsh katastasis tis alisidas gia ton hash index */
    printf("Hash Index: %d\n", h);
    printf("Stations in Chain:\n");
    Station* current = StationsHT[h];
    int count = 0;
    while (current != NULL) {
        printf("  Station %d\n", current->sid);
        current = current->next;
        count++;
    }
    printf("Total Stations in Chain: %d\n", count);
}

/* 
 *  EventCreateDistrict: Dimiourgei mia nea eklogiki perifereia
 * Dexetai ws input to district ID (did) kai ta seats
 */
void EventCreateDistrict(int did, int seats) {
    DebugPrint("D %d %d\n", did, seats);

    /* Elegxos gia mi egkyro arithmo edrwn */
    if (seats <= 0) {
        printf("Error: Invalid number of seats (%d) for district %d\n", seats, did);
        return;
    }

    /* Elegxos an to district yparxei idi */
    if (FindDistrict(did) != NULL) {
        printf("Error: District %d already exists\n", did);
        return;
    }

    /* Evresh tis prwths diathesimis thesis ston pinaka twn districts */
    for (int i = 0; i < DISTRICTS_SZ; ++i) {
        if (Districts[i].did == -1) { /* Eleutheri thesi */
            Districts[i].did = did;    /* Orizei to district ID */
            Districts[i].seats = seats; /* Orizei ton arithmo edron */
            Districts[i].blanks = 0;    /* leuka */
            Districts[i].invalids = 0;  /* akyra*/
            memset(Districts[i].partyVotes, 0, sizeof(Districts[i].partyVotes));

            /* Epivevaiwsh oti t district dimiourgithike */
            printf("District %d created with %d seats\n", did, seats);

            /* Ektypwsi olon ton  districts */
            printf("Current Districts:\n");
            for (int j = 0; j < DISTRICTS_SZ; ++j) {
                if (Districts[j].did != -1) {
                    printf("  District %d with %d seats\n", Districts[j].did, Districts[j].seats);
                }
            }
            return;
        }
    }

    /* An den yparxei diathesimi thesi gia tin perifereia */
    printf("Error: No available slot for new district\n");
}

/* 
 *  EventRegisterVoter: Kanei eggrafi enos psifoforou se sygkekrimeno station
 * Dexetai os eisodo to voter ID (vid) kai to station ID (sid)
 */
void EventRegisterVoter(int vid, int sid) {
    DebugPrint("R %d %d\n", vid, sid);

    /* Ypologismos timis katakermatismou gia to station ID */
    int h = sid % MaxStationsCount;

    /* Eyresi tou eklogikou tmimatos sto chain */
    Station* station = StationsHT[h];
    while (station != NULL) {
        if (station->sid == sid) {
            /* Eisagwgh psifoforou sto dentro tou station */
            station->voters = InsertVoter(station->voters, vid);

            /* aukshsh tou plithous eggrafon */
            station->registered++;

            /* Ektypwsh olon ton psifoforwn */
            printf("Station ID: %d\n", sid);
            printf("Registered Voters (%d):\n", station->registered);
            PrintVotersInOrder(station->voters);
            return;
        }
        station = station->next;
    }

    /* An den vrethei to eklogiko tmima */
    printf("Error: Station %d not found\n", sid);
}

/*
 *  EventRegisterCandidate: Kanei eggrafi enos ypopsifiou se sygekrimeno koma kai perifereia
 * Dexetai os eisodo to candidate ID (cid), party ID (pid) kai district ID (did)
 */
void EventRegisterCandidate(int cid, int pid, int did) {
    DebugPrint("C %d %d %d\n", cid, pid, did);

    /* Elegxos an yparxei i perifereia */
    if (FindDistrict(did) == NULL) {
        printf("Error: District %d not found\n", did);
        return;
    }

    /* Elegxos egkyrotitas tou party ID */
    if (pid < 0 || pid >= PARTIES_SZ) {
        printf("Error: Invalid party ID %d\n", pid);
        return;
    }

    /* Elegxos an yparxei i perifereia ston pinaka */
    bool districtExists = false;
    for (int i = 0; i < DISTRICTS_SZ; ++i) {
        if (Districts[i].did == did) {
            districtExists = true;
            break;
        }
    }
    if (!districtExists) {
        printf("Error: District %d not found for candidate %d\n", did, cid);
        return;
    }

    /* Dimiourgia neou ypopsifiou */
    Candidate* newCandidate = (Candidate*)malloc(sizeof(Candidate));
    if (newCandidate == NULL) {
        perror("Failed to allocate memory for new candidate");
        exit(EXIT_FAILURE);
    }
    newCandidate->cid = cid;     /* Candidate ID */
    newCandidate->did = did;     /* District ID */
    newCandidate->votes = 0;     /* Arxikopoisi twn psifwn */
    newCandidate->isElected = false; 
    newCandidate->lc = NULL;
    newCandidate->rc = NULL;

    /* Eisagwgh sto dentro ypopsifiwn tou kommatos */
    Candidate** root = &Parties[pid].candidates;
    while (*root != NULL) {
        if (cid < (*root)->cid) {
            root = &((*root)->lc);
        } else {
            root = &((*root)->rc);
        }
    }
    *root = newCandidate;

    /* Ektypwsh olon twn ypopsifiwn tou kommatos me in-order traversal */
    printf("Party ID: %d\n", pid);
    printf("Registered Candidates:\n");
    PrintCandidatesInOrder(Parties[pid].candidates);
    printf("\n");
}


/*
 *  PrintCandidatesInOrder: Ektypwnei olo to dentro ypopsifiwn
 * me in-order traversal.
 */
void PrintCandidatesInOrder(Candidate* node) {
    if (node == NULL) return;
    PrintCandidatesInOrder(node->lc);
    printf("  Candidate ID: %d, District ID: %d\n", node->cid, node->did);
    PrintCandidatesInOrder(node->rc);
}

/*
 *  EventVote: Kataxorei mia psifo gia sygkekrimeno psifoforo, candidate kai party
 */
void EventVote(int vid, int sid, int cid, int pid) {
    DebugPrint("V %d %d %d %d\n", vid, sid, cid, pid);

    /* Eyresi tou station */
    int h = sid % MaxStationsCount;
    Station* station = StationsHT[h];
    while (station != NULL && station->sid != sid) {
        station = station->next;
    }
    if (station == NULL) {
        printf("Error: Station %d not found\n", sid);
        return;
    }

    /* Eyresi tou voter */
    Voter* voter = station->voters;
    while (voter != NULL && voter->vid != vid) {
        if (vid < voter->vid) {
            voter = voter->lc;
        } else {
            voter = voter->rc;
        }
    }
    if (voter == NULL) {
        printf("Error: Voter %d not found\n", vid);
        return;
    }
    if (voter->voted) {
        printf("Error: Voter %d has already voted\n", vid);
        return;
    }

    /* Simainei oti o psifoforos exei psifisei */
    voter->voted = true;

    /* Eyresi tou district */
    int did = station->did;
    District* district = NULL;
    for (int i = 0; i < DISTRICTS_SZ; ++i) {
        if (Districts[i].did == did) {
            district = &Districts[i];
            break;
        }
    }
    if (district == NULL) {
        printf("Error: District %d not found\n", did);
        return;
    }

   
    if (cid == -1) { /* leuko  */
        district->blanks++;
    } else if (cid == -2) { /* akuro  */
        district->invalids++;
    } else {
        /* Eyresh ypopsifiou sto party */
        Candidate* candidate = Parties[pid].candidates;
        while (candidate != NULL && candidate->cid != cid) {
            if (cid < candidate->cid) {
                candidate = candidate->lc;
            } else {
                candidate = candidate->rc;
            }
        }
        if (candidate == NULL) {
            printf("Error: Candidate %d not found in Party %d\n", cid, pid);
            return;
        }

        /* Enhmerwsh twn psifwn */
        candidate->votes++;
        district->partyVotes[pid]++;
    }

    /* Ektypwsi apotelesmatwn gia tin perifereia */
    printf("District ID: %d\n", did);
    printf("Party Votes:\n");
    for (int i = 0; i < PARTIES_SZ; ++i) {
        printf("  Party %d: %d votes\n", i, district->partyVotes[i]);
    }
    printf("Blanks: %d\n", district->blanks);
    printf("Invalids: %d\n", district->invalids);
}


/*
 * MinHeapify: Diatirei tin idiotita tou Min-Heap apo ena sygkekrimeno komvo.
 * Dexetai ton pinaka heap[], ton komvo i pou elegxetai kai to megethos size tou heap.
 * Anamenetai oti oi paidikoi komvoi exoun idio gia tous aristerous kai dexious ypokomvous.
 */
void MinHeapify(MinHeapNode heap[], int i, int size) {
    int smallest = i;            /* Arxika thetoume ton trexonta komvo ws mikrotero */
    int left = 2 * i + 1;        /* Index tou lc  */
    int right = 2 * i + 2;       /* Index tou rc */

    /* Elegxos an to lc einai mikrotero */
    if (left < size && heap[left].votes < heap[smallest].votes)
        smallest = left;

    /* Elegxos an to rc einai mikrotero */
    if (right < size && heap[right].votes < heap[smallest].votes)
        smallest = right;

    /* An brethike mikroteros komvos, kanoume antallagi kai ksanakaloume anadromika */
    if (smallest != i) {
        MinHeapNode temp = heap[i];
        heap[i] = heap[smallest];
        heap[smallest] = temp;
        MinHeapify(heap, smallest, size);
    }
}



/*
 *  InsertMinHeap: Eisagei neo komvo sto Min-Heap kai anadiorganonei to heap gia na diatirithei i idiotita
 * Dexetai ton pinaka heap[], ton neo candidate kai ton deikti sto megethos tou heap 
 */
void InsertMinHeap(MinHeapNode heap[], Candidate* candidate, int* size) {
    heap[*size].candidate = candidate; /* Eisagwgh tou ypopsifiou stin teleftaia thesi */
    heap[*size].votes = candidate->votes; /* Apo8ikeush twn psifwn tou ypopsifiou */
    int i = (*size)++; /* Au3anoume to megethos tou heap */

    /* anadromiki diorthosi gia na diatirithei to Min-Heap */
    while (i != 0 && heap[(i - 1) / 2].votes > heap[i].votes) {
        MinHeapNode temp = heap[i];
        heap[i] = heap[(i - 1) / 2];
        heap[(i - 1) / 2] = temp;
        i = (i - 1) / 2; /* Metafora ston gonea tou komvou */
    }
}


/*
 *  ReplaceRoot: Antikathista tin riza tou Min-Heap me neo ypopsifio kai ksanaftiaxnei to heap
 * Dexetai ton pinaka heap[], ton neo candidate kai to megethos size
 */
void ReplaceRoot(MinHeapNode heap[], Candidate* candidate, int size) {
    heap[0].candidate = candidate; /* Antikathista ton komvo stin riza */
    heap[0].votes = candidate->votes;
    MinHeapify(heap, 0, size); /* Anadromiki diorthosi gia na diatirithei i idiotita tou Min-Heap */
}

/*
 *  TraverseCandidates: Diatrexei olo to dentro twn ypopsifiwn me anadromi
 * kai eisagei tous ypopsifious sto Min-Heap
 * Dexetai to riza tou dentrou root, ton pinaka heap[], to megethos size kai to μέγιστο megethos maxSize.
 */
void TraverseCandidates(Candidate* root, MinHeapNode heap[], int* size, int maxSize) {
    if (root == NULL) return; /* Termatismos an se keno komvo */

    /* Eisagogi sto Min-Heap an yparxei diathesimos xwros */
    if (*size < maxSize) {
        InsertMinHeap(heap, root, size);
    }
    /* Elegxos gia antikatastasi tis rizas an o ypopsifios exei perissoteres psifous */
    else if (heap[0].votes < root->votes) {
        ReplaceRoot(heap, root, *size);
    }

    /* Anadromiki episkepsh sta subtrees */
    TraverseCandidates(root->lc, heap, size, maxSize);
    TraverseCandidates(root->rc, heap, size, maxSize);
}



/*
 *  ElectPartyCandidatesInDistrict: Eklegei ypopsifious enos kommatos se sygkekrimeni perifereia
 * Dexetai to ID tou kommatos  to ID tis perifereias  kai ton arithmo twn edrwn 
 * Xrisimopoiei Min-Heap gia na epilexei tous ypopsifious me tis perissoteres psifous
 */
void ElectPartyCandidatesInDistrict(int pid, int did, int seats) {
    (void)did; /* Kainourgio usage  gia to did, an den xrhsimopoihtai */
    MinHeapNode* heap = (MinHeapNode*)malloc(seats * sizeof(MinHeapNode));
    if (heap == NULL) {
        perror("Failed to allocate memory for heap");
        exit(EXIT_FAILURE);
    }

    int size = 0;
    /* eisagei tous katallilous ypopshfious sto Min-Heap */
    TraverseCandidates(Parties[pid].candidates, heap, &size, seats);

    /* mark  tous ypopsifious pou eklegontai */
    for (int i = 0; i < size; i++) {
        heap[i].candidate->isElected = true;
    }

    free(heap); /* Apeleutherwsh mnimis gia to Min-Heap */
}

/*
 *  EventCountVotes: Metraei tis psifous se mia sygkekrimeni perifereia kai eklegei ypopsifious
 * Dexetai to ID tis perifereias  kai metraei ta eklogika apotelesmata me vasi tin eklogiki pososta
 */
void EventCountVotes(int did) {
    District* district = NULL;

    /* Evresh tis perifereias me to sygkekrimeno ID */
    for (int i = 0; i < DISTRICTS_SZ; i++) {
        if (Districts[i].did == did) {
            district = &Districts[i];
            break;
        }
    }

    if (district == NULL) {
        printf("Error: District %d not found\n", did);
        return;
    }

    int totalVotes = 0;
    /* Ypologismos sinolikwn psifwn gia ola ta kommata */
    for (int i = 0; i < PARTIES_SZ; i++) {
        totalVotes += district->partyVotes[i];
    }

    if (district->seats == 0 || totalVotes == 0) {
        printf("No seats allocated or no votes in district %d\n", did);
        return;
    }

    int electoralQuota = totalVotes / district->seats; /* Ypologismos eklogikis posostas */
    int partyElected[PARTIES_SZ] = {0};

    for (int pid = 0; pid < PARTIES_SZ; pid++) {
        if (electoralQuota > 0) {
            partyElected[pid] = district->partyVotes[pid] / electoralQuota;
        }

        /* Elegxos twn diathesimwn ypopsifiwn tou kommatos */
        int availableCandidates = 0;
        Candidate* current = Parties[pid].candidates;
        while (current != NULL) {
            availableCandidates++;
            current = (current->lc != NULL) ? current->lc : current->rc;
        }

        /* Diasfalisi oti o arithmos eklegmenwn den ypobainei ton arithmo diathesimwn ypopsifiwn */
        if (partyElected[pid] > availableCandidates) {
            partyElected[pid] = availableCandidates;
        }

        /* Enhmerwsh ton eklogwn apotelesmatwn kai meiwsi twn diathesimwn edrwn */
        Parties[pid].electedCount += partyElected[pid];
        district->seats -= partyElected[pid];

        ElectPartyCandidatesInDistrict(pid, did, partyElected[pid]);
    }

    /* Ektypwsi apotelesmatwn */
    printf("District ID: %d\n", did);
    printf("Blanks: %d\n", district->blanks);
    printf("Invalids: %d\n", district->invalids);
    for (int pid = 0; pid < PARTIES_SZ; pid++) {
        printf("Party %d Votes: %d, Elected: %d\n", pid, district->partyVotes[pid], partyElected[pid]);
    }
}


/*
 *  TraverseElectedCandidates: Diatrexei tous ypopsifious enos kommatos kai prosthetei tous eklegmenous sto koinoboulio.
 * Dexetai riza to root, to parliamentHead kai to ID tou kommatos
 */
void TraverseElectedCandidates(Candidate* root, ElectedCandidate** parliamentHead, int pid) {
    if (root == NULL) return;

    /* anadromh sto left subtree */
    TraverseElectedCandidates(root->lc, parliamentHead, pid);

    /* Prosthetei eklegmeno ypopsifio sto koinoboulio */
    if (root->isElected) {
        printf("Adding Candidate ID: %d, Party ID: %d, District ID: %d\n", root->cid, pid, root->did);
        ElectedCandidate* newMember = (ElectedCandidate*)malloc(sizeof(ElectedCandidate));
        if (newMember == NULL) {
            perror("Failed to allocate memory");
            exit(EXIT_FAILURE);
        }
        newMember->cid = root->cid;
        newMember->pid = pid;
        newMember->did = root->did;
        newMember->next = NULL;

        /* Eisagogi stin lista */
        if (*parliamentHead == NULL) {
            *parliamentHead = newMember;
        } else {
            ElectedCandidate* current = *parliamentHead;
            while (current->next != NULL) current = current->next;
            current->next = newMember;
        }
    }

    /* Anadromikh episkepsh tou right subtree */
    TraverseElectedCandidates(root->rc, parliamentHead, pid);
}


/*
 *  EventFormParliament: Dimiourgei to teliko koinoboulio apo tous eklegmenous ypopsifious
 * Diatrexei ola ta kommata kai tous ypopsifious tous gia na prostethoun sto koinoboulio
 */
void EventFormParliament(void) {
    ElectedCandidate* parliamentHead = NULL;

    /* Diatrexei ola ta kommata gia na vrei tous eklegmenous ypopsifious */
    for (int pid = 0; pid < PARTIES_SZ; pid++) {
        TraverseElectedCandidates(Parties[pid].candidates, &parliamentHead, pid);
    }

    printf("Debug: Checking Parliament Members...\n");
    ElectedCandidate* current = parliamentHead;

    if (current == NULL) {
        printf("No elected candidates found.\n");
        return;
    }

    /* Ektypwsi twn melwn tou koinobouliou */
    printf("Parliament Members:\n");
    while (current != NULL) {
        printf("  Candidate ID: %d, Party ID: %d, District ID: %d\n", current->cid, current->pid, current->did);
        current = current->next;
    }

    /* Apeleutherwsh mnimis */
    while (parliamentHead != NULL) {
        ElectedCandidate* temp = parliamentHead;
        parliamentHead = parliamentHead->next;
        free(temp);
    }
}


/*
 *  EventPrintDistrict: Ektypwnei info gia mia sygkekrimeni perifereia
 * Dexetai to ID tis perifereias  kai emfanizei ton arithmo edrwn
 * ton arithmo la8os psifwn kai tous psifous gia kathe komma
 */
void EventPrintDistrict(int did) {
    District* district = NULL;

    /* Anazitish tis perifereias ston pinaka Districts */
    for (int i = 0; i < DISTRICTS_SZ; ++i) {
        if (Districts[i].did == did) {
            district = &Districts[i];
            break;
        }
    }

    /* Elegxos an vrethike i perifereia */
    if (district == NULL) {
        printf("Error: District %d not found\n", did);
        return;
    }

    /* Ektypwsi pliroforion tis perifereias */
    printf("District ID: %d\n", did);
    printf("Seats: %d\n", district->seats);
    printf("Blanks: %d\n", district->blanks);
    printf("Invalids: %d\n", district->invalids);
    printf("Party Votes:\n");

    /* Ektypwsi psifwn gia kathe komma */
    for (int i = 0; i < PARTIES_SZ; ++i) {
        printf("  Party %d: %d votes\n", i, district->partyVotes[i]);
    }
}



/*
 *  EventPrintParty: Ektypwnei plirofories gia ena sygkekrimeno komma.
 * Dexetai to ID tou kommatos kai emfanizei tous eklegmenous ypopsifious.
 */
void EventPrintParty(int pid) {
    /* Elegxos an to ID tou kommatos einai egkyro */
    if (pid < 0 || pid >= PARTIES_SZ) {
        printf("Error: Party %d not found\n", pid);
        return;
    }

    /* Ektypwsi stoixeiwn kommatos */
    printf("Party ID: %d\n", pid);
    printf("Elected Candidates:\n");

    /* Ektypwsh twn ypopsifiwn me endodiatetagmeni diasxish */
    PrintCandidatesInOrder(Parties[pid].candidates);
}



/*
 *  PrintVotersInOrder: Diatrexei kai ektypwnei to dentro twn eggrafwn psifoforwn me seira
 * Dexetai riza tou dentrou (root) kai emfanizei ID kai katastasi (an exei psifisei)
 */
void PrintVotersInOrder(Voter* root) {
    if (root == NULL) return;

    /* Anadromh lc */
    PrintVotersInOrder(root->lc);

    /* Ektypwsh stoixeiwn tou psifoforou */
    printf("  Voter ID: %d, Voted: %s\n", root->vid, root->voted ? "Yes" : "No");

    /* Anadromh rc */
    PrintVotersInOrder(root->rc);
}

/*
 *  EventPrintStation: Ektypwnei plirofories gia ena sygkekrimeno eklogiko tmima
 * Dexetai to ID tou tmimatos  kai emfanizei tous eggrafous psifoforous
 */
void EventPrintStation(int sid) {
    /* Ypologismos hash index gia to eklogiko tmima */
    int h = sid % MaxStationsCount;
    Station* station = StationsHT[h];

    /* Anazhthsh tou eklogikou tmimatos stin lista */
    while (station != NULL && station->sid != sid) {
        station = station->next;
    }

    /* Elegxos an vrethike to tmima */
    if (station == NULL) {
        printf("Error: Station %d not found\n", sid);
        return;
    }

    /* Ektypwsh pliroforiwn tou eklogikou tmimatos */
    printf("Station ID: %d\n", sid);
    printf("District ID: %d\n", station->did);
    printf("Registered Voters: %d\n", station->registered);
    printf("Voters:\n");

    /* Ektypwsh twn psifoforwn me xrhsh endodiatetagmenis diatakshs
     */
    PrintVotersInOrder(station->voters);
}

/*
 *  EventPrintParliament: Ektypwnei ta melh tou koinovouliou kai tin katastasi twn perifereiwn
 * Elegxei tis edres kai praxeis gia kathe perifereia, analoga me tis psifous
 */
void EventPrintParliament() {
    printf("Debug: Checking and Printing Parliament Members...\n");

    /* Epanafora eklegmenwn upopsifiwn se 0 prin tin nea katamoni */
    for (int i = 0; i < PARTIES_SZ; ++i) {
        Parties[i].electedCount = 0;
    }

    /* Epanafora kai katamoni edrwn gia kathe perifereia */
    for (int i = 0; i < DISTRICTS_SZ; ++i) {
        if (Districts[i].did == -1) continue;

        int remainingSeats = Districts[i].seats; // Diathesimes edres
        int tempVotes[PARTIES_SZ];

        /* Antigrafi ton psifwn twn kommatwn */
        for (int p = 0; p < PARTIES_SZ; ++p) {
            tempVotes[p] = Districts[i].partyVotes[p];
        }

        /* Katanomh edrwn stous nikhtes */
        while (remainingSeats > 0) {
            int maxVotes = 0, winningParty = -1;

            for (int p = 0; p < PARTIES_SZ; ++p) {
                if (tempVotes[p] > maxVotes) {
                    maxVotes = tempVotes[p];
                    winningParty = p;
                }
            }

            if (winningParty == -1) break;

            Parties[winningParty].electedCount++;
            tempVotes[winningParty] = 0;
            remainingSeats--;
        }
    }

    /* Ektypwsh melwn tou koinovouliou */
    printf("\nParliament Members:\n");
    for (int p = 0; p < PARTIES_SZ; ++p) {
        printf("Party ID: %d, Elected Candidates: %d\n", p, Parties[p].electedCount);
    }

    /* Ektypwsh psifwn gia kathe perifereia */
    for (int i = 0; i < DISTRICTS_SZ; ++i) {
        if (Districts[i].did == -1) continue;

        printf("\nDistrict ID: %d\n", Districts[i].did);
        printf("Seats: %d\n", Districts[i].seats);
        printf("Blanks: %d\n", Districts[i].blanks);
        printf("Invalids: %d\n", Districts[i].invalids);
        printf("Party Votes:\n");
        for (int p = 0; p < PARTIES_SZ; ++p) {
            printf("  Party %d: %d votes\n", p, Districts[i].partyVotes[p]);
        }
    }
}

/*
 *  EventBonusUnregisterVoter: Kanei diagrafi enos psifoforou apo to eklogiko tmima
 * Dexetai voter ID  kai station ID 
 */
void EventBonusUnregisterVoter(int vid, int sid) {
    /* Ypologismos hash index gia to eklogiko tmima */
    int h = sid % MaxStationsCount;
    Station* station = StationsHT[h];

    /* Anazitish tou eklogikou tmimatos */
    while (station != NULL && station->sid != sid) {
        station = station->next;
    }

    if (station == NULL) {
        printf("Error: Station %d not found\n", sid);
        return;
    }

    /* Diagrafh psifoforou apo to dentro */
    station->voters = DeleteVoterFromTree(station->voters, vid, &(station->registered));

    /* Ektypwsh ananeomenwn pliroforiwn tou tmimatos */
    printf("Station ID: %d\n", sid);
    printf("Registered Voters: %d\n", station->registered);
    PrintVotersInOrder(station->voters);
}


/*
 *  DeleteVoterFromTree: Diagrafei ena voter apo to dentro me anazitisi kai epanafora
 */
Voter* DeleteVoterFromTree(Voter* root, int vid, int* registeredCount) {
    if (root == NULL) return NULL;

    if (vid < root->vid) {
        root->lc = DeleteVoterFromTree(root->lc, vid, registeredCount);
    } else if (vid > root->vid) {
        root->rc = DeleteVoterFromTree(root->rc, vid, registeredCount);
    } else {
        /* Vrethike o voter kai diagrafetai */
        (*registeredCount)--;

        if (root->lc == NULL) {
            Voter* temp = root->rc;
            free(root);
            return temp;
        } else if (root->rc == NULL) {
            Voter* temp = root->lc;
            free(root);
            return temp;
        }

        /* Evresh tou elaxistou sto right subtree */
        Voter* temp = FindMin(root->rc);
        root->vid = temp->vid;
        root->voted = temp->voted;
        root->rc = DeleteVoterFromTree(root->rc, temp->vid, registeredCount);
    }

    return root;
}


/*
 *  FindMin: Vriskei ton komvo me to mikrotero voter ID se ena dentro
 */
Voter* FindMin(Voter* root) {
    while (root->lc != NULL) {
        root = root->lc;
    }
    return root;
}


/*
 *  EventBonusFreeMemory: Apodesmevei olous tous porous kai mnhmes twn domwn dedomenwn
 */
void EventBonusFreeMemory(void) {
    /* Apodesmeush mnimis gia StationsHT */
    for (int i = 0; i < MaxStationsCount; ++i) {
        Station* current = StationsHT[i];
        while (current != NULL) {
            Station* temp = current;
            FreeVotersTree(temp->voters);
            current = current->next;
            free(temp);
        }
    }
    free(StationsHT);
    StationsHT = NULL;

    /* Apodesmeush mnimis gia ta kommatika dedomena */
    for (int i = 0; i < PARTIES_SZ; ++i) {
        FreeCandidatesTree(Parties[i].candidates);
        Parties[i].candidates = NULL;
    }

    /* Apodesmeush mnimis gia to koinovoulio */
    ElectedCandidate* current = Parliament;
    while (current != NULL) {
        ElectedCandidate* temp = current;
        current = current->next;
        free(temp);
    }
    Parliament = NULL;

    printf("BF\nDONE\n");
}



/*
 *  FreeVotersTree: Apodesmevei anadromika olo to dentro twn voters
 */
void FreeVotersTree(Voter* root) {
    if (root == NULL) return;
    FreeVotersTree(root->lc);
    FreeVotersTree(root->rc);
    free(root);
}

/*
 * Synartisi FreeCandidatesTree: Apodesmevei anadromika olo to dentro twn ypopsifiwn
 */
void FreeCandidatesTree(Candidate* root) {
    if (root == NULL) return;
    FreeCandidatesTree(root->lc);
    FreeCandidatesTree(root->rc);
    free(root);
}


