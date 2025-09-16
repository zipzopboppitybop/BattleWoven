#pragma once

#include <SFML/Graphics.hpp>
#include "Vec2.hpp"

class Animation {
    sf::Sprite mSprite;
    Vec2f mPos;
    Vec2f mFrameSize;
    int mFrameCount;
    float mFrameTime;
    const std::string mName;

public:
    Animation() = default;

    Animation(const std::string& animationName, const sf::Texture& texture, Vec2f pos, Vec2f size, int frameCount, float frameTime)
        : mName(animationName), mSprite(texture), mPos(pos), mFrameSize(size), mFrameCount(frameCount), mFrameTime(frameTime)
    {
        mSprite.setTextureRect(sf::IntRect({ (int)mPos.x, (int)mPos.y }, { (int)mFrameSize.x, (int)mFrameSize.y }));
    }

    Vec2f frameSize() const
    {
        return mFrameSize;
    }

    int frameCount() const
    {
        return mFrameCount;
    }

    float frameTime() const
    {
        return mFrameTime;
    }

    void setFrame(int index) {
        if (index < 0 || index >= mFrameCount) return;
        int left = (int)mPos.x + index * (int)mFrameSize.x;
        int top = (int)mPos.y;
        mSprite.setTextureRect(sf::IntRect({ left, top }, { (int)mFrameSize.x, (int)mFrameSize.y }));
    }

    sf::Sprite& sprite()
    {
        return mSprite;
    }

    const std::string& name() const
    {
        return mName;
    }
};