namespace tune
{
    #pragma macro(push)
    #define pause(x) { 1, 1, x }

    using namespace scale;

    // clang-format off

    song::note const charge[] = {
        { G5, G5, 140 },
        { C6, C6, 140 },
        { E6, E6, 140 },
        { G6, G6, 280 },
        { E6, E6, 120 },
        { G6, G6, 500 }
    };

    song::note const new_game[] = {
        { 2000, 1500, 150 },
        { 1, 1, 20 }, 
        { 1000, 800, 125 },
        { 1, 1, 20 }, 
        { 600, 400, 100 }
    };

    song::note const waiting_to_start[] = {
        { C4, C4, 60 },
        pause(500),
        { A4, A4, 60 },
        pause(500)
    };
    
    song::note const snap[] = {
        { E7, D_SHARP_7, 20 },
        { C7, C_SHARP_7, 30 },
        pause(10)
    };
    
    song::note const winner[] = {
        pause(50),
        { E6, E6, 120 },
        { G6, G6, 300 },
        pause(20),
        { E6, E6, 120 },
        { G6, G6, 500 }
    };
    
    song::note const loser[] = {
        { C5, C5, 500 },
        pause(20),
        { B3, LOW, 1000 },
        pause(300)
    };
    
    song::note const game_over[] = { 
        pause(30),
        { C6, C6, 100 },
        pause(20),
        { A5, A5, 200 },
        pause(20),
        { A5, A5, 100 },
        pause(20),
        { A5, A5, 60 },
        pause(20),
        { G5, G5, 60 },
        pause(20),
        { A5, A5, 150 },
        pause(20),
        { B5, B5, 400 },
        pause(20),
        { A5, A5, 400 }
    };

    // clang-format on

    #pragma macro pop
};

