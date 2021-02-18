#ifndef CALCULATIONS_HPP_4A01F791ECBA4E3CB8BC12436C41ABC9
#define CALCULATIONS_HPP_4A01F791ECBA4E3CB8BC12436C41ABC9

class Calculator {
  public:
    float altitude() const { return alt; };
    float velocityGroundZ() const { return vel_gnd_z; }
    float alt;
    float vel_gnd_z;
};

#endif
