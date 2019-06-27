#include "innfosimu.h"


extern "C"
{

    INNFOSIMU* IMU_ptr=NULL;


    void initIMU(){
        IMU_ptr = new INNFOSIMU();
    }

    INNFOSIMU::Quaternion ReceiveQue(int id){
        return IMU_ptr->ReceiveQue(id);
    }

}

