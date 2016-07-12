/**
 * PS3EYEDriver Simple SDL 2 example, using OpenGL where available.
 * Thomas Perl <m@thp.io>; 2014-01-10
 * Joseph Howse <josephhowse@nummist.com>; 2014-12-26
 **/

#include <SDL.h>
#include "ps4eye.h"

struct ps4eye_context {
    ps4eye_context()
        : eye(nullptr)
        , devices(ps4eye::PS4EYECam::getDevices())
        , running(true)
        , last_ticks(0)
        , last_frames(0)
    {

    }
    
    bool setup(int deviceIndex, int width, int height, int fps)
    {
        bool bSuccess= true;
               
        if (bSuccess)
        {
            if (deviceIndex >= 0 && deviceIndex < (int)devices.size())
            {
                eye = devices[deviceIndex];
            }
            else
            {
                bSuccess= false;
            }
        }

        if (bSuccess)
        {        
            eye->firmware_path="resources/firmware.bin";
            eye->firmware_upload();        
        }
        
        if (bSuccess)
        {
            bSuccess= eye->init(1, (uint8_t)fps);
        }
               
        return bSuccess;
    }

    bool hasDevices()
    {
        return (devices.size() > 0);
    }

    bool isEyeInitialized()
    {
        return eye != NULL; // && eye->isInitialized();
    }
    
    ps4eye::PS4EYECam::PS4EYERef eye;
    std::vector<ps4eye::PS4EYECam::PS4EYERef> devices;

    bool running;
    Uint32 last_ticks;
    Uint32 last_frames;
};

void
print_renderer_info(SDL_Renderer *renderer)
{
    SDL_RendererInfo renderer_info;
    SDL_GetRendererInfo(renderer, &renderer_info);
    printf("Renderer: %s\n", renderer_info.name);
}

int
main(int argc, char *argv[])
{
    int camera_index = 0;

    if (argc >= 2)
    {
        if (sscanf_s(argv[1], "%d", &camera_index) != 1)
        {
            printf("usage: ps3eye_sdl.exe <camera index>");
        }
    }

    ps4eye_context *ctx = new ps4eye_context;
        
    if (!ctx->hasDevices()) {
        printf("No PS3 Eye camera connected\n");
        return EXIT_FAILURE;
    }

    if (!ctx->setup(camera_index, 1280, 800, 30))
    {
        printf("PS3 Eye camera(%d) failed to initialize\n", camera_index);
        return EXIT_FAILURE;
    }
    
    ctx->eye->set_mirror_sensors(1); /* mirrored left-right */

    //char usb_port_path[64];
    //if (ctx->eye->getUSBPortPath(usb_port_path, sizeof(usb_port_path)))
    //{
    //    printf("PS3 Eye camera opened on USB port path: %s\n", usb_port_path);
    //}
    //else 
    //{
    //    printf("PS3 Eye camera opened on USB port path: <ERROR>\n");
    //}

    if (SDL_Init(SDL_INIT_VIDEO) < 0) {
        printf("Failed to initialize SDL: %s\n", SDL_GetError());
        return EXIT_FAILURE;
    }

    SDL_Window *window = SDL_CreateWindow(
            "PS3 Eye - SDL 2", SDL_WINDOWPOS_UNDEFINED,
            SDL_WINDOWPOS_UNDEFINED, 640, 480, 0);
    if (window == NULL) {
        printf("Failed to create window: %s\n", SDL_GetError());
        return EXIT_FAILURE;
    }

    SDL_Renderer *renderer = SDL_CreateRenderer(window, -1,
                                                SDL_RENDERER_PRESENTVSYNC);
    if (renderer == NULL) {
        printf("Failed to create renderer: %s\n", SDL_GetError());
        SDL_DestroyWindow(window);
        return EXIT_FAILURE;
    }
    SDL_RenderSetLogicalSize(renderer, ctx->eye->getWidth(), ctx->eye->getHeight());
    print_renderer_info(renderer);

    SDL_Texture *video_tex = SDL_CreateTexture(
            renderer, SDL_PIXELFORMAT_YUY2, SDL_TEXTUREACCESS_STREAMING,
            ctx->eye->getWidth(), ctx->eye->getHeight());
    if (video_tex == NULL) {
        printf("Failed to create video texture: %s\n", SDL_GetError());
        SDL_DestroyRenderer(renderer);
        SDL_DestroyWindow(window);
        return EXIT_FAILURE;
    }

    ctx->eye->start();

    printf("Camera mode: %dx%d@%d\n", ctx->eye->getWidth(), ctx->eye->getHeight(), ctx->eye->getFrameRate());

    SDL_Event e;
    bool bShowLeftFrame = true;
    
    while (ctx->running) {
        while (SDL_PollEvent(&e)) {
            if (e.type == SDL_QUIT) {
                ctx->running = false;
            }
            else if (e.type == SDL_KEYDOWN && e.key.keysym.sym == SDLK_SPACE)
            {
                bShowLeftFrame= !bShowLeftFrame;
            }
        }

        ps4eye::PS4EYECam::updateDevices();

        if (ctx->eye->isNewFrame())
        {
            ctx->eye->check_ff71();
            eyeframe *frame= ctx->eye->getLastVideoFramePointer();        
            uint8_t* new_pixels = bShowLeftFrame ? frame->videoLeftFrame : frame->videoRightFrame;

            {
                Uint32 now_ticks = SDL_GetTicks();

                ctx->last_frames++;

                if (now_ticks - ctx->last_ticks > 1000) 
                {
                    printf("FPS: %.2f\n", 1000 * ctx->last_frames / (float(now_ticks - ctx->last_ticks)));
                    ctx->last_ticks = now_ticks;
                    ctx->last_frames = 0;
                }
            }

            {
                void *video_tex_pixels;
                int pitch;
                SDL_LockTexture(video_tex, NULL, &video_tex_pixels, &pitch);
                memcpy(video_tex_pixels, new_pixels, ctx->eye->getRowBytes() * ctx->eye->getHeight());
                SDL_UnlockTexture(video_tex);
            }
        }

        SDL_RenderCopy(renderer, video_tex, NULL, NULL);
        SDL_RenderPresent(renderer);
    }

    ctx->eye->stop();
    delete ctx;

    SDL_DestroyTexture(video_tex);
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);

    return EXIT_SUCCESS;
}
