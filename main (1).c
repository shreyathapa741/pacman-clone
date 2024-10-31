#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#define ROWS 10
#define COLS 10

// Defining characters for display
#define PACMAN 'P'
#define WALL '#'
#define PELLET '.'
#define EMPTY ' '
#define GHOST 'G'

// Structure to represent Pac-Man's position
typedef struct {
    int x, y;
} Position;

// Node structure for linked list to track pellets
typedef struct PelletNode {
    Position position;
    struct PelletNode *next;
} PelletNode;

// Queue structure for ghost movement
typedef struct {
    Position positions[2]; // Fixed size for 2 ghosts
    int front;
    int rear;
} GhostQueue;

// Graph representation of the maze
typedef struct {
    int adj[ROWS * COLS][ROWS * COLS]; // Adjacency matrix for graph representation
} MazeGraph;

// Function prototypes
void initializeMaze(char maze[ROWS][COLS], PelletNode **pelletList, Position *pacman, Position ghosts[], int *pelletCount, MazeGraph *graph);
void displayMaze(char maze[ROWS][COLS], int score, int lives);
void movePacman(char maze[ROWS][COLS], Position *pacman, char move, int *score, int *lives, PelletNode **pelletList);
void moveGhosts(char maze[ROWS][COLS], Position ghosts[], GhostQueue *queue);
void clearInputBuffer();
void randomizeMaze(char maze[ROWS][COLS], PelletNode **pelletList, int *pelletCount, MazeGraph *graph);
void respawnPacman(Position *pacman);
void pauseGame();
void restartGame(char maze[ROWS][COLS], PelletNode **pelletList, Position *pacman, Position ghosts[], int *pelletCount, int *score, int *lives, MazeGraph *graph);
void addPellet(PelletNode **pelletList, Position pos);


// Initialize maze layout and positions of Pac-Man and ghosts
void initializeMaze(char maze[ROWS][COLS], PelletNode **pelletList, Position *pacman, Position ghosts[], int *pelletCount, MazeGraph *graph) {
    randomizeMaze(maze, pelletList, pelletCount, graph);
    pacman->x = 1;
    pacman->y = 1;
    ghosts[0].x = 5;
    ghosts[0].y = 8;
    ghosts[1].x = 6;
    ghosts[1].y = 8;
    printf("Total Pellets: %d\n", *pelletCount);
}

// Generate a random maze layout
void randomizeMaze(char maze[ROWS][COLS], PelletNode **pelletList, int *pelletCount, MazeGraph *graph) {
    *pelletCount = 0;

    for (int i = 0; i < ROWS; i++) {
        for (int j = 0; j < COLS; j++) {
            if (i == 0 || i == ROWS - 1 || j == 0 || j == COLS - 1) {
                maze[i][j] = WALL; // Set the borders as walls
            } else {
                if (rand() % 5 == 0) {
                    maze[i][j] = WALL; // 20% chance of a wall
                } else {
                    maze[i][j] = EMPTY; // Otherwise empty space
                    if (rand() % 4 == 0) {
                        maze[i][j] = PELLET; // 25% chance of a pellet
                        Position pos = {i, j};
                        addPellet(pelletList, pos); // Add pellet to linked list
                        (*pelletCount)++;
                    }
                }
            }
        }
    }
    maze[1][1] = PACMAN; // Place Pac-Man in the starting position

    // Graph representation (adjacency matrix)
    for (int i = 0; i < ROWS * COLS; i++) {
        for (int j = 0; j < ROWS * COLS; j++) {
            graph->adj[i][j] = 0; // Initialize adjacency matrix to 0
        }
    }

    // Populate the graph with connections based on the maze
    for (int i = 1; i < ROWS - 1; i++) {
        for (int j = 1; j < COLS - 1; j++) {
            if (maze[i][j] != WALL) {
                int current = i * COLS + j; // Convert 2D coordinates to 1D index
                if (maze[i - 1][j] != WALL) // Up
                    graph->adj[current][(i - 1) * COLS + j] = 1;
                if (maze[i + 1][j] != WALL) // Down
                    graph->adj[current][(i + 1) * COLS + j] = 1;
                if (maze[i][j - 1] != WALL) // Left
                    graph->adj[current][i * COLS + (j - 1)] = 1;
                if (maze[i][j + 1] != WALL) // Right
                    graph->adj[current][i * COLS + (j + 1)] = 1;
            }
        }
    }
}

// Function to add a pellet to the linked list
void addPellet(PelletNode **pelletList, Position pos) {
    PelletNode *newNode = (PelletNode *)malloc(sizeof(PelletNode));
    newNode->position = pos;
    newNode->next = *pelletList;
    *pelletList = newNode;
}

// Display maze, score, and lives
void displayMaze(char maze[ROWS][COLS], int score, int lives) {
    printf("Score: %d  Lives: %d\n", score, lives);
    for (int i = 0; i < ROWS; i++) {
        for (int j = 0; j < COLS; j++) {
            printf("%c", maze[i][j]);
        }
        printf("\n");
    }
}

