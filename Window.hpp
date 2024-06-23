#pragma once

#include <SDL2/SDL.h>

#include <stack>
#include <string>
#include <utility>

#ifndef UPS
constexpr int UPDATES_PER_SECOND = 60;
#elif
constexpr int UPDATES_PER_SECOND = UPS;
#endif

class Window {
public:
    Window(const char* p_title, int p_w, int p_h);
    ~Window();
    // window related
    int getRefreshrate();
    std::pair<int, int> getWindowSize();
    void setWindowSize(int x, int y);
    // drawing related
    bool render(SDL_Rect src, SDL_Rect dst, SDL_Texture* tex);

    void renderCopy(SDL_Texture* texture,
        const SDL_Rect* srcrect,
        const SDL_Rect* dstrect);

    void display();
public:
    void clear();
    void drawCircle(int x, int y, int r);
    void drawRect(SDL_Rect rec);
    void drawRectFilled(SDL_Rect rec);
    // utility
    SDL_Texture* CreateTextureFromSurface(SDL_Surface* surface);

    SDL_Texture* CreateTextureFromWindow();

    void setDrawColor(Uint8 r, Uint8 g, Uint8 b, Uint8 a);
    void getDrawColor(Uint8& r, Uint8& g, Uint8& b, Uint8& a);

    void push_color(Uint8 r, Uint8 g, Uint8 b, Uint8 a);
    void pop_color();

    SDL_Rect getInnerRect(SDL_Rect parent, float aspect_ratio);

private:
    SDL_Window* window;
    SDL_Renderer* renderer;

    SDL_Renderer* getRenderer();

    std::stack<SDL_Color> colors;
};

#ifdef WINDOW_IMPL

#include <SDL2/SDL.h>

#include <iostream>

Window::Window(const char* p_title, const int p_w, const int p_h)
    : window(NULL), renderer(NULL) {
    this->window =
        SDL_CreateWindow(p_title, SDL_WINDOWPOS_UNDEFINED,
            SDL_WINDOWPOS_UNDEFINED, p_w, p_h, SDL_WINDOW_SHOWN | SDL_WINDOW_RESIZABLE | SDL_WINDOW_OPENGL);
    if (this->window == NULL) {
        std::cout << "Window failed to init. Error: " << SDL_GetError() << std::endl;
    }

    renderer = SDL_CreateRenderer(
        this->window, -1, SDL_RENDERER_ACCELERATED
        | SDL_RendererFlags::SDL_RENDERER_PRESENTVSYNC
    );
    //SDL_RenderSetLogicalSize(this->renderer, DEFAULT_SCREEN_WIDTH, DEFAULT_SCREEN_HEIGHT); 



    SDL_Surface* surface;     // Declare an SDL_Surface to be filled in with pixel data from an image file
#define O 0x0000
#define _ 0xffff
    Uint16 pixels[16 * 16] = {  // ...or with raw pixel data:
      _, _, _, _, _, _, _, _, _, _, _, _, _, _, _, _,
      _, _, _, _, _, _, O, O, O, O, O, _, _, _, _, _,
      _, _, _, _, _, O, _, _, _, _, _, O, _, _, _, _,
      _, _, _, _, O, _, _, _, _, _, _, O, _, _, _, _,
      _, _, _, _, O, _, _, _, _, _, O, _, O, _, _, _,
      _, _, O, O, _, _, O, _, _, _, _, _, O, O, _, _,
      _, _, _, O, _, _, _, _, O, O, _, _, O, _, _, _,
      _, _, _, O, _, _, _, _, O, O, _, O, O, _, _, _,
      _, _, _, O, _, _, _, _, _, _, _, O, _, _, _, _,
      _, _, _, O, _, _, _, _, _, _, _, O, _, _, _, _,
      _, _, _, _, O, _, _, _, _, O, O, _, _, _, _, _,
      _, _, _, _, _, O, O, O, O, _, _, O, _, _, _, _,
      _, _, _, _, _, O, _, _, _, _, _, O, O, _, _, _,
      _, _, _, _, _, O, _, _, _, _, _, _, _, _, _, _,
      _, _, _, _, O, O, _, _, _, _, _, _, _, _, _, _,
      _, _, _, _, _, _, _, _, _, _, _, _, _, _, _, _
    };
#undef _
#undef O
    surface = SDL_CreateRGBSurfaceFrom(pixels, 16, 16, 16, 32, 0x0f00, 0x00f0, 0x000f, 0xf000);

    // The icon is attached to the window pointer
    SDL_SetWindowIcon(this->window, surface);

    // ...and the surface containing the icon pixel data is no longer required.
    SDL_FreeSurface(surface);
}

