#pragma once

#if ENGINE_MAJOR_VERSION == 4
#include "Styling/SlateStyle.h"


#define DECLARE_THIEF_FUNCTION(Class, MemberType, MemberName)\
	template <MemberType Class::* MEMBER_PTR>\
	struct GenerateThiefFunction_##MemberName{\
		friend MemberType* Steal_##MemberName(Class* Victim)       { return &(Victim->*MEMBER_PTR); }\
		friend MemberType* Steal_##MemberName(Class& Victim)       { return &(Victim.*MEMBER_PTR);  }\
		friend MemberType* Steal_##MemberName(const Class* Victim) { return &(const_cast<Class*>(Victim)->*MEMBER_PTR);  }\
		friend MemberType* Steal_##MemberName(const Class& Victim) { return &(const_cast<Class&>(Victim).*MEMBER_PTR);   }\
	};\
	template struct GenerateThiefFunction_##MemberName<&Class::MemberName>;\
	MemberType* Steal_##MemberName(Class* Victim);\
	template struct GenerateThiefFunction_##MemberName<&Class::MemberName>;\
	MemberType* Steal_##MemberName(Class& Victim);\
	template struct GenerateThiefFunction_##MemberName<&Class::MemberName>;\
	MemberType* Steal_##MemberName(const Class* Victim);\
	template struct GenerateThiefFunction_##MemberName<&Class::MemberName>;\
	MemberType* Steal_##MemberName(const Class& Victim);


namespace Hacker
{
	typedef TMap<FName, TSharedRef<FSlateWidgetStyle>> FWidgetStyleValuesMap;
	typedef TMap<FName, float> FFloatValuesMap;
	typedef TMap<FName, FVector2D> FVector2DValuesMap;
	typedef TMap<FName, FLinearColor> FFColorValuesMap;
	typedef TMap<FName, FSlateColor> FSlateColorValuesMap;
	typedef TMap<FName, FMargin> FMarginValuesMap;
	typedef TMap<FName, FSlateBrush*> FBrushResourcesMap;
	typedef TMap<FName, FSlateSound> FSoundsMap;
	typedef TMap<FName, FSlateFontInfo> FFontInfoResourcesMap;

	DECLARE_THIEF_FUNCTION(FSlateStyleSet, FWidgetStyleValuesMap, WidgetStyleValues)
	DECLARE_THIEF_FUNCTION(FSlateStyleSet, FFloatValuesMap, FloatValues)
	DECLARE_THIEF_FUNCTION(FSlateStyleSet, FVector2DValuesMap, Vector2DValues)
	DECLARE_THIEF_FUNCTION(FSlateStyleSet, FFColorValuesMap, ColorValues)
	DECLARE_THIEF_FUNCTION(FSlateStyleSet, FSlateColorValuesMap, SlateColorValues)
	DECLARE_THIEF_FUNCTION(FSlateStyleSet, FMarginValuesMap, MarginValues)
	DECLARE_THIEF_FUNCTION(FSlateStyleSet, FBrushResourcesMap, BrushResources)
	DECLARE_THIEF_FUNCTION(FSlateStyleSet, FSoundsMap, Sounds)
	DECLARE_THIEF_FUNCTION(FSlateStyleSet, FFontInfoResourcesMap, FontInfoResources)
}


namespace HackerMisc
{
	TSet<FName> GetStyleKeys(const FSlateStyleSet* Style);
}

#undef DECLARE_THIEF_FUNCTION
#endif