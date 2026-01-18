#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#define MAX_PLAYERS 64
#define MAX_NAME_LENGTH 30
#define MAX_HISTORY 50


typedef struct Player {
    char name[MAX_NAME_LENGTH];
    int seed;
    int wins;
    int losses;
} Player;

typedef struct Match {
    Player *player1;
    Player *player2;
    Player *winner;
    int score1;
    int score2;
    int round;
    int match_id;
    struct Match *left;   
    struct Match *right;  
} Match;

typedef struct Tournament {
    char name[MAX_NAME_LENGTH];
    Player players[MAX_PLAYERS];
    Match *bracket;  
    int player_count;
    int current_round;
    int total_rounds;
    int is_completed;
} Tournament;

typedef struct History {
    char tournament_name[MAX_NAME_LENGTH];
    char winner[MAX_NAME_LENGTH];
    char runner_up[MAX_NAME_LENGTH];
    int year, month, day;
} History;


History history[MAX_HISTORY];
int history_count = 0;
int global_match_id = 1;  


void create_tournament();
void setup_players(Tournament *t);
void manual_seeding(Tournament *t);
void auto_seeding(Tournament *t);
void generate_bracket(Tournament *t);
Match* create_match_tree(Player** players, int start, int end, int *round_counter);
void display_bracket(Tournament *t);
void display_bracket_tree(Match *match, int depth, int is_left);
void display_bracket_grid(Tournament *t);
void display_round_matches(Match *match, int round);
void play_tournament(Tournament *t);
void play_round(Match *match, int target_round, int *matches_played);
void play_match(Match *match);
void update_history(Tournament *t);
void view_history();
void save_history();
void load_history();

int main() {
    int choice;
    
    printf("=== TOURNAMENT BRACKET GENERATOR ===\n");
    load_history();
    srand(time(NULL));
    
    while (1) {
        printf("\n1. Create New Tournament\n");
        printf("2. View Tournament History\n");
        printf("3. Exit\n");
        printf("Choice: ");
        scanf("%d", &choice);
        
        switch (choice) {
            case 1:
                create_tournament();
                break;
            case 2:
                view_history();
                break;
            case 3:
                save_history();
                
                exit(0);
            default:
                printf("Invalid choice!\n");
        }
    }
    return 0;
}

void create_tournament() {
    Tournament t;
    t.is_completed = 0;
    
    printf("\nEnter tournament name: ");
    scanf("%s", t.name);
    
    printf("Enter number of players: ");
    scanf("%d", &t.player_count);
    
    if (t.player_count < 2 || t.player_count > MAX_PLAYERS) {
        printf("Invalid number of players! (2-%d)\n", MAX_PLAYERS);
        return;
    }
    
    setup_players(&t);
    generate_bracket(&t);
    
    
   
    display_bracket(&t);
    
    
    printf("\nDo you want to simulate this tournament? (1=Yes, 0=No): ");
    int simulate;
    scanf("%d", &simulate);
    
    if (simulate) {
        play_tournament(&t);
        if (t.is_completed) {
            update_history(&t);
        }
    }
}

void setup_players(Tournament *t) {
    int choice;
    
    printf("\n=== Player Setup ===\n");
    printf("1. Manual Seeding\n");
    printf("2. Auto Seeding\n");
    printf("Choice: ");
    scanf("%d", &choice);
    
    if (choice == 1) {
        manual_seeding(t);
    } else {
        auto_seeding(t);
    }
}

void manual_seeding(Tournament *t) {
    printf("\nEnter player names and seeds:\n");
    for (int i = 0; i < t->player_count; i++) {
        printf("Player %d name: ", i + 1);
        scanf("%s", t->players[i].name);
        printf("Seed for %s: ", t->players[i].name);
        scanf("%d", &t->players[i].seed);
        t->players[i].wins = 0;
        t->players[i].losses = 0;
    }
}

void auto_seeding(Tournament *t) {
    printf("\nEnter player names:\n");
    for (int i = 0; i < t->player_count; i++) {
        printf("Player %d name: ", i + 1);
        scanf("%s", t->players[i].name);
        t->players[i].seed = i + 1;
        t->players[i].wins = 0;
        t->players[i].losses = 0;
    }
    
    
    for (int i = t->player_count - 1; i > 0; i--) {
        int j = rand() % (i + 1);
        
        Player temp = t->players[i];
        t->players[i] = t->players[j];
        t->players[j] = temp;
    }
    
    
    for (int i = 0; i < t->player_count; i++) {
        t->players[i].seed = i + 1;
    }
    
    
}

