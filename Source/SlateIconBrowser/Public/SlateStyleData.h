// Copyright sirjofri. Licensed under MIT license. See License.txt for full license text.

#pragma once

enum EDefaultCopyStyle
{
	DCS_FirstEntry,
	DCS_QuickStyle,
};

class SLATEICONBROWSER_API FSlateStyleData
{
public:
	// Override this to generate your own visualization widget
	virtual TSharedRef<SWidget> GenerateRowWidget() = 0;

	// Override this to fill in the Details map as well as the ExtendedPreview
	virtual void InitializeDetails() {};
	
	virtual void FillRowContextMenu(FMenuBuilder& MenuBuilder);
	virtual void CopyDefault(EDefaultCopyStyle DefaultCopyStyle, const FString& QuickStyle);
	
	virtual TSharedPtr<SWidget> GetExtendedPreview() { return ExtendedPreview; };
	virtual TArray<TTuple<FString,FString>>& GetDetails() { return Details; };
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
	const T& GetWidgetStyle(bool &Found)
	{
		const ISlateStyle* Style = GetSlateStyle();
		if (!(Style && Style->HasWidgetStyle<T>(GetPropertyName()))) {
			Found = false;
			return T::GetDefault();
		}
		Found = true;
		return GetSlateStyle()->GetWidgetStyle<T>(GetPropertyName());
	};

	// Helper function to get the string value for the specified Enum type
	template<typename T>
	static FString GetEnumValue(T Value)
	{
		UEnum* Enum = StaticEnum<T>();
		return Enum->GetNameStringByValue((int64)Value);
	}

	void AddDetail(const FString& Name, const FString& Value);
#define AddDetailf(Name, Format, ...) AddDetail(Name, FString::Printf(Format, ##__VA_ARGS__))

protected:
	static FString MarginString(FMargin Margin);

protected:
	FName StyleSetName;
	FName PropertyName;
	FName Type;
	FName WidgetStyleType;

	// This is the extended preview that's visible within the tooltip
	TSharedPtr<SWidget> ExtendedPreview = nullptr;

private:
	// This holds various metadata for the tooltip display
	TArray<TTuple<FString,FString>> Details;
};

class SLATEICONBROWSER_API ISlateStyleDataProvider
{
public:
	virtual ~ISlateStyleDataProvider() = default;
	virtual TSharedPtr<FSlateStyleData> MakeSlateStyleData(const ISlateStyle* SlateStyle, FName PropertyName, FName WidgetType) = 0;
	virtual TArray<FName> GetSupportedWidgetTypes() = 0;
};
