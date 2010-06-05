#include "XeTeX_ext.h"

#include <map>
#include <iostream>
#include <assert.h>
using namespace std;

typedef pair<int, unsigned int> GlyphId;
typedef map<GlyphId, int>  ProtrusionFactor;
ProtrusionFactor leftProt, rightProt;

extern "C" {

void set_cp_code(int fontNum, unsigned int code, int side, int value)
{
    GlyphId id(fontNum, code);
    switch (side) {
    case LEFT_SIDE:
        leftProt[id] = value;
        break;
    case RIGHT_SIDE:
        rightProt[id] = value;
        break;
    default:
        assert(0); // we should not reach here
    }
}

int get_cp_code(int fontNum, unsigned int code, int side)
{
    GlyphId id(fontNum, code);
    ProtrusionFactor* container;
    switch (side) {
    case LEFT_SIDE:
        container = &leftProt;
        break;
    case RIGHT_SIDE:
        container = &rightProt;
        break;
    default:
        assert(0); // we should not reach here
    }
    ProtrusionFactor::iterator it = container->find(id);
    if (it == container->end())
        return 0;
    return it->second;
}

}


