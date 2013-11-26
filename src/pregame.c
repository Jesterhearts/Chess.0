/*
 * pregame.c
 * 
 * Implementations of the functions described in pregame.h
 * 
 * @author Daniel Rogers
 *
 */

#include "pregame.h"

/*
 * Generates the table of valid moves for each piece/position in addition to
 * the bitboards representing attacking squares for each piece/position. The
 * resulting table will be exported as a binary file to be used by
 * loadMoveTables()
 *
 * @modifies legal_moves, attacked_squares in board.h
 */
void generateMoveTable()
{
    //TODO Generate the tables and store in a file
}

/*
 * Loads in a binary file created by generateMoveTable to initializer the
 * pre-calculated move tables.
 *
 * @modifies legal_moves, attacked_squares in board.h
 *
 * @returns true if the file loaded successfully, false if some error occurred
 *          which prevented a successful initialization of legal_moves and
 *          attacked_squares.
 */
bool loadMoveTables()
{
    //TODO Load in the table
    return (false);
}

/*
 * Populates the hashkey table for the board with randomly generated N-bit
 * values, where N is the bitwidth of HASHKEY.
 *
 * @modifies key_table in board.h
 */
void generateHashkeys()
{
    //TODO Generate the hashkey tables
}

/*
 * Calculates the moves available to a pawn piece from a location
 *
 * @owner Daniel Rogers
 *
 * @param location The location of the pawn
 * @param moves An array to fill with the available moves from that location
 *              They will be in order from left to right
 *              (from white's perspective)
 * @param white true if calculating moves for the white pieces
 */
void calcPawnMoves(uint8_t location, uint8_t moves[3], bool white)
{
    //If location / 8 == 7, then it's a value in the range 56-63
    //  and in the last row, so no moves are valid for white (except promotion)
    //  Same if location / 8 == 0 and black
    if ((white && ((location / 8) < 7)) || (!white && location / 8 > 0))
    {
        //white moves away from 0:0, black moves towards 0:0
        int8_t delta = (white) ? 8 : -8;
        uint8_t col = location % 8;
        //Valid moves are up and left, up, up and right
        //up
        moves[1] = location + delta;

        //Check for left edge, if location % 8 == 0, then it's a leftmost square
        //  and can have no up/left value, otherwise move up a row and back 1
        moves[0] = (col == 0) ? INVALID_SQUARE : (moves[1] - 1);

        //Check for right edge, if location %8 == 7, then it's a rightmost square
        moves[2] = (col == 7) ? INVALID_SQUARE : (moves[1] + 1);
    }
    else
    {
        moves[0] = moves[1] = moves[2] = INVALID_SQUARE;
    }
}

/*
 * Calculates the moves available to a knight piece from a location
 *
 * @owner Daniel Rogers
 *
 * @param location The location of the knight
 * @param moves An array to fill with the available moves from that location
 *              They will travel clockwise from the top-right
 *              (from white's perspective)
 */
