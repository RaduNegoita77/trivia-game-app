#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <windows.h>

#define TOTAL_QUESTIONS 50
#define QUESTIONS_PER_GAME 10
#define MAX_NAME_LENGTH 50
#define LEADERBOARD_SIZE 10

typedef struct {
    char question[256];
    char options[4][100];
    int correct_option;
    int difficulty;
} Question;

typedef struct {
    char name[MAX_NAME_LENGTH];
    int score;
} Player;

typedef struct {
    char name[MAX_NAME_LENGTH];
    int currentIndex;
    int score;
} Progress;

Question questionSet[TOTAL_QUESTIONS];
Player leaderboard[LEADERBOARD_SIZE];
int leaderboardCount = 0;

void clearScreen() {
    system("cls");
}

void setColor(int color) {
    HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
    SetConsoleTextAttribute(hConsole, color);
}


void loadQuestionsFromFile() {
    FILE *file = fopen("questions.txt", "r");
    if (!file) {
        printf("Failed to open questions.txt\n");
        return;
    }

    int i = 0;
    char buffer[256];
    while (i < TOTAL_QUESTIONS && fgets(buffer, sizeof(buffer), file)) {
        buffer[strcspn(buffer, "\n")] = 0;
        strncpy(questionSet[i].question, buffer, sizeof(questionSet[i].question));

        for (int j = 0; j < 4; j++) {
            if (!fgets(buffer, sizeof(buffer), file)) break;
            buffer[strcspn(buffer, "\n")] = 0;
            strncpy(questionSet[i].options[j], buffer, sizeof(questionSet[i].options[j]));
        }

        if (!fgets(buffer, sizeof(buffer), file)) break;
        questionSet[i].correct_option = atoi(buffer);

        if (!fgets(buffer, sizeof(buffer), file)) break;
        questionSet[i].difficulty = atoi(buffer);

        fgets(buffer, sizeof(buffer), file);
        i++;
    }
    fclose(file);
}

void saveLeaderboardToFile() {
    FILE *file = fopen("leaderboard.txt", "w");
    if (!file) return;
    for (int i = 0; i < leaderboardCount; i++) {
        fprintf(file, "%s\t%d\n", leaderboard[i].name, leaderboard[i].score);
    }
    fclose(file);
}

void loadLeaderboardFromFile() {
    FILE *file = fopen("leaderboard.txt", "r");
    if (!file) return;

    leaderboardCount = 0;
    char name[MAX_NAME_LENGTH];
    int score;

    while (fscanf(file, "%s\t%d\n", name, &score) == 2 && leaderboardCount < LEADERBOARD_SIZE) {
        strncpy(leaderboard[leaderboardCount].name, name, MAX_NAME_LENGTH - 1);
        leaderboard[leaderboardCount].name[MAX_NAME_LENGTH - 1] = '\0';
        leaderboard[leaderboardCount].score = score;
        leaderboardCount++;
    }
    fclose(file);
}

void saveProgress(const Progress *progress) {
    FILE *file = fopen("progress.txt", "w");
    if (!file) return;
    fprintf(file, "%s\n%d\n%d\n", progress->name, progress->currentIndex, progress->score);
    fclose(file);
}

int loadProgress(Progress *progress) {
    FILE *file = fopen("progress.txt", "r");
    if (!file) return 0;
    fgets(progress->name, MAX_NAME_LENGTH, file);
    progress->name[strcspn(progress->name, "\n")] = 0;
    fscanf(file, "%d\n%d", &progress->currentIndex, &progress->score);
    fclose(file);
    return 1;
}

void deleteProgressFile() {
    remove("progress.txt");
}