int compare_seeds(const void *a, const void *b) {
    Player *p1 = *(Player **)a;
    Player *p2 = *(Player **)b;
    return p1->seed - p2->seed;
}

void generate_bracket(Tournament *t) {
    
    global_match_id = 1;
    
    
    Player *sorted_players[MAX_PLAYERS];
    for (int i = 0; i < t->player_count; i++) {
        sorted_players[i] = &t->players[i];
    }
    
    
   qsort(sorted_players, t->player_count, sizeof(Player*), compare_seeds);
    
    
    int players = t->player_count;
    t->total_rounds = 0;
    while (players > 1) {
        players = (players + 1) / 2;  
        t->total_rounds++;
    }
    
    
    int round_counter = t->total_rounds;
    t->bracket = create_match_tree(sorted_players, 0, t->player_count - 1, &round_counter);
    t->current_round = 1;
}

Match* create_match_tree(Player** players, int start, int end, int *round_counter) {
    if (start > end) return NULL;
    
    Match *match = (Match*)malloc(sizeof(Match));
    match->match_id = global_match_id++;  
    match->score1 = 0;
    match->score2 = 0;
    match->winner = NULL;
    match->left = NULL;
    match->right = NULL;
    
    
    if (end - start <= 1) {
        match->round = 1;
        match->player1 = players[start];
        
        
        if (end > start) {
            match->player2 = players[end];
        } else {
            
            match->player2 = NULL;
            match->winner = players[start];
        }
    } else {
        
        match->round = *round_counter;
        match->player1 = NULL;
        match->player2 = NULL;
        
        int mid = (start + end) / 2;
        
        
        (*round_counter)--;
        match->left = create_match_tree(players, start, mid, round_counter);
        match->right = create_match_tree(players, mid + 1, end, round_counter);
        (*round_counter)++;
        
        
        if (match->left && match->left->winner) {
            match->player1 = match->left->winner;
        }
        if (match->right && match->right->winner) {
            match->player2 = match->right->winner;
        }
    }
    
    return match;
}

void display_bracket(Tournament *t) {
    printf("\n=== TOURNAMENT BRACKET ===\n");
    display_bracket_grid(t);
}

void display_bracket_tree(Match *match, int depth, int is_left) {
    if (match == NULL) return;
    
    
    display_bracket_tree(match->right, depth + 1, 0);
    

    for (int i = 0; i < depth; i++) {
        printf("    ");
    }
    
    if (depth > 0) {
        printf(is_left ? "+-- " : "+-- ");
    }
    
    
    printf("R%d: ", match->round);
    if (match->player1 && match->player2) {
        printf("%s(%d) vs %s(%d)", 
               match->player1->name, match->player1->seed,
               match->player2->name, match->player2->seed);
    } else if (match->player1) {
        printf("%s(%d) - BYE", match->player1->name, match->player1->seed);
    } else {
        printf("TBD");
    }
    
    if (match->winner) {
        printf(" -> Winner: %s", match->winner->name);
    }
    printf("\n");
    
    
    display_bracket_tree(match->left, depth + 1, 1);
}

void display_bracket_grid(Tournament *t) {
    printf("Tournament: %s\n", t->name);
    printf("Players: %d\n\n", t->player_count);
    
    
    printf("Players List:\n");
    for (int i = 0; i < t->player_count; i++) {
        printf("%d. %s (Seed: %d)\n", i+1, t->players[i].name, t->players[i].seed);
    }
    
    printf("\nBracket Structure by Rounds:\n");
    for (int round = 1; round <= t->total_rounds; round++) {
        printf("\n--- Round %d ---\n", round);
        display_round_matches(t->bracket, round);
    }
}

void display_round_matches(Match *match, int round) {
    if (match == NULL) return;
    
    if (match->round == round) {
        if (match->player1 && match->player2) {
            printf("- %s (Seed %d) vs %s (Seed %d)", 
                   match->player1->name, match->player1->seed,
                   match->player2->name, match->player2->seed);
        } else if (match->player1) {
            printf("- %s (Seed %d) - BYE", match->player1->name, match->player1->seed);
        } else {
            printf("- TBD vs TBD");
        }
        if (match->winner) {
            printf(" -> %s WINS!", match->winner->name);
        }
        printf("\n");
    }
    
    display_round_matches(match->left, round);
    display_round_matches(match->right, round);
}

