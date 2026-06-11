#include <cmath>
#include <unitree/robot/go2/sport/sport_client.hpp>

// BPM 120 → beat interval = 500ms
const int BEAT_US = 500000;

void sectionA_sway(unitree::robot::go2::SportClient& sc) {
    // 8 beats: body sways left-right with roll
    for (int beat = 0; beat < 8; beat++) {
        float roll  =  0.3f * sin(beat * M_PI / 2.0f);
        float pitch =  0.15f * cos(beat * M_PI / 2.0f);
        sc.Euler(roll, pitch, 0.0f);
        sc.BalanceStand();
        usleep(BEAT_US);
    }
}

void sectionB_circle(unitree::robot::go2::SportClient& sc) {
    // 4 beats: circle walk
    for (int beat = 0; beat < 4; beat++) {
        sc.Move(0.2f, 0.0f, 1.0f); // forward + yaw turn
        usleep(BEAT_US);
    }
    sc.StopMove();
}

int main(int argc, char** argv) {
    unitree::robot::ChannelFactory::Instance()->Init(0, argv[1]);
    unitree::robot::go2::SportClient sc;
    sc.SetTimeout(10.0f);
    sc.Init();
    sc.SpeedLevel(1);

    // Intro
    sc.Hello();   sleep(3);
    sc.Stretch(); sleep(3);
    sc.BalanceStand();

    // Dance loop (3 rounds)
    for (int i = 0; i < 3; i++) {
        sectionA_sway(sc);
        sectionB_circle(sc);
        sc.Dance1(); sleep(5);
        sc.Dance2(); sleep(5);
    }

    // Outro
    sc.Euler(0.0f, 0.0f, 0.0f);
    sc.BalanceStand();
    return 0;
}