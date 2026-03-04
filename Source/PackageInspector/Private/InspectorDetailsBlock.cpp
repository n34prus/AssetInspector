#include "InspectorDetailsBlock.h"
#include "PropertyEditorModule.h"
#include "Modules/ModuleManager.h"

void SInspectorDetailsBlock::Construct(const FArguments& InArgs)
{
	FPropertyEditorModule& PropModule =
		FModuleManager::LoadModuleChecked<FPropertyEditorModule>("PropertyEditor");

	FDetailsViewArgs Args;
	
	Args.bLockable = false;
	Args.bUpdatesFromSelection = false;
	Args.bAllowSearch = true;
	Args.bShowOptions = true;
	Args.bShowScrollBar = true;
	Args.bShowPropertyMatrixButton = true;
	Args.NameAreaSettings = FDetailsViewArgs::ObjectsUseNameArea;
	Args.DefaultsOnlyVisibility = EEditDefaultsOnlyNodeVisibility::Automatic;
	Args.bForceHiddenPropertyVisibility = true;
	Args.bShowHiddenPropertiesWhilePlayingOption = true;
	Args.bAllowFavoriteSystem = true;
	Args.bShowModifiedPropertiesOption = true;

	DetailsView = PropModule.CreateDetailView(Args);

	ChildSlot
	[
		DetailsView.ToSharedRef()
	];
}

void SInspectorDetailsBlock::SetObject(UObject* Object)
{
	if (DetailsView.IsValid())
	{
		DetailsView->SetObject(Object);
	}
}

void SInspectorDetailsBlock::SetEditingEnabled(bool bEnabled)
{
	if (DetailsView.IsValid())
	{
		DetailsView->SetIsPropertyEditingEnabledDelegate(
			FIsPropertyEditingEnabled::CreateLambda([bEnabled]()
			{
				return bEnabled;
			}));
	}
}