/*
 * TextureManager.h
 *
 *  Created on: Apr 8, 2016
 *      Author: gonzalo
 */

#ifndef SINGLETONS_TEXTUREMANAGER_H_
#define SINGLETONS_TEXTUREMANAGER_H_

#include <iostream>
#include <string>
#include <map>
#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>

class TextureManager
{
public:

    static TextureManager* Instance()
    {
        if(s_pInstance == 0)
        {
            s_pInstance = new TextureManager();
            return s_pInstance;
        }
        return s_pInstance;
    }

    //Carga en el map la textura del archivo fileName
    //devuelve true si se cargo con éxito y false si no lo hizo
    bool load(std::string fileName, std::string id, SDL_Renderer* pRenderer);

    // Libera la memoria utilizada por todas las imagenes cargadas al map
    void clearTextureMap();
    // Libera la memoria de la imagen con id pasado como parametro (la saca del map)
    void clearFromTextureMap(std::string id);

    //Funcion para dibujar el objeto en pantalla en forma simple
    void draw(std::string id, int x, int y, int width, int height, double angle, SDL_Renderer* renderer, SDL_RendererFlip flip = SDL_FLIP_NONE);
    //Funcion para dibujar el objeto en pantalla mas completa, tomando en cuenta las variables de animación y transparencia
    void drawFrame(std::string id, int x, int y, int width, int height, int currentRow, int currentFrame, SDL_Renderer* renderer, double angle, int alpha, SDL_RendererFlip flip = SDL_FLIP_NONE);

    std::map<std::string, SDL_Texture*> getTextureMap() { return m_textureMap; }

private:

    std::map<std::string, SDL_Texture*> m_textureMap;

    static TextureManager* s_pInstance;

    TextureManager() {}
    ~TextureManager() {}
    TextureManager(const TextureManager&);
	TextureManager& operator=(const TextureManager&);
};

#endif /* SINGLETONS_TEXTUREMANAGER_H_ */
