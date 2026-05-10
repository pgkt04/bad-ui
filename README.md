# bad-ui
This was made as an alternative to imgui, nuklear, fgui that is minimal and doesn't contain bloat like these other ones do.  
This works with all renders including:
- directx9
- directx11
- opengl
- vulkan
- anything that can draw a line and render text

  

![ui](resources/ui.png)

## macOS

Build and run the native Cocoa/CoreGraphics sample host:

```sh
make macos
./build/bad-ui-macos
```

The Visual Studio/D3D9 sample remains available under `src/kitto-ui.sln` for Windows.
