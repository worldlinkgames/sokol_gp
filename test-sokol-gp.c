#define SOKOL_IMPL
#include "sokol_gfx.h"
#include "sokol_gp.h"
#include "sokol_app.h"
#include "sokol_glue.h"
#include "sokol_log.h"

#include <stdio.h>
#include <stdlib.h>

// Called on every frame of the application.
static void frame(void) {
    // Get current window size.
    int width = sapp_width(), height = sapp_height();

    // Begin recording draw commands
    sgp_begin(width, height);
    
    // Set drawing region to (0,0,width,height).
    sgp_viewport(0, 0, width, height);
    
    // Clear the frame buffer.
    sgp_set_color(0.1f, 0.1f, 0.1f, 1.0f);
    sgp_clear();

    // Draw a simple rectangle
    sgp_set_color(1.0f, 0.0f, 0.0f, 1.0f);
    sgp_draw_filled_rect(100, 100, 200, 200);

    // Begin a render pass.
    sg_pass pass = {.swapchain = sglue_swapchain()};
    sg_begin_pass(&pass);
    
    // Dispatch all draw commands to Sokol GFX.
    sgp_flush();
    
    // Finish a draw command queue, clearing it.
    sgp_end();
    
    // End render pass.
    sg_end_pass();
    
    // Commit Sokol render.
    sg_commit();
}

// Called when the application is initializing.
static void init(void) {
    // Initialize Sokol GFX.
    sg_desc sgdesc = {
        .environment = sglue_environment(),
        .logger.func = slog_func
    };
    sg_setup(&sgdesc);
    if (!sg_isvalid()) {
        fprintf(stderr, "Failed to create Sokol GFX context!\n");
        exit(-1);
    }

    // Initialize Sokol GP
    sgp_desc sgpdesc = {0};
    sgp_setup(&sgpdesc);
    if (!sgp_is_valid()) {
        fprintf(stderr, "Failed to create Sokol GP context: %s\n", sgp_get_error_message(sgp_get_last_error()));
        exit(-1);
    }
}

// Called when the application is shutting down.
static void cleanup(void) {
    // Cleanup Sokol GP and Sokol GFX resources.
    sgp_shutdown();
    sg_shutdown();
}

// Implement application main through Sokol APP.
sapp_desc sokol_main(int argc, char* argv[]) {
    (void)argc;
    (void)argv;
    return (sapp_desc){
        .init_cb = init,
        .frame_cb = frame,
        .cleanup_cb = cleanup,
        .window_title = "Sokol GP Test",
        .logger.func = slog_func,
    };
} 