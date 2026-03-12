#include "InspectorMetadataBlock.h"

void SInspectorMetaRow::Construct(const FArguments& InArgs, const TSharedRef<STableViewBase>& OwnerTable)
{
	Item = InArgs._Item;
    SMultiColumnTableRow<TSharedPtr<FMetaRow>>::Construct(FSuperRowType::FArguments(), OwnerTable);
}

TSharedRef<SWidget> SInspectorMetaRow::GenerateWidgetForColumn(const FName& ColumnName)
{
	// empty row is used for "add new meta" function 
	const bool bIsEmpty = Item->Key.IsNone( ) && Item->Value.IsEmpty();

	// add new meta
	if (bIsEmpty)
	{
		if (ColumnName == "Key")
		{
			return SNew(SComboBox<TSharedPtr<FString>>)
				.OptionsSource(&AvailableKeys)
				.OnSelectionChanged_Lambda([this](TSharedPtr<FString> NewSelected, ESelectInfo::Type)
				{
					SelectedKey = NewSelected;
				})
				[
					SNew(STextBlock)
					.Text_Lambda([this]() -> FText
					{
						return SelectedKey.IsValid() ? FText::FromString(*SelectedKey) : FText::GetEmpty();
					})
				];
		}

		if (ColumnName == "Value")
		{
			return SNew(STextBlock)
				.Text(FText::FromString(Item->Value));
		}

		if (ColumnName == "Action")
		{
			return SNew(SButton)
				.HAlign(HAlign_Center)
				.VAlign(VAlign_Center)
				.ButtonStyle(FAppStyle::Get(), "SimpleButton")
				.OnClicked_Lambda([this]() -> FReply
				{
					OnDeleteRequested.ExecuteIfBound(Item);
					return FReply::Handled();
				})
				.Content()
				[
					SNew(SImage)
					.Image(FAppStyle::GetBrush("Icons.Delete"))
				];
		}
	}

	// show existed meta
	if (ColumnName == "Key")
	{
		return SNew(STextBlock)
			.Text(FText::FromName(Item->Key));
	}

	if (ColumnName == "Value")
	{
		return SNew(STextBlock)
			.Text(FText::FromString(Item->Value));
	}

	if (ColumnName == "Action")
	{
		return SNew(SButton)
			.HAlign(HAlign_Center)
			.VAlign(VAlign_Center)
			.ButtonStyle(FAppStyle::Get(), "SimpleButton")
			.OnClicked_Lambda([this]() -> FReply
			{
				OnDeleteRequested.ExecuteIfBound(Item);
				return FReply::Handled();
			})
			.Content()
			[
				SNew(SImage)
				.Image(FAppStyle::GetBrush("Icons.Delete"))
			];
	}

	return SNullWidget::NullWidget;
}

void SInspectorMetadataBlock::Construct(const FArguments& InArgs)
{
	ChildSlot
	[
		SAssignNew(TableView, SListView<TSharedPtr<FMetaRow>>)
		.ListItemsSource(&MetaRows)
		.OnGenerateRow(this, &SInspectorMetadataBlock::OnGenerateRow)
		.HeaderRow
		(
			SNew(SHeaderRow)
			+ SHeaderRow::Column("Key").DefaultLabel(FText::FromString("Key"))
			+ SHeaderRow::Column("Value").DefaultLabel(FText::FromString("Value"))
			+ SHeaderRow::Column("Action").DefaultLabel(FText::FromString("Action"))
		)
	];
}

void SInspectorMetadataBlock::SetTargetObject(UObject* Object)
{
	TargetObject = Object;
	UpdateLayout();
}