void calcKnightMoves(uint8_t location, uint8_t moves[8])
{
    //Check if location allows movement upwards, and how much
    //  Needed for up2/right, right2/up, up2/left, left2/up
    //  if location is < 7 * 8, then able to move upwards at least 1
    if (location / 8 < 7)
    {
        //Needed for up2/right, up2/left
        //  if location is < 6 * 8, then able to move upwards at least 2
        if (location / 8 < 6)
        {
            //if location % 8 < 7, then can move one right
            //  Knight moves 2 up, 1 over, so set to 2*8 + 1 for + 2 rows + 1 col
            moves[0] = (location % 8 > 6) ? INVALID_SQUARE : location + 16 + 1;
            //  if location % 8 > 0, then can move one left
            moves[7] = (location % 8 == 0) ? INVALID_SQUARE : location + 16 - 1;
        }
        else
        {
            moves[0] = INVALID_SQUARE;
            moves[7] = INVALID_SQUARE;
        }

        //If location % 8 < 6, then can move 2 right and up 1
        moves[1] = (location % 8 > 5) ? INVALID_SQUARE : location + 2 + 8;
        //if location % 8 > 1, then can move 2 left and up 1
        moves[6] = (location % 8 < 2) ? INVALID_SQUARE : location - 2 + 8;
    }
    else
    {
        //Can't move into higher columns
        moves[0] = INVALID_SQUARE;
        moves[1] = INVALID_SQUARE;
        moves[7] = INVALID_SQUARE;
        moves[6] = INVALID_SQUARE;
    }

    //Check for movement downwards and how much
    //  Needed for right2/down, down2/right, down2/left, left2/down
    if (location / 8 > 0)
    {
        //Needed for down2/right, down2/left
        //  if location / 8 > 1, then able to move down at least 2
        if (location / 8 > 1)
        {
            //if location % 8 < 7, then able to move right at least 1
            moves[3] = (location % 8 > 6) ? INVALID_SQUARE : location - 16 + 1;
            //if location % 8 > 0, then able to move left at least 1
            moves[4] = (location % 8 == 0) ? INVALID_SQUARE : location - 16 - 1;
        }
        else
        {
            moves[3] = INVALID_SQUARE;
            moves[4] = INVALID_SQUARE;
        }

        //if location % 8 < 6, then able to move right at least 2
        moves[2] = (location % 8 > 5) ? INVALID_SQUARE : location + 2 - 8;
        //if location % 8 > 1, then able to move left at least 2
        moves[5] = (location % 8 < 2) ? INVALID_SQUARE : location - 2 - 8;
    }
    else
    {
        //Can't move into lower columns
        moves[2] = INVALID_SQUARE;
        moves[3] = INVALID_SQUARE;
        moves[5] = INVALID_SQUARE;
        moves[4] = INVALID_SQUARE;
    }
}

/*
 * Calculates the moves available to a bishop piece from a location
 *
 * @owner Daniel Rogers
 *
 * @param location The location of the bishop
 * @param moves An array of arrays to fill with the available moves from that
 *              location. Each sub-array will be filled with the moves
 *              traveling radially from the piece, with each moveset being
 *              filled clockwise from the top
 *              (from white's perspective)
 */
void calcBishopMoves(uint8_t location, uint8_t moves[4][7])
{
    //This is the same as for the queen, but without the
    //  vertical/horizontal moves
    uint8_t NE, SE, SW, NW;
    NE = SE = SW = NW = location;

    uint8_t i;
    //Extend rays
    for (uint8_t i = 0; i < 7; ++i)
    {
        moves[0][i] = (NE < INVALID_SQUARE) ? NE : INVALID_SQUARE;
        moves[1][i] = (SE < INVALID_SQUARE) ? SE : INVALID_SQUARE;
        moves[2][i] = (SW < INVALID_SQUARE) ? SW : INVALID_SQUARE;
        moves[3][i] = (NW < INVALID_SQUARE) ? NW : INVALID_SQUARE;

        //Update ray positions on board
        //Uses the fact that unsigned integer overflow has defined behavior
        //Moving up a row and right a column
        //  Don't go off the right edge of the board
        NE = (NE % 8 < 7 && NE < INVALID_SQUARE) ? NE + 8 + 1 : INVALID_SQUARE;
        //Moving down a row and right a column
        SE = (SE % 8 < 7 && SE < INVALID_SQUARE) ? SE - 8 + 1 : INVALID_SQUARE;
        //Moving down a row and left a column
        SW = (SW % 8 > 0 && SW < INVALID_SQUARE) ? SW - 8 - 1 : INVALID_SQUARE;
        //Moving up a row and left a column
        NW = (NW % 8 > 0 && NW < INVALID_SQUARE) ? NW + 8 - 1 : INVALID_SQUARE;
    }
}

/*
 * Calculates the moves available to a rook piece from a location
 *
 * @owner Daniel Rogers
 *
 * @param location The location of the rook
 * @param moves An array of arrays to fill with the available moves from that
 *              location. Each sub-array will be filled with the moves
 *              traveling radially from the piece, with each moveset being
 *              filled clockwise from the top
 *              (from white's perspective)
 */
