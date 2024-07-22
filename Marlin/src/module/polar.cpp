
// #include "../inc/MarlinConfigPre.h"

// #if ENABLED(POLAR)

// #include "polar.h"
// #include "motion.h"
// #include "planner.h"

// #include "../inc/MarlinConfig.h"

// #include "../../src/pins/stm32f4/pins_BTT_GTR_V1_0.h"



// float segments_per_second = DEFAULT_SEGMENTS_PER_SECOND,
//       polar_center_offset = POLAR_CENTER_OFFSET,
//       nozzle_offset = R_LINE_OFFSET;

// float absoluteAngle(float a) {
//   if (a < 0.0) while (a < 0.0) a += 360.0;
//   else if (a > 360.0) while (a > 360.0) a -= 360.0;
//   return a;
// }

// float theta_zero;

// void zero_theta(){

//   int trig_min_sw = digitalRead(PF2);

//   if(trig_min_sw == LOW){
//     theta_zero = 0;
//   }

// }


// void forward_kinematics(const_float_t r, const_float_t theta) {
//   const float absTheta = absoluteAngle(theta);
//   float radius = r;
//   // if (polar_center_offset > 0.0) radius = SQRT( ABS( sq(r) - sq(-polar_center_offset) ) );
//   cartes.x = cos(RADIANS(absTheta))*radius;
//   cartes.y = sin(RADIANS(absTheta))*radius;
// }

// void inverse_kinematics(const xyz_pos_t &raw) {
//     const float x = raw.x, y = raw.y,
//                 rawRadius = HYPOT(x,y),
//                 posTheta = DEGREES(ATAN2(y, x));


//     float r = rawRadius,
//           normal_theta = posTheta,
//           OverTheta = absoluteAngle(posTheta),
//           OldTheta = absoluteAngle(theta_zero),
//           theta,
//           totalTheta = 0;

//     if( ABS(0) <= normal_theta && normal_theta <= ABS(180) ){
//       theta = normal_theta;
//       OldTheta = theta;
//       totalTheta = theta;
//     }

//     if( OverTheta > 180){
//       theta = OverTheta;
//       OldTheta = theta;
//       totalTheta = theta;
//     }

//   /*   // float conastTheta = OverTheta - OldTheta;

//     // if( totalTheta >=  ABS(360.0) || totalTheta == ABS(0) ){
//     //   while(totalTheta = 0){
//     //     theta = 
//     //   }
//     // }

//     // theta = normal_theta;
//  */

//     delta.set(r, theta, raw.z);
// }

// void polar_report_positions() {
//   SERIAL_ECHOLNPGM("R liner : ", planner.get_axis_position_mm(X_AXIS),
//          " Theta :", planner.get_axis_position_degrees(B_AXIS),
//                   " Z: ", planner.get_axis_position_mm(Z_AXIS)
//   );
// }

// #endif








#include "../inc/MarlinConfigPre.h"

#if ENABLED(POLAR)

#include "polar.h"
#include "motion.h"
#include "planner.h"

#include "../inc/MarlinConfig.h"

#include "../../src/pins/stm32f4/pins_BTT_GTR_V1_0.h"



float segments_per_second = DEFAULT_SEGMENTS_PER_SECOND,
      polar_center_offset = POLAR_CENTER_OFFSET,
      nozzle_offset = R_LINE_OFFSET;

float absoluteAngle(float a) {
  if (a < 0.0) while (a < 0.0) a += 360.0;
  else if (a > 360.0) while (a > 360.0) a -= 360.0;
  return a;
}


void forward_kinematics(const_float_t r, const_float_t theta) {
  const float absTheta = absoluteAngle(theta);
  float radius = r;
  // if (polar_center_offset > 0.0) radius = SQRT( ABS( sq(r) - sq(-polar_center_offset) ) );
  cartes.x = cos(RADIANS(absTheta))*radius;
  cartes.y = sin(RADIANS(absTheta))*radius;
}

void inverse_kinematics(const xyz_pos_t &raw) {
    const float x = raw.x, y = raw.y,
                rawRadius = HYPOT(x,y),
                posTheta = DEGREES(ATAN2(y, x));


    float r = rawRadius,
          ori_theta = posTheta,
          // normal_theta = 0,
          // OverTheta = absoluteAngle(posTheta),
          // OldTheta = absoluteAngle(0),
          now_theta = 0,
          theta;

    if(0 <= ori_theta && ori_theta <= 180){
      theta = ori_theta;
      now_theta = theta;
    }
    if (now_theta >= 0 && ori_theta >=180 && ori_theta >= 359.9999){
      theta = ori_theta;
      now_theta = theta;
    }

    if(ori_theta < 0 && ori_theta >= -180){
      theta = ori_theta;
      now_theta = theta;
    }
    if(ori_theta >= -180 && ori_theta <= -359.99999999 && now_theta > -180){
      theta = ori_theta;
      now_theta = theta;
    }

    // if( ABS(0) <= normal_theta && normal_theta <= ABS(180) ){
    //   theta = normal_theta;
    //   OldTheta = theta;
    //   totalTheta = theta;
    // }

    // if( OverTheta > 180){
    //   theta = OverTheta;
    //   OldTheta = theta;
    //   theta = OldTheta;
    //   totalTheta = theta;
    // }

    
  /*   // float conastTheta = OverTheta - OldTheta;

    // if( totalTheta >=  ABS(360.0) || totalTheta == ABS(0) ){
    //   while(totalTheta = 0){
    //     theta = 
    //   }
    // }

    // theta = normal_theta;
 */

    delta.set(r, theta, raw.z);
}

void polar_report_positions() {
  SERIAL_ECHOLNPGM("R liner : ", planner.get_axis_position_mm(X_AXIS),
         " Theta :", planner.get_axis_position_degrees(B_AXIS),
                  " Z: ", planner.get_axis_position_mm(Z_AXIS)
  );
}

#endif
