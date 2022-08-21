#include "../include/yds_key_maps.h"

#include <SDL2/SDL.h>

ysKey::Code *ysKeyMaps::m_sdlKeyMap;

const ysKey::Code *ysKeyMaps::GetSdlKeyMap() {
    if (m_sdlKeyMap == NULL) {
        m_sdlKeyMap = new ysKey::Code[SDL_NUM_SCANCODES];
        for (int i = 0; i < SDL_NUM_SCANCODES; ++i) m_sdlKeyMap[i] = ysKey::Code::Undefined;

        m_sdlKeyMap[SDL_SCANCODE_A] = ysKey::Code::A;
        m_sdlKeyMap[SDL_SCANCODE_B] = ysKey::Code::B;
        m_sdlKeyMap[SDL_SCANCODE_C] = ysKey::Code::C;
        m_sdlKeyMap[SDL_SCANCODE_D] = ysKey::Code::D;
        m_sdlKeyMap[SDL_SCANCODE_E] = ysKey::Code::E;
        m_sdlKeyMap[SDL_SCANCODE_F] = ysKey::Code::F;
        m_sdlKeyMap[SDL_SCANCODE_G] = ysKey::Code::G;
        m_sdlKeyMap[SDL_SCANCODE_H] = ysKey::Code::H;
        m_sdlKeyMap[SDL_SCANCODE_I] = ysKey::Code::I;
        m_sdlKeyMap[SDL_SCANCODE_J] = ysKey::Code::J;
        m_sdlKeyMap[SDL_SCANCODE_K] = ysKey::Code::K;
        m_sdlKeyMap[SDL_SCANCODE_L] = ysKey::Code::L;
        m_sdlKeyMap[SDL_SCANCODE_M] = ysKey::Code::M;
        m_sdlKeyMap[SDL_SCANCODE_N] = ysKey::Code::N;
        m_sdlKeyMap[SDL_SCANCODE_O] = ysKey::Code::O;
        m_sdlKeyMap[SDL_SCANCODE_P] = ysKey::Code::P;
        m_sdlKeyMap[SDL_SCANCODE_Q] = ysKey::Code::Q;
        m_sdlKeyMap[SDL_SCANCODE_R] = ysKey::Code::R;
        m_sdlKeyMap[SDL_SCANCODE_S] = ysKey::Code::S;
        m_sdlKeyMap[SDL_SCANCODE_T] = ysKey::Code::T;
        m_sdlKeyMap[SDL_SCANCODE_U] = ysKey::Code::U;
        m_sdlKeyMap[SDL_SCANCODE_V] = ysKey::Code::V;
        m_sdlKeyMap[SDL_SCANCODE_W] = ysKey::Code::W;
        m_sdlKeyMap[SDL_SCANCODE_X] = ysKey::Code::X;
        m_sdlKeyMap[SDL_SCANCODE_Y] = ysKey::Code::Y;
        m_sdlKeyMap[SDL_SCANCODE_Z] = ysKey::Code::Z;

        m_sdlKeyMap[SDL_SCANCODE_KP_1] = ysKey::Code::N1;
        m_sdlKeyMap[SDL_SCANCODE_KP_2] = ysKey::Code::N2;
        m_sdlKeyMap[SDL_SCANCODE_KP_3] = ysKey::Code::N3;
        m_sdlKeyMap[SDL_SCANCODE_KP_4] = ysKey::Code::N4;
        m_sdlKeyMap[SDL_SCANCODE_KP_5] = ysKey::Code::N5;

        m_sdlKeyMap[SDL_SCANCODE_UP] = ysKey::Code::Up;
        m_sdlKeyMap[SDL_SCANCODE_DOWN] = ysKey::Code::Down;
        m_sdlKeyMap[SDL_SCANCODE_LEFT] = ysKey::Code::Left;
        m_sdlKeyMap[SDL_SCANCODE_RIGHT] = ysKey::Code::Right;

        m_sdlKeyMap[SDL_SCANCODE_TAB] = ysKey::Code::Tab;
        m_sdlKeyMap[SDL_SCANCODE_ESCAPE] = ysKey::Code::Escape;
        m_sdlKeyMap[SDL_SCANCODE_SPACE] = ysKey::Code::Space;

        m_sdlKeyMap[SDL_SCANCODE_LSHIFT] = ysKey::Code::Shift;
        m_sdlKeyMap[SDL_SCANCODE_RSHIFT] = ysKey::Code::Shift;

        m_sdlKeyMap[SDL_SCANCODE_COMMA] = ysKey::Code::OEM_Comma;
    }

    return m_sdlKeyMap;
}
