#include "../../core/cog.h"

#include <assert.h>
#include <stdlib.h>
#include <stdint.h>
#include <wpe/webkit.h>
#include <wpe/fdo.h>
#include <wpe/unstable/fdo-shm.h>

#include <wayland-server.h>

#include <xcb/xcb.h>
#include <xcb/xcb_image.h>


#define DEFAULT_WIDTH  1024
#define DEFAULT_HEIGHT  768


struct CogXCBDisplay {
    struct {
        xcb_connection_t *connection;
        xcb_screen_t *screen;

        xcb_atom_t atom_wm_protocols;
        xcb_atom_t atom_wm_delete_window;
        xcb_atom_t atom_net_wm_name;
        xcb_atom_t atom_utf8_string;

        struct {
            int32_t x;
            int32_t y;
            uint32_t button;
            uint32_t state;
        } pointer;

        GSource *source;
    } xcb;
};

struct CogXCBWindow {
    struct {
        xcb_window_t window;

        bool needs_initial_paint;
        bool needs_frame_completion;
        unsigned width;
        unsigned height;
    } xcb;

    struct {
        struct wpe_view_backend_exportable_fdo *exportable;
        struct wpe_view_backend *backend;
    } wpe;

  gboolean frame_complete;
    uint8_t* framebuffer ;
    gsize framebuffer_size ;

  int32_t image_width;
  int32_t image_height;
  int32_t image_format;
  int32_t image_stride;

    guint tick_source;
};

static struct CogXCBDisplay *s_display = NULL;
static struct CogXCBWindow *s_window = NULL;


static void
xcb_schedule_repaint (void)
{
  /*
    xcb_client_message_event_t client_message = {
        .response_type = XCB_CLIENT_MESSAGE,
        .format = 32,
        .window = s_window->xcb.window,
        .type = XCB_ATOM_NOTICE,
    };

    xcb_send_event (s_display->xcb.connection, 0, s_window->xcb.window,
                    0, (char *) &client_message);
    xcb_flush (s_display->xcb.connection);
  */
}

#if 0
static void
xcb_initial_paint (void)
{
}
#endif

#if 0
static void
xcb_paint_image (struct wpe_fdo_egl_exported_image *image)
{
    s_window->xcb.needs_initial_paint = false;
    s_window->xcb.needs_frame_completion = true;
    xcb_schedule_repaint ();
}
#endif

#if 0
static void
xcb_frame_completion (void)
{
  /*
    if (s_window->wpe.image) {
        wpe_view_backend_exportable_fdo_egl_dispatch_release_exported_image (s_window->wpe.exportable, s_window->wpe.image);
        s_window->wpe.image = NULL;
    }
  */

    //wpe_view_backend_exportable_fdo_dispatch_frame_complete (s_window->wpe.exportable);
}
#endif

#if 0
static void
xcb_handle_key_press (xcb_key_press_event_t *event)
{
  //    wpe_view_backend_dispatch_keyboard_event (s_window->wpe.backend, &input_event);
}

static void
xcb_handle_key_release (xcb_key_press_event_t *event)
{
  /*
    struct wpe_input_keyboard_event input_event = {
        .time = event->time,
        .key_code = keysym,
        .hardware_key_code = unicode,
        .pressed = false,
        .modifiers = s_display->xkb.modifiers,
    };
    wpe_view_backend_dispatch_keyboard_event (s_window->wpe.backend, &input_event);
  */
}

static void
xcb_handle_axis (xcb_button_press_event_t *event, const int16_t axis_delta[2])
{
  /*
    struct wpe_input_axis_2d_event input_event = {
        .base = {
            .type = wpe_input_axis_event_type_mask_2d | wpe_input_axis_event_type_motion_smooth,
            .time = event->time,
            .x = s_display->xcb.pointer.x,
            .y = s_display->xcb.pointer.y,
        },
        .x_axis = axis_delta[0],
        .y_axis = axis_delta[1],
    };

    wpe_view_backend_dispatch_axis_event (s_window->wpe.backend, &input_event.base);
  */
}

