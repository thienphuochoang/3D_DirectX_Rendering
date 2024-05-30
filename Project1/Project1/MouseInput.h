#pragma once
#include <queue>
class MouseInput
{
    friend class Window;
public:
    class Event
    {
    public:
        enum struct Type
        {
            LeftPress,
            RightPress,
            LeftRelease,
            RightRelease,
            WheelUp,
            WheelDown,
            Move,
            Invalid
        };
        Event();
        Event(Type type, const MouseInput& mouseInput);
        bool IsValid() const;
        Type GetType() const;
        std::pair<int, int> GetPos() const;
        int GetPosX() const;
        int GetPosY() const;
        bool IsLeftMousePressed() const;
        bool IsRightMousePressed() const;
    private:
        Type type;
        bool leftMousePressed;
        bool rightMousePressed;
        int x;
        int y;
    };
public:
    MouseInput() = default;
    MouseInput(const MouseInput&) = delete;
    MouseInput& operator=(const MouseInput&) = delete;
    std::pair<int, int> GetPos() const;
    int GetPosX() const;
    int GetPosY() const;
    bool IsLeftMousePressed() const;
    bool IsRightMousePressed() const;
    MouseInput::Event Read();
    bool IsEmpty();
    void Clear();
private:
    static const unsigned int bufferSize = 16;
    int x;
    int y;
    bool leftMousePressed = false;
    bool rightMousePressed = false;
    std::queue<Event> buffer;
    void OnMouseMove(int x, int y);
    void OnLeftMousePressed(int x, int y);
    void OnLeftMouseReleased(int x, int y);
    void OnRightMousePressed(int x, int y);
    void OnRightMouseReleased(int x, int y);
    void OnMouseWheelUp(int x, int y);
    void OnMouseWheelDown(int x, int y);
    void TrimBuffer();
};

