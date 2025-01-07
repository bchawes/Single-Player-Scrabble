#include "scrabble.h"
#include "random.h"
#include <assert.h>
#include <stdio.h>
#include <string.h>

// Global variables to hold game board and bag of tiles.
// NOTE: These are **not** used to store strings. They are arrays,
// and each array element is a character.

// Board for this version is an NxN grid, where N is given by the BOARD_SIZE variable.
// The board is stored as a 2D array of characters.
// The first index is the row and the second is the column.
// To the user, the numbering of rows/columns starts with 1,
// but the array indices start with 0.
// (The real Scrabble board uses letter for columns, but we'll keep it simple.)
// In other words, square 11 is the top left, and its character is stored
// in board[0][0].

char board[BOARD_SIZE][BOARD_SIZE];

// This version of the game does not use blank tiles, so there are 98 tiles,
// following the standard Scrabble distribution of letters.
char bag[NUM_TILES];
unsigned int tilesInBag;  // how many tiles are currently in the bag?
// NOTE: tiles will be taken from the end of the array, to make it simpler
// to discard/exchange tiles and then remix.

// points for each letter, in A-Z order
const int letterPoints[26] = {1, 3, 3,  2, 1, 4, 2, // A, B, C, D, E, F, G
                              4, 1, 8,  5, 1, 3, 1, // H, I, J, K, L, M, N
                              1, 3, 10, 1, 1, 1, 1, // O, P, Q, R, S, T, U
                              4, 4, 8, 4, 10        // V, W, X, Y, Z
};
// number of tile for each letter, in A-Z order
const int letterTiles[26] = {9, 2, 2, 4, 12, 2, 3, // A, B, C, D, E, F,G
                             2, 9, 1, 1, 4, 2, 6, // H, I, J, K, L, M, N
                             8, 2, 1, 6, 4, 6, 4, // O, P, Q, R, S, T, U
                             2, 2, 1, 2, 1         // V, W, X, Y, Z
};

const char * dictionaryFileName = "nofile.txt";


// FUNCTION DEFINITIONS
// board is initialized to empty
// bag of tile is initialized the proper number of each letter,
// and then mixed randomly

int scrabbleInit(const char *filename) {
   //Place all tiles in bag
   int i, j;   // loop counters
   tilesInBag = 0; // initialize number of tiles in bag to 0
   tilesInBag = 0;
   for (i = 0; i < 26; i++) { // for each letter
      for (j = 0; j < letterTiles[i]; j++) { // add the right number of tiles
         bag[tilesInBag++] = 'a' + i;  // add the letter to the bag
      }
   }
   mixBag();  // mix the bag
   dictionaryFileName = filename;   //Saves dictionary file name
   for (i = 0; i < BOARD_SIZE; i++) {  // initialize board to empty
      for (j = 0; j < BOARD_SIZE; j++) {
         board[i][j] = 0;  // empty square
      }
   }
   FILE *fp = fopen(filename, "r");   //Checks if file can be opened
   if (fp == NULL) {
      printf("FILE NOT FOUND\n");
      return -1;
   }  // Error opening file, returns -1
   return 0;   //Otherwise, return 0
}

// fill empty slots in tile rack from the bag
// if the bag gets empty, stop
// return the number of tiles removed
int chooseTiles(char rack[]) {
   int i;
   for(i = 0; i < RACK_SIZE; i++) { // for each slot in the rack
      if(rack[i] == 0) { // if the slot is empty
         if(tilesInBag == 0) {return i;} // if the bag is empty, return the number of tiles removed
         rack[i] = bag[--tilesInBag]; // add a tile to the rack
      }
   }
   return i; // return the number of tiles removed
}

