 #ifndef CHECKERSSTATE_H
#define CHECKERSSTATE_H

#include <cstdlib>
#include <cstring>
#include <iostream>
#include <vector>

typedef unsigned char uint8;

enum checker {
    WHITE = 1,				// white cheker
    BLACK = 2,				// black checker
    WHITEKING = 3,			// white king
    BLACKKING = 4,			// black king
    WHITECELL = 10,			// non-playing cell, i%2 != j%2

    MOVEDFROM = 20,
    MOVEDTO = 21,
    MOVEDTHROUGH = 22,
    DELETED = 23,
    MARKDELETED = 24,
    TOKING = 25				// became king
};

class point {
public:
	point() {
		x=0; y=0; type=0;
	}
	point(int x, int y, int type) {
		this->x = x;
		this->y = y;
		this->type = type;
	}
	uint8 x;
	uint8 y;
	uint8 type;
bool operator == (const point & p ) {
        if ( x == p.x && y == p.y && type == p.type )
			return true;
		return false;
	}
};

class CheckersState
{
public:
    CheckersState();
	CheckersState(uint8 size);
	~CheckersState();
	CheckersState(const CheckersState &source);
	CheckersState(const CheckersState *source);

    CheckersState * genNextState(std::vector <point> & v);	// generetes new state from current
                                                            // takes move`s vector as argument
	void setParent(CheckersState * parent);
    std::vector <uint8> & counts();
	CheckersState * parent();
    std::vector < CheckersState * > & childs();
	std::vector < point > & move();
    int & deletedAtMove();	// number of checkers beated as result of going into this state
	int & score();
	void print();
    uint8 size() const;
	uint8 & at(uint8 i, uint8 j);
    uint8 color(uint8 i, uint8 j);
	bool isWhite(uint8 i, uint8 j);
	bool isBlack(uint8 i, uint8 j);
	bool isKing(uint8 i, uint8 j);
	bool isNull(uint8 i, uint8 j);

private:
    CheckersState * p;
    std::vector < CheckersState * > xchilds;
    std::vector < point > xmove;				// description of move from parent state to current
    std::vector < uint8 > xcounts;              // number of checkers of diff types and number of moves possible
                                                // [0]-white checkers, [1]-white kings, [2]- moves possible for white
                                                // [3]-black checkers, [4]-black kings, [5]- moves possible for black
	void allocate(uint8 size);
	uint8 ** data;
	uint8 n;
	uint8 tmp;
	int xscore;
	int deletedatmove;
};

#endif // CHECKERSSTATE_H
