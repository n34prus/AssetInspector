#include "InspectorPackageBlock.h"
#include "Widgets/Text/STextBlock.h"
//#include "AssetRegistry/AssetRegistryModule.h"

void SInspectorPackageRow::Construct(const FArguments& InArgs, const TSharedRef<STableViewBase>& OwnerTable)
{
    Package = InArgs._Item;

    SMultiColumnTableRow<TWeakObjectPtr<UPackage>>::Construct(
        SMultiColumnTableRow<TWeakObjectPtr<UPackage>>::FArguments(),
        OwnerTable);
}

TSharedRef<SWidget> SInspectorPackageRow::GenerateWidgetForColumn(const FName& ColumnName)
{
    if (!Package.IsValid())
    {
        return SNew(STextBlock).Text(FText::FromString(TEXT("-")));
    }

    const uint32 Flags = Package->GetPackageFlags();
    const bool bHasFlag =
        (ColumnName == "Transient" && (Flags & RF_Transient)) ||
        (ColumnName == "PlayInEditor" && (Flags & PKG_PlayInEditor)) ||
        (ColumnName == "CompiledIn" && (Flags & PKG_CompiledIn)) ||
        (ColumnName == "EditorOnly" && (Flags & PKG_EditorOnly)) ||
        (ColumnName == "FilterEditorOnly" && (Flags & PKG_FilterEditorOnly)) ||
        (ColumnName == "ContainsMapData" && (Flags & PKG_ContainsMapData)) ||
        (ColumnName == "RuntimeGenerated" && (Flags & PKG_RuntimeGenerated)) ||
        (ColumnName == "Cooked" && (Flags & PKG_Cooked));

    if (ColumnName == "Package")
    {
        return SNew(STextBlock)
            .Text(FText::FromString(Package->GetName()));
    }

    if (bHasFlag)
    {
        if (const FString* Short = ShortNames.Find(ColumnName))
        {
            return SNew(STextBlock).Text(FText::FromString(*Short));
        }
        return SNew(STextBlock).Text(FText::FromString("X"));
    }
    return SNew(STextBlock).Text(FText::FromString(" "));
}

void SInspectorPackageBlock::Construct(const FArguments& InArgs)
{
    ListView = SNew(SListView<TWeakObjectPtr<UPackage>>)
        .ListItemsSource(&Packages)
        .OnGenerateRow(this, &SInspectorPackageBlock::OnGenerateRow)
        .OnSelectionChanged(this, &SInspectorPackageBlock::OnSelectionChanged)
        .HeaderRow
        (
            SNew(SHeaderRow)
            + SHeaderRow::Column("Package").DefaultLabel(FText::FromString("Package"))
            + SHeaderRow::Column("Transient").DefaultLabel(FText::FromString("Transient"))
            + SHeaderRow::Column("PlayInEditor").DefaultLabel(FText::FromString("PIE"))
            + SHeaderRow::Column("CompiledIn").DefaultLabel(FText::FromString("CompiledIn"))
            + SHeaderRow::Column("EditorOnly").DefaultLabel(FText::FromString("EditorOnly"))
            + SHeaderRow::Column("FilterEditorOnly").DefaultLabel(FText::FromString("FilterEditorOnly"))
            + SHeaderRow::Column("ContainsMapData").DefaultLabel(FText::FromString("ContainsMapData"))
            + SHeaderRow::Column("RuntimeGenerated").DefaultLabel(FText::FromString("RuntimeGenerated"))
            + SHeaderRow::Column("Cooked").DefaultLabel(FText::FromString("Cooked"))
        );

    ChildSlot
    [
        ListView.ToSharedRef()
    ];

    UpdatePackages();
}

TSharedRef<ITableRow> SInspectorPackageBlock::OnGenerateRow(
    TWeakObjectPtr<UPackage> Item,
    const TSharedRef<STableViewBase>& OwnerTable)
{
    return SNew(SInspectorPackageRow, OwnerTable)
        .Item(Item);
}

void SInspectorPackageBlock::OnSelectionChanged(TWeakObjectPtr<UPackage> Package, ESelectInfo::Type SelectInfo)
{
    if (!Package.IsValid())
        return;

    UPackage* SelectedPackage = Package.Get();
    
    if (OnObjectSelected.IsBound())
    {
        OnObjectSelected.Execute(SelectedPackage);
    }
}

void SInspectorPackageBlock::UpdatePackages()
{
    Packages.Empty();

    /*
    FAssetRegistryModule& AssetRegistryModule =
        FModuleManager::LoadModuleChecked<FAssetRegistryModule>("AssetRegistry");

    IAssetRegistry& AssetRegistry = AssetRegistryModule.Get();

    TArray<FAssetData> Assets;
    AssetRegistry.GetAllAssets(Assets, true);

    TSet<FName> UniquePackages;

    for (const FAssetData& Asset : Assets)
    {
        UniquePackages.Add(Asset.PackageName);
    }
    */
    //for (const FName& PackageName : UniquePackages)
    for (TObjectIterator<UPackage> It; It; ++It)
    {
        //UPackage* Package = FindPackage(nullptr, *PackageName.ToString());
        UPackage* Package = *It;

        /*
        if (!Package)
        {
            Package = LoadPackage(nullptr, *PackageName.ToString(), LOAD_None);
        }
        */

        if (Package)
        {
            Packages.Add(Package);
        }
    }

    if (ListView.IsValid())
    {
        ListView->RequestListRefresh();
    }
}
