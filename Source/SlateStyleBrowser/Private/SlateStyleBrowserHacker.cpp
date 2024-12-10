#include "SlateStyleBrowserHacker.h"

#if ENGINE_MAJOR_VERSION == 4

TSet<FName> HackerMisc::
GetStyleKeys(const FSlateStyleSet* Style)
{
	TSet<FName> AllKeys;

	{
		TArray<FName> Keys;
		Hacker::Steal_WidgetStyleValues(Style)->GenerateKeyArray(Keys);
		AllKeys.Append(Keys);
	}

	{
		TArray<FName> Keys;
		Hacker::Steal_FloatValues(Style)->GenerateKeyArray(Keys);
		AllKeys.Append(Keys);
	}

	{
		TArray<FName> Keys;
		Hacker::Steal_Vector2DValues(Style)->GenerateKeyArray(Keys);
		AllKeys.Append(Keys);
	}

	{
		TArray<FName> Keys;
		Hacker::Steal_ColorValues(Style)->GenerateKeyArray(Keys);
		AllKeys.Append(Keys);
	}

	{
		TArray<FName> Keys;
		Hacker::Steal_SlateColorValues(Style)->GenerateKeyArray(Keys);
		AllKeys.Append(Keys);
	}

	{
		TArray<FName> Keys;
		Hacker::Steal_MarginValues(Style)->GenerateKeyArray(Keys);
		AllKeys.Append(Keys);
	}

	{
		TArray<FName> Keys;
		Hacker::Steal_BrushResources(Style)->GenerateKeyArray(Keys);
		AllKeys.Append(Keys);
	}

	{
		TArray<FName> Keys;
		Hacker::Steal_Sounds(Style)->GenerateKeyArray(Keys);
		AllKeys.Append(Keys);
	}

	{
		TArray<FName> Keys;
		Hacker::Steal_FontInfoResources(Style)->GenerateKeyArray(Keys);
		AllKeys.Append(Keys);
	}

	return AllKeys;
}

#endif