void calcRookMoves(uint8_t location, uint8_t moves[4][7])
{
    //Same as for queen, but only horizontal/vertical
    uint8_t N, E, S, W;
    N = E = S = W = location;

    uint8_t i;
    //Extend rays
    for (uint8_t i = 0; i < 7; ++i)
    {
        moves[0][i] = (N < INVALID_SQUARE) ? N : INVALID_SQUARE;
        moves[1][i] = (E < INVALID_SQUARE) ? E : INVALID_SQUARE;
        moves[2][i] = (S < INVALID_SQUARE) ? S : INVALID_SQUARE;
        moves[3][i] = (W < INVALID_SQUARE) ? W : INVALID_SQUARE;

        //Update ray positions on board
        //Moving up a row
        N = (N < INVALID_SQUARE) ? N + 8 : INVALID_SQUARE;
        //Moving right a column
        E = (E % 8 < 7 && E < INVALID_SQUARE) ? E + 1 : INVALID_SQUARE;
        //Moving down a row
        S = (S < INVALID_SQUARE) ? S - 8 : INVALID_SQUARE;
        //Moving left a column
        W = (W % 8 > 0 && W < INVALID_SQUARE) ? W - 1 : INVALID_SQUARE;
    }
}

/*
 * Calculates the moves available to a queen piece from a location
 *
 * @owner Daniel Rogers
 *
 * @param location The location of the queen
 * @param moves An array of arrays to fill with the available moves from that
 *              location. Each sub-array will be filled with the moves
 *              traveling radially from the piece, with each moveset being
 *              filled clockwise from the top
 *              (from white's perspective)
 */
void calcQueenMoves(uint8_t location, uint8_t moves[8][7])
{
    uint8_t N, NE, E, SE, S, SW, W, NW;
    N = NE = E = SE = S = SW = W = NW = location;

    uint8_t i;
    //Extend rays
    for (uint8_t i = 0; i < 7; ++i)
    {
        moves[0][i] = (N < INVALID_SQUARE) ? N : INVALID_SQUARE;
        moves[1][i] = (NE < INVALID_SQUARE) ? NE : INVALID_SQUARE;
        moves[2][i] = (E < INVALID_SQUARE) ? E : INVALID_SQUARE;
        moves[3][i] = (SE < INVALID_SQUARE) ? SE : INVALID_SQUARE;
        moves[4][i] = (S < INVALID_SQUARE) ? S : INVALID_SQUARE;
        moves[5][i] = (SW < INVALID_SQUARE) ? SW : INVALID_SQUARE;
        moves[6][i] = (W < INVALID_SQUARE) ? W : INVALID_SQUARE;
        moves[7][i] = (NW < INVALID_SQUARE) ? NW : INVALID_SQUARE;

        //Update ray positions on board
        //Moving up a row
        N = (N < INVALID_SQUARE) ? N + 8 : INVALID_SQUARE;
        //Moving up a row and right a column
        //  Don't go off the right edge of the board
        NE = (NE % 8 < 7 && NE < INVALID_SQUARE) ? NE + 8 + 1 : INVALID_SQUARE;
        //Moving right a column
        E = (E % 8 < 7 && E < INVALID_SQUARE) ? E + 1 : INVALID_SQUARE;
        //Moving down a row and right a column
        //  Since using unsigned integers
        //  underflow (which has defined behavior) will cause the value to
        //      be > 64 (INVALID_SQUARE)
        SE = (SE % 8 < 7 && SE < INVALID_SQUARE) ? SE - 8 + 1 : INVALID_SQUARE;
        //Moving down a row
        S = (S < INVALID_SQUARE) ? S - 8 : INVALID_SQUARE;
        //Moving down a row and left a column
        SW = (SW % 8 > 0 && SW < INVALID_SQUARE) ? SW - 8 - 1 : INVALID_SQUARE;
        //Moving left a column
        W = (W % 8 > 0 && W < INVALID_SQUARE) ? W - 1 : INVALID_SQUARE;
        //Moving up a row and left a column
        NW = (NW % 8 > 0 && NW < INVALID_SQUARE) ? NW + 8 - 1 : INVALID_SQUARE;
    }
}
/*
 * Calculates the moves available to a king from a location
 *
 * @owner Daniel Rogers
 *
 * @param location The location of the king
 * @param moves An array to fill with the available moves from that location.
 *              Each array will be filled with the moves available to the
 *              piece, traveling clockwise from the top
 *              (from white's perspective)
 */
void calcKingMoves(uint8_t location, uint8_t moves[8])
{

}

