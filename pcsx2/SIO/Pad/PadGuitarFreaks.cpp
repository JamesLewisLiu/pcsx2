// SPDX-FileCopyrightText: 2002-2026 PCSX2 Dev Team
// SPDX-License-Identifier: GPL-3.0+

#include "SIO/Pad/PadGuitarFreaks.h"
#include "SIO/Pad/Pad.h"
#include "SIO/Sio.h"
#include "SIO/Sio0.h"

#include "Host.h"

#include "IconsPromptFont.h"

#include "common/Console.h"

#include <algorithm>

static const InputBindingInfo s_bindings[] = {
	// clang-format off
	{"ButtonR", TRANSLATE_NOOP("Pad", "Button R"), nullptr, InputBindingInfo::Type::Button, PadGuitarFreaks::Inputs::BUTTON_R, GenericInputBinding::R2},
	{"ButtonG", TRANSLATE_NOOP("Pad", "Button G"), nullptr, InputBindingInfo::Type::Button, PadGuitarFreaks::Inputs::BUTTON_G, GenericInputBinding::Circle},
	{"ButtonB", TRANSLATE_NOOP("Pad", "Button B"), nullptr, InputBindingInfo::Type::Button, PadGuitarFreaks::Inputs::BUTTON_B, GenericInputBinding::Triangle},
	{"Pick", "Pick", nullptr, InputBindingInfo::Type::Button, PadGuitarFreaks::Inputs::PICK, GenericInputBinding::DPadUp},
	{"Wailing", "Wailing", nullptr, InputBindingInfo::Type::Button, PadGuitarFreaks::Inputs::WAILING, GenericInputBinding::L2},
	{"Select", TRANSLATE_NOOP("Pad", "Select"), nullptr, InputBindingInfo::Type::Button, PadGuitarFreaks::Inputs::SELECT, GenericInputBinding::Select},
	{"Start", TRANSLATE_NOOP("Pad", "Start"), nullptr, InputBindingInfo::Type::Button, PadGuitarFreaks::Inputs::START, GenericInputBinding::Start},
	// clang-format on
};

const Pad::ControllerInfo PadGuitarFreaks::ControllerInfo = {Pad::ControllerType::GuitarFreaks, "GuitarFreaks",
	TRANSLATE_NOOP("Pad", "GuitarFreaks"), ICON_PF_GUITAR, s_bindings, {}, Pad::VibrationCapabilities::NoVibration};

u8 PadGuitarFreaks::Mystery(u8 commandByte)
{
	switch (commandBytesReceived)
	{
		case 5:
			return 0x02;
		case 8:
			return 0x5a;
		default:
			return 0x00;
	}
}

u8 PadGuitarFreaks::ButtonQuery(u8 commandByte)
{
	switch (commandBytesReceived)
	{
		case 3:
		case 4:
			return 0xff;
		case 5:
			return 0x03;
		case 8:
			g_Sio0.SetAcknowledge(false);
			return 0x5a;
		default:
			return 0x00;
	}
}

u8 PadGuitarFreaks::Poll(u8 commandByte)
{
	const u32 currentButtons = GetButtons();

	switch (commandBytesReceived)
	{
		case 3:
			return (currentButtons >> 8) & 0xff;
		case 4:
			// This controller is digital, so the poll ends after the two button bytes.
			g_Sio0.SetAcknowledge(false);
			return currentButtons & 0xff;
		default:
			Console.Warning("%s(%02X) Did not reach a valid return path! Returning zero as a failsafe!", __FUNCTION__, commandByte);
			return 0x00;
	}
}

u8 PadGuitarFreaks::Config(u8 commandByte)
{
	if (commandBytesReceived == 3)
	{
		if (commandByte)
		{
			if (!isInConfig)
				isInConfig = true;
			else
				Console.Warning("%s(%02X) Unexpected enter while already in config mode", __FUNCTION__, commandByte);
		}
		else
		{
			if (isInConfig)
				isInConfig = false;
			else
				Console.Warning("%s(%02X) Unexpected exit while not in config mode", __FUNCTION__, commandByte);
		}
	}

	if (isInConfig && currentMode == Pad::Mode::DIGITAL && commandBytesReceived == 4)
		g_Sio0.SetAcknowledge(false);

	return 0x00;
}

