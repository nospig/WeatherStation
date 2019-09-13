#include "DisplayBase.h"

void DisplayBase::setDisplayMode(CurrentDisplayMode mode)
{
    displayMode = mode;
}

CurrentDisplayMode DisplayBase::getDisplayMode()
{
    return displayMode;
}
