#ifndef BUZZER_INTERFACE_H_D0A5A9D4E9C849189F33B7F063A06D32
#define BUZZER_INTERFACE_H_D0A5A9D4E9C849189F33B7F063A06D32

enum SongTypes{
    SongTypes_SUCCESS,
    SongTypes_NONCRITFAIL,
    SongTypes_CRITICALFAIL
};

/*Classes--------------------------------------------------------------*/
class IBuzzer {
public:
    virtual void sing(SongTypes song) const = 0;
};


#endif
