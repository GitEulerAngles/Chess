#include "basic.h"
//Vector2

SDL_Texture* bas::texture::loadTexture(std::string path, SDL_Renderer* r) {
    //The final texture
    SDL_Texture* newTexture = NULL;

    //Load image at specified path
    SDL_Surface* loadedSurface = IMG_Load(path.c_str());
    if (loadedSurface == NULL) {
        printf("Unable to load image %s! SDL_image Error: %s\n", path.c_str(), IMG_GetError());
        return newTexture;
    }
    //Create texture from surface pixels
    newTexture = SDL_CreateTextureFromSurface(r, loadedSurface);
    if (newTexture == NULL)
        printf("Unable to create texture from %s! SDL Error: %s\n", path.c_str(), SDL_GetError());

    //Get rid of old loaded surface
    SDL_FreeSurface(loadedSurface);

    return newTexture;
}
void bas::texture::loadMedia(SDL_Renderer* r, std::string path) {
    //Load PNG texture
    gTexture = loadTexture(path, r);
    if (gTexture == NULL)
    {
        printf("Failed to load texture image!\n");
    }
}
void bas::texture::drawTexture(SDL_Renderer* r) {
    SDL_RenderCopy(r, gTexture, &imageCoords, &screenCoords);
}
