#pragma once
#include <queue>
#include <bitset>
class KeyboardInput
{
    // Let the Window Class get and use the Properties and Methods (private, protected) of this class
    friend class Window;
public:
    class Event
    {
    public:
        enum struct Type
        {
            Press,
            Release,
            Invalid
        };
        Event();
        Event(Type type, unsigned char code);
        bool IsPressed() const;
        bool IsReleased() const;
        bool IsValid() const;
        unsigned char GetCode() const;
    private:
        Type type;
        unsigned char code;
    };
public:
    KeyboardInput() = default;
    KeyboardInput(const KeyboardInput&) = delete;
    KeyboardInput& operator=(const KeyboardInput&) = delete;
    // Key event
    // Check if a specific key is pressed
    bool IsKeyPressed(unsigned char keycode) const;
    // Pull an event off the event queue
    Event ReadKey();
    // Check if any event in the event queue
    bool IsKeyEmpty() const;
    // Clear the queue
    void ClearKey();

    // Char event
    char ReadChar();
    bool IsCharEmpty() const;
    void ClearChar();
    void ClearAll();

    // Autorepeat control
    void EnableAutoRepeat();
    void DisableAutoRepeat();
    bool IsAutoRepeatEnabled() const;

private:
    // These 3 functions are used by Window Class
    void OnKeyPressed(unsigned char keycode);
    void OnKeyReleased(unsigned char keycode);
    void OnChar(char character);
    void ClearState();
    template <typename T>
    static void TrimBuffer(std::queue<T>& buffer);
private:
    static const unsigned int nKeys = 256;
    static const unsigned int bufferSize = 16;
    bool autoRepeatEnabled = false;
    std::bitset<nKeys> keyStates;
    std::queue<Event> keyBuffer;
    std::queue<char> charBuffer;
};