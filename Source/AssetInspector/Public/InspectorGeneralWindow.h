#pragma once

#include "CoreMinimal.h"
#include "Widgets/SCompoundWidget.h"

class SInspectorSettingsBlock;
class SInspectorTreeView;
class SInspectorDetailsBlock;
class SInspectorPackageBlock;

class SInspectorGeneralWindow : public SCompoundWidget
{
public:
	SLATE_BEGIN_ARGS(SInspectorGeneralWindow) {}
	SLATE_END_ARGS()

	virtual ~SInspectorGeneralWindow();
	
	void Construct(const FArguments& InArgs);

private:
	
	TSharedPtr<SInspectorSettingsBlock> SettingsBlock;
	TSharedPtr<SInspectorPackageBlock>  PackageBlock;
	TSharedPtr<SInspectorTreeView>      TreeBlock;
	TSharedPtr<SInspectorDetailsBlock>  DetailsBlock;
	
	FDelegateHandle ContentBrowserHandle;

	void BindToContentBrowser();
	void UnbindFromContentBrowser();
	void OnAssetSelectionChanged(const TArray<FAssetData>& SelectedAssets, bool bIsPrimary);
	void RebuildLayout();
};