// Copyright sirjofri. Licensed under MIT license. See License.txt for full license text.

#pragma once

class ISlateStyleData
{
public:
	virtual TSharedRef<SWidget> GenerateRowWidget() = 0;

	
	virtual ~ISlateStyleData() = default;

	virtual void Initialize(FName InStyle, FName InPropertyName)
	{
		StyleName = InStyle;
		PropertyName = InPropertyName;
	};
	
	virtual FName GetStyleName() { return StyleName;};
	virtual FName GetPropertyName() { return PropertyName; };

protected:
	FName StyleName;
	FName PropertyName;
};
