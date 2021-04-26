namespace tune
{
    using namespace scale;

    // clang-format off

    song::note const charge[6] = {
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
        { 1, 1, 500 },
        { A4, A4, 60 },
        { 1, 1, 500 }
    };
    
    song::note const snap[] = {
        { E7, D_SHARP_7, 20 },
        { C7, C_SHARP_7, 30 },
        { 1, 1, 10 }
    };
    
    song::note const loser[] = {
        { C5, C5, 200 },
        { 1, 1, 20 },
        { B3, B3, 500 }
    };
    
    song::note const winner[] = { 
        { C6, C6, 200 },
        { 1, 1, 40 },
        { A5, A5, 200 },
        { 1, 1, 40 },
        { A5, A5, 150 },
        { 1, 1, 40 },
        { A5, A5, 100 },
        { 1, 1, 40 },
        { G5, G5, 100 },
        { 1, 1, 40 },
        { A5, A5, 150 },
        { 1, 1, 40 },
        { B5, B5, 400 },
        { 1, 1, 40 },
        { A5, A5, 400 }
    };

    // clang-format on
};

