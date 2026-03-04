#pragma once

DECLARE_DELEGATE_OneParam(
	FOnObjectSelected,
	UObject*
);

DECLARE_DELEGATE_OneParam(
	FOnMultipleObjectsSelected,
	const TArray<UObject*>&
);
