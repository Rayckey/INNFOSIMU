#include "innfosimu.h"
#include <iostream>
#include <chrono> 
#include <thread> 


INNFOSIMU::INNFOSIMU() {

    // Actuator stuff --------------------------------------------------


    ActuatorController::initController();
    ActuatorController::getInstance()->autoRecoginze();


    m_timer1 = new CrossTimer();
    m_timer1->start(5 , [=]{ActuatorController::processEvents();});

    m_pController = ActuatorController::getInstance();
    std::cout << "init controller" << m_pController << std::endl;
    // -----------------------------------------------------------------

    m_pController->m_sOperationFinished->connect_member(this, &INNFOSIMU::actuatorOperation);

    // -----------------------------------------------


    // FucKiNG tiLTing BiaaAAachi ---------------------------------------------------------------------
     m_pController->m_sQuaternion->connect_member(this, &INNFOSIMU::QuaCallBack);
    // ------------------------------------------------------------------------------------------------



    std::cout << "controller ready" << std::endl;
    
}


INNFOSIMU::~INNFOSIMU(){

    delete m_timer1;

    delete m_timer2;
}


void INNFOSIMU::actuatorOperation(uint8_t nId, uint8_t nType) {

    switch (nType) {
        case Actuator::Recognize_Finished:{

            m_timer2 = new CrossTimer();
            m_timer2->start(10 , std::bind(&INNFOSIMU::requestQue, this));

        }

            break;
        default:
            break;
    }
}



void INNFOSIMU::QuaCallBack(uint8_t id, double w, double x, double y, double z) {

    m_temp_qua.x = x;
    m_temp_qua.y = y;
    m_temp_qua.z = z;
    m_temp_qua.w = w;


    m_qua_buffer[id] = m_temp_qua;

}


void INNFOSIMU::requestQue() {
    // get the sweet sweet bottom IMU reading
    m_pController->requestAllQuaternions();
}



INNFOSIMU::Quaternion INNFOSIMU::ReceiveQue(int id){

    if (m_qua_buffer.find(id) == m_qua_buffer.end()){
        INNFOSIMU::Quaternion stuff;

        return stuff;
    }

    return m_qua_buffer[id];

}