static void
xcb_handle_button_press (xcb_button_press_event_t *event)
{
  /*
    static const int16_t axis_delta[4][2] = {
        {   0, -20 },
        {   0,  20 },
        { -20,   0 },
        {  20,   0 },
    };

    switch (event->detail) {
    case 1:
    case 2:
    case 3:
        s_display->xcb.pointer.button = event->detail;
        s_display->xcb.pointer.state = 1;
        break;
    case 4:
    case 5:
    case 6:
    case 7:
        xcb_handle_axis (event, axis_delta[event->detail - 4]);
        return;
    default:
        return;
    }

    struct wpe_input_pointer_event input_event = {
        .type = wpe_input_pointer_event_type_button,
        .time = event->time,
        .x = s_display->xcb.pointer.x,
        .y = s_display->xcb.pointer.y,
        .button = s_display->xcb.pointer.button,
        .state = s_display->xcb.pointer.state,
    };

    wpe_view_backend_dispatch_pointer_event (s_window->wpe.backend, &input_event);
  */
}

static void
xcb_handle_button_release (xcb_button_release_event_t *event)
{
  /*
    switch (event->detail) {
    case 1:
    case 2:
    case 3:
        s_display->xcb.pointer.button = event->detail;
        s_display->xcb.pointer.state = 0;
        break;
    default:
        return;
    }

    struct wpe_input_pointer_event input_event = {
        .type = wpe_input_pointer_event_type_button,
        .time = event->time,
        .x = s_display->xcb.pointer.x,
        .y = s_display->xcb.pointer.y,
        .button = s_display->xcb.pointer.button,
        .state = s_display->xcb.pointer.state,
    };

    wpe_view_backend_dispatch_pointer_event (s_window->wpe.backend, &input_event);

  */
}

static void
xcb_handle_motion_event (xcb_motion_notify_event_t *event)
{
  /*
    s_display->xcb.pointer.x = event->event_x;
    s_display->xcb.pointer.y = event->event_y;

    struct wpe_input_pointer_event input_event = {
        .type = wpe_input_pointer_event_type_motion,
        .time = event->time,
        .x = s_display->xcb.pointer.x,
        .y = s_display->xcb.pointer.y,
        .button = s_display->xcb.pointer.button,
        .state = s_display->xcb.pointer.state,
    };

    wpe_view_backend_dispatch_pointer_event (s_window->wpe.backend, &input_event);
  */
}
#endif

static xcb_gcontext_t foreground;
static int  image_format = XCB_IMAGE_FORMAT_Z_PIXMAP;
static  xcb_image_t *image = NULL;
//static  xcb_gcontext_t gc_image;

static void
xcb_repaint_window(xcb_expose_event_t *expose)
{
  g_debug("repaint_window - start");

  xcb_connection_t    *c = s_display->xcb.connection ;
  xcb_drawable_t       win = s_window->xcb.window ;

    /* geometric objects */
  xcb_point_t          points[] = {
    {10, 10},
    {10, 20},
    {20, 10},
    {20, 20}};

  xcb_point_t          polyline[] = {
    {50, 10},
    { 5, 20},     /* rest of points are relative */
    {25,-20},
    {10, 10}};

  xcb_segment_t        segments[] = {
    {100, 10, 140, 30},
    {110, 25, 130, 60}};

  xcb_rectangle_t      rectangles[] = {
    { 10, 50, 40, 20},
    { 80, 50, 10, 40}};

  xcb_arc_t            arcs[] = {
    {10, 100, 60, 40, 0, 90 << 6},
    {90, 100, 55, 40, 0, 270 << 6}};

  //from: https://www.x.org/releases/X11R7.5/doc/libxcb/tutorial/#drawing
      /* We draw the points */
      xcb_poly_point (c, XCB_COORD_MODE_ORIGIN, win, foreground, 4, points);

      /* We draw the polygonal line */
      xcb_poly_line (c, XCB_COORD_MODE_PREVIOUS, win, foreground, 4, polyline);

      /* We draw the segements */
      xcb_poly_segment (c, win, foreground, 2, segments);

      /* We draw the rectangles */
      xcb_poly_rectangle (c, win, foreground, 2, rectangles);

      /* We draw the arcs */
      xcb_poly_arc (c, win, foreground, 2, arcs);

      if (image) {
	xcb_image_put(c, win, foreground, image, 0, 0, 0);
      }

      /* We flush the request */
      xcb_flush (c);
}

