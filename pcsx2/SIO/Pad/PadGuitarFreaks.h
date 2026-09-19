// SPDX-FileCopyrightText: 2002-2026 PCSX2 Dev Team
// SPDX-License-Identifier: GPL-3.0+

#pragma once

#include "SIO/Pad/PadBase.h"

class PadGuitarFreaks : public PadBase
{
public:
	enum Inputs
	{
		BUTTON_R,
		BUTTON_G,
		BUTTON_B,
		PICK,
		WAILING,
		SELECT,
		START,
		LENGTH,
	};

protected:
	u32 buttons = 0xffffffffu;
	bool commandStage = false;
	float buttonDeadzone = 0.0f;
	Pad::ControllerType controllerType;
	const Pad::ControllerInfo& controllerInfo;
	std::span<const u8> buttonBitMapping;
	u16 controllerIdentifierMask;
	const char* freezeMarker;

	// Since the bindings are ordered for the GuitarFreaks UI, remap them to PS2 button bits.
	static constexpr std::array<u8, Inputs::LENGTH> bitmaskMapping = {{
		1, // BUTTON_R (R2)
		5, // BUTTON_G (Circle)
		4, // BUTTON_B (Triangle)
		12, // PICK (D-pad Up)
		0, // WAILING (L2)
		8, // SELECT
		11, // START
	}};

	PadGuitarFreaks(u8 unifiedSlot, size_t ejectTicks, Pad::ControllerType type,
		const Pad::ControllerInfo& info, std::span<const u8> mapping, u16 identifier_mask, const char* marker);

	u8 Mystery(u8 commandByte);
	u8 ButtonQuery(u8 commandByte);
	u8 Poll(u8 commandByte);
	u8 Config(u8 commandByte);
	u8 ModeSwitch(u8 commandByte);
	u8 StatusInfo(u8 commandByte);
	u8 Constant1(u8 commandByte);
	u8 Constant2(u8 commandByte);
	u8 Constant3(u8 commandByte);
	u8 VibrationMap(u8 commandByte);

public:
	PadGuitarFreaks(u8 unifiedSlot, size_t ejectTicks);
	~PadGuitarFreaks() override;

	Pad::ControllerType GetType() const override;
	const Pad::ControllerInfo& GetInfo() const override;
	void Set(u32 index, float value) override;
	void SetRawAnalogs(const std::tuple<u8, u8> left, const std::tuple<u8, u8> right) override;
	void SetRawPressureButton(u32 index, const std::tuple<bool, u8> value) override;
	void SetAxisScale(float deadzone, float scale) override;
	void SetDiagonalScaleCorrection(bool enabled) override;
	float GetVibrationScale(u32 motor) const override;
	void SetVibrationScale(u32 motor, float scale) override;
	float GetPressureModifier() const override;
	void SetPressureModifier(float mod) override;
	void SetButtonDeadzone(float deadzone) override;
	void SetAnalogInvertL(bool x, bool y) override;
	void SetAnalogInvertR(bool x, bool y) override;
	float GetEffectiveInput(u32 index) const override;
	u8 GetRawInput(u32 index) const override;
	std::tuple<u8, u8> GetRawLeftAnalog() const override;
	std::tuple<u8, u8> GetRawRightAnalog() const override;
	u32 GetButtons() const override;
	u8 GetPressure(u32 index) const override;
	bool IsAnalogLightEnabled() const override;
	bool IsAnalogLocked() const override;

	bool Freeze(StateWrapper& sw) override;

	u8 SendCommandByte(u8 commandByte) override;

	static const Pad::ControllerInfo ControllerInfo;
};
