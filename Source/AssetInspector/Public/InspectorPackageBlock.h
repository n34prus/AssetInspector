#pragma once

#include "CoreMinimal.h"
#include "Widgets/SCompoundWidget.h"
#include "Widgets/Views/SListView.h"
#include "UObject/Package.h"
#include "InspectorCore.h"

class SInspectorPackageRow : public SMultiColumnTableRow<TWeakObjectPtr<UPackage>>
{
public:
	SLATE_BEGIN_ARGS(SInspectorPackageRow) {}
	SLATE_ARGUMENT(TWeakObjectPtr<UPackage>, Item)
SLATE_END_ARGS()

void Construct(const FArguments& InArgs, const TSharedRef<STableViewBase>& OwnerTable);

	virtual TSharedRef<SWidget> GenerateWidgetForColumn(const FName& ColumnName) override;

private:
	TWeakObjectPtr<UPackage> Package;
	const TMap <FName, FString> ShortNames
	{
		{ "Transient",            "TR"  },
		{ "PlayInEditor",         "PIE" },
		{ "CompiledIn",           "CI"  },
		{ "EditorOnly",           "EO"  },
		{ "FilterEditorOnly",     "FEO" },
		{ "ContainsMapData",      "CMD" },
		{ "RuntimeGenerated",     "RG"  },
		{ "Cooked",               "CK"  }
	};
};

class SInspectorPackageBlock : public SCompoundWidget
{
public:
	SLATE_BEGIN_ARGS(SInspectorPackageBlock) {}
	SLATE_END_ARGS()

	FOnObjectSelected OnObjectSelected;
	
	void Construct(const FArguments& InArgs);

	void UpdatePackages();

private:
	TSharedRef<ITableRow> OnGenerateRow(TWeakObjectPtr<UPackage> Item,
		const TSharedRef<STableViewBase>& OwnerTable);

	void OnSelectionChanged(TWeakObjectPtr<UPackage> Package, ESelectInfo::Type SelectInfo);

private:
	TSharedPtr<SListView<TWeakObjectPtr<UPackage>>> ListView;
	TArray<TWeakObjectPtr<UPackage>> Packages;
};