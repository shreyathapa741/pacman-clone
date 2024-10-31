Overview:
This is a simple text-based implementation of the classic Pac-Man game in C. The game features Pac-Man navigating through a randomly generated maze, collecting pellets while avoiding ghosts. The objective is to collect all pellets without losing all your lives.

Features:
Maze Generation: The maze is randomly generated with walls and pellets.
Pac-Man Movement: Control Pac-Man using the keyboard (WASD keys) to navigate through the maze.
Ghosts: Two ghosts roam the maze, and if they collide with Pac-Man, a life is lost.
Pellet Collection: Collect pellets to increase your score. The game tracks the number of remaining pellets.

Game Controls:
w: Move up
s: Move down
a: Move left
d: Move right
p: Pause the game
r: Restart the game
q: Quit the game

Structure:
Maze Representation: The maze is represented as a 2D array of characters, with specific symbols for Pac-Man, walls, pellets, and ghosts.
Data Structures:
Linked List: Used to track the positions of collected pellets.
Queue: Used for ghost movement visualization.
Graph: An adjacency matrix representing the maze connections.