static void
xcb_process_events (void)
{
  //g_debug("xcb_process_events - start");

    xcb_generic_event_t *event = NULL;

    while ((event = xcb_poll_for_event (s_display->xcb.connection))) {
      //g_debug("xck_process_events - got event");

        switch (event->response_type & 0x7f) {
	case 0: {
	  xcb_generic_error_t *err = (xcb_generic_error_t*)event;
	  g_error("Got X11/XCB error!\n");
	  break;
	}
        case XCB_CONFIGURE_NOTIFY:
        {
	  /*
            xcb_configure_notify_event_t *configure_notify = (xcb_configure_notify_event_t *) event;
            if (configure_notify->width == s_window->xcb.width
                && configure_notify->height == s_window->xcb.height)
                break;

            s_window->xcb.width = configure_notify->width;
            s_window->xcb.height = configure_notify->height;

            wpe_view_backend_dispatch_set_size (s_window->wpe.backend,
                                                s_window->xcb.width,
                                                s_window->xcb.height);
						xcb_schedule_repaint ();*/
	  //g_debug("XCB_CONFIGURE_NOTIFY");
            break;
        }
	case XCB_EXPOSE:
	  {
	    xcb_expose_event_t *expose = (xcb_expose_event_t *)event;

	    printf ("Window %"PRIu32" exposed. Region to be redrawn at location (%"PRIu16",%"PRIu16"), with dimension (%"PRIu16",%"PRIu16")\n",
		    expose->window, expose->x, expose->y, expose->width, expose->height );
	    g_debug("EXPOSE");
	    xcb_repaint_window(expose);
	    break;
	  }
        case XCB_CLIENT_MESSAGE:
        {
	  /*
            xcb_client_message_event_t *client_message = (xcb_client_message_event_t *) event;
            if (client_message->window != s_window->xcb.window)
                break;

            if (client_message->type == s_display->xcb.atom_wm_protocols
                && client_message->data.data32[0] == s_display->xcb.atom_wm_delete_window) {
                g_application_quit (g_application_get_default ());
                break;
            }

            if (client_message->type == XCB_ATOM_NOTICE) {
                if (s_window->xcb.needs_initial_paint) {
                    xcb_initial_paint ();
                    s_window->xcb.needs_initial_paint = false;
                }

                if (s_window->xcb.needs_frame_completion) {
                    xcb_frame_completion ();
                    s_window->xcb.needs_frame_completion = false;
                }
                break;
            }
	  */
            break;
        }
        case XCB_KEY_PRESS:
	  //xcb_handle_key_press ((xcb_key_press_event_t *) event);
            break;
        case XCB_KEY_RELEASE:
	  //xcb_handle_key_release ((xcb_key_release_event_t *) event);
            break;
        case XCB_BUTTON_PRESS:
	  //xcb_handle_button_press ((xcb_button_press_event_t *) event);
            break;
        case XCB_BUTTON_RELEASE:
	  //xcb_handle_button_release ((xcb_button_release_event_t *) event);
            break;
        case XCB_MOTION_NOTIFY:
	  //xcb_handle_motion_event ((xcb_motion_notify_event_t *) event);
            break;
        default:
            break;
        }
    }
};

struct xcb_source {
    GSource source;
    GPollFD pfd;
    xcb_connection_t *connection;
};

static gboolean
xcb_source_check (GSource *base)
{
  //  g_debug("xcb_source_check - start");
    struct xcb_source *source = (struct xcb_source *) base;
    return !!source->pfd.revents;
}

static gboolean
xcb_source_dispatch (GSource *base, GSourceFunc callback, gpointer user_data)
{
  //g_debug("xcb_source_dispatch - start");

  struct xcb_source *source = (struct xcb_source *) base;
    if (xcb_connection_has_error (source->connection))
        return G_SOURCE_REMOVE;

    if (source->pfd.revents & (G_IO_ERR | G_IO_HUP))
        return G_SOURCE_REMOVE;

    xcb_process_events ();
    source->pfd.revents = 0;
    return G_SOURCE_CONTINUE;
}

