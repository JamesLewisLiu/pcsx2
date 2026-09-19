// SPDX-FileCopyrightText: 2002-2026 PCSX2 Dev Team
// SPDX-License-Identifier: GPL-3.0+

#include "SIO/Pad/PadDrumMania.h"

#include "Host.h"

#include "IconsPromptFont.h"

static const InputBindingInfo s_bindings[] = {
	// clang-format off
	{"HiHat", TRANSLATE_NOOP("Pad", "Hi-Hat"), nullptr, InputBindingInfo::Type::Button, PadDrumMania::Inputs::HI_HAT, GenericInputBinding::Triangle},
	{"Snare", TRANSLATE_NOOP("Pad", "Snare"), nullptr, InputBindingInfo::Type::Button, PadDrumMania::Inputs::SNARE, GenericInputBinding::Circle},
	{"BassPedal", TRANSLATE_NOOP("Pad", "Bass Pedal"), nullptr, InputBindingInfo::Type::Button, PadDrumMania::Inputs::BASS_PEDAL, GenericInputBinding::L2},
	{"HighTom", TRANSLATE_NOOP("Pad", "High Tom"), nullptr, InputBindingInfo::Type::Button, PadDrumMania::Inputs::HIGH_TOM, GenericInputBinding::R2},
	{"LowTom", TRANSLATE_NOOP("Pad", "Low Tom"), nullptr, InputBindingInfo::Type::Button, PadDrumMania::Inputs::LOW_TOM, GenericInputBinding::R1},
	{"Cymbal", TRANSLATE_NOOP("Pad", "Cymbal"), nullptr, InputBindingInfo::Type::Button, PadDrumMania::Inputs::CYMBAL, GenericInputBinding::Cross},
	{"Select", TRANSLATE_NOOP("Pad", "Select"), nullptr, InputBindingInfo::Type::Button, PadDrumMania::Inputs::SELECT, GenericInputBinding::Select},
	{"Start", TRANSLATE_NOOP("Pad", "Start"), nullptr, InputBindingInfo::Type::Button, PadDrumMania::Inputs::START, GenericInputBinding::Start},
	// clang-format on
};

const Pad::ControllerInfo PadDrumMania::ControllerInfo = {Pad::ControllerType::DrumMania, "DrumMania",
	TRANSLATE_NOOP("Pad", "DrumMania"), ICON_FA_DRUM, s_bindings, {}, Pad::VibrationCapabilities::NoVibration};

PadDrumMania::PadDrumMania(u8 unifiedSlot, size_t ejectTicks)
	: PadGuitarFreaks(unifiedSlot, ejectTicks, Pad::ControllerType::DrumMania, ControllerInfo,
		bitmaskMapping, 0xb000, "PadDrumMania")
{
}

PadDrumMania::~PadDrumMania() = default;
