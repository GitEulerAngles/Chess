#ifndef BASIC
#define BASIC

#include <cstdint>
#include <chrono>
#include <iostream>
#include "SDL_image.h"
#include "SDL_ttf.h"

namespace bas {
    //Vector2
    template<typename type_v2>
    struct vector2 {
        type_v2 x;
        type_v2 y;
        vector2(type_v2 Ix, type_v2 Iy) {
            x = Ix;
            y = Iy;
        }
        vector2() {
            x = 0;
            y = 0;
        }
    };
    template<typename type_v2> inline bas::vector2 <type_v2> addVectors2(bas::vector2 <type_v2> v1, bas::vector2 <type_v2> v2) {
        return bas::vector2<type_v2>(v1.x + v2.x, v1.y + v2.y);
    }
    template<typename type_v2> inline bas::vector2 <type_v2> subVectors2(bas::vector2 <type_v2> v1, bas::vector2 <type_v2> v2) {
        return bas::vector2<type_v2>(v1.x - v2.x, v1.y - v2.y);
    }
    template<typename type_v2> inline bas::vector2 <type_v2> mulVectors2(bas::vector2 <type_v2> v1, bas::vector2 <type_v2> v2) {
        return bas::vector2<type_v2>(v1.x * v2.x, v1.y * v2.y);
    }
    template<typename type_v2> inline bas::vector2 <type_v2> divVectors2(bas::vector2 <type_v2> v1, bas::vector2 <type_v2> v2) {
        return bas::vector2<type_v2>(v1.x / v2.x, v1.y / v2.y);
    }

    //Vector3
    template<typename type_v3>
    struct vector3 {
        type_v3 x;
        type_v3 y;
        type_v3 z;
        vector3(type_v3 Ix, type_v3 Iy, type_v3 Iz) {
            x = Ix;
            y = Iy;
            z = Iz;
        }
        vector3() {
            x = 0;
            y = 0;
            z = 0;
        }
    };
    template<typename type_v3> inline bas::vector2 <type_v3> addVectors3(bas::vector3 <type_v3> v1, bas::vector3 <type_v3> v2) {
        return bas::vector3<type_v3>(v1.x + v2.x, v1.y + v2.y, v1.z + v2.z);
    }
    template<typename type_v3> inline bas::vector2 <type_v3> subVectors3(bas::vector3 <type_v3> v1, bas::vector3 <type_v3> v2) {
        return bas::vector3<type_v3>(v1.x - v2.x, v1.y - v2.y, v1.z - v2.z);
    }
    template<typename type_v3> inline bas::vector2 <type_v3> mulVectors3(bas::vector3 <type_v3> v1, bas::vector3 <type_v3> v2) {
        return bas::vector3<type_v3>(v1.x * v2.x, v1.y * v2.y, v1.z * v2.z);
    }
    template<typename type_v3> inline bas::vector2 <type_v3> divVectors3(bas::vector3 <type_v3> v1, bas::vector3 <type_v3> v2) {
        return bas::vector3<type_v3>(v1.x / v2.x, v1.y / v2.y, v1.z / v2.z);
    }

    //Line
    template<typename type_l>
    struct line {
        bas::vector2 <type_l> p1;
        bas::vector2 <type_l> p2;

        line(type_l p1xI, type_l p1yI, type_l p2xI, type_l p2yI) {
            p1.x = p1xI;
            p1.y = p1yI;
            p2.x = p2xI;
            p2.y = p2yI;
        }
        line(bas::vector2 <type_l> p1I, bas::vector2 <type_l> p2I) {
            p1 = p1I;
            p2 = p2I;
        }
        line() {};
    };

    //Textures
    struct timer {
        std::chrono::time_point<std::chrono::steady_clock> start, end;
        std::chrono::duration<float> duration;

        void restartTime() {
            start = std::chrono::high_resolution_clock::now();
        }

        timer() {
            restartTime();
        }

        float getTime() {
            end = std::chrono::high_resolution_clock::now();

            duration = end - start;

            return duration.count()*1000;
        }

        float getFPS() {
            return 1/(getTime() / 1000);
        }

        float getFPS(float Time) {
            return 1/(Time / 1000);
        }
    };
    class  text {
    public:
        SDL_Color Color;
        TTF_Font* Font;

        void drawText(SDL_Renderer* r, std::string message, SDL_Rect rect) {
            surfaceMessage = TTF_RenderText_Solid(Font, message.c_str(), Color);
            Message = SDL_CreateTextureFromSurface(r, surfaceMessage);
            SDL_RenderCopy(r, Message, NULL, &rect);
            SDL_FreeSurface(surfaceMessage);
            SDL_DestroyTexture(Message);
        }
        ~text() {
            TTF_CloseFont(Font);
        }
    private:
        SDL_Surface* surfaceMessage;
        SDL_Texture* Message;
    };
    class  texture {
    public:
        SDL_Texture* gTexture;
        SDL_Rect     imageCoords;
        SDL_Rect     screenCoords;
        void setImageCoords(SDL_Rect rect) {
            imageCoords = rect;
        }
        void setScreenCoords(SDL_Rect rect) {
            screenCoords = rect;
        }
        void loadMedia(SDL_Renderer* r, std::string path);
        void drawTexture(SDL_Renderer* r);
    protected:
        SDL_Texture* loadTexture(std::string path, SDL_Renderer* r);
    };
};

#endif //BASIC
