// SPDX-FileCopyrightText: 2002-2026 PCSX2 Dev Team
// SPDX-License-Identifier: GPL-3.0+

#pragma once

#include "SIO/Pad/PadGuitarFreaks.h"

class PadDrumMania final : public PadGuitarFreaks
{
public:
	enum Inputs
	{
		HI_HAT,
		SNARE,
		BASS_PEDAL,
		HIGH_TOM,
		LOW_TOM,
		CYMBAL,
		SELECT,
		START,
		LENGTH,
	};

	PadDrumMania(u8 unifiedSlot, size_t ejectTicks);
	~PadDrumMania() override;

	static const Pad::ControllerInfo ControllerInfo;

private:
	static constexpr std::array<u8, Inputs::LENGTH> bitmaskMapping = {{
		4, // HI_HAT (Triangle)
		5, // SNARE (Circle)
		0, // BASS_PEDAL (L2)
		1, // HIGH_TOM (R2)
		3, // LOW_TOM (R1)
		6, // CYMBAL (Cross)
		8, // SELECT
		11, // START
	}};
};
