#include "UserInput.h"

bool UserInput::IsKeyPressed(unsigned char keycode) const
{
    return keyStates[keycode];
}

UserInput::Event UserInput::ReadKey()
{
    if (keyBuffer.size() > 0)
    {
        Event e = keyBuffer.front();
        keyBuffer.pop();
        return e;
    }
    return Event();
}

bool UserInput::IsKeyEmpty() const
{
    return keyBuffer.empty();
}

void UserInput::ClearKey()
{
    keyBuffer = std::queue<Event>();
}

char UserInput::ReadChar()
{
    if (charBuffer.size() > 0)
    {
        unsigned char charCode = charBuffer.front();
        charBuffer.pop();
        return charCode;
    }
    return 0;
}

bool UserInput::IsCharEmpty() const
{
    return charBuffer.empty();
}

void UserInput::ClearChar()
{
    charBuffer = std::queue<char>();
}

void UserInput::ClearAll()
{
    ClearKey();
    ClearChar();
}

void UserInput::EnableAutoRepeat()
{
    autoRepeatEnabled = true;
}

void UserInput::DisableAutoRepeat()
{
    autoRepeatEnabled = false;
}

bool UserInput::IsAutoRepeatEnabled() const
{
    return autoRepeatEnabled;
}

void UserInput::OnKeyPressed(unsigned char keycode)
{
    keyStates[keycode] = true;
    Event newKeycodeEvent = Event(UserInput::Event::Type::Press, keycode);
    keyBuffer.push(newKeycodeEvent);
    TrimBuffer(keyBuffer);
}

void UserInput::OnKeyReleased(unsigned char keycode)
{
    keyStates[keycode] = false;
    Event newKeycodeEvent = Event(UserInput::Event::Type::Release, keycode);
    keyBuffer.push(newKeycodeEvent);
    TrimBuffer(keyBuffer);
}

void UserInput::OnChar(char character)
{
    charBuffer.push(character);
    TrimBuffer(charBuffer);
}

void UserInput::ClearState()
{
    keyStates.reset();
}

template<typename T>
void UserInput::TrimBuffer(std::queue<T>& buffer)
{
    while (buffer.size() > bufferSize)
    {
        buffer.pop();
    }
}

UserInput::Event::Event()
{
    type = Type::Invalid;
    code = 0;
}

UserInput::Event::Event(Type type, unsigned char code)
{
    this->type = type;
    this->code = code;
}

bool UserInput::Event::IsPressed() const
{
    return type == Type::Press;
}

bool UserInput::Event::IsReleased() const
{
    return type == Type::Release;
}

bool UserInput::Event::IsValid() const
{
    return type != Type::Invalid;
}

unsigned char UserInput::Event::GetCode() const
{
    return code;
}


