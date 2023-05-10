//
// Created by kamil-hp on 10.05.23.
//
#include "Event.h"
#include "ImGui/imgui_include.h"

ImGuiKey bf::event::toImGui(bf::event::Key key) {
    int k = static_cast<int>(key);
    if(key>=Key::Zero && key<=Key::Nine) {
        return static_cast<ImGuiKey>(ImGuiKey_0 + k - static_cast<int>(Key::Zero));
    }
    if(key>=Key::A && key<=Key::Z) {
        return static_cast<ImGuiKey>(ImGuiKey_A + k - static_cast<int>(Key::A));
    }
    if(key>=Key::F1 && key<=Key::F12) {
        return static_cast<ImGuiKey>(ImGuiKey_F1 + k - static_cast<int>(Key::F1));
    }
    if(key>=Key::KeyPad0 && key<=Key::KeyPadEqual) {
        return static_cast<ImGuiKey>(ImGuiKey_Keypad0 + k - static_cast<int>(Key::KeyPad0));
    }
    switch(key) {
        case Key::Unknown:
            break;
        case Key::Space: return ImGuiKey_Space;
        case Key::Apostrophe: return ImGuiKey_Apostrophe;
        case Key::Comma: return ImGuiKey_Comma;
        case Key::Minus: return ImGuiKey_Minus;
        case Key::Period: return ImGuiKey_Period;
        case Key::Slash: return ImGuiKey_Slash;
        case Key::Semicolon: return ImGuiKey_Semicolon;
        case Key::Equal: return ImGuiKey_Equal;
        case Key::LeftBracket: return ImGuiKey_LeftBracket;
        case Key::Backslash: return ImGuiKey_Backslash;
        case Key::RightBracket: return ImGuiKey_RightBracket;
        case Key::GraveAccent: return ImGuiKey_GraveAccent;
        case Key::Escape: return ImGuiKey_Escape;
        case Key::Enter: return ImGuiKey_Enter;
        case Key::Tab: return ImGuiKey_Tab;
        case Key::Backspace: return ImGuiKey_Backspace;
        case Key::Insert: return ImGuiKey_Insert;
        case Key::Delete: return ImGuiKey_Delete;
        case Key::Right: return ImGuiKey_RightArrow;
        case Key::Left: return ImGuiKey_LeftArrow;
        case Key::Down: return ImGuiKey_DownArrow;
        case Key::Up: return ImGuiKey_UpArrow;
        case Key::PageUp: return ImGuiKey_PageUp;
        case Key::PageDown: return ImGuiKey_PageDown;
        case Key::Home: return ImGuiKey_Home;
        case Key::End: return ImGuiKey_End;
        case Key::CapsLock: return ImGuiKey_CapsLock;
        case Key::ScrollLock: return ImGuiKey_ScrollLock;
        case Key::NumLock: return ImGuiKey_NumLock;
        case Key::PrintScreen: return ImGuiKey_PrintScreen;
        case Key::Pause: return ImGuiKey_Pause;
        //F13 - F25, World1, World2 not supported
        case Key::LeftShift: return ImGuiKey_LeftShift;
        case Key::LeftControl: return ImGuiKey_LeftCtrl;
        case Key::LeftAlt: return ImGuiKey_LeftAlt;
        case Key::LeftSuper: return ImGuiKey_LeftSuper;
        case Key::RightShift: return ImGuiKey_RightShift;
        case Key::RightControl: return ImGuiKey_RightCtrl;
        case Key::RightAlt: return ImGuiKey_RightAlt;
        case Key::RightSuper: return ImGuiKey_RightSuper;
        case Key::Menu: return ImGuiKey_Menu;
        default: break;
    }
    return ImGuiKey_None;
}

int bf::event::toImGui(bf::event::MouseButton button) {
    switch(button) {
        case MouseButton::Left: return ImGuiMouseButton_Left;
        case MouseButton::Right: return ImGuiMouseButton_Right;
        case MouseButton::Middle: return ImGuiMouseButton_Middle;
        //not supported
        case MouseButton::Four:
        case MouseButton::Five:
        case MouseButton::Six:
        case MouseButton::Seven:
        case MouseButton::Eight: return ImGuiMouseButton_COUNT;
    }
    return ImGuiMouseButton_COUNT;
}

int bf::event::toImGui(bf::event::ModifierKeyBit bit) {
    int ret=0;
    if(bit&ModShift) ret|=ImGuiMod_Shift;
    if(bit&ModControl) ret|=ImGuiMod_Ctrl;
    if(bit&ModAlt) ret|=ImGuiMod_Alt;
    if(bit&ModSuper) ret|=ImGuiMod_Super;
    //ModCapsLock, ModNumLock not supported in ImGui
    return ret;
}

bool bf::event::toImGui(bf::event::MouseButtonState state) {return state==MouseButtonState::Press;}

bool bf::event::toImGui(bf::event::KeyState state) {return state!=KeyState::Release;}
