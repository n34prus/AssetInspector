#pragma once

#include "CoreMinimal.h"
#include "Widgets/SCompoundWidget.h"
#include "Widgets/Views/STreeView.h"
#include "InspectorCore.h"

struct FAssetData;

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

	using FNodePtr = TWeakObjectPtr<UObject>;
	
	class SInspectorRow : public SMultiColumnTableRow<FNodePtr>
	{
	public:
		SLATE_BEGIN_ARGS(SInspectorRow) {}
		SLATE_ARGUMENT(FNodePtr, Item)
	SLATE_END_ARGS()

	void Construct(
		const FArguments& InArgs,
		const TSharedRef<STableViewBase>& OwnerTable)
		{
			Item = InArgs._Item;

			SMultiColumnTableRow<FNodePtr>::Construct(
				SMultiColumnTableRow<FNodePtr>::FArguments(),
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
		FNodePtr Item;
	};
	
	TSharedPtr<STreeView<FNodePtr>> TreeViewWidget;

	TArray<FNodePtr> RootItems;

	TMap<FNodePtr, TSet<FNodePtr>> ChildrenMap;
	
	void ExtractPackageObjects(UObject* RootObject, FNodePtr RootNode, uint8_t depth = 0);
	
	TSharedRef<ITableRow> OnGenerateRow(FNodePtr Item, const TSharedRef<STableViewBase>& OwnerTable);
	void OnGetChildren(FNodePtr Item, TArray<FNodePtr>& OutChildren);
	void OnSelectionChanged(FNodePtr Item, ESelectInfo::Type SelectInfo);
};