/*
 * board.c
 * 
 * Implementations of the functions defined in board.h
 * 
 * @author Daniel Rogers
 * 
 */

#include "board.h"

/*
 * Initializes the board to its base state, where no moves have been made yet
 *
 * @uses white_initial, black_initial, location_bitboards
 *
 * @owner Daniel Rogers
 *
 * @param board The chessboard to initialize.
 */
void initBoard(chessboard * board)
{
    //Set up the occupation bitboards for all white/black pieces
    //White pieces are in positions 0-15 -> low 16 bits on
    board->all_w_pieces = 0;
    //Black pieces are in positions 48-63 -> low 48 off
    board->all_b_pieces = 0;

    //Set location arrays
    memcpy(board->w_pieces, white_initial, 16 * sizeof(uint8_t));
    memcpy(board->b_pieces, black_initial, 16 * sizeof(uint8_t));

    //Set location bitboards
    for (uint8_t i = 0; i < 16; ++i)
    {
        board->w_locations[i] = location_boards[board->w_pieces[i]];
        board->b_locations[i] = location_boards[board->b_pieces[i]];
        //Update population bitboard
        board->all_w_pieces |= board->w_locations[i];
        board->all_b_pieces |= board->b_locations[i];
    }

#ifdef DEBUG
    for (uint8_t i = 0; i < 16; ++i)
    {
        printf("w%d: %d\n", i, board->w_pieces[i]);
        printf("wloc%d: %llx\n", i, board->w_locations[i]);
        printf("b%d: %d\n", i, board->b_pieces[i]);
        printf("bloc%d: %llx\n", i, board->b_locations[i]);
    }
    printf("woc: %llx\n", board->all_w_pieces);
    printf("boc: %llx\n", board->all_b_pieces);
#endif
}

/*
 * Expands the set of all possible board states from an initial state
 *
 * @owner Daniel Rogers
 *
 * @uses location_boards, legal_moves
 *
 * @param board A pointer to the board to expand
 * @param storage A pointer to an array in which to store the expanded states
 *         !!This pointer will be realloc'd to fit the set of expanded states!!
 * @param white true if expanding the set of white moves
 * @return The number of states expanded
 */
uint8_t expandStates(chessboard * const board, chessboard * storage, bool white)
{
    //Select the appropriate sets of data
    //Piece locations
    uint8_t * pieces = (white) ? board->w_pieces : board->b_pieces;
    //Side location occupancy boards
    bitboard self = (white) ? board->all_w_pieces : board->all_b_pieces;
    bitboard op = (white) ? board->all_b_pieces : board->all_w_pieces;
    //Lookup table piece codes
    const uint8_t * codes = (white) ? board->w_codes : board->b_codes;

    //Number of states generated
    uint8_t states = 0;

    //Loop variables
    uint8_t i, j, k;
    //The set of moves
    uint8_t (*moves)[7];

    //Allocate enough storage
    storage = realloc(storage, 35 * sizeof(chessboard));

    //For each piece, get the set of moves it can make from its location
    for (i = 0; i < 16; ++i)
    {
        //Check if piece is captured
        if (pieces[i] == 64)
        {
            continue;
        }

        printf("piece: %d, @%d\n", codes[i], pieces[i]);
        printf("move0: %d\n", legal_moves[codes[i]][pieces[i]][0][0]);

        moves = legal_moves[codes[i]][pieces[i]];
        //Go through each move ray
        for (j = 0; j < 8; ++j)
        {
            //Go through each move in ray
            for (k = 0; k < 7; ++k)
            {
#ifdef DEBUG
                printf("making move: %d\n", moves[j][k]);
#endif
                //Check for end of ray, or own piece @ location
                //Location check works as follows:
                //  Get the bitboard representing the destination
                //  Then AND with pieces for own side -> all 0 if no pieces at
                //      destination, meaning valid move. !0 if own piece at
                //      location, meaning invalid move
                //  If it's a pawn, diagonals are only allowed on capture
                //location_boards[64] == 0xffffffffffffffff, so any AND will
                //  make a non-0 value
                if ((location_boards[moves[j][k]] & self)
                        || ((codes[i] == W_P || codes[i] == B_P) && (j != 0)
                                && (!(location_boards[moves[j][k]] & op))))
                {
                    //Stop looking through ray
#ifdef DEBUG
                    puts("breaking");
#endif
                    break;
                }
                else
                {
                    //Make the move with the piece
                    makeMove(i, moves[j][k], white, board, &storage[states++]);
                    if (((states % 35) == 0))
                    {
                        //Allocate more storage
                        storage = realloc(storage,
                                (states + 35) * sizeof(chessboard));
                    }
                }
            }
        }
    }

    return (states);
}

