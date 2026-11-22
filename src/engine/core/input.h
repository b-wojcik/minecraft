#pragma once

#include <array>
#include "keycode.h"

enum class KeyState : int {
    Release = 0,
    Click = 1,
    Hold = 2
};

class Input {
public:
    static Input& get();

    // returns true once when the key is pressed
    bool didKeyClick(KeyCode key);

    // returns true while a key is pressed
    bool isKeyDown(KeyCode key);

    float cursorX() const;

    float cursorY() const;

    // !!! should be called only by Window !!!
    void setWindowDimensions(float width, float height);

    // !!! should be called only by Window !!!
    void setKeyState(int key, KeyState state);

    // !!! should be called only by Window !!!
    void setMouseButtonState(int button, KeyState state);

    // !!! should be called only by Window !!!
    void setCursorPos(float x, float y);

    // delete copy and move constructors
    Input(const Input&) = delete;
    Input(Input&&) = delete;
private:
    // GLFW constans for mouse buttons and keyboard keys do not overlap
    std::array<KeyState, GLFW_KEY_LAST + 1> keys;

    // window size
    float m_windowWidth = 0.0f;
    float m_windowHeight = 0.0f;

    // cursor position
    float m_cursorX = 0.0f;
    float m_cursorY = 0.0f;

    Input();

    ~Input() = default;
};