void play_tournament(Tournament *t) {
    printf("\n=== SIMULATING TOURNAMENT ===\n");
    
    // Play from round 1 to finals
    for (int round = 1; round <= t->total_rounds; round++) {
        printf("\n====================================\n");
        printf("         ROUND %d (of %d)           \n", round, t->total_rounds);
        printf("====================================\n");
        
        int matches_played = 0;
        play_round(t->bracket, round, &matches_played);
        
        if (matches_played == 0) {
            printf("No matches in this round (all byes)\n");
        }
        
        // Display updated bracket after each round
        printf("\n--- Bracket after Round %d ---\n", round);
        display_bracket_tree(t->bracket, 0, 0);
        
        if (round < t->total_rounds) {
            printf("\nPress Enter to continue to next round...");
            while(getchar() != '\n');  // Clear buffer
            getchar();  // Wait for Enter
        }
    }
    
    printf("\n****************************************\n");
    printf("     TOURNAMENT COMPLETE!               \n");
    printf("****************************************\n");
    if (t->bracket->winner) {
        printf("       CHAMPION: %s!\n", t->bracket->winner->name);
        printf("****************************************\n");
    }
    t->is_completed = 1;
}

void play_round(Match *match, int target_round, int *matches_played) {
    if (match == NULL) return;
    
    
    play_round(match->left, target_round, matches_played);
    play_round(match->right, target_round, matches_played);
    
    
    if (match->round == target_round && match->winner == NULL) {

        if (match->left && match->left->winner) {
            match->player1 = match->left->winner;
        }
        if (match->right && match->right->winner) {
            match->player2 = match->right->winner;
        }
        
        
        if (match->player1 && match->player2) {
            play_match(match);
            (*matches_played)++;
        } else if (match->player1 && !match->player2) {
            
            match->winner = match->player1;
            printf("\n%s advances (BYE)\n", match->player1->name);
        }
    }
}

void play_match(Match *match) {
    printf("\n>>> Match: %s vs %s <<<\n", match->player1->name, match->player2->name);
    
    
    match->score1 = 50 + (rand() % 51);  
    match->score2 = 50 + (rand() % 51);  
    
    
    if (match->score1 == match->score2) {
        match->score1++;
    }
    
    if (match->score1 > match->score2) {
        match->winner = match->player1;
    } else {
        match->winner = match->player2;
    }
    
    printf("    Score: %s %d - %d %s\n", 
           match->player1->name, match->score1, 
           match->score2, match->player2->name);
    printf("    ** WINNER: %s **\n", match->winner->name);
    
    
    match->winner->wins++;
    if (match->winner == match->player1) {
        match->player2->losses++;
    } else {
        match->player1->losses++;
    }
}

void update_history(Tournament *t) {
    if (history_count >= MAX_HISTORY) {
        printf("History storage full!\n");
        return;
    }
    
    History *h = &history[history_count];
    strcpy(h->tournament_name, t->name);
    strcpy(h->winner, t->bracket->winner->name);
    
    
    if (t->bracket->player1 == t->bracket->winner) {
        strcpy(h->runner_up, t->bracket->player2->name);
    } else {
        strcpy(h->runner_up, t->bracket->player1->name);
    }
    
    
    time_t now = time(NULL);
    struct tm *local = localtime(&now);
    h->year = local->tm_year + 1900;
    h->month = local->tm_mon + 1;
    h->day = local->tm_mday;
    
    history_count++;
    printf("Tournament added to history!\n");
}

void view_history() {
    printf("\n=== TOURNAMENT HISTORY ===\n");
    if (history_count == 0) {
        printf("No tournament history available.\n");
        return;
    }
    
    for (int i = 0; i < history_count; i++) {
        printf("%d. %s\n", i + 1, history[i].tournament_name);
        printf("   Winner: %s\n", history[i].winner);
        printf("   Runner-up: %s\n", history[i].runner_up);
        printf("   Date: %d/%d/%d\n\n", 
               history[i].month, history[i].day, history[i].year);
    }
}

void save_history() {
    FILE *file = fopen("tournament_history.dat", "wb");
    if (file) {
        fwrite(&history_count, sizeof(int), 1, file);
        fwrite(history, sizeof(History), history_count, file);
        fclose(file);
    }
}

void load_history() {
    FILE *file = fopen("tournament_history.dat", "rb");
    if (file) {
        fread(&history_count, sizeof(int), 1, file);
        fread(history, sizeof(History), history_count, file);
        fclose(file);
    }
}