#ifndef FALL24_SCRABBLE_SCRABBLE_H
#define FALL24_SCRABBLE_SCRABBLE_H

// Global variables to hold game board and bag of tiles.

#define BOARD_SIZE 8
extern char board[BOARD_SIZE][BOARD_SIZE];

// This version of the game does not use blank tiles, so there are 98 tiles,
// following the standard Scrabble distribution of letters.
#define NUM_TILES 98
extern char bag[NUM_TILES];
extern unsigned int tilesInBag;  // how many tiles are currently in the bag?
// NOTE: tiles will be taken from the end of the array, to make it simpler
// to discard/exchange tiles and then remix.

// points for each letter, in A-Z order
extern const int letterPoints[26];

// number of tiles in each player's tile rack
#define RACK_SIZE 7

// Scrabble-related functions
int scrabbleInit(const char *dictionaryFile);  // initialize board, bag, and dictionary
void mixBag();
int chooseTiles(char rack[]);
int checkTiles(const char *selection, const char rack[]);
int scoreMove(const char* selection, unsigned int row, unsigned int col, unsigned int dir);
int scoreWord(const char *word);
int buildWord(const char *selection, unsigned int row, unsigned int col,
              unsigned int dir, char* word);
void placeWord(const char *selection, unsigned int row, unsigned int col, unsigned int dir,
               char rack[]);
int findInDictionary(const char *word);
void discardTiles(const char *selection, char rack[]);

#endif //FALL24_SCRABBLE_SCRABBLE_H