// check whether all tiles in the selection string are in the player's tile rack
int checkTiles(const char *selection, const char rack[]) {
   //check if each tile in selection is in the rack
   int alreadyused[8] = {0}; // array to keep track of tiles already used
   for(int i = 0; i < strlen(selection); i++) {
      int tilehere = 0;
      for(int j = 0; j< RACK_SIZE; j++) {
         if(selection[i] == rack[j]) {
            if(alreadyused[j] == 1 ){}
            else {
               alreadyused[j] = 1;
               tilehere = 1; break;
            }
         }
      }
      if(tilehere != 1) {return 0;}
   }
   return 1; // return 1 if all tiles are in the rack
}

// given a word (null-terminated string), calculate its Scrabble score
int scoreWord(const char *word) {   // calculate the score of a word
   int score = 0; // initialize score to 0
   // loop counter
   for(int i = 0; i < strlen(word); i++) { // for each letter in the word
      int id = word[i] - 'a'; // get the letter's id
      score += letterPoints[id]; // add the letter's score to the total score
   }
   return score; // return the total score
}

// place tiles on the board to form a word
// this function does not test whether the word is legal, or will even fit on the board
// that checking should be done BEFORE calling this function
void placeWord(const char *word, unsigned row, unsigned col, unsigned dir, char rack[]) { // place the word on the board
   // place the word on the board
   // loop counter
   for(int i = 0; i < strlen(word); i++) { // for each letter in the word, place it on the board

      for(int j = 0; j < RACK_SIZE; j++) { // for each tile in the rack
         if(word[i] == rack[j]) {
            rack[j] = 0; break;
         } // if the tile is in the rack, empty the slot and break
      }
      if (board[row][col] == 0){
         board[row][col] = word[i]; // place the letter on the board
      }
      else {
         i--;
      }
      if(dir == 0) { // if the direction is across
         col++;   // move to the next column
      } // if the direction is across, move to the next column
      else {row++;} // if the direction is down, move to the next row
   }
   return;
}


// using characters provided by player (selection) and from the board,
// determine the word created in the specified direction
// testWord array is provided by the caller
// testWord must have enough elements to hold the longest word (entire row/column)
// returns the number of on-board characters used (must be >0 except for first turn)




int buildWord(const char* selection, unsigned int row, unsigned int col,
              unsigned int dir, char * testWord) {
   // build the word in the specified direction
   int k = 0;
   int off = 0;
   int numTiles = 0;
   int maxlim;
   *testWord = '\0';
   //Orient Direction and limit
   if (dir == 0) {
      maxlim = BOARD_SIZE-col;
   } // if the direction is across, set the maximum limit to the number of columns left
   else {
      maxlim = BOARD_SIZE-row;
   } // if the direction is down, set the maximum limit to the number of rows left

   if (strlen(selection) > maxlim) { // if the selection is too long, return -1
      return -1;
   }
   int incol = col;
   int inrow = row;
   int tmpcol = 0;
   int tmprow = 0;
   int is =0;
   while (tmpcol < 8 && tmprow < 8) {
      if(dir == 0) {// if the direction is across
         printf("Current col is %d\n", incol +is);
         if(board[tmprow][tmpcol] != 0) {
            testWord[off++] = board[tmprow][tmpcol];
            numTiles++;
         }
         else if (tmpcol == (incol + is)){
            testWord[off++] = selection[k++];
            is++;
         }
         else{printf("\nepic fail\n");}
         printf("At postion %d,%d, there is %c \n" , tmprow, tmpcol, board[tmprow][tmpcol]);
         printf("Current word: %s\n", testWord);
         tmpcol++;
      }
      else {
         if(board[tmprow][tmpcol] != 0) {
            testWord[off++] = board[tmprow][tmpcol];
            numTiles++;
         }
         else {
            testWord[off++] = selection[k++];
         }
         printf("At postion %d,%d, there is %c \n" , tmprow, tmpcol, board[tmprow][tmpcol]);
         tmprow++;
      }
   }
   row = inrow;
   col = incol;
   printf("Word to test: %s, at %d,%d\n", testWord, row, col); //Debugging
   return numTiles;
}



