#pragma once

#include "HAL/time.h"
#include "buzzer.h"


/**
 * @brief R
 */
class  LandedBuzzer{
  private:
    Buzzer buzzer_;

  public:
	LandedBuzzer(Buzzer &buzzer) : buzzer_(buzzer){
		
	}
    static void run(void * self) {
		  //reinterpret_cast<LandedBuzzer *>(self)->buzzer_.sing(SongTypes_CRITICALFAIL);
		  reinterpret_cast<LandedBuzzer *>(self)->buzzer_.sing(SongTypes_LANDED);
    }
    static constexpr Hal::ms freq{500};
};