void SInspectorMetadataBlock::UpdateLayout()
{
	MetaRows.Empty();

	if (TargetObject.IsValid( ))
	{
		auto PackageMetaData = GetMetaData(TargetObject->GetPackage());
		auto ObjectMetaData = PackageMetaData.Find(TargetObject.Get());
		if (ObjectMetaData)
		{
			for (const auto [Key,Value] : *ObjectMetaData)
			{
				MetaRows.Add(MakeShared<FMetaRow>(FMetaRow{Key, Value}));
			}
		}
	}
	// last empty row is used for "add new meta" function 
	MetaRows.Add(MakeShared<FMetaRow>(FMetaRow{FName(), FString()}));
	TableView->RebuildList();
}
/*
TSharedRef<ITableRow> SInspectorMetadataBlock::OnGenerateRow(TSharedPtr<FMetaRow> Item, const TSharedRef<STableViewBase>& OwnerTable)
{
	if (Item == MetaRows.Last())
	{
		const bool bLastRowIsEmpty = Item->Key.IsNone( ) && Item->Value.IsEmpty( );
		ensure(bLastRowIsEmpty); 
		return SNew(STableRow<TSharedPtr<FMetaRow>>, OwnerTable)
		[
			SNew(SHorizontalBox)
			+ SHorizontalBox::Slot().AutoWidth()
			[
				SNew(SComboBox<TSharedPtr<FString>>)
				.OptionsSource(&AvailableKeys)
				.OnSelectionChanged_Lambda([this](TSharedPtr<FString> NewSelected, ESelectInfo::Type)
				{
					SelectedKey = NewSelected;
				})
				[
					SNew(STextBlock)
					.Text_Lambda([this]() -> FText
					{
						return SelectedKey.IsValid() ? FText::FromString(*SelectedKey) : FText::GetEmpty();
					})
				]
			]
			
			+ SHorizontalBox::Slot().AutoWidth()
			[
				SNew(SEditableTextBox)
				.Text(FText::FromString("DefaultValue"))
			]
			
			+ SHorizontalBox::Slot().AutoWidth()
			[
				SNew(SButton)
				.OnClicked_Lambda([this]() { OnAddMetaRow("Key", "Value"); return FReply::Handled(); })
				.Content()
				[
					SNew(SImage).Image(FAppStyle::GetBrush("Icons.Plus"))
				]
			]
		];
	}
	
	return SNew(STableRow<TSharedPtr<FMetaRow>>, OwnerTable)
	[
		SNew(SHorizontalBox)
		+ SHorizontalBox::Slot().AutoWidth()
		[
			SNew(SEditableTextBox)
			.Text(FText::FromName(Item->Key))
			.OnTextCommitted_Lambda([Item](const FText& NewText, ETextCommit::Type)
			{
				Item->Key = FName(NewText.ToString());
			})
		]
		+ SHorizontalBox::Slot().AutoWidth()
		[
			SNew(SEditableTextBox)
			.Text(FText::FromString(Item->Value))
			.OnTextCommitted_Lambda([Item](const FText& NewText, ETextCommit::Type)
			{
				Item->Value = NewText.ToString();
			})
		]
		+ SHorizontalBox::Slot().AutoWidth()
		[
			SNew(SButton)
			.ButtonStyle(FAppStyle::Get(), "NoBorder")
			.ContentPadding(0)
			.OnClicked_Lambda([this, Item]() -> FReply
			{
				OnDeleteMetaRow(Item);
				return FReply::Handled();
			})
			.Content()
			[
				SNew(SImage)
				.Image(FAppStyle::GetBrush("Icons.Delete"))
			]
		]
	];
}*/

TSharedRef<ITableRow> SInspectorMetadataBlock::OnGenerateRow(TSharedPtr<FMetaRow> Item, const TSharedRef<STableViewBase>& OwnerTable)
{
    const bool bIsLast = (Item == MetaRows.Last());
	const bool bIsEmpty = Item->Key.IsNone( ) && Item->Value.IsEmpty( );
	ensureMsgf((!bIsLast || bIsEmpty), TEXT("Last row must be empty, but it is not!"));

	auto Row = SNew(SInspectorMetaRow, OwnerTable).Item(Item);
	Row->OnDeleteRequested.BindSP(SharedThis(this), &SInspectorMetadataBlock::OnDeleteMetaRow);
	return Row;
}

void SInspectorMetadataBlock::OnDeleteMetaRow(TSharedPtr<FMetaRow> Row)
{
	RemoveMetaData(TargetObject.Get(), Row->Key);
	UpdateLayout();
}

void SInspectorMetadataBlock::OnAddMetaRow(FName Key, FString Value)
{
	SetMetaData(TargetObject.Get(), Key, Value);
	UpdateLayout();
}

TMap<UObject*, FInspectorObjectMetaData> SInspectorMetadataBlock::GetMetaData(const UPackage* Package)
{
	TMap<UObject *, FInspectorObjectMetaData> Result;

	bool bHasMetaData = Package->HasMetaData();


	if (!Package || !bHasMetaData)
		return Result;

	auto ConstPackage = const_cast<UPackage*>(Package);
	if (!ConstPackage)
		return Result;

	for (const auto & Entry : ConstPackage->GetMetaData()->ObjectMetaDataMap)
	{
		if (Entry.Key.IsValid( ))
		{
			Result.Add(Entry.Key.Get( ), FInspectorObjectMetaData{ Entry.Value });
		}
	}

	return Result;
}

TArray<FInspectorObjectMetaData> SInspectorMetadataBlock::GetMetaDataForUnreachableObjects(const UPackage* Package)
{
	TArray<FInspectorObjectMetaData> Result;

	if (!Package)
		return Result;

	auto ConstPackage = const_cast<UPackage*>(Package);
	if (!ConstPackage)
		return Result;

	for (const auto & Entry : ConstPackage->GetMetaData()->ObjectMetaDataMap)
	{
		if (!Entry.Key.IsValid( ))
		{
			Result.Emplace(FInspectorObjectMetaData{ Entry.Value });
		}
	}

	return Result;
}

void SInspectorMetadataBlock::SetMetaData(const UObject* Object, const FName Key, const FString Value)
{
	if (!Object || Key.IsNone( )) return;

	Object->GetPackage( )->GetMetaData( )->SetValue(Object, Key, *Value);
}

void SInspectorMetadataBlock::RemoveMetaData(const UObject* Object, const FName Key)
{
	if (!Object || Key.IsNone( )) return;

	Object->GetPackage( )->GetMetaData( )->RemoveValue(Object, Key);
}