// look for word in dictionary
// return 1 if found, 0 if not found
int findInDictionary(const char* word) {
   FILE *fp = fopen(dictionaryFileName, "r"); // open dictionary file
   if (fp == NULL) {fclose(fp); printf("File not found...\n"); return 0;} // if file cannot be opened, return 0
   char check[50]; // create a string to hold the word being checked
   while(fscanf(fp, "%s", check) != EOF) { // while there are words in the dictionary
      if(strcmp(check, word) == 0) {fclose(fp); return 1;} // if the word is found, return 1
   }
   fclose(fp); //close the file
   return 0; // if the word is not found, return 0
}

// remove some tiles from the rack
// pull new tiles from the bag
// add removed tiles to the bag and reshuffle
void discardTiles(const char* selection, char rack[]) {
   // discard the tiles in the selection
   int i;   // loop counter
   for(i = 0; i < strlen(selection); i++) { // for each tile in the selection
      for (int j = 0; j < RACK_SIZE; j++) { // for each tile in the rack
         if (selection[i] == rack[j]) {   // if the tile is in the rack
            rack[j] = 0;
            break;  // empty the slot and break
         }
      }
   }
   chooseTiles(rack); // choose new tiles to fill the rack
   for(i = 0; i<strlen(selection); i++) {
      bag[tilesInBag] = selection[i];
      tilesInBag ++;
   } //add the discarded tiles back to the bag
   mixBag(); // mix the bag

   return;
}

// accept user move -- string of chars, starting square, direction -- and
// calculate the score earned by that move; if illegal, return -1
// if move is legal but word not found in dictionary, return -2
// direction: 0 = across, 1 = down
//
// this function does not check whether the characters are in the player's
// tile rack -- that should be checked before this function is called
//
int isBoardEmpty();  // helper function

int scoreMove(const char* selection, unsigned int row, unsigned int col,
              unsigned int dir) {
   assert(row < BOARD_SIZE);
   assert(col < BOARD_SIZE);
   assert(dir <= 1);
   assert(selection);
   // if starting square is occupied, illegal
   if (board[row][col] != 0) return -1;


   // first, assemble the new word created by this move
   char testWord[9] = "";
   int score = 0;
   // first, make sure it's a legal word in the specified direction
   int tilesUsed = buildWord(selection, row, col, dir, testWord);
   // if this is a single-letter word, it's illegal
   if (strlen(testWord) < 2) return -1;
   // if found in dictionary, get score -- if not, it's illegal
   if (findInDictionary(testWord)) {
      score += scoreWord(testWord);
   }
   else
      return -2;  // word not found

   // now look at words created by these letters in the opposite direction
   const char *wptr = selection;
   char single[2] = ""; // string to test in the other direction
   while (*wptr) {
      single[0] = *wptr++;
      // try to build word using this char at this position (ignore return value)
      tilesUsed += buildWord(single, row, col, !dir, testWord);
      // if word is only one character, ignore it
      // otherwise, check in dictionary and add to score
      if (strlen(testWord) > 1) {
         if (findInDictionary(testWord)) {
            score += scoreWord(testWord);
         }
         else {
            return -2;  // word not found
         }
      }
      do {  // skip tiles that were already on the board
         if (dir == 0) col++; else row++;
      } while (board[row][col]);
   }
   // must use board tiles, unless this is the first turn
   if (!tilesUsed && !isBoardEmpty()) return -1;
   return score;
}

// check if board is completely empty (i.e., this is the first turn)
int isBoardEmpty() {
   // loop counters
   for (int i = 0; i<BOARD_SIZE; i++) {   // for each row
      for (int j = 0; j<BOARD_SIZE; j++) {   // for each column
         if (board[i][j]) return 0;   // return 0 if not empty
      }
   }
   return 1;  // return 1 if empty
}

// mix letters in bag, using permutation algorithm described in PS3
void mixBag() {
   for (unsigned int i = tilesInBag - 1; i >= 1; i--) {
      unsigned int j = getRandom(i + 1);
      char tmp = bag[i];
      bag[i] = bag[j];
      bag[j] = tmp;
   }
}