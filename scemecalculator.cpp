#include "scemecalculator.h"

scemecalculator::scemecalculator(double roomt, double semicondI, double furanceI)
    : roomtemp(roomt), isourse(semicondI) ifurance(furanceI)
{
    getValue(Temp)
    getValue(Voltage);
    getValue(Current)

}
