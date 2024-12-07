// Copyright sirjofri. Licensed under MIT license. See License.txt for full license text.

#pragma once

enum EDefaultCopyStyle : int;

class FSlateStyleData
{
public:
	virtual TSharedRef<SWidget> GenerateRowWidget() = 0;
	virtual void FillRowContextMenu(FMenuBuilder& MenuBuilder);
	virtual void CopyDefault(EDefaultCopyStyle DefaultCopyStyle, const FString& QuickStyle);

	
	virtual ~FSlateStyleData() = default;

	virtual void Initialize(FName InStyle, FName InPropertyName, FName InType, FName InWidgetStyleType)
	{
		StyleName = InStyle;
		PropertyName = InPropertyName;
		Type = InType;
		WidgetStyleType = InWidgetStyleType;
	};
	
	virtual FName GetStyleName() { return StyleName;};
	virtual FName GetPropertyName() { return PropertyName; };
	virtual FName GetType() { return Type; };

protected:
	FString ReadabilityReplace(const FString& Code);
	FString GenerateCopyCode(const FString& StyleCode);
	virtual void ClipboardCode(const FString& CopyCode);

protected:
	FName StyleName;
	FName PropertyName;
	FName Type;
	FName WidgetStyleType;
};

class ISlateStyleDataProvider
{
public:
	virtual ~ISlateStyleDataProvider() = default;
	virtual TSharedPtr<FSlateStyleData> MakeSlateStyleData(const ISlateStyle* SlateStyle, FName PropertyName, FName WidgetType) = 0;
	virtual TArray<FName> GetSupportedWidgetTypes() = 0;
};
