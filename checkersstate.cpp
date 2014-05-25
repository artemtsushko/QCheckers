#include "checkersstate.h"

CheckersState::CheckersState()
{
    allocate(8);
}

CheckersState::CheckersState(uint8 size)
{
	allocate(size);
}

CheckersState::CheckersState(const CheckersState &source) {
	allocate(source.n);
    for (int i = 0; i < n; i++)
        for (int j = 0; j < n / 2; j++)
			data[i][j] = source.data[i][j];
	xcounts = source.xcounts;
}

CheckersState::CheckersState(const CheckersState *source) {
	allocate(source->n);
    for (int i = 0; i < n; i++)
        for (int j = 0; j < n / 2; j++)
			data[i][j] = source->data[i][j];
	xcounts = source->xcounts;
}

CheckersState::~CheckersState() {
    for (int i = 0; i < n; i++)
        delete[] data[i];
    delete[] data;
}

void CheckersState::allocate(uint8 size) {
	n = size;
	data = new uint8 * [n];
    for (int i = 0; i < n; i++) {
        data[i] = new uint8[n / 2];
        for (int j = 0; j < n / 2; j++)
			data[i][j] = 0;
	}
	tmp = 0;
	p = NULL;
	deletedatmove = 0;
}

uint8 CheckersState::size() const {
	return n;
}

std::vector <uint8> & CheckersState::counts() {
	return xcounts;
}

CheckersState * CheckersState::genNextState(std::vector <point> & v) {
	CheckersState * state = new CheckersState(this);
	uint8 tmp;
    for (unsigned k = 0; k < v.size(); k++) {
		uint8 i = v.at(k).x;
		uint8 j = v.at(k).y;
        switch (v.at(k).type) {
            case (MOVEDFROM):
				tmp = state->at(i,j);
				state->at(i,j) = 0;
				break;
            case (MOVEDTO):
				state->at(i,j) = tmp;
				break;
            case (MARKDELETED):
				state->at(i,j) = MARKDELETED;
				break;
            case (DELETED):
				state->at(i,j) = 0;
				break;
            case (TOKING):
                if ( tmp == WHITE )
					state->at(i,j) = WHITEKING;
                if ( tmp == BLACK )
					state->at(i,j) = BLACKKING;
				break;
			default:
				break;
		}
	}
	state->move() = v;
	state->counts().clear();
	return state;
}

void CheckersState::setParent(CheckersState * parent) {
	p = parent;
}

CheckersState * CheckersState::parent() {
	return p;
}

std::vector<CheckersState *> &CheckersState::childs() {
	return xchilds;
}

std::vector < point > & CheckersState::move() {
	return xmove;
}

int & CheckersState::score() {
	return xscore;
}

int & CheckersState::deletedAtMove() {
	return deletedatmove;
}

uint8 & CheckersState::at(uint8 i, uint8 j) {
    if (i % 2 != j % 2) {
		tmp = WHITECELL;
		return tmp;
	}
	return data[i][j/2];
}

uint8 CheckersState::color(uint8 i, uint8 j) {
    if ( at(i,j) == WHITE || at(i,j) == WHITEKING )
		return WHITE;
    if ( at(i,j) == BLACK || at(i,j) == BLACKKING )
		return BLACK;
	return false;
}

bool CheckersState::isWhite(uint8 i, uint8 j) {
    if ( at(i,j) == WHITE || at(i,j) == WHITEKING )
		return true;
	return false;
}

bool CheckersState::isBlack(uint8 i, uint8 j) {
    if ( at(i,j) == BLACK || at(i,j) == BLACKKING )
		return true;
	return false;
}

bool CheckersState::isKing(uint8 i, uint8 j) {
    if ( at(i,j) == WHITEKING || at(i,j) == BLACKKING )
		return true;
	return false;
}

bool CheckersState::isNull(uint8 i, uint8 j) {
    if ( at(i,j) == WHITECELL )
		return true;
	return !at(i,j);
}

