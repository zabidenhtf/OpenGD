/* Copyright (C) 2025-2026 Mykyta Polishyk */
/* This project is licensed under the GNU General Public License v3.0 or later. */
/* See the LICENSE file for details. */

#include "utils/system.hpp"
#include "utils/graphics.hpp"
#include "utils/audio.hpp"
#include "utils/data.hpp"
#include "utils/input.hpp"
#include "utils/config.hpp"
#include "version.hpp"

#include "includes.hpp"

// Target fps and another configs
const float FPS = 60.00f;
const float frame_time = 1.0 / FPS;
double last_time = Graphics->GetTime();

// Cores global objects (Now nothing)

int main(){
    Console.Write("Launched OpenGD " + string(GAME_MILESTONE));
    // Initializating systems
    Config = new ConfigureSystem();
    Data = new DataSystem();
    Graphics = new GraphicsSystem();
    Audio = new AudioSystem();
    Input::Init(); // TODO: make it objective

    // On some time abonded
    // Fonts
    //data::fonts::font_paths.push_back("assets/fonts/eurostile_roman.ttf");

    // Loading all stuff
    Data->LoadEverything();

    while(!Graphics->ShouldWindowClose()){
        Graphics->ClearScreen(vec3(0,0,0));
        double frame_start = Graphics->GetTime();

        double delta = frame_start - last_time;
        last_time = frame_start;

        Input::Clear();
        Graphics->PollEvents();

        double frame_end = Graphics->GetTime();
        double time_taken = frame_end - frame_start;

        if (time_taken < frame_time) {
            // fps limiter
            this_thread::sleep_for(
                chrono::duration<double>(frame_time - time_taken)
            );
        }
    }
    return 0;
}