#if 0
static xcb_atom_t
get_atom (struct xcb_connection_t *connection, const char *name)
{
  /*
    xcb_intern_atom_cookie_t cookie = xcb_intern_atom (connection, 0, strlen(name), name);
    xcb_intern_atom_reply_t *reply = xcb_intern_atom_reply (connection, cookie, NULL);

    xcb_atom_t atom;
    if (reply) {
        atom = reply->atom;
        free(reply);
    } else
        atom = XCB_NONE;

    return atom;
  */
  return 0;
}
#endif

static gboolean
init_xcb ()
{
  int err;

  g_info("init_xcb");

    s_window->xcb.width = DEFAULT_WIDTH;
    s_window->xcb.height = DEFAULT_HEIGHT;

    s_display->xcb.connection = xcb_connect(NULL,NULL);

    if ((err = xcb_connection_has_error (s_display->xcb.connection))) {
	g_error("failed to connect to X11/XCB server, error code: 0x%0x", err); //FIXME: convert XCB_CONN to strings
        return FALSE;
      }

    s_window->xcb.window = xcb_generate_id (s_display->xcb.connection);

    const struct xcb_setup_t *setup = xcb_get_setup (s_display->xcb.connection);
      g_assert( setup != NULL );
    s_display->xcb.screen = xcb_setup_roots_iterator (setup).data;

    static uint32_t window_values[2] = {
					      0,

        XCB_EVENT_MASK_EXPOSURE |
        XCB_EVENT_MASK_STRUCTURE_NOTIFY |
        XCB_EVENT_MASK_KEY_PRESS |
        XCB_EVENT_MASK_KEY_RELEASE |
        XCB_EVENT_MASK_BUTTON_PRESS |
        XCB_EVENT_MASK_BUTTON_RELEASE |
        XCB_EVENT_MASK_POINTER_MOTION
    };
    window_values[0] =     s_display->xcb.screen->white_pixel;

    xcb_create_window (s_display->xcb.connection,
                       XCB_COPY_FROM_PARENT,         /* depth */
                       s_window->xcb.window,         /* window-id */
                       s_display->xcb.screen->root,  /* parent window */
                       0, 0,                         /* X, Y */
		       s_window->xcb.width, s_window->xcb.height, /* W, H */
                       5,                            /* Border Width */
                       XCB_WINDOW_CLASS_INPUT_OUTPUT,/* Class */
                       s_display->xcb.screen->root_visual, /* Visual */
                       XCB_CW_BACK_PIXEL | XCB_CW_EVENT_MASK, window_values); /* MASK & VALUES */

      /*
    s_display->xcb.atom_wm_protocols = get_atom (s_display->xcb.connection, "WM_PROTOCOLS");
    s_display->xcb.atom_wm_delete_window = get_atom (s_display->xcb.connection, "WM_DELETE_WINDOW");
    s_display->xcb.atom_net_wm_name = get_atom (s_display->xcb.connection, "_NET_WM_NAME");
    s_display->xcb.atom_utf8_string = get_atom (s_display->xcb.connection, "UTF8_STRING");
    xcb_change_property (s_display->xcb.connection,
                         XCB_PROP_MODE_REPLACE,
                         s_window->xcb.window,
                         s_display->xcb.atom_wm_protocols,
                         XCB_ATOM_ATOM,
                         32,
                         1, &s_display->xcb.atom_wm_delete_window);

    xcb_change_property (s_display->xcb.connection,
                         XCB_PROP_MODE_REPLACE,
                         s_window->xcb.window,
                         s_display->xcb.atom_net_wm_name,
                         s_display->xcb.atom_utf8_string,
                         8,
                         strlen("Cog"), "Cog");
      */


    {
       uint32_t             mask = 0;
       uint32_t             values[2];
         foreground = xcb_generate_id (s_display->xcb.connection);

        mask = XCB_GC_FOREGROUND | XCB_GC_GRAPHICS_EXPOSURES;
	values[0] = s_display->xcb.screen->black_pixel;
	values[1] = 0;
	xcb_create_gc (s_display->xcb.connection, foreground, s_window->xcb.window, mask, values);

	//gc_image = xcb_generate_id(s_display->xcb.connection);
	//xcb_create_gc(s_display->xcb.connection,gc_image,  s_window->xcb.window,0,NULL);
    }

    xcb_map_window (s_display->xcb.connection, s_window->xcb.window);
    xcb_flush (s_display->xcb.connection);

    xcb_schedule_repaint ();

    g_info("init_xcb - done");

    return TRUE;
}

