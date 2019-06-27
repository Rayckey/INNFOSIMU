#ifndef INNFOSIMU_H
#define INNFOSIMU_H


#include "actuatorcontroller.h"
#include "crosstimer.h"


class INNFOSIMU {



public:

    struct Quaternion
    {
        double x = 0;
        double y = 0;
        double z = 0;
        double w = 1;
    };

    INNFOSIMU();
    ~INNFOSIMU();

    INNFOSIMU::Quaternion ReceiveQue(int id);


private:

    // tilting and stuff -----------------------------------------------
    void actuatorOperation(uint8_t nId, uint8_t nType) ;
    void requestQue();
    void QuaCallBack(uint8_t id, double w, double x, double y, double z);
    // -----------------------------------------------------------------


    // qua buffer ----------------------------------------

    std::map<int , INNFOSIMU::Quaternion> m_qua_buffer;
    INNFOSIMU::Quaternion  m_temp_qua;

    // ---------------------------------------------------

    // Pay no Attention Here Plz ---------------------------
    ActuatorController *m_pController;
    // ----------------------------------------------------


    // Cross Timer -------------------------------------
    CrossTimer * m_timer1;
    CrossTimer * m_timer2;
    // -------------------------------------------------


}; //class

#endif
