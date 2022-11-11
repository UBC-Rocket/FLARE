#pragma once

enum SongTypes {
    SongTypes_SUCCESS,
    SongTypes_NONCRITFAIL,
    SongTypes_CRITICALFAIL
};

/*Classes--------------------------------------------------------------*/
class IBuzzer {
  public:
    /**
     * @brief todo
     * @param song song to play
     */
    virtual void sing(SongTypes song) const = 0;
};
