#include "KeyboardInput.h"
#include "MouseInput.h"

bool KeyboardInput::IsKeyPressed(unsigned char keycode) const
{
    return keyStates[keycode];
}

KeyboardInput::Event KeyboardInput::ReadKey()
{
    if (keyBuffer.size() > 0)
    {
        Event e = keyBuffer.front();
        keyBuffer.pop();
        return e;
    }
    return Event();
}

bool KeyboardInput::IsKeyEmpty() const
{
    return keyBuffer.empty();
}

void KeyboardInput::ClearKey()
{
    keyBuffer = std::queue<Event>();
}

char KeyboardInput::ReadChar()
{
    if (charBuffer.size() > 0)
    {
        unsigned char charCode = charBuffer.front();
        charBuffer.pop();
        return charCode;
    }
    return 0;
}

bool KeyboardInput::IsCharEmpty() const
{
    return charBuffer.empty();
}

void KeyboardInput::ClearChar()
{
    charBuffer = std::queue<char>();
}

void KeyboardInput::ClearAll()
{
    ClearKey();
    ClearChar();
}

void KeyboardInput::EnableAutoRepeat()
{
    autoRepeatEnabled = true;
}

void KeyboardInput::DisableAutoRepeat()
{
    autoRepeatEnabled = false;
}

bool KeyboardInput::IsAutoRepeatEnabled() const
{
    return autoRepeatEnabled;
}

void KeyboardInput::OnKeyPressed(unsigned char keycode)
{
    keyStates[keycode] = true;
    Event newKeycodeEvent = Event(KeyboardInput::Event::Type::Press, keycode);
    keyBuffer.push(newKeycodeEvent);
    TrimBuffer(keyBuffer);
}

void KeyboardInput::OnKeyReleased(unsigned char keycode)
{
    keyStates[keycode] = false;
    Event newKeycodeEvent = Event(KeyboardInput::Event::Type::Release, keycode);
    keyBuffer.push(newKeycodeEvent);
    TrimBuffer(keyBuffer);
}

void KeyboardInput::OnChar(char character)
{
    charBuffer.push(character);
    TrimBuffer(charBuffer);
}

void KeyboardInput::ClearState()
{
    keyStates.reset();
}

template<typename T>
void KeyboardInput::TrimBuffer(std::queue<T>& buffer)
{
    while (buffer.size() > bufferSize)
    {
        buffer.pop();
    }
}

KeyboardInput::Event::Event()
{
    type = Type::Invalid;
    code = 0;
}

KeyboardInput::Event::Event(Type type, unsigned char code)
{
    this->type = type;
    this->code = code;
}

bool KeyboardInput::Event::IsPressed() const
{
    return type == Type::Press;
}

bool KeyboardInput::Event::IsReleased() const
{
    return type == Type::Release;
}

bool KeyboardInput::Event::IsValid() const
{
    return type != Type::Invalid;
}

unsigned char KeyboardInput::Event::GetCode() const
{
    return code;
}