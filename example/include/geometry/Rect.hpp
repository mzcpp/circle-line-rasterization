#ifndef RECT_HPP
#define RECT_HPP

#include "Point.hpp"

#include <iostream>
#include <type_traits>
#include <iomanip>
#include <cmath>

template <typename T>
class Rect
{
    static_assert(std::is_arithmetic_v<T>);

public:
    Point<T> top_left_;
    T width_;
    T height_;

    Rect(T x = 0, T y = 0, T width = 0, T height = 0) noexcept : width_(width), height_(height) 
    {
        top_left_.x_ = x;
        top_left_.y_ = y;
    }

    Point<T> GetTopLeft() const
    {
        return top_left_;
    }

    Point<T> GetTopRight() const
    {
        return { top_left_.x_ + width_, top_left_.y_ };
    }

    Point<T> GetBottomRight() const
    {
        return { top_left_.x_ + width_, top_left_.y_ + height_ };
    }

    Point<T> GetBottomLeft() const
    {
        return { top_left_.x_, top_left_.y_ + height_ };
    }

    bool Contains(const Point<T>& point) const
    {
        return point.x_ >= top_left_.x_ && point.y_ >= top_left_.y_ && point.x_ < GetBottomRight().x_ && point.y_ < GetBottomRight().y_;
    }

    bool Contains(const Rect<T>& rect) const
    {
        return rect.top_left_.x_ >= top_left_.x_ && rect.top_left_.y_ >= top_left_.y_ && 
            rect.GetBottomRight().x_ < GetBottomRight().x_ && rect.GetBottomRight().y_ < GetBottomRight().y_;
    }

    bool Intersects(const Rect<T>& rect) const
    {
        return top_left_.x_ < rect.top_left_.x_ + rect.width_ && top_left_.x_ + width_ >= rect.top_left_.x_ && 
            top_left_.y_ < rect.top_left_.y_ + rect.height_ && top_left_.y_ + height_ >= rect.top_left_.y_;
    }

    void MoveTo(const Point<T>& destination)
    {
        top_left_.x_ = destination.x_ - (width_ / 2.0);
        top_left_.y_ = destination.y_ - (height_ / 2.0);
    }

    friend std::ostream& operator<<(std::ostream& os, const Rect<T>& rect)
    {
        os << "Top Left: " << std::setw(1) << rect.top_left_;
        os << "Width: " << std::setw(8) << rect.width_ << '\n';
        os << "Height: " << std::setw(7) << rect.height_ << '\n';
        return os;
    }
};

#endif
