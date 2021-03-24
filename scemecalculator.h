#pragma once


class scemecalculator
{
    double roomtemp,
        ifurance,
        isourse;
    double furancetemp,
        volt,
        amper;


public:
    scemecalculator(double room_tempirature = 20,
                    double semicondacter_current = 0,
                    double ifurance_current = 0);
    void setValue(int value_type, double);
    double getValue(int value_type);
    static const int
        Room_tempirature = 1,
        Ifurance = 2,
        Isource = 3,
        Furance_tempirature = 4,
        Voltage = 5,
        Current = 6;
};
