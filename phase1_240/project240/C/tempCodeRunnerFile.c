/* Count votes and allocate seats (Event M) */
void count_votes(int did) {
    for (int i = 0; i < 56; i++) {
        if (districts[i].did == did) {
            struct candidate *cand = districts[i].candidates;
            int seats_left = districts[i].seats;

            while (cand && seats_left > 0) {
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
                cand = cand->next;
            }
            districts[i].allotted = districts[i].seats - seats_left;
            return;
        }
    }
}
