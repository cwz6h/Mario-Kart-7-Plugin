#include <3ds.h>
#include "csvc.h"
#include <CTRPluginFramework.hpp>

#include "cheats.hpp"
#include <vector>

namespace CTRPluginFramework
{
    // This patch the NFC disabling the touchscreen when scanning an amiibo, which prevents ctrpf to be used

        static MenuEntry *EntryWithHotkey(MenuEntry *entry, const std::vector<Hotkey> &hotkeys)
    {
        if (entry != nullptr)
        {
            for (const Hotkey &hotkey : hotkeys)
                entry->Hotkeys += hotkey;
        }

        return (entry);
    } 



    static void    ToggleTouchscreenForceOn(void)
    {
        static u32 original = 0;
        static u32 *patchAddress = nullptr;

        if (patchAddress && original)
        {
            *patchAddress = original;
            return;
        }

        static const std::vector<u32> pattern =
        {
            0xE59F10C0, 0xE5840004, 0xE5841000, 0xE5DD0000,
            0xE5C40008, 0xE28DD03C, 0xE8BD80F0, 0xE5D51001,
            0xE1D400D4, 0xE3510003, 0x159F0034, 0x1A000003
        };

        Result  res;
        Handle  processHandle;
        s64     textTotalSize = 0;
        s64     startAddress = 0;
        u32 *   found;

        if (R_FAILED(svcOpenProcess(&processHandle, 16)))
            return;

        svcGetProcessInfo(&textTotalSize, processHandle, 0x10002);
        svcGetProcessInfo(&startAddress, processHandle, 0x10005);
        if(R_FAILED(svcMapProcessMemoryEx(CUR_PROCESS_HANDLE, 0x14000000, processHandle, (u32)startAddress, textTotalSize)))
            goto exit;

        found = (u32 *)Utils::Search<u32>(0x14000000, (u32)textTotalSize, pattern);

        if (found != nullptr)
        {
            original = found[13];
            patchAddress = (u32 *)PA_FROM_VA((found + 13));
            found[13] = 0xE1A00000;
        }

        svcUnmapProcessMemoryEx(CUR_PROCESS_HANDLE, 0x14000000, textTotalSize);
exit:
        svcCloseHandle(processHandle);
    }

    // This function is called before main and before the game starts
    // Useful to do code edits safely
    void    PatchProcess(FwkSettings &settings)
    {
        ToggleTouchscreenForceOn();
    }

    // This function is called when the process exits
    // Useful to save settings, undo patchs or clean up things
    void    OnProcessExit(void)
    {
        ToggleTouchscreenForceOn();
    }

    void    InitMenu(PluginMenu &menu)
    {
        // Create your entries here, or elsewhere
        // You can create your entries whenever/wherever you feel like it
        
        // Example entry
        /*menu += new MenuEntry("Test", nullptr, [](MenuEntry *entry)
        {
            std::string body("What's the answer ?\n");

            body += std::to_string(42);

            MessageBox("UA", body)();
        });*/
    }

    

 int main()
    {
        if (Process::GetTitleID() == 0x0004000000030800)
        {
            PluginMenu *menu = new PluginMenu(Color::Green << "Mario Kart 7", 1, 0, 0, "Plugin For MK7");
            menu->SynchronizeWithFrame(true);
            
            const std::vector<MenuEntry *> Battle {

            new MenuEntry("Invincible", invincible),
            new MenuEntry("Always Star Power", alwaysStarPower),
            new MenuEntry("Truly Random Items", trulyRandomItems),
 
            };

            menu->Append(new MenuFolder("Battle", "Use Battle Cheats", Battle));

            const std::vector<MenuEntry *> Kart{

            new MenuEntry("Always Black Kart", alwaysBlackKart),
            new MenuEntry("Always Shocked", alwaysShocked),
            new MenuEntry("Always Crushed", alwaysCrushed),
            EntryWithHotkey(new MenuEntry("Trick Jump (ZR)", trickAnywhere, "Trick Jump (ZR)"), { Hotkey(Key::ZR, "Trick Jump (ZR)") }),
            };
            menu->Append(new MenuFolder("Kart", "Use Kart Cheats", Kart));


            
            


            menu->Run();
            delete menu;
        }
        else {
            MessageBox("This game is not supported.")();
            PluginMenu *menu = new PluginMenu("This Game Is Unsupported", 0, 0, 0, "Unsupported");
            menu->SynchronizeWithFrame(true);
            menu->Run();
            delete menu;
            return (0);
        }

        // Exit plugin
        
    }
}
    



                
                