
Looking at your current setup, there are several ways to optimize this conversion:

1. **Modify the shader to match your engine's convention**:
   ```glsl
   // In vertex shader, change this line
   gl_Position = vec4(coord.xy, z_value, 1.0);
   ```
   This would make higher z_value = closer to screen, matching your engine's convention directly.

2. **Use a single efficient conversion in set_zdepth**:
   ```rust
   fn set_zdepth(layer: u32) {
       unsafe {
           // Direct conversion from layer to normalized depth
           // Use MAX_LAYER as your maximum possible layer value
           let normalized_z = 1.0 - (layer as f32 / MAX_LAYER as f32);
           ZDEPTH_UNIFORM.z_value = normalized_z;
           
           sgp_set_uniform(
               &ZDEPTH_UNIFORM as *const _ as *const core::ffi::c_void,
               std::mem::size_of::<ZDepthUniform>() as u32,
               std::ptr::null(), 0
           );
       }
   }
   ```

3. **Use fixed-point arithmetic** if you need extremely high performance:
   - Precompute a lookup table for common layer values
   - Or use bit shifts for approximating the conversion

Option 1 is the most efficient long-term solution as it eliminates both the conversion and inversion.

Excellent! Let me explain why this worked:

1. We created a proper depth-enabled pipeline with `sg_make_pipeline()` instead of using sokol_gp's pipeline.

2. The custom pipeline configuration specifically enables depth testing with:
   ```c
   sg_pip_desc.depth.write_enabled = true;
   sg_pip_desc.depth.compare = SG_COMPAREFUNC_LESS_EQUAL;
   ```

3. The z-values now correctly work with OpenGL's depth buffer convention (0.0 = closest, 1.0 = farthest).

4. We're keeping the shader's inversion (`1.0 - z_value`), but also flipping our z-values in the code (0.9 for front, 0.1 for back), which creates a consistent system:
   - Blue rect: z=0.1 → shader maps to 0.9 → appears furthest
   - Green rect: z=0.5 → shader maps to 0.5 → appears in middle
   - Red rect: z=0.9 → shader maps to 0.1 → appears closest

The painter's algorithm is no longer needed because the depth buffer is now handling the ordering correctly. This is more efficient and will work correctly with complex overlapping shapes too!
