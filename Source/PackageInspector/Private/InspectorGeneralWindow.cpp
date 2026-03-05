#include "InspectorGeneralWindow.h"
#include "InspectorSettingsBlock.h"
#include "InspectorObjectBlock.h"
#include "InspectorDetailsBlock.h"
#include "InspectorPackageBlock.h"

#include "ContentBrowserModule.h"
#include "Widgets/Layout/SSplitter.h"

SInspectorGeneralWindow::~SInspectorGeneralWindow()
{
	UnbindFromContentBrowser();
}

void SInspectorGeneralWindow::Construct(const FArguments& InArgs)
{
	RebuildLayout();
	BindToContentBrowser();
}

void SInspectorGeneralWindow::RebuildLayout()
{
	ChildSlot
	[
		SNew(SSplitter)

		+ SSplitter::Slot()
		.Value(0.1f)
		[
			SAssignNew(SettingsBlock, SInspectorSettingsBlock)
		]

		+ SSplitter::Slot()
		.Value(0.25f)
		[
			SAssignNew(PackageBlock, SInspectorPackageBlock)
		]

		+ SSplitter::Slot()
		.Value(0.4f)
		[
			SAssignNew(TreeBlock, SInspectorObjectBlock)
		]

		+ SSplitter::Slot()
		.Value(0.25f)
		[
			SAssignNew(DetailsBlock, SInspectorDetailsBlock)
		]
	];

	SettingsBlock->OnSettingsChanged.BindLambda(
[this](bool bAll, bool bTransient, bool bEdit)
		{
			//TreeBlock->SetMode(bAll, bTransient);
			DetailsBlock->SetEditingEnabled(bEdit);
		});

	TreeBlock->OnObjectSelected.BindLambda(
[this](UObject* Obj)
		{
			if (DetailsBlock)
			{
				DetailsBlock->SetObject(Obj);
			}
		});

	PackageBlock->OnMultipleObjectsSelected.BindLambda(
[this](const TArray<UObject*>& ObjArr)
		{
			if (TreeBlock)
			{
				TreeBlock->SetRootObjects(ObjArr);
			}
		});
}

void SInspectorGeneralWindow::BindToContentBrowser()
{
	FContentBrowserModule& CBModule =
	FModuleManager::LoadModuleChecked<FContentBrowserModule>("ContentBrowser");

	ContentBrowserHandle = CBModule.GetOnAssetSelectionChanged().AddRaw(
		this,
		&SInspectorGeneralWindow::OnAssetSelectionChanged
	);
}

void SInspectorGeneralWindow::UnbindFromContentBrowser()
{
	FContentBrowserModule& CBModule =
	FModuleManager::LoadModuleChecked<FContentBrowserModule>("ContentBrowser");

	CBModule.GetOnAssetSelectionChanged().Remove(ContentBrowserHandle);
}

void SInspectorGeneralWindow::OnAssetSelectionChanged(
	const TArray<FAssetData>& SelectedAssets,
	bool bIsPrimary)
{
	if (!SelectedAssets.Num() || !TreeBlock) return;

	TArray<UObject*> RootObjects;
	
	for (auto AssetData : SelectedAssets)
	{
		UObject* Asset = AssetData.GetAsset();	// immediate loading
		if (!Asset) continue;
		UPackage* Package = Asset->GetPackage();
		if (!Package) continue;
		RootObjects.Add(Package);
	}
	
	TreeBlock->SetRootObjects(RootObjects);
}
