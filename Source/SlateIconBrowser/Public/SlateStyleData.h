// Copyright sirjofri. Licensed under MIT license. See License.txt for full license text.

#pragma once

enum EDefaultCopyStyle
{
	DCS_FirstEntry,
	DCS_QuickStyle,
};

class FSlateStyleData
{
public:
	// Override this to generate your own visualization widget
	virtual TSharedRef<SWidget> GenerateRowWidget() = 0;

	// Override this to fill in the Details map as well as the ExtendedPreview
	virtual void InitializeDetails() {};
	
	virtual void FillRowContextMenu(FMenuBuilder& MenuBuilder);
	virtual void CopyDefault(EDefaultCopyStyle DefaultCopyStyle, const FString& QuickStyle);
	
	virtual TSharedPtr<SWidget> GetExtendedPreview() { return ExtendedPreview; };
	virtual TMap<FString,FString>& GetDetails() { return Details; };
	virtual bool HasDetails() { return Details.Num() > 0; };

	
	virtual ~FSlateStyleData() = default;

	virtual void Initialize(FName InStyle, FName InPropertyName, FName InType, FName InWidgetStyleType)
	{
		StyleSetName = InStyle;
		PropertyName = InPropertyName;
		Type = InType;
		WidgetStyleType = InWidgetStyleType;

		Details.Empty();
		InitializeDetails();
	};
	
	virtual FName GetStyleSetName() { return StyleSetName;};
	virtual FName GetPropertyName() { return PropertyName; };
	virtual FName GetType() { return Type; };

protected:
	FString ReadabilityReplace(const FString& Code);
	FString GenerateCopyCode(const FString& StyleCode);
	virtual void ClipboardCode(const FString& CopyCode);

	// Helper function to get the Slate Style
	const ISlateStyle* GetSlateStyle();

	/** Helper function to get the widget style structure.
	 * @return true if the widget style exists (success)
	 */
	template<typename T>
	bool GetWidgetStyle(T& WidgetStyle)
	{
		const ISlateStyle* Style = GetSlateStyle();
		if (!(Style && Style->HasWidgetStyle<T>(GetPropertyName())))
			return false;
		WidgetStyle = GetSlateStyle()->GetWidgetStyle<T>(GetPropertyName());
		return true;
	};

	// Helper function to get the string value for the specified Enum type
	template<typename T>
	static FString GetEnumValue(T Value)
	{
		UEnum* Enum = StaticEnum<T>();
		return Enum->GetNameStringByValue((int64)Value);
	}

protected:
	FName StyleSetName;
	FName PropertyName;
	FName Type;
	FName WidgetStyleType;

	// This holds various metadata for the tooltip display
	TMap<FString,FString> Details;
	// This is the extended preview that's visible within the tooltip
	TSharedPtr<SWidget> ExtendedPreview = nullptr;
};

class ISlateStyleDataProvider
{
public:
	virtual ~ISlateStyleDataProvider() = default;
	virtual TSharedPtr<FSlateStyleData> MakeSlateStyleData(const ISlateStyle* SlateStyle, FName PropertyName, FName WidgetType) = 0;
	virtual TArray<FName> GetSupportedWidgetTypes() = 0;
};