static void
clear_xcb (void)
{
  xcb_disconnect(s_display->xcb.connection);
}

static gboolean
init_glib (void)
{
  g_debug("init_glib - start");

    static GSourceFuncs xcb_source_funcs = {
        .check = xcb_source_check,
        .dispatch = xcb_source_dispatch,
    };

    s_display->xcb.source = g_source_new (&xcb_source_funcs,
                                          sizeof (struct xcb_source));
    {
        struct xcb_source *source = (struct xcb_source *) s_display->xcb.source;
        source->connection = s_display->xcb.connection;

        source->pfd.fd = xcb_get_file_descriptor (s_display->xcb.connection);
        source->pfd.events = G_IO_IN | G_IO_ERR | G_IO_HUP;
        source->pfd.revents = 0;
        g_source_add_poll (s_display->xcb.source, &source->pfd);

        g_source_set_name (s_display->xcb.source, "cog-xcb: xcb");
        g_source_set_can_recurse (s_display->xcb.source, TRUE);
        g_source_attach (s_display->xcb.source, g_main_context_get_thread_default ());
    }

  g_debug("init_glib - done");
    return TRUE;
}

static void
clear_glib (void)
{
    if (s_display->xcb.source)
        g_source_destroy (s_display->xcb.source);
    g_clear_pointer (&s_display->xcb.source, g_source_unref);
}

static void
ttt_copy_shm_buffer (struct wl_shm_buffer *shm_buffer)
{
    g_warning("ttt_copy_shm_buffer - start");

    int32_t width = wl_shm_buffer_get_width (shm_buffer);
    int32_t height = wl_shm_buffer_get_height (shm_buffer);
    int32_t stride = wl_shm_buffer_get_stride (shm_buffer);
    uint32_t format = wl_shm_buffer_get_format (shm_buffer);

    //uint32_t bo_stride = 0;

    wl_shm_buffer_begin_access (shm_buffer);

    uint8_t *src = wl_shm_buffer_get_data (shm_buffer);

    g_info("shm_buffer = %dx%d (%d) format=%u src = %p", width, height, stride, format,  src);

    gsize need_size = stride * height ;
    if (need_size > s_window->framebuffer_size) {
      g_info("reallocating framebuffer to %lu", need_size);
      s_window->framebuffer = g_realloc(s_window->framebuffer, need_size);
      s_window->framebuffer_size = need_size ;
    }

    //memset(s_window->framebuffer, 128, need_size);

    if ( (image == NULL) ||
	 (width != s_window->image_width) ||
	 (height != s_window->image_height) ||
	 (stride != s_window->image_stride) ||
	 (format != s_window->image_format)) {
      if (image) {
	g_info("ttt_copy_shm_buffer - destroying previous XCB image");
	xcb_image_destroy(image);
	image = NULL;
      }
      g_info("ttt_copy_shm_buffer - re-creating native XCB image");

      image = xcb_image_create_native(s_display->xcb.connection,
				      width,
				      height,
				      XCB_IMAGE_FORMAT_Z_PIXMAP,
				      s_display->xcb.screen->root_depth,
				      NULL,
				      0,
				      s_window->framebuffer);

      g_info("new image = %p", image);

      s_window->image_width = width ;
      s_window->image_height = height ;
      s_window->image_format = format ;
      s_window->image_stride = stride ;
    }

    memmove(s_window->framebuffer, src, need_size);
}

