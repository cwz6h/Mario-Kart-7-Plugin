#include "cheats.hpp"

namespace CTRPluginFramework
{
    using StringVector = std::vector<std::string>;
    u32 offset = 0, data = 0;
    static u32 original_speed[0x2D];

    	/////////////////////////////////////////////////////////    Start of custom functions    /////////////////////////////////////////////////////////

	/*void	dumpFrames(MenuEntry *entry)
	{
		File file;
		Directory dir("frames", true);
		int frame;
		for (int x = 0; x < 400; x++)
		{
			for (int y = 0; y < 240; y++)
			{
				Color c;
				ReadPixel(x, y, c, false);
				*(unsigned int)(0x18000000 + (4 * x) + (4 * y)) = c.ToU32();
			}
		}
		if (dir.OpenFile(file, "frame" + to_string(frame) + ".bmp", File::RWC) == 0)
		{
			file.Dump(0x18000000, 384000);
			frame++;
		}
	}*/
	
	bool is_in_range(u32 value, u32 lowerbound, u32 upperbound)
	{
		return value > lowerbound && value < upperbound;
	}

	bool IsInRace()
	{
		if (Process::Read32(0x14000084, offset) && is_in_range(offset, 0x14000000, 0x18000000))
			if (Process::Read32(offset + 0x316C, offset) && is_in_range(offset, 0x14000000, 0x18000000))
				return Process::Read32(offset + 0x118, offset) && (offset & 0xFF);
		return 0;
	}

	u32 GetRacePointer()
	{
		if (IsInRace())
			if (Process::Read32(0x140002F4, offset) && is_in_range(offset, 0x14000000, 0x18000000))
				if (Process::Read32(offset + 0x14, offset) && is_in_range(offset, 0x14000000, 0x18000000))
					if (Process::Read32(offset + 0x518, offset) && is_in_range(offset, 0x14000000, 0x18000000))
						if (Process::Read32(offset + 0x1C, offset) && is_in_range(offset, 0x14000000, 0x18000000))
							return offset;
		return 0;
	}

	u32	GetFNsPointer()
	{
		if (IsInRace() && Process::Read32(0xFFFF6F0, offset) && is_in_range(offset, 0x14000000, 0x18000000))
			return offset + 8;
		return 0;
	}

	u32	GetOldPointer5CC()
	{
		if (IsInRace() && Process::Read32(0xFFFFBF4, offset) && is_in_range(offset, 0x14000000, 0x18000000))
			if (Process::Read32(offset + 0x5CC, offset) && is_in_range(offset, 0x14000000, 0x18000000))
				return offset;
		return 0;
	}

	u32	GetOldPointer5D0()
	{
		if (IsInRace() && Process::Read32(0xFFFFBF4, offset) && is_in_range(offset, 0x14000000, 0x18000000))
			if (Process::Read32(offset + 0x5D0, offset) && is_in_range(offset, 0x14000000, 0x18000000))
				return offset;
		return 0;
	}

	u32	GetItemPointer()
	{
		if (IsInRace() && Process::Read32(0x14000074, offset) && is_in_range(offset, 0x14000000, 0x18000000))
			if (Process::Read32(offset - 0x1B5C, offset) && is_in_range(offset, 0x14000000, 0x18000000))
				if (Process::Read32(offset + 0x27AC, offset) && is_in_range(offset, 0x14000000, 0x18000000))
					return offset;
		return 0;
	}

	u8 GetTotalPlayers()
	{
		u8 total;
		if (Process::Read32(0x14000074, offset) && is_in_range(offset, 0x14000000, 0x18000000))
			if (Process::Read32(offset - 0xC10, offset) && is_in_range(offset, 0x14000000, 0x18000000))
				if (Process::Read32(offset + 0x23C, offset) && is_in_range(offset, 0x14000000, 0x18000000))
					if (Process::Read8(offset - 0xF28, total) && is_in_range(offset, 0x14000000, 0x18000000))
						return total;
		return 0;
	}

	u16	GetTime()
	{
		u16 time = 0;
		if (GetOldPointer5D0())
		{
			Process::Read16(GetOldPointer5D0() + 0x484, time);
			time /= 60;
			if (300 - time < 0)
				return 0;
			else
				return 300 - time;
		}
		return 0;
	}

	u16	GetMinutes()
	{
		u16 time = GetTime(), minutes = 0;
		while (time - 60 >= 0)
		{
			time -= 60;
			minutes++;
		}
		return minutes;
	}

	u16	GetSeconds()
	{
		u16 time = GetTime();
		while (time - 60 >= 0)
			time -= 60;
		return time;
	}

	void	SubToTime(u16 seconds)
	{
		u16 time = 0;
		if (offset)
		{
			Process::Read16(GetOldPointer5D0() + 0x484, time);
			time -= (seconds * 60);
			Process::Write16(GetOldPointer5D0() + 0x484, time);
			Process::Write16(GetRacePointer() + 0xC4, time);
		}
	}