u8 PadGuitarFreaks::ModeSwitch(u8 commandByte)
{
	switch (commandBytesReceived)
	{
		case 3:
			analogLight = commandByte;
			currentMode = analogLight ? Pad::Mode::ANALOG : Pad::Mode::DIGITAL;
			break;
		case 4:
			analogLocked = (commandByte == 0x03);
			break;
		default:
			break;
	}

	return 0x00;
}

u8 PadGuitarFreaks::StatusInfo(u8 commandByte)
{
	switch (commandBytesReceived)
	{
		case 3:
			return static_cast<u8>(Pad::PhysicalType::STANDARD);
		case 4:
			return 0x02;
		case 5:
			return analogLight;
		case 6:
			return 0x02;
		case 7:
			return 0x01;
		default:
			return 0x00;
	}
}

u8 PadGuitarFreaks::Constant1(u8 commandByte)
{
	switch (commandBytesReceived)
	{
		case 3:
			commandStage = (commandByte != 0);
			return 0x00;
		case 5:
			return 0x01;
		case 6:
			return commandStage ? 0x01 : 0x02;
		case 7:
			return commandStage ? 0x01 : 0x00;
		case 8:
			return commandStage ? 0x0a : 0x14;
		default:
			return 0x00;
	}
}

u8 PadGuitarFreaks::Constant2(u8 commandByte)
{
	switch (commandBytesReceived)
	{
		case 5:
			return 0x02;
		case 7:
			return 0x01;
		default:
			return 0x00;
	}
}

u8 PadGuitarFreaks::Constant3(u8 commandByte)
{
	switch (commandBytesReceived)
	{
		case 3:
			commandStage = (commandByte != 0);
			return 0x00;
		case 6:
			return commandStage ? 0x07 : 0x04;
		default:
			return 0x00;
	}
}

u8 PadGuitarFreaks::VibrationMap(u8 commandByte)
{
	return 0xff;
}

PadGuitarFreaks::PadGuitarFreaks(u8 unifiedSlot, size_t ejectTicks)
	: PadGuitarFreaks(unifiedSlot, ejectTicks, Pad::ControllerType::GuitarFreaks, ControllerInfo,
		bitmaskMapping, 0xa000, "PadGuitarFreaks")
{
}

PadGuitarFreaks::PadGuitarFreaks(u8 unifiedSlot, size_t ejectTicks, Pad::ControllerType type,
	const Pad::ControllerInfo& info, std::span<const u8> mapping, u16 identifier_mask, const char* marker)
	: PadBase(unifiedSlot, ejectTicks)
	, controllerType(type)
	, controllerInfo(info)
	, buttonBitMapping(mapping)
	, controllerIdentifierMask(identifier_mask)
	, freezeMarker(marker)
{
	currentMode = Pad::Mode::DIGITAL;
}

PadGuitarFreaks::~PadGuitarFreaks() = default;

Pad::ControllerType PadGuitarFreaks::GetType() const
{
	return controllerType;
}

const Pad::ControllerInfo& PadGuitarFreaks::GetInfo() const
{
	return controllerInfo;
}

void PadGuitarFreaks::Set(u32 index, float value)
{
	if (index >= buttonBitMapping.size())
		return;

	const float dzValue = (value < buttonDeadzone) ? 0.0f : value;
	rawInputs[index] = static_cast<u8>(std::clamp(dzValue * 255.0f, 0.0f, 255.0f));

	if (dzValue > 0.0f)
		buttons &= ~(1u << buttonBitMapping[index]);
	else
		buttons |= (1u << buttonBitMapping[index]);
}

void PadGuitarFreaks::SetRawAnalogs(const std::tuple<u8, u8> left, const std::tuple<u8, u8> right)
{
}

void PadGuitarFreaks::SetRawPressureButton(u32 index, const std::tuple<bool, u8> value)
{
	if (index >= buttonBitMapping.size())
		return;

	rawInputs[index] = std::get<1>(value);
	if (std::get<0>(value))
		buttons &= ~(1u << buttonBitMapping[index]);
	else
		buttons |= (1u << buttonBitMapping[index]);
}

