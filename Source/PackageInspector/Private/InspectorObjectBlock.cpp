#include "InspectorObjectBlock.h"
#include "AssetRegistry/AssetData.h"
#include "Widgets/Text/STextBlock.h"
#include "Widgets/Views/STableRow.h"
#include "UObject/UObjectIterator.h"
#include "HAL/PlatformApplicationMisc.h"

void SInspectorObjectBlock::Construct(const FArguments& InArgs)
{
	ChildSlot
	[
		SAssignNew(TreeView, STreeView<FInspectObjectPtr>)
		.TreeItemsSource(&RootItems)
		.OnGenerateRow(this, &SInspectorObjectBlock::OnGenerateRow)
		.OnGetChildren(this, &SInspectorObjectBlock::OnGetChildren)
		.OnSelectionChanged(this, &SInspectorObjectBlock::OnSelectionChanged)
		.OnContextMenuOpening(this, &SInspectorObjectBlock::OnContextMenuOpening)
		.HeaderRow
		(
			SNew(SHeaderRow)
			+ SHeaderRow::Column("Class").DefaultLabel(FText::FromString("Class"))
			+ SHeaderRow::Column("Name").DefaultLabel(FText::FromString("Name"))
			+ SHeaderRow::Column("Path").DefaultLabel(FText::FromString("Path"))
		)
	];
}

SInspectorObjectBlock::~SInspectorObjectBlock()
{
}

void SInspectorObjectBlock::AddRootObjects(const TArray<UObject*>& RootObjects, bool ClearRoot)
{
	if (ClearRoot)
	{
		RootItems.Empty();
		ChildrenMap.Empty();
	}
	for (UObject* Obj : RootObjects)
	{
		if (RootItems.Contains(Obj)) continue;
		FInspectObjectPtr RootNode (Obj);
		RootItems.Add(RootNode);
		ExtractPackageObjects(Obj, RootNode, 1);
	}
	TreeView->RequestTreeRefresh();
}

void SInspectorObjectBlock::RemoveRootObjects(const TArray<UObject*>& RootObject)
{
	for (UObject* Obj : RootObject)
		if (RootItems.Contains(Obj)) RootItems.Remove(Obj);
	TreeView->RequestTreeRefresh();
}

void SInspectorObjectBlock::ExtractPackageObjects(UObject* RootObject, FInspectObjectPtr RootNode, uint8_t depth)
{
	if (!RootObject) return;
	TArray<UObject*> Objects;
	GetObjectsWithOuter(RootObject, Objects, false);

	for (UObject* Obj : Objects)
	{
		FInspectObjectPtr Node (Obj);
		
		ChildrenMap.FindOrAdd(RootNode).Add(Node);

		if (depth) ExtractPackageObjects(Obj, Node, depth - 1);
	}
}

TSharedRef<ITableRow> SInspectorObjectBlock::OnGenerateRow(
	FInspectObjectPtr Item,
	const TSharedRef<STableViewBase>& OwnerTable)
{
	/*
	UObject* Obj = Item.Get();

	FString Label = Obj
		? FString::Printf(TEXT("%s (%s)"),
			*Obj->GetName(),
			*Obj->GetClass()->GetName())
		: TEXT("Invalid");

	return SNew(STableRow<FNodePtr>, OwnerTable)
	[
		SNew(STextBlock).Text(FText::FromString(Label))
	];
	*/
	return SNew(SInspectorObjectRow, OwnerTable).Item(Item);
}

void SInspectorObjectBlock::OnGetChildren(
	FInspectObjectPtr Item,
	TArray<FInspectObjectPtr>& OutChildren)
{
	if (ChildrenMap.Contains(Item))
	{
		UObject* Obj = Item.Get();
		ExtractPackageObjects(Obj, Item, 1);
		OutChildren = ChildrenMap[Item].Array();
	}
}

void SInspectorObjectBlock::OnSelectionChanged(
	FInspectObjectPtr Item,
	ESelectInfo::Type SelectInfo)
{
	UObject* Selected = Item.IsValid()
			? Item.Get()
			: nullptr;

	OnObjectSelected.ExecuteIfBound(Selected);
}

TSharedPtr<SWidget> SInspectorObjectBlock::OnContextMenuOpening()
{
	FMenuBuilder MenuBuilder(true, nullptr);

	MenuBuilder.AddMenuEntry(
		FText::FromString("Copy To Clipboard"),
		FText::FromString("Copy selected package paths"),
		FSlateIcon(),
		FUIAction(
			FExecuteAction::CreateSP(
				this,
				&SInspectorObjectBlock::CopySelectionToClipboard)
		)
	);

	return MenuBuilder.MakeWidget();
}

void SInspectorObjectBlock::CopySelectionToClipboard()
{
	TArray<FInspectObjectPtr> Selected;
	TreeView->GetSelectedItems(Selected);

	FString Result;

	// probably there is only one selected item, but still
	for (FInspectObjectPtr Item : Selected)
	{
		UObject* Obj = Item.Get();
		if (Obj)
		{
			Result += Obj->GetPathName();
			Result += "\n";
		}
	}

	FPlatformApplicationMisc::ClipboardCopy(*Result);
}
