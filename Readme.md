# Slate Style Browser

This small tool lets you browse Unreal Engine's Slate styles easily, search for specific ones and copy slate code for the selected style or brush.

![Screenshot of the window](Documentation/WindowScreenshot.png)

## Invocation

When the plugin is activated there's only one window which you can open via `Tools -> Slate Style Browser`.

## Features

### Searching

Using the search bar it's possible to filter the styles based on their names.

### Select a Style Set

Using a dropdown menu you can select another style set besides the default Editor style.

### Select a Style Type

The tool supports various different types, which you can select using another dropdown menu.

### Copying Slate Code

Slate Style Browser supports multiple code copying styles you can set up and use.
In addition to your custom presets, you can always type some _quick style_ right into the tool window.

To adjust the presets:

* `Edit` &rarr; `Editor Preferences`
* `Plugins` &rarr; `Slate Style Browser`
* Configure as you like (see also: [Configuration](#Configuration))

To reset the configuration to a default, you first have to check the `Allow Reset to Default` checkbox, then click the `Reset to Defaults` button.

Within the copy styles, there are a few placeholders that will be replaced:

* `$1` is the property name
* `$2` is the style set name
* `$3` is the widget style type
* `$$` is just `$`

In addition to that, there are generic replacements that can help to:

* replace `FSlateStyleRegistry::FindSlateStyle("AppStyle")` with `FAppStyle::Get()`
* replace `FSlateStyleRegistry::FindSlateStyle("CoreStyle")` with `FCoreStyle::Get()`
* etc.

As with everything source-code related you are supposed to read it before you include it and adjust it to make it work in your environment.

## Configuration

(`Editor Preferences` &rarr; `Plugins` &rarr; `Slate Style Browser`)

* `Allow Reset to Default`: Check to make the `Reset to Defaults` button work.
* `Copy Styles`: Per type, set preset copy styles here.
* `Readability Replacements`: Automatically replace text to enhance readability of copied code.

---

# Source Code and License

The source code is licensed under the MIT license and is hosted at <https://github.com/sirjofri/SlateIconBrowser>.
See also the [License.txt](https://github.com/sirjofri/SlateIconBrowser/blob/master/License.txt) file inside the repository for the full license text.