#ifndef CORE_PUYO_COLOR_H_
#define CORE_PUYO_COLOR_H_

#include <glog/logging.h>
#include "base/base.h"

// TODO(mayah): Consider use enum class.
enum PuyoColor : byte {
    EMPTY = 0,
    OJAMA = 1,
    WALL = 2,
    RED = 4,
    BLUE = 5,
    YELLOW = 6,
    GREEN = 7,
    INVALID = 9,
};

const int PUYO_COLORS = 10;
const int PUYO_ERASE_NUM = 4;
const int NUM_NORMAL_PUYO_COLORS = 4;

inline PuyoColor normalPuyoColorOf(int index)
{
    DCHECK(0 <= index && index < NUM_NORMAL_PUYO_COLORS);
    return static_cast<PuyoColor>(index + 4);
}

// Returns puyo color from character.
PuyoColor puyoColorOf(char);

// Converts PuyoColor to character.
char charOfPuyoColor(PuyoColor);

const char* toString(PuyoColor);

// Returns true if puyo is normal color puyo.
inline bool isNormalColor(PuyoColor color)
{
    return static_cast<int>(PuyoColor::RED) <= static_cast<int>(color) && static_cast<int>(color) <= static_cast<int>(PuyoColor::GREEN);
}

#endif
