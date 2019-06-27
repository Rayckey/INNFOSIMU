#include <string>

#include "innfosimu.h"

#include "actuatorcontroller.h"
#include "actuatordefine.h"
#include "crosstimer.h"
#include <thread>


int main(int argc, char **argv) {


    INNFOSIMU * innfos_imu = new INNFOSIMU();


    while (true){
        std::this_thread::sleep_for(chrono::milliseconds(10));

        INNFOSIMU::Quaternion temp_imu = innfos_imu->ReceiveQue(5);

        std::cout << temp_imu.w << " " <<temp_imu.x << " " <<temp_imu.y << " " <<temp_imu.z << std::endl;

    }

  return 0;
}
