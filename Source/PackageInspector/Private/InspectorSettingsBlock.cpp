// Copyright (c) 2026 Mikhail Kargin. All Rights Reserved.

#include "InspectorSettingsBlock.h"
#include "Widgets/Text/STextBlock.h"
#include "Widgets/Input/SCheckBox.h"
#include "Widgets/SBoxPanel.h"
#include "Widgets/Input/SComboBox.h"

void SInspectorSettingsBlock::Construct(const FArguments& InArgs)
{
	const bool bComingSoon = true;
	if (bComingSoon)
	{
		ChildSlot
		[
			SNew(STextBlock).Text(FText::FromString("Coming soon")).Justification(ETextJustify::Center)
		];
		return;
	}
	
	ChildSlot
	[
		SNew(SVerticalBox)

		+ SVerticalBox::Slot()
		.AutoHeight()
		[
			SNew(STextBlock)
			.Text(FText::FromString("Mode"))
		]

		+ SVerticalBox::Slot()
		.AutoHeight()
		[
			SNew(SCheckBox)
			.IsChecked_Lambda([this]()
			{
				return bAllPackages
					? ECheckBoxState::Checked
					: ECheckBoxState::Unchecked;
			})
			.OnCheckStateChanged_Lambda([this](ECheckBoxState State)
			{
				bAllPackages = (State == ECheckBoxState::Checked);
				OnSettingChanged();
			})
			[
				SNew(STextBlock).Text(FText::FromString("All packages"))
			]
		]

		+ SVerticalBox::Slot()
		.AutoHeight()
		[
			SNew(SCheckBox)
			.IsChecked_Lambda([this]()
			{
				return bIncludeTransient
					? ECheckBoxState::Checked
					: ECheckBoxState::Unchecked;
			})
			.OnCheckStateChanged_Lambda([this](ECheckBoxState State)
			{
				bIncludeTransient = (State == ECheckBoxState::Checked);
				OnSettingChanged();
			})
			[
				SNew(STextBlock).Text(FText::FromString("Include transient"))
			]
		]

		+ SVerticalBox::Slot()
		.AutoHeight()
		[
			SNew(SCheckBox)
			.IsChecked_Lambda([this]()
			{
				return bAllowEditing
					? ECheckBoxState::Checked
					: ECheckBoxState::Unchecked;
			})
			.OnCheckStateChanged_Lambda([this](ECheckBoxState State)
			{
				bAllowEditing = (State == ECheckBoxState::Checked);
				OnSettingChanged();
			})
			[
				SNew(STextBlock).Text(FText::FromString("Allow editing"))
			]
		]
	];
}

void SInspectorSettingsBlock::OnSettingChanged()
{
	if (OnSettingsChanged.IsBound())
	{
		OnSettingsChanged.Execute(
			bAllPackages,
			bIncludeTransient,
			bAllowEditing);
	}
}