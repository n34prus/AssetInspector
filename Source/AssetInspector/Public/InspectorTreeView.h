#pragma once

#include "CoreMinimal.h"
#include "Widgets/SCompoundWidget.h"
#include "Widgets/Views/STreeView.h"
#include "InspectorCore.h"

struct FAssetData;

using FInspectObjectPtr = TWeakObjectPtr<UObject>;
	
class SInspectorObjectRow : public SMultiColumnTableRow<FInspectObjectPtr>
{
public:
	SLATE_BEGIN_ARGS(SInspectorObjectRow) {}
	SLATE_ARGUMENT(FInspectObjectPtr, Item)
	SLATE_END_ARGS()

void Construct(
	const FArguments& InArgs,
	const TSharedRef<STableViewBase>& OwnerTable)
	{
		Item = InArgs._Item;

		SMultiColumnTableRow<FInspectObjectPtr>::Construct(
			SMultiColumnTableRow<FInspectObjectPtr>::FArguments(),
			OwnerTable);
	}

virtual TSharedRef<SWidget> GenerateWidgetForColumn(
	const FName& ColumnName) override
	{
		UObject* Obj = Item.Get();
		if (!Obj)
			return SNew(STextBlock).Text(FText::FromString("Invalid"));
			
		if (ColumnName == "Class")
		{
			return SNew(SHorizontalBox)
					
				+ SHorizontalBox::Slot()
				.AutoWidth()
				.VAlign(VAlign_Center)
				[
					SNew(SExpanderArrow, SharedThis(this))
				]

				+ SHorizontalBox::Slot()
				.FillWidth(1.f)
				.VAlign(VAlign_Center)
				[
					SNew(STextBlock)
					.Text(FText::FromString(Obj->GetClass()->GetName()))
				];
		}
			
		if (ColumnName == "Name")
			return SNew(STextBlock)
				.Text(FText::FromString(
					Obj ? Obj->GetName() : TEXT("-")));

		if (ColumnName == "Path")
			return SNew(STextBlock)
				.Text(FText::FromString(
					Obj ? Obj->GetPathName() : TEXT("-")));

		return SNew(STextBlock).Text(FText::FromString("Invalid"));
	}

private:
	FInspectObjectPtr Item;
};

class SInspectorTreeView : public SCompoundWidget
{
public:

	SLATE_BEGIN_ARGS(SInspectorTreeView) {}
	SLATE_END_ARGS()

	FOnObjectSelected OnObjectSelected;
	
	void Construct(const FArguments& InArgs);
	virtual ~SInspectorTreeView();

	void AddRootObjects(const TArray<UObject*>& RootObjects, bool ClearRoot = false);
	inline void AddRootObject(UObject* RootObject, bool ClearRoot = false) { AddRootObjects({RootObject}, ClearRoot); }
	inline void SetRootObjects(const TArray<UObject*>& RootObjects) { AddRootObjects(RootObjects, true); }
	inline void SetRootObject(UObject* RootObject) { AddRootObjects({RootObject}, true); }
	
	void RemoveRootObjects(const TArray<UObject*>& RootObject);
	inline void RemoveRootObject(UObject* RootObject) { RemoveRootObjects({RootObject}); }

private:
	
	TSharedPtr<STreeView<FInspectObjectPtr>> TreeViewWidget;

	TArray<FInspectObjectPtr> RootItems;

	TMap<FInspectObjectPtr, TSet<FInspectObjectPtr>> ChildrenMap;
	
	void ExtractPackageObjects(UObject* RootObject, FInspectObjectPtr RootNode, uint8_t depth = 0);
	
	TSharedRef<ITableRow> OnGenerateRow(FInspectObjectPtr Item, const TSharedRef<STableViewBase>& OwnerTable);
	void OnGetChildren(FInspectObjectPtr Item, TArray<FInspectObjectPtr>& OutChildren);
	void OnSelectionChanged(FInspectObjectPtr Item, ESelectInfo::Type SelectInfo);
};