/*
 * Generates a new board state based on a piece move
 *
 * @owner Daniel Rogers
 *
 * @param piece The index of the piece to move
 * @param location The location to move to
 * @param white true If the piece being moved is white
 * @param current The chessboard state being referenced
 * @param new The new chessboard state to write to
 */
void makeMove(uint8_t piece, uint8_t location, bool white,
        chessboard * const current, chessboard * new)
{
    //Generate the new location bitboard for the new location
    bitboard new_loc = location_boards[location];

    //copy data
    memcpy(new, current, sizeof(chessboard));

    //Update the location
    if (white)
    {
        //Update the occupancy bitboard
        //XOR out the old location, XOR in the new location
        new->all_w_pieces ^= (new->w_locations[piece] ^ new_loc);
        //Update position bitboard
        new->w_locations[piece] = new_loc;
        //update piece location
        new->w_pieces[piece] = location;
    }
    else
    {
        //for black pieces
        //Update the occupancy bitboard
        //XOR out the old location, XOR in the new location
        new->all_b_pieces ^= (new->b_locations[piece] ^ new_loc);
        //Update position bitboard
        new->b_locations[piece] = new_loc;
        //update piece location
        new->b_pieces[piece] = location;
    }

}

/*
 * Handles making special moves such as castling/en passant/promotion
 *
 * @param piece The index of the piece to move
 * @param location The location to move to
 * @param white true If the piece being moved is white
 * @param current The chessboard state being referenced
 * @param new The new chessboard state to write to
 * @param promote_to If promoting, this is the piece code of the desired
 *                   promotion
 */
void moveSpecial(uint8_t piece, uint8_t location, bool white,
        chessboard * const current, chessboard * new, uint8_t promote_to)
{
    //Generate the new location bitboard for the new location
    bitboard new_loc = location_boards[location];

    //copy data
    memcpy(new, current, sizeof(chessboard));

    //If it's a pawn, then it's a promotion or en passant
    if (piece == W_P || piece == B_P)
    {
        //If in row 1 or row 8, then promoting pawn
        if ((location / 8) == 7)
        {
            //Row 8, so it's white
            //Update the occupancy bitboard
            //XOR out the old location, XOR in the new location
            new->all_w_pieces ^= (new->w_locations[piece] ^ new_loc);
            //Update position bitboard
            new->w_locations[piece] = new_loc;
            //update piece location
            new->w_pieces[piece] = location;
            //Update piece code
            new->w_codes[piece] = promote_to;
        }
        else if ((location / 8) == 0)
        {
            //Row 1, so it's black
            //Update the occupancy bitboard
            //XOR out the old location, XOR in the new location
            new->all_b_pieces ^= (new->b_locations[piece] ^ new_loc);
            //Update position bitboard
            new->b_locations[piece] = new_loc;
            //update piece location
            new->b_pieces[piece] = location;
            //Update piece code
            new->b_codes[piece] = promote_to;
        }
        else
        {
            //TODO Handle en passant captures
        }
    }
    else
    {
        //TODO Handle castling
    }
}

/*
 * Evaluates the value of a particular board
 *
 * @owner Daniel Rogers
 *
 * Based on:
 *   http://chessprogramming.wikispaces.com/Simplified+evaluation+function
 *
 * @uses all of the *_*_positions globals, modifier, piece_vals, *_codes
 *
 * @param board The board to evaluate
 * @return The value of the board in a form usable in a negamax function
 */
int evaluateState(chessboard * const board)
{
    int value = 0;
    int w_val = 0;
    int b_val = 0;

    //Sum up the values for white and black
    for (uint8_t i = 0; i < 16; ++i)
    {
        //Get each piece's value, and add in the value of its position
        //If it's captured, then add 0
        //White
        w_val +=
                (board->w_pieces[i] == CAPTURED) ?
                        0 :
                        piece_vals[board->w_codes[i]]
                                + board_position_vals[board->w_codes[i]][board->w_pieces[i]];
        //Same for Black
        b_val +=
                (board->b_pieces[i] == CAPTURED) ?
                        0 :
                        piece_vals[board->b_codes[i]]
                                + board_position_vals[board->b_codes[i]][board->b_pieces[i]];
    }

    //Value = white - black
    value = w_val - b_val;

    //If we are white, then we want max white, and this is already going to
    //  have higher scores for better values for white
    //If black, then multiply by -1 to flip it so higher scores returned mean
    //  better values for black
    value *= modifier;

    return (value);
}