Window::~Window() {
    SDL_DestroyRenderer(this->renderer);
    SDL_DestroyWindow(this->window);
}

int Window::getRefreshrate() {
    int displayIndex = SDL_GetWindowDisplayIndex(this->window);
    SDL_DisplayMode mode;
    SDL_GetDisplayMode(displayIndex, 0, &mode);
    return mode.refresh_rate;
}

std::pair<int, int> Window::getWindowSize() {
    int x, y;
    SDL_GetWindowSize(this->window, &x, &y);
    return { x, y };
}

void Window::setWindowSize(int x, int y) {
    SDL_SetWindowSize(this->window, x, y);
}

// passing in a src with all zeros will grab the entire texture
// returns weather it worked or not
bool Window::render(SDL_Rect src, SDL_Rect dst, SDL_Texture* tex) {

    auto checkIfSet = [&](SDL_Rect box) {if ((box.x == 0) && (box.y == 0) && (box.w == 0) && (box.h == 0)) return true; else return false; };

    auto err = SDL_RenderCopy(this->renderer, tex, (checkIfSet(src)) ? NULL : &src, &dst);

    if (err != 0) {
        SDL_Log("SDL2 Error: %s", SDL_GetError());
        return false;
    }
    return true;

}

void Window::renderCopy(SDL_Texture* texture, const SDL_Rect* srcrect, const SDL_Rect* dstrect) {
    SDL_RenderCopy(renderer, texture, srcrect, dstrect);
}


SDL_Renderer* Window::getRenderer() {
    return renderer;
}

void Window::display() { SDL_RenderPresent(this->renderer); }

void Window::clear() { SDL_RenderClear(this->renderer); }

void Window::drawCircle(int X, int Y, int r) {
    const int32_t diameter = (r * 2);

    int32_t x = (r - 1);
    int32_t y = 0;
    int32_t tx = 1;
    int32_t ty = 1;
    int32_t error = (tx - diameter);

    while (x >= y)
    {
        //  Each of the following renders an octant of the circle
        SDL_Point points[8] = {
            {X + x, Y - y},
            {X + x, Y + y},
            {X - x, Y - y},
            {X - x, Y + y},
            {X + y, Y - x},
            {X + y, Y + x},
            {X - y, Y - x},
            {X - y, Y + x} };
        SDL_RenderDrawPoints(renderer, points, 8);

        if (error <= 0)
        {
            ++y;
            error += ty;
            ty += 2;
        }

        if (error > 0)
        {
            --x;
            tx += 2;
            error += (tx - diameter);
        }
    }
}



void Window::drawRect(SDL_Rect rec)
{
    SDL_RenderDrawRect(renderer, &rec);
}

void Window::drawRectFilled(SDL_Rect rec)
{
    SDL_RenderFillRect(renderer, &rec);
}

SDL_Texture* Window::CreateTextureFromSurface(SDL_Surface* surface) {
    return SDL_CreateTextureFromSurface(this->renderer, surface);
}

SDL_Texture* Window::CreateTextureFromWindow()
{
    auto surface = SDL_GetWindowSurface(window);
    auto texture = SDL_CreateTextureFromSurface(renderer, surface);

    SDL_FreeSurface(surface);

    return texture;
}

void Window::setDrawColor(Uint8 r, Uint8 g, Uint8 b, Uint8 a)
{
    SDL_SetRenderDrawColor(renderer, r, g, b, a);
}

void Window::getDrawColor(Uint8& r, Uint8& g, Uint8& b, Uint8& a)
{
    SDL_GetRenderDrawColor(renderer, &r, &g, &b, &a);
}

void Window::push_color(Uint8 r, Uint8 g, Uint8 b, Uint8 a) {
    SDL_Color color;
    SDL_GetRenderDrawColor(renderer, &color.r, &color.g, &color.b, &color.a);
    colors.push(color);
    SDL_SetRenderDrawColor(renderer, r, g, b, a);
}

void Window::pop_color() {
    SDL_Color color = colors.top();
    colors.pop();
    SDL_SetRenderDrawColor(renderer, color.r, color.g, color.b, color.a);
}

SDL_Rect Window::getInnerRect(SDL_Rect parent, float aspect_ratio) {
    int height, width;
    if ((float)parent.w / parent.h > aspect_ratio) {
        height = std::min(parent.h, int(parent.w / aspect_ratio));
        width = int(height * aspect_ratio);
    }
    else {
        width = std::min(parent.w, int(parent.h * aspect_ratio));
        height = int(width / aspect_ratio);
    }

    SDL_Rect board = { parent.x + (parent.w - width) / 2, parent.y + (parent.h - height) / 2, width, height };

    return board;
}

#endif // WINDOW_IMPL

