#pragma once

#include <SFML/Graphics.hpp>
#include "Vec2.hpp"

class Animation {
    sf::Sprite mSprite;  
    Vec2f mPos;
    Vec2f mFrameSize;   
    int mFrameCount;           
    int mCurrentFrame = 0;     
    float mFrameTime;          
    float mElapsed = 0.f;    
    const std::string mName;

public:
    Animation(const std::string& animationName, const sf::Texture& texture, Vec2f pos, Vec2f size, int frameCount, float frameTime)
        : mName(animationName), mSprite(texture), mPos(pos), mFrameSize(size), mFrameCount(frameCount), mFrameTime(frameTime)
    {
        mSprite.setTexture(texture);
        mSprite.setTextureRect(sf::IntRect({ (int)mPos.x, (int)mPos.y }, { (int)mFrameSize.x, (int)mFrameSize.y }));
    }

    const std::string& name()
    {
        return mName;
    }
};