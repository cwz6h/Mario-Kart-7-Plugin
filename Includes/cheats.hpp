#ifndef CHEATS_H
#define CHEATS_H

#include <CTRPluginFramework.hpp>
#include "Helpers.hpp"
#include "Unicode.h"

namespace CTRPluginFramework
{
    using StringVector = std::vector<std::string>;

    void invincible(MenuEntry *entry);
    void alwaysStarPower(MenuEntry *entry);
    void alwaysBlackKart(MenuEntry *entry);
    void alwaysShocked(MenuEntry *entry);
    void alwaysCrushed(MenuEntry *entry);
    void trickAnywhere(MenuEntry *entry);
    void randomItems(MenuEntry *entry);

}
#endif
