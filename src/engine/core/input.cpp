#include "input.h"

Input& Input::get() {
    static Input instance;
    return instance;
}

// returns true once when the key is pressed
bool Input::didKeyClick(KeyCode key) {
    if (keys[int(key)] == KeyState::Click) {
        keys[int(key)] = KeyState::Hold;
        return true;
    }

    return false;
}

// returns true while a key is pressed
bool Input::isKeyDown(KeyCode key) {
    KeyState state = keys[int(key)];
    return state == KeyState::Click || state == KeyState::Hold;
}

float Input::cursorX() const {
    return m_cursorX;
}

float Input::cursorY() const {
    return m_cursorY;
}

// !!! should be called only by Window !!!
void Input::setWindowDimensions(float width, float height) {
    m_windowWidth = width;
    m_windowHeight = height;
}

// !!! should be called only by Window !!!
void Input::setKeyState(int key, KeyState state) {
    keys[key] = state;
}

// !!! should be called only by Window !!!
void Input::setMouseButtonState(int button, KeyState state) {
    keys[button] = state;
}

// !!! should be called only by Window !!!
void Input::setCursorPos(float x, float y) {
    m_cursorX = x;
    m_cursorY = m_windowHeight - y;
}

Input::Input() {
    keys.fill(KeyState::Release);
}