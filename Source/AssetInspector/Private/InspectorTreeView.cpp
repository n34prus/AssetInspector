#include "InspectorTreeView.h"

#include "AssetRegistry/AssetData.h"

#include "Widgets/Text/STextBlock.h"
#include "Widgets/Views/STableRow.h"

#include "Engine/Blueprint.h"
#include "UObject/UObjectIterator.h"

void SInspectorTreeView::Construct(const FArguments& InArgs)
{
	ChildSlot
	[
		SAssignNew(TreeViewWidget, STreeView<FNodePtr>)
		.TreeItemsSource(&RootItems)
		.OnGenerateRow(this, &SInspectorTreeView::OnGenerateRow)
		.OnGetChildren(this, &SInspectorTreeView::OnGetChildren)
		.OnSelectionChanged(this, &SInspectorTreeView::OnSelectionChanged)
		.HeaderRow
		(
			SNew(SHeaderRow)
			+ SHeaderRow::Column("Class").DefaultLabel(FText::FromString("Class"))
			+ SHeaderRow::Column("Name").DefaultLabel(FText::FromString("Name"))
			+ SHeaderRow::Column("Path").DefaultLabel(FText::FromString("Path"))
		)
	];
}

SInspectorTreeView::~SInspectorTreeView()
{
}

void SInspectorTreeView::AddRootObjects(const TArray<UObject*>& RootObjects, bool ClearRoot)
{
	if (ClearRoot)
	{
		RootItems.Empty();
		ChildrenMap.Empty();
	}
	for (UObject* Obj : RootObjects)
	{
		if (RootItems.Contains(Obj)) continue;
		FNodePtr RootNode (Obj);
		RootItems.Add(RootNode);
		ExtractPackageObjects(Obj, RootNode, 1);
	}
	TreeViewWidget->RequestTreeRefresh();
}

void SInspectorTreeView::RemoveRootObjects(const TArray<UObject*>& RootObject)
{
	for (UObject* Obj : RootObject)
		if (RootItems.Contains(Obj)) RootItems.Remove(Obj);
	TreeViewWidget->RequestTreeRefresh();
}

void SInspectorTreeView::ExtractPackageObjects(UObject* RootObject, FNodePtr RootNode, uint8_t depth)
{
	if (!RootObject) return;
	TArray<UObject*> Objects;
	GetObjectsWithOuter(RootObject, Objects, false);

	for (UObject* Obj : Objects)
	{
		FNodePtr Node (Obj);
		
		ChildrenMap.FindOrAdd(RootNode).Add(Node);

		if (depth) ExtractPackageObjects(Obj, Node, depth - 1);
	}
}

TSharedRef<ITableRow> SInspectorTreeView::OnGenerateRow(
	FNodePtr Item,
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
	return SNew(SInspectorRow, OwnerTable).Item(Item);
}

void SInspectorTreeView::OnGetChildren(
	FNodePtr Item,
	TArray<FNodePtr>& OutChildren)
{
	if (ChildrenMap.Contains(Item))
	{
		UObject* Obj = Item.Get();
		ExtractPackageObjects(Obj, Item, 1);
		OutChildren = ChildrenMap[Item].Array();
	}
}

void SInspectorTreeView::OnSelectionChanged(
	FNodePtr Item,
	ESelectInfo::Type SelectInfo)
{
	UObject* Selected = Item.IsValid()
			? Item.Get()
			: nullptr;

	OnObjectSelected.ExecuteIfBound(Selected);
}