	void	writeItem(u32 item)
	{
		if (GetItemPointer())
		{
			Process::Write32(GetItemPointer() + 0x3C, 0xFFFFFFFF);
			Process::Write32(GetItemPointer() + 0xA8, 3);
			Process::Write32(GetItemPointer() + 0xC8, item);
			Process::Write32(GetItemPointer() + 0xD8, 0x3F800000);
		}
	}

	void	writeSpeed(u32 speed)
	{
		for (int i = 0; i < 0x2D; i++)
		{
			if (Process::Read32(0x140002F4, offset) && is_in_range(offset, 0x14000000, 0x18000000) && Process::Read32(offset - 0xA4, offset) && is_in_range(offset, 0x14000000, 0x18000000))
			{
				if (original_speed[2] != speed)
					Process::Read32((offset - 0x2C3B0) + (i * 4), original_speed[i]);
				Process::Write32((offset - 0x2C3B0) + (i * 4), speed);
				Process::Write32((offset - 0x28E90) + (i * 4), speed);
				Process::Write32((offset - 0x1C730) + (i * 4), speed);
			}
		}
	}

	void	writeVR(u32 vr)
	{
		if (Process::Read32(0x663D04, offset) && is_in_range(offset, 0x10000000, 0x18000000))
			Process::Write32(offset - 0xE30, vr);
	}

	void	writeLocation(u32 location)
	{
		if (Process::Read32(0x6673C8, offset) && is_in_range(offset, 0x10000000, 0x18000000))
			Process::Write32(offset + 0xF7CC, location);
	}
	void	writeFlag(u16 flag)
	{
		if (Process::Read32(0x6673C8, offset) && is_in_range(offset, 0x10000000, 0x18000000))
		{
			Process::Write8(offset + 0xE7CA, flag);
			Process::Write8(offset + 0xF7D0, flag);
		}
	}
	void invincible(MenuEntry *entry)
	{
		if (IsInRace())
			Process::Write16(GetRacePointer() + 0x102C, 0xFFFF);
	}
	void alwaysStarPower(MenuEntry *entry)
	{
		if (IsInRace())
			Process::Write16(GetRacePointer() + 0xFF4, 0xFFFF);
	}
	void alwaysBlackKart(MenuEntry *entry)
	{
		if (IsInRace())
			Process::Write16(GetRacePointer() + 0xFF8, 0xFFFF);
	}

	void alwaysShocked(MenuEntry *entry)
	{
		if (IsInRace())
			Process::Write16(GetRacePointer() + 0x1000, 0xFFFF);
	}

	void alwaysCrushed(MenuEntry *entry)
	{
		if (IsInRace())
			Process::Write16(GetRacePointer() + 0x1000, 0xFFFF);
	}
	void trickAnywhere(MenuEntry *entry)
	{
		if (IsInRace())
			Process::Write16(GetRacePointer() + 0xFF0, 0xFFFF);
	}
		void    trulyRandomItems(MenuEntry *entry)
	{
		u32 randNumber = Utils::Random(0, 0x14), g_itemPointer = GetItemPointer(), tmp = 0, tmp2 = 0;
		u16 tmp3 = 0;
		bool in_race = IsInRace(), alreadyGivenItem = false;
		if (randNumber == 0xF || randNumber == 0x10)
			randNumber = Utils::Random(0, 0xE);
		if (!alreadyGivenItem && in_race && is_in_range(g_itemPointer, 0x14000000, 0x18000000) && Process::Read32(g_itemPointer + 0x3C, tmp) && is_in_range(tmp, 0, 4) && Process::Read32(g_itemPointer + 0xC8, tmp2) && tmp2 != 0xFFFFFFFF && Process::Read32(0x14000074, tmp2) && is_in_range(tmp2, 0x14000000, 0x18000000) && Process::Read32(tmp2 - 0x1B5C, tmp2) && is_in_range(tmp2, 0x14000000, 0x18000000) && Process::Read16(tmp2 + 0x882, tmp3) && tmp3 == 1)
		{
			if ((randNumber == 7 || randNumber == 0x11 || randNumber == 0x12 || randNumber == 0x13) && tmp == 1)
				Process::Write32(g_itemPointer + 0x3C, 3);
			else if ((randNumber != 7 && randNumber != 0x11 && randNumber != 0x12 && randNumber != 0x13) && tmp == 3)
				Process::Write32(g_itemPointer + 0x3C, 1);
			Process::Write32(g_itemPointer + 0xA8, 3);
			Process::Write32(g_itemPointer + 0xC8, randNumber);
			Process::Write32(g_itemPointer + 0xD8, 0x3F800000);
			alreadyGivenItem = true;
		}
		if (tmp3 == 0)
			alreadyGivenItem = false;
	}


}
