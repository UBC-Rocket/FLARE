#pragma once

enum SongTypes {
    SongTypes_SUCCESS,
    SongTypes_NONCRITFAIL,
    SongTypes_CRITICALFAIL
};

/*Classes--------------------------------------------------------------*/
class IBuzzer {
  public:
    virtual void sing(SongTypes song) const = 0;
};
