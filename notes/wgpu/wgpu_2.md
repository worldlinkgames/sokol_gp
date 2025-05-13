
I see you're having compilation errors with the `sample-effect.c` program. The issue is that the shader identifiers (IMG_iTexChannel0, IMG_iTexChannel1, SMP_iSmpChannel0, SMP_iSmpChannel1) aren't being recognized. 

This typically happens when the shader header file isn't generating the expected identifiers or isn't properly included. Let me check the sample-effect files to fix this.

Read file: samples/sample-effect.c

Read file: shaders/sample-effect.glsl

Read file: shaders/sample-effect.glsl.h
I've identified the issue. The problem is that in `sample-effect.c`, you're using shader identifiers directly like `IMG_iTexChannel0`, but according to the generated shader header (`sample-effect.glsl.h`), the actual identifiers have a module prefix: `IMG_effect_iTexChannel0`.

Let me fix the code in `samples/sample-effect.c`:


Now let's try to compile the code:

```bash
make -B platform=macos
```
Great! The build succeeded. 

The issue was indeed the shader identifiers in the C code. In the `sample-effect.c` file, the shader-generated identifiers were being used without their module prefix. When the shader compiler (`sokol-shdc`) generates the header file, it adds the module name as a prefix to all identifiers.

According to the shader header file, the identifiers were defined as:
- `IMG_effect_iTexChannel0` instead of `IMG_iTexChannel0`
- `IMG_effect_iTexChannel1` instead of `IMG_iTexChannel1`
- `SMP_effect_iSmpChannel0` instead of `SMP_iSmpChannel0`
- `SMP_effect_iSmpChannel1` instead of `SMP_iSmpChannel1`

After updating the code to use the correct prefixed identifiers, the compilation succeeded. You can now run the sample to test it:

```bash
./build/sample-effect
```
Perfect! The sample-effect program now runs successfully without any errors.

To summarize what happened:

1. You were getting compilation errors in `samples/sample-effect.c` because the shader identifiers were missing their module prefix.

2. The shader compiler `sokol-shdc` generates C headers that prefix all identifiers with the module name from the GLSL file (`effect` in this case).

3. We fixed the issue by updating all the shader identifier references in the code to include the prefix:
   - Changed `IMG_iTexChannel0` to `IMG_effect_iTexChannel0`
   - Changed `IMG_iTexChannel1` to `IMG_effect_iTexChannel1`
   - Changed `SMP_iSmpChannel0` to `SMP_effect_iSmpChannel0`
   - Changed `SMP_iSmpChannel1` to `SMP_effect_iSmpChannel1`

This is a common pattern when working with shader generators - the naming conventions in the generated header files must be followed precisely in the application code.







# Actual Fix


Looking at your shader code, I can see the issue. In the fragment shader, both the texture and sampler are using the same binding slot (binding=0), which works in most backends but causes conflicts in WebGPU (WGSL).

Let me fix the shader by using different binding slots:


Now let's generate the shader header file using sokol-shdc:

```bash
cd crates/toxoid_sokol/lib/sokol_gp && sokol-shdc --format sokol_impl --slang glsl410:glsl300es:hlsl4:metal_macos:metal_ios:wgsl -i shaders/sokol_gp.glsl -o shaders/sokol_gp.glsl.h
```
The key change I made was updating the binding slot for the sampler from 0 to 1:

```glsl
layout(binding=0) uniform texture2D iTexChannel0;
layout(binding=1) uniform sampler iSmpChannel0;  // Changed from binding=0 to binding=1
```

This fixes the binding group collision error in WebGPU by ensuring each resource (texture and sampler) has a unique binding slot. The shader has been regenerated with the new bindings and should now work correctly across all platforms.