void updateLeaderboard(char *name, int score) {
    Player newPlayer;
    strncpy(newPlayer.name, name, MAX_NAME_LENGTH - 1);
    newPlayer.name[MAX_NAME_LENGTH - 1] = '\0';
    newPlayer.score = score;

    if (leaderboardCount < LEADERBOARD_SIZE) {
        leaderboard[leaderboardCount++] = newPlayer;
    } else if (score > leaderboard[LEADERBOARD_SIZE - 1].score) {
        leaderboard[LEADERBOARD_SIZE - 1] = newPlayer;
    }

    for (int i = 0; i < leaderboardCount - 1; i++) {
        for (int j = i + 1; j < leaderboardCount; j++) {
            if (leaderboard[j].score > leaderboard[i].score) {
                Player temp = leaderboard[i];
                leaderboard[i] = leaderboard[j];
                leaderboard[j] = temp;
            }
        }
    }

    saveLeaderboardToFile();
}


void viewLeaderboard() {
    clearScreen();
    setColor(11);
    printf("\n=== Leaderboard ===\n");
    setColor(15);
    if (leaderboardCount == 0) {
        printf("No scores yet!\n");
    } else {
        for (int i = 0; i < leaderboardCount; i++) {
            printf("%d. %s - %d points\n", i + 1, leaderboard[i].name, leaderboard[i].score);
        }
    }
    printf("\nPress Enter to return to the menu...\n");
    getchar();
}

void playGame(Progress *resumeProgress, int isResume) {
    Progress progress;

    if (isResume && resumeProgress != NULL) {
        progress = *resumeProgress;
    } else {
        clearScreen();
        printf("Enter your name: ");
        scanf("%49s", progress.name);
        getchar();
        progress.currentIndex = 0;
        progress.score = 0;
    }

    for (int i = progress.currentIndex; i < QUESTIONS_PER_GAME && i < TOTAL_QUESTIONS; i++) {
        clearScreen();
        Question q = questionSet[i];
        setColor(11);
        printf("Question %d/%d:\n", i + 1, QUESTIONS_PER_GAME);
        setColor(15);
        printf("%s\n", q.question);
        for (int j = 0; j < 4; j++) {
            printf("%d) %s\n", j + 1, q.options[j]);
        }

        printf("Your answer: ");
        int answer;
        scanf("%d", &answer);
        getchar();

        if (answer - 1 == q.correct_option) {
            setColor(10);
            printf("Correct!\n");
            progress.score += q.difficulty;
        } else {
            setColor(12);
            printf("Wrong! The correct answer was: %s\n", q.options[q.correct_option]);
        }

        progress.currentIndex = i + 1;
        saveProgress(&progress);
        setColor(15);
        printf("\nPress Enter to continue...\n");
        getchar();
    }

    if (progress.currentIndex >= QUESTIONS_PER_GAME) {
        deleteProgressFile();
        clearScreen();
        printf("Game Over, %s! Your score: %d\n", progress.name, progress.score);
        updateLeaderboard(progress.name, progress.score);
        printf("\nPress Enter to return to the menu...\n");
        getchar();
    }
}


int main() {
    srand(time(NULL));
    loadQuestionsFromFile();
    loadLeaderboardFromFile();

    int choice;
    while (1) {
        clearScreen();
        setColor(11);
        printf("\n=== Trivia Game Menu ===\n");
        setColor(15); printf("1. "); setColor(14); printf("Start New Game\n");
        setColor(15); printf("2. "); setColor(14); printf("Resume Last Game\n");
        setColor(15); printf("3. "); setColor(14); printf("View Leaderboard\n");
        setColor(15); printf("4. "); setColor(12); printf("Exit\n");
        setColor(15); printf("Select an option: ");

        scanf("%d", &choice);
        getchar();

        switch (choice) {
            case 1:
                deleteProgressFile();
                playGame(NULL, 0);
                break;
            case 2: {
                Progress progress;
                if (loadProgress(&progress)) {
                    playGame(&progress, 1);
                } else {
                    printf("No saved game found.\nPress Enter to return.\n");
                    getchar();
                }
                break;
            }
            case 3:
                viewLeaderboard();
                break;
            case 4:
                printf("Exiting game...\n");
                return 0;
            default:
                printf("Invalid choice. Press Enter to retry.\n");
                getchar();
        }
    }
}
