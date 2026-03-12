#pragma once

#include "CoreMinimal.h"
#include "Widgets/SCompoundWidget.h"
//#include "Widgets/Views/STreeView.h"
#include "PackageInspector.h"

using FInspectorObjectMetaData = TMap<FName, FString>;
struct FMetaRow
{
	FName Key;
	FString Value;
};

DECLARE_DELEGATE_OneParam(
	FOnInspectorMetaDeleteRequested,
	TSharedPtr<FMetaRow>
);

class SInspectorMetaRow : public SMultiColumnTableRow<TSharedPtr<FMetaRow>>
{
public:
	SLATE_BEGIN_ARGS(SInspectorMetaRow) {}
	SLATE_ARGUMENT(TSharedPtr<FMetaRow>, Item)
	SLATE_END_ARGS()

	void Construct(const FArguments& InArgs, const TSharedRef<STableViewBase>& OwnerTable);
	FOnInspectorMetaDeleteRequested OnDeleteRequested;
	virtual TSharedRef<SWidget> GenerateWidgetForColumn(const FName& ColumnName) override;
private:
	TSharedPtr<FMetaRow> Item;
};

class SInspectorMetadataBlock : public SCompoundWidget
{
public:
	SLATE_BEGIN_ARGS(SInspectorMetadataBlock) {}
	SLATE_END_ARGS()

	void Construct(const FArguments& InArgs);

	void SetTargetObject(UObject* Object);
	void UpdateLayout();

private:	
	TArray<TSharedPtr<FMetaRow>> MetaRows;
	TWeakObjectPtr<UObject> TargetObject;
	TSharedPtr<SListView<TSharedPtr<FMetaRow>>> TableView;
	TArray<TSharedPtr<FString>> AvailableKeys {MakeShared<FString>("KeyA"), MakeShared<FString>("KeyB")};
	TSharedPtr<FString> SelectedKey;

	TSharedRef<ITableRow> OnGenerateRow(TSharedPtr<FMetaRow> Item, const TSharedRef<STableViewBase>& OwnerTable);

	void OnDeleteMetaRow(TSharedPtr<FMetaRow> Row);
	void OnAddMetaRow(FName Key, FString Value);

	TMap<UObject*, FInspectorObjectMetaData> GetMetaData(const UPackage * Package);
	TArray<FInspectorObjectMetaData> GetMetaDataForUnreachableObjects(const UPackage * Package);
	void SetMetaData(const UObject * Object, const FName Key, const FString Value);
	void RemoveMetaData(const UObject * Object, const FName Key);
};