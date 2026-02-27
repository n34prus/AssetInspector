#include "InspectorPackageBlock.h"
#include "Widgets/Text/STextBlock.h"

void SInspectorPackageRow::Construct(const FArguments& InArgs, const TSharedRef<STableViewBase>& OwnerTable)
{
    Item = InArgs._Item;

    SMultiColumnTableRow<FInspectPackagePtr>::Construct(
        SMultiColumnTableRow<FInspectPackagePtr>::FArguments(),
        OwnerTable);
}

TSharedRef<SWidget> SInspectorPackageRow::GenerateWidgetForColumn(const FName& ColumnName)
{
    if (!Item.IsValid())
    {
        return SNew(STextBlock).Text(FText::FromString("-"));
    }
    	
    if (ColumnName == "Path")
    {
        return SNew(SHorizontalBox)
            + SHorizontalBox::Slot()
            .AutoWidth()
            [
                SNew(SExpanderArrow, SharedThis(this))
            ]
            + SHorizontalBox::Slot()
            .FillWidth(1.f)
            [
                SNew(STextBlock).Text(FText::FromString(Item->Name))
            ];
    }
    	
    if (ColumnName == "Flags")
    {
        if (Item->Package.IsValid())
        {
            uint32 Flags = Item->Package->GetPackageFlags();

            FString Out;
            if (Flags & RF_Transient)          Out += "TR ";
            if (Flags & PKG_PlayInEditor)      Out += "PIE ";
            if (Flags & PKG_CompiledIn)         Out += "CI ";
            if (Flags & PKG_EditorOnly)         Out += "EO ";
            if (Flags & PKG_FilterEditorOnly)   Out += "FEO ";
            if (Flags & PKG_ContainsMapData)    Out += "CMD ";
            if (Flags & PKG_RuntimeGenerated)   Out += "RG ";
            if (Flags & PKG_Cooked)             Out += "CK ";

            return SNew(STextBlock)
                .Text(FText::FromString(Out.IsEmpty() ? "-" : Out));
        }

        return SNew(STextBlock).Text(FText::FromString(" "));
    }

    return SNew(STextBlock).Text(FText::FromString("-"));
}

void SInspectorPackageBlock::Construct(const FArguments& InArgs)
{
    TreeView = SNew(STreeView<FInspectPackagePtr>)
        .TreeItemsSource(&RootNodes)
        .OnGenerateRow(this, &SInspectorPackageBlock::OnGenerateRow)
        .OnGetChildren(this, &SInspectorPackageBlock::OnGetChildren)
        .SelectionMode(ESelectionMode::Multi)
        .OnSelectionChanged(this, &SInspectorPackageBlock::OnSelectionChanged)
        .HeaderRow
        (
            SNew(SHeaderRow)
            + SHeaderRow::Column("Path").DefaultLabel(FText::FromString("Path"))
            + SHeaderRow::Column("Flags").DefaultLabel(FText::FromString("Flags"))
        );
    
    ChildSlot
    [
        TreeView.ToSharedRef()
    ];

    UpdatePackages();
}

void SInspectorPackageBlock::UpdatePackages()
{
    RootNodes.Empty();

    TMap<FString, FInspectPackagePtr> FolderMap;

    for (TObjectIterator<UPackage> It; It; ++It)
    {
        UPackage* Package = *It;
        if (!Package)
            continue;

        FString Path = Package->GetPathName(); 

        TArray<FString> Parts;
        Path.ParseIntoArray(Parts, TEXT("/"), true);

        if (Parts.Num() == 0)
            continue;

        FInspectPackagePtr CurrentParent = nullptr;
        FString CurrentPath;

        // create folders
        for (int32 i = 0; i < Parts.Num() - 1; ++i)
        {
            if (CurrentPath.IsEmpty())
                CurrentPath = "/" + Parts[i];
            else
                CurrentPath += "/" + Parts[i];

            FInspectPackagePtr* ExistingNode = FolderMap.Find(CurrentPath);

            if (!ExistingNode)
            {
                FInspectPackagePtr NewFolder = MakeShared<FPackageTreeNode>();
                NewFolder->Name = "/" + Parts[i];
                NewFolder->FullPath = CurrentPath;

                FolderMap.Add(CurrentPath, NewFolder);

                if (CurrentParent.IsValid())
                {
                    CurrentParent->Children.Add(NewFolder);
                }
                else
                {
                    RootNodes.Add(NewFolder);
                }

                CurrentParent = NewFolder;
            }
            else
            {
                CurrentParent = *ExistingNode;
            }
        }

        // create node
        FInspectPackagePtr PackageNode = MakeShared<FPackageTreeNode>();
        PackageNode->Name = Parts.Last();
        PackageNode->FullPath = Path;
        PackageNode->Package = Package;

        if (CurrentParent.IsValid())
        {
            CurrentParent->Children.Add(PackageNode);
        }
        else
        {
            RootNodes.Add(PackageNode);
        }
    }

    // sort packages to top, folders to bottom
    SortTree(RootNodes);
    if (TreeView.IsValid())
    {
        TreeView->RequestTreeRefresh();
    }
}

void SInspectorPackageBlock::OnSelectionChanged(FInspectPackagePtr Item, ESelectInfo::Type SelectInfo)
{
    TArray<FInspectPackagePtr> Selected;
    TreeView->GetSelectedItems(Selected);

    TArray<UObject*> SelectedObjects;

    for (auto& Node : Selected)
    {
        if (!Node.IsValid())
            continue;

        if (Node->IsFolder())
        {
            CollectPackagesRecursive(Node, SelectedObjects);
        }
        else if (Node->Package.IsValid())
        {
            SelectedObjects.Add(Node->Package.Get());
        }
    }

    if (OnMultipleObjectsSelected.IsBound())
    {
        OnMultipleObjectsSelected.Execute(SelectedObjects);
    }
}

void SInspectorPackageBlock::OnGetChildren(FInspectPackagePtr InItem,
    TArray<FInspectPackagePtr>& OutChildren)
{
    OutChildren = InItem->Children;
}

TSharedRef<ITableRow> SInspectorPackageBlock::OnGenerateRow(FInspectPackagePtr Item,
    const TSharedRef<STableViewBase>& OwnerTable)
{
    return SNew(SInspectorPackageRow, OwnerTable)
        .Item(Item);
}

void SInspectorPackageBlock::CollectPackagesRecursive(const FInspectPackagePtr& Node, TArray<UObject*>& Out)
{
    if (!Node.IsValid())
        return;

    if (Node->Package.IsValid())
    {
        UObject* Obj = Node->Package.Get();
        if (Obj)
        {
            Out.Add(Obj);
        }
    }

    for (const auto& Child : Node->Children)
    {
        CollectPackagesRecursive(Child, Out);
    }
}

void SInspectorPackageBlock::SortTree(TArray<FInspectPackagePtr>& Nodes)
{
    Nodes.Sort([](const FInspectPackagePtr& A,
                  const FInspectPackagePtr& B)
    {
        const bool AFolder = A.IsValid() && A->IsFolder();
        const bool BFolder = B.IsValid() && B->IsFolder();

        // packages to top over folders
        if (AFolder != BFolder)
        {
            return !AFolder && BFolder;
        }
			
        FString AName = A.IsValid() ? A->Name : FString();
        FString BName = B.IsValid() ? B->Name : FString();

        return AName < BName;
    });

    for (auto& Node : Nodes)
    {
        if (Node.IsValid())
        {
            SortTree(Node->Children);
        }
    }
}