static void on_export_shm_buffer(void* data, struct wpe_fdo_shm_exported_buffer* exported_buffer)
{
  //struct platform_window* window = (struct platform_window*) data;
    g_info("shm_buffer - start");

    struct wl_shm_buffer *exported_shm_buffer = wpe_fdo_shm_exported_buffer_get_shm_buffer (exported_buffer);

    g_info("shm_buffer - exported_shm_buffer = '%p'", exported_shm_buffer);

    ttt_copy_shm_buffer(exported_shm_buffer);


    wpe_view_backend_exportable_fdo_dispatch_release_shm_exported_buffer(s_window->wpe.exportable, exported_buffer);
    //window->frame_complete = TRUE;

    g_info("shm_buffer - repainting");
    xcb_repaint_window(NULL);

    g_info("shm_buffer - frame-complete");
    //wpe_view_backend_exportable_fdo_dispatch_frame_complete(s_window->wpe.exportable);

    s_window->frame_complete = TRUE ;

    g_info("shm_buffer - done");
}

static gboolean tick_callback(gpointer data)
{
  if (s_window->frame_complete) {
    g_debug("tick - FRAME COMPLETE");
    s_window->frame_complete = false ;
    wpe_view_backend_exportable_fdo_dispatch_frame_complete(s_window->wpe.exportable);
  } else {
    g_debug("tick - no frame");
  }
  return G_SOURCE_CONTINUE;
}


gboolean
cog_platform_plugin_setup (CogPlatform *platform,
                           CogShell    *shell G_GNUC_UNUSED,
                           const char  *params,
                           GError     **error)
{
    g_assert (platform);
    g_return_val_if_fail (COG_IS_SHELL (shell), FALSE);

    s_display = calloc (sizeof (struct CogXCBDisplay), 1);
    s_window = calloc (sizeof (struct CogXCBWindow), 1);

    g_info("XCB options: '%s'", params);

    if (!wpe_loader_init ("libWPEBackend-fdo-1.0.so")) {
        g_set_error_literal (error,
                             COG_PLATFORM_WPE_ERROR,
                             COG_PLATFORM_WPE_ERROR_INIT,
                             "Failed to set backend library name");
        return FALSE;
    }

    g_info("before init_xcb");

    if (!init_xcb ()) {
        g_set_error_literal (error,
                             COG_PLATFORM_WPE_ERROR,
                             COG_PLATFORM_WPE_ERROR_INIT,
                             "Failed to initialize XCB");
        return FALSE;
    }

    g_info("before init_glib");

    if (!init_glib ()) {
        g_set_error_literal (error,
                             COG_PLATFORM_WPE_ERROR,
                             COG_PLATFORM_WPE_ERROR_INIT,
                             "Failed to initialize GLib");
        return FALSE;
    }

    g_info("tick setup");

    int fps = 2 ;
    s_window->tick_source = g_timeout_add(1000/fps, G_SOURCE_FUNC(tick_callback), NULL);


    g_info("init - complete");

    return TRUE;
}

void
cog_platform_plugin_teardown (CogPlatform *platform)
{
    g_info("Teardown - start");

    g_assert (platform);

    g_source_remove(s_window->tick_source);

    clear_glib ();
    clear_xcb ();

    g_clear_pointer (&s_window, free);
    g_clear_pointer (&s_display, free);
}

WebKitWebViewBackend*
cog_platform_plugin_get_view_backend (CogPlatform   *platform,
                                      WebKitWebView *related_view,
                                      GError       **error)
{
    g_assert_nonnull(platform);

    g_debug("get_view_backend - start ");

    wpe_fdo_initialize_shm();

    g_debug("Step 1 ");

    static const struct wpe_view_backend_exportable_fdo_client client = {
        .export_shm_buffer = on_export_shm_buffer,
    };

    g_debug("step 2");

    s_window->wpe.exportable = wpe_view_backend_exportable_fdo_create(&client, s_window, s_window->xcb.width, s_window->xcb.height);

    g_debug("step 3");

    /* init WPE view backend */
    s_window->wpe.backend =
        wpe_view_backend_exportable_fdo_get_view_backend (s_window->wpe.exportable);
    g_assert (s_window->wpe.backend);

    g_debug("step 3");

    WebKitWebViewBackend *wk_view_backend =
        webkit_web_view_backend_new (s_window->wpe.backend,
                                     (GDestroyNotify) wpe_view_backend_exportable_fdo_destroy,
                                     s_window->wpe.exportable);
    g_assert (wk_view_backend);

    g_debug("step 4 - done");

    return wk_view_backend;
}
