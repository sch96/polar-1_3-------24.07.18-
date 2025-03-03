
#include "../gcode.h"
#include "../../module/motion.h"

#include "../../MarlinCore.h"

#if ALL(FWRETRACT, FWRETRACT_AUTORETRACT)
  #include "../../feature/fwretract.h"
#endif

#include "../../sd/cardreader.h"

#if ENABLED(NANODLP_Z_SYNC)
  #include "../../module/planner.h"
#endif

extern xyze_pos_t destination;

#if ENABLED(VARIABLE_G0_FEEDRATE)
  feedRate_t fast_move_feedrate = MMM_TO_MMS(G0_FEEDRATE);
#endif

/**
 * G0, G1: x y z e 축의 조정 된 움직임
 */
void GcodeSuite::G0_G1(TERN_(HAS_FAST_MOVES, const bool fast_move/*=false*/)) {
  if (!MOTION_CONDITIONS) return;

  TERN_(FULL_REPORT_TO_HOST_FEATURE, set_and_report_grblstate(M_RUNNING));

  #ifdef G0_FEEDRATE
    feedRate_t old_feedrate;
    #if ENABLED(VARIABLE_G0_FEEDRATE)
      if (fast_move) {
        old_feedrate = feedrate_mm_s;             // Back up the (old) motion mode feedrate
        feedrate_mm_s = fast_move_feedrate;       // Get G0 feedrate from last usage
      }
    #endif
  #endif



  get_destination_from_command();                 // Get X Y [Z[I[J[K]]]] [E] F (and set cutter power)




  #ifdef G0_FEEDRATE
    if (fast_move) {
      #if ENABLED(VARIABLE_G0_FEEDRATE)
        fast_move_feedrate = feedrate_mm_s;       // Save feedrate for the next G0
      #else
        old_feedrate = feedrate_mm_s;             // Back up the (new) motion mode feedrate
        feedrate_mm_s = MMM_TO_MMS(G0_FEEDRATE);  // Get the fixed G0 feedrate
      #endif
    }
  #endif

  #if ALL(FWRETRACT, FWRETRACT_AUTORETRACT)

    if (MIN_AUTORETRACT <= MAX_AUTORETRACT) {
      // When M209 Autoretract is enabled, convert E-only moves to firmware retract/recover moves
      if (fwretract.autoretract_enabled && parser.seen_test('E')
        && !parser.seen(STR_AXES_MAIN)
      ) {
        const float echange = destination.e - current_position.e;
        // Is this a retract or recover move?
        if (WITHIN(ABS(echange), MIN_AUTORETRACT, MAX_AUTORETRACT) && fwretract.retracted[active_extruder] == (echange > 0.0)) {
          current_position.e = destination.e;       // Hide a G1-based retract/recover from calculations
          sync_plan_position_e();                   // AND from the planner
          return fwretract.retract(echange < 0.0);  // Firmware-based retract/recover (double-retract ignored)
        }
      }
    }

  #endif // FWRETRACT

  #if ANY(IS_SCARA, POLAR)
    fast_move ? prepare_fast_move_to_destination() : prepare_line_to_destination();
  #else
    prepare_line_to_destination();
  #endif

  #ifdef G0_FEEDRATE
    // Restore the motion mode feedrate
    if (fast_move) feedrate_mm_s = old_feedrate;
  #endif

  #if ENABLED(NANODLP_Z_SYNC)
    #if ENABLED(NANODLP_ALL_AXIS)
      #define _MOVE_SYNC parser.seenval('X') || parser.seenval('Y') || parser.seenval('Z')  // For any move wait and output sync message
    #else
      #define _MOVE_SYNC parser.seenval('Z')  // Only for Z move
    #endif
    if (_MOVE_SYNC) {
      planner.synchronize();
      SERIAL_ECHOLNPGM(STR_Z_MOVE_COMP);
    }
    TERN_(FULL_REPORT_TO_HOST_FEATURE, set_and_report_grblstate(M_IDLE));
  #else
    TERN_(FULL_REPORT_TO_HOST_FEATURE, report_current_grblstate_moving());
  #endif
}
