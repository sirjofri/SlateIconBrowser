// Copyright sirjofri. Licensed under MIT license. See License.txt for full license text.

#pragma once

enum EDefaultCopyStyle
{
	DCS_FirstEntry,
	DCS_QuickStyle,
};

class SLATESTYLEBROWSER_API FSlateStyleData
{
public:
	// Override this to generate your own visualization widget
	virtual TSharedRef<SWidget> GenerateRowWidget() = 0;

	// Override this to fill in the Details map as well as the ExtendedPreview
	virtual void InitializeDetails() {};
	// Override this to generate the extended preview
	virtual void InitializePreview() {};
	
	virtual void FillRowContextMenu(FMenuBuilder& MenuBuilder);
	virtual void CopyDefault(EDefaultCopyStyle DefaultCopyStyle, const FString& QuickStyle);
	
	virtual TSharedPtr<SWidget> GetExtendedPreview() { return ExtendedPreview; };
	virtual TArray<TTuple<FString,FString>>& GetDetails() { return Details; };
	virtual bool HasDetails() { return Details.Num() > 0; };

	
	virtual ~FSlateStyleData() = default;

	virtual void Initialize(FName InStyle, FName InPropertyName, FName InType, FName InWidgetStyleType);;
	
	virtual FName GetStyleSetName() { return StyleSetName;};
	virtual FName GetPropertyName() { return PropertyName; };
	virtual FName GetType() { return Type; };

	// A collection of default values which can be used by everyone
	struct DefaultValuesStruct
	{
#define LOCTEXT_NAMESPACE "SlateStyleBrowser"
		FText HelloWorld = LOCTEXT("Default.HelloWorld", "Hello, World!");
		FText Left = LOCTEXT("Default.Left", "Left");
		FText Right = LOCTEXT("Default.Right", "Right");
#undef LOCTEXT_NAMESPACE
	};
	DefaultValuesStruct DefaultValues;

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
	const T* GetWidgetStyle()
	{
		const ISlateStyle* Style = GetSlateStyle();
		if (!(Style && Style->HasWidgetStyle<T>(GetPropertyName()))) {
			return nullptr;
		}
		return &GetSlateStyle()->GetWidgetStyle<T>(GetPropertyName());
	};

	// Helper function to get the string value for the specified Enum type
	template<typename T>
	static FString GetEnumValue(T Value)
	{
		UEnum* Enum = StaticEnum<T>();
		return Enum->GetNameStringByValue((int64)Value);
	}

	// Add detail by name and value
	void AddDetail(const FString& Name, const FString& Value);
	// Add detail by name and value using format string
#define AddDetailf(Name, Format, ...) AddDetail(Name, FString::Printf(Format, ##__VA_ARGS__))

	// Automatically fill all the details with the properties in the template struct
	template<typename T>
	void FillDetailsWithProperties()
	{
		const T* ws = GetWidgetStyle<T>();
		if (ws)
			FillDetailsInternal(T::StaticStruct(), ws, 0);
	};

protected:
	// generate string for FMargin struct
	static FString MarginString(FMargin Margin);

	// Fill details using a custom struct class and a custom container
	void FillDetailsInternal(const UScriptStruct* Struct, const void* Container, int Level=0);

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

class SLATESTYLEBROWSER_API ISlateStyleDataProvider
{
public:
	virtual ~ISlateStyleDataProvider() = default;
	virtual TSharedPtr<FSlateStyleData> MakeSlateStyleData(const ISlateStyle* SlateStyle, FName PropertyName, FName WidgetType) = 0;
	virtual TArray<FName> GetSupportedWidgetTypes() = 0;
	virtual TArray<FString> GetDefaultCopyStyles(FName WidgetType) { return {}; };
};