// Move Pac-Man based on user input and update score/lives
void movePacman(char maze[ROWS][COLS], Position *pacman, char move, int *score, int *lives, PelletNode **pelletList) {
    int newX = pacman->x;
    int newY = pacman->y;

    switch (move) {
        case 'w': newX--; break; // Move up
        case 's': newX++; break; // Move down
        case 'a': newY--; break; // Move left
        case 'd': newY++; break; // Move right
        default: return;
    }

    // Check for collisions with walls
    if (maze[newX][newY] == WALL) return;

    // Check for pellets
    if (maze[newX][newY] == PELLET) {
        (*score)++;
        maze[newX][newY] = EMPTY;

        // Remove the pellet from the linked list
        PelletNode *current = *pelletList;
        PelletNode *prev = NULL;
        while (current != NULL) {
            if (current->position.x == newX && current->position.y == newY) {
                if (prev == NULL) {
                    *pelletList = current->next; // Update head
                } else {
                    prev->next = current->next; // Bypass the node
                }
                free(current); // Free memory
                break;
            }
            prev = current;
            current = current->next;
        }
    }

    // Check for ghost collision
    if (maze[newX][newY] == GHOST) {
        (*lives)--;
        maze[pacman->x][pacman->y] = EMPTY; // Clear old position
        respawnPacman(pacman); // Respawn Pac-Man
        maze[pacman->x][pacman->y] = PACMAN; // Update new position
        return;
    }

    // Move Pac-Man
    maze[pacman->x][pacman->y] = EMPTY; // Clear old position
    pacman->x = newX; // Update position
    pacman->y = newY;
    maze[newX][newY] = PACMAN; // Update new position
}

// Move ghosts and ensure they don't eat pellets
void moveGhosts(char maze[ROWS][COLS], Position ghosts[], GhostQueue *queue) {
    for (int i = 0; i < 2; i++) {
        int newX = ghosts[i].x + (rand() % 3 - 1); // Random movement in x direction
        int newY = ghosts[i].y + (rand() % 3 - 1); // Random movement in y direction

        // Keep ghosts within bounds
        if (newX < 1) newX = 1;
        if (newX >= ROWS - 1) newX = ROWS - 2;
        if (newY < 1) newY = 1;
        if (newY >= COLS - 1) newY = COLS - 2;

        // Check for walls
        if (maze[newX][newY] != WALL) {
            maze[ghosts[i].x][ghosts[i].y] = EMPTY; // Clear old position
            ghosts[i].x = newX; // Update position
            ghosts[i].y = newY;
            maze[newX][newY] = GHOST; // Update new position

            // Enqueue the ghost movement (not using for collision but for visualization if needed)
            queue->positions[++queue->rear % 2] = ghosts[i];
            if (queue->rear - queue->front >= 2) {
                queue->front++;
            }
        }
    }
}

// Clear the input buffer
void clearInputBuffer() {
    while (getchar() != '\n');
}

// Respawn Pac-Man in a random position
void respawnPacman(Position *pacman) {
    pacman->x = 1; // Respawn at a fixed location, could be randomized
    pacman->y = 1;
}

// Pause game function (placeholder)
void pauseGame() {
    printf("Game paused. Press Enter to continue...");
    clearInputBuffer();

}

// Restart game with initial values
void restartGame(char maze[ROWS][COLS], PelletNode **pelletList, Position *pacman, Position ghosts[], int *pelletCount, int *score, int *lives, MazeGraph *graph) {
    *score = 0;
    *lives = 3;
    initializeMaze(maze, pelletList, pacman, ghosts, pelletCount, graph);
}

int main() {
    char maze[ROWS][COLS];            // 2D array for the maze
    PelletNode *pelletList = NULL;     // Linked list to track pellets
    Position pacman;                   // Pac-Man's position
    Position ghosts[2];                // Ghost positions
    GhostQueue queue = { .front = 0, .rear = -1 }; // Initialize ghost queue
    MazeGraph graph;                   // Graph representation of the maze
    int score = 0, lives = 3, pelletCount = 0;
    char move;

    srand(time(NULL)); // Seed for random number generation

    initializeMaze(maze, &pelletList, &pacman, ghosts, &pelletCount, &graph);

    while (lives > 0) {
        system("clear"); // Use "cls" for Windows, "clear" for Linux/macOS
        displayMaze(maze, score, lives);
        printf("Pellets remaining: %d\n", pelletCount - score);
        printf("Controls: w (up), s (down), a (left), d (right), p (pause), r (restart), q (quit)\n");
        move = getchar();
        clearInputBuffer();

        if (move == 'p') {
            pauseGame();
        } else if (move == 'r') {
            restartGame(maze, &pelletList, &pacman, ghosts, &pelletCount, &score, &lives, &graph);
        } else if (move == 'q') {
            printf("Quitting the game. Goodbye!\n");
            break;
        } else {
            movePacman(maze, &pacman, move, &score, &lives, &pelletList);
            moveGhosts(maze, ghosts, &queue);
        }

        if (score == pelletCount) {
            system("clear");
            printf("Congratulations! You've eaten all the pellets!\n");
            break;
        }
    }

    system("clear");
    if (lives == 0) {
        printf("Game Over! You've run out of lives.\n");
    }

    // Free linked list memory
    while (pelletList) {
        PelletNode *temp = pelletList;
        pelletList = pelletList->next;
        free(temp);
    }

    return 0;
}


// End of the code