void PadGuitarFreaks::SetAxisScale(float deadzone, float scale)
{
}

void PadGuitarFreaks::SetDiagonalScaleCorrection(bool enabled)
{
}

float PadGuitarFreaks::GetVibrationScale(u32 motor) const
{
	return 0.0f;
}

void PadGuitarFreaks::SetVibrationScale(u32 motor, float scale)
{
}

float PadGuitarFreaks::GetPressureModifier() const
{
	return 0.0f;
}

void PadGuitarFreaks::SetPressureModifier(float mod)
{
}

void PadGuitarFreaks::SetButtonDeadzone(float deadzone)
{
	buttonDeadzone = deadzone;
}

void PadGuitarFreaks::SetAnalogInvertL(bool x, bool y)
{
}

void PadGuitarFreaks::SetAnalogInvertR(bool x, bool y)
{
}

float PadGuitarFreaks::GetEffectiveInput(u32 index) const
{
	return GetRawInput(index) / 255.0f;
}

u8 PadGuitarFreaks::GetRawInput(u32 index) const
{
	return rawInputs[index];
}

std::tuple<u8, u8> PadGuitarFreaks::GetRawLeftAnalog() const
{
	return {0x7f, 0x7f};
}

std::tuple<u8, u8> PadGuitarFreaks::GetRawRightAnalog() const
{
	return {0x7f, 0x7f};
}

u32 PadGuitarFreaks::GetButtons() const
{
	// Dedicated GF/DM controllers identify themselves with a fixed D-pad chord.
	// needs proper testing against original hardware and more games.
	return buttons & ~controllerIdentifierMask;
}

u8 PadGuitarFreaks::GetPressure(u32 index) const
{
	return 0;
}

bool PadGuitarFreaks::IsAnalogLightEnabled() const
{
	return analogLight;
}

bool PadGuitarFreaks::IsAnalogLocked() const
{
	return analogLocked;
}

bool PadGuitarFreaks::Freeze(StateWrapper& sw)
{
	if (!PadBase::Freeze(sw) || !sw.DoMarker(freezeMarker))
		return false;

	sw.Do(&analogLight);
	sw.Do(&analogLocked);
	sw.Do(&commandStage);
	sw.Do(&buttonDeadzone);
	return !sw.HasError();
}

u8 PadGuitarFreaks::SendCommandByte(u8 commandByte)
{
	u8 ret = 0;

	switch (commandBytesReceived)
	{
		case 0:
			ret = 0x00;
			break;
		case 1:
			currentCommand = static_cast<Pad::Command>(commandByte);
			if (currentCommand != Pad::Command::POLL && currentCommand != Pad::Command::CONFIG && !isInConfig)
				Console.Warning("%s(%02X) Config-only command was sent to a pad outside of config mode!", __FUNCTION__, commandByte);
			ret = isInConfig ? static_cast<u8>(Pad::Mode::CONFIG) : static_cast<u8>(currentMode);
			break;
		case 2:
			ret = 0x5a;
			break;
		default:
			switch (currentCommand)
			{
				case Pad::Command::MYSTERY: ret = Mystery(commandByte); break;
				case Pad::Command::BUTTON_QUERY: ret = ButtonQuery(commandByte); break;
				case Pad::Command::POLL: ret = Poll(commandByte); break;
				case Pad::Command::CONFIG: ret = Config(commandByte); break;
				case Pad::Command::MODE_SWITCH: ret = ModeSwitch(commandByte); break;
				case Pad::Command::STATUS_INFO: ret = StatusInfo(commandByte); break;
				case Pad::Command::CONST_1: ret = Constant1(commandByte); break;
				case Pad::Command::CONST_2: ret = Constant2(commandByte); break;
				case Pad::Command::CONST_3: ret = Constant3(commandByte); break;
				case Pad::Command::VIBRATION_MAP: ret = VibrationMap(commandByte); break;
				default: ret = 0x00; break;
			}
			break;
	}

	commandBytesReceived++;
	return ret;
}
