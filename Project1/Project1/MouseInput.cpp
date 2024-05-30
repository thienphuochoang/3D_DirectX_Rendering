#include "MouseInput.h"

MouseInput::Event::Event()
{
    type = Type::Invalid;
    leftMousePressed = false;
    rightMousePressed = false;
    x = 0;
    y = 0;
}

MouseInput::Event::Event(Type type, const MouseInput& userInput)
{
    this->type = type;
    this->leftMousePressed = userInput.leftMousePressed;
    this->rightMousePressed = userInput.rightMousePressed;
    this->x = userInput.x;
    this->y = userInput.y;
}

MouseInput::Event::Type MouseInput::Event::GetType() const
{
    return type;
}

std::pair<int, int> MouseInput::Event::GetPos() const
{
    return { x, y };
}

int MouseInput::Event::GetPosX() const
{
    return x;
}

int MouseInput::Event::GetPosY() const
{
    return y;
}

bool MouseInput::Event::IsLeftMousePressed() const
{
    return leftMousePressed;
}

bool MouseInput::Event::IsRightMousePressed() const
{
    return rightMousePressed;
}

bool MouseInput::IsEmpty()
{
    return buffer.empty();
}

void MouseInput::Clear()
{
    buffer = std::queue<Event>();
}

std::pair<int, int> MouseInput::GetPos() const
{
    return { x,y };
}

int MouseInput::GetPosX() const
{
    return x;
}

int MouseInput::GetPosY() const
{
    return y;
}

bool MouseInput::IsLeftMousePressed() const
{
    return leftMousePressed;
}

bool MouseInput::IsRightMousePressed() const
{
    return rightMousePressed;
}

MouseInput::Event MouseInput::Read()
{
    if (buffer.size() > 0)
    {
        MouseInput::Event e = buffer.front();
        buffer.pop();
        return e;
    }
    return MouseInput::Event();
}

void MouseInput::OnMouseMove(int x, int y)
{
    this->x = x;
    this->y = y;
    buffer.push(MouseInput::Event(MouseInput::Event::Type::Move, *this));
    TrimBuffer();
}

void MouseInput::OnLeftMousePressed(int x, int y)
{
    leftMousePressed = true;
    buffer.push(MouseInput::Event(MouseInput::Event::Type::LeftPress, *this));
    TrimBuffer();
}

void MouseInput::OnLeftMouseReleased(int x, int y)
{
    leftMousePressed = false;
    buffer.push(MouseInput::Event(MouseInput::Event::Type::LeftRelease, *this));
    TrimBuffer();
}

void MouseInput::OnRightMousePressed(int x, int y)
{
    rightMousePressed = true;
    buffer.push(MouseInput::Event(MouseInput::Event::Type::RightPress, *this));
    TrimBuffer();
}

void MouseInput::OnRightMouseReleased(int x, int y)
{
    rightMousePressed = false;
    buffer.push(MouseInput::Event(MouseInput::Event::Type::RightRelease, *this));
    TrimBuffer();
}

void MouseInput::OnMouseWheelUp(int x, int y)
{
    buffer.push(MouseInput::Event(MouseInput::Event::Type::WheelUp, *this));
    TrimBuffer();
}

void MouseInput::OnMouseWheelDown(int x, int y)
{
    buffer.push(MouseInput::Event(MouseInput::Event::Type::WheelDown, *this));
    TrimBuffer();
}

void MouseInput::TrimBuffer()
{
    while (buffer.size() > bufferSize)
    {
        buffer.pop();
    }
}
