[1mdiff --git a/avionics/common/include/states/landed.h b/avionics/common/include/states/landed.h[m
[1mindex 050fff4..511c3d5 100644[m
[1m--- a/avionics/common/include/states/landed.h[m
[1m+++ b/avionics/common/include/states/landed.h[m
[36m@@ -10,7 +10,7 @@[m [mnamespace State {[m
 [m
 class Landed : public IState {[m
   public:[m
[31m-    Landed() {}[m
[32m+[m[32m    Landed(Camera &cam) : cam_(cam) {}[m
 [m
     /*[m
      * @brief Return the assigned enumeration code.[m
[36m@@ -33,7 +33,11 @@[m [mclass Landed : public IState {[m
         constexpr static unsigned int LANDED_POLLING_INTERVAL = 5000;[m
         Scheduler::get_task(static_cast<int>(TaskID::ReadEvalLog)).period =[m
             Hal::ms(LANDED_POLLING_INTERVAL);[m
[32m+[m[32m        cam_.stop_record();[m
     }[m
[32m+[m
[32m+[m[32m  private:[m
[32m+[m[32m    Camera &cam_;[m
 };[m
 [m
 } // namespace State[m
[1mdiff --git a/avionics/rockets/hollyburn/include/states_config.hpp b/avionics/rockets/hollyburn/include/states_config.hpp[m
[1mindex 3038f10..f233855 100644[m
[1m--- a/avionics/rockets/hollyburn/include/states_config.hpp[m
[1m+++ b/avionics/rockets/hollyburn/include/states_config.hpp[m
[36m@@ -67,7 +67,8 @@[m [mstruct StateMachineConfig {[m
                 ignitors.drogue),[m
           drogue(MAIN_DEPLOY_ALTITUDE, ignitors.main),[m
           main(StateId::LANDED, LAND_CHECK_TIME_INTERVAL, LAND_CHECKS,[m
[31m-               LAND_VELOCITY_THRESHOLD, calc) {}[m
[32m+[m[32m               LAND_VELOCITY_THRESHOLD, calc),[m
[32m+[m[32m          landed(camera) {}[m
 [m
     std::unordered_map<StateId, IState *> state_map = {[m
         {StateId::STANDBY, &standby},[m
[1mdiff --git a/avionics/rockets/tantalus_stage_1/include/states_config.hpp b/avionics/rockets/tantalus_stage_1/include/states_config.hpp[m
[1mindex e39cac5..3411e35 100644[m
[1m--- a/avionics/rockets/tantalus_stage_1/include/states_config.hpp[m
[1m+++ b/avionics/rockets/tantalus_stage_1/include/states_config.hpp[m
[36m@@ -84,7 +84,8 @@[m [mstruct StateMachineConfig {[m
                 ignitors.drogue),[m
           drogue(MAIN_DEPLOY_ALTITUDE, ignitors.main),[m
           main(StateId::LANDED, LAND_CHECK_TIME_INTERVAL, LAND_CHECKS,[m
[31m-               LAND_VELOCITY_THRESHOLD, calc) {}[m
[32m+[m[32m               LAND_VELOCITY_THRESHOLD, calc),[m
[32m+[m[32m          landed(camera) {}[m
 [m
     std::unordered_map<StateId, IState *> state_map = {[m
         {StateId::STANDBY, &standby},[m
[1mdiff --git a/avionics/rockets/tantalus_stage_2/include/states_config.hpp b/avionics/rockets/tantalus_stage_2/include/states_config.hpp[m
[1mindex 3038f10..f233855 100644[m
[1m--- a/avionics/rockets/tantalus_stage_2/include/states_config.hpp[m
[1m+++ b/avionics/rockets/tantalus_stage_2/include/states_config.hpp[m
[36m@@ -67,7 +67,8 @@[m [mstruct StateMachineConfig {[m
                 ignitors.drogue),[m
           drogue(MAIN_DEPLOY_ALTITUDE, ignitors.main),[m
           main(StateId::LANDED, LAND_CHECK_TIME_INTERVAL, LAND_CHECKS,[m
[31m-               LAND_VELOCITY_THRESHOLD, calc) {}[m
[32m+[m[32m               LAND_VELOCITY_THRESHOLD, calc),[m
[32m+[m[32m          landed(camera) {}[m
 [m
     std::unordered_map<StateId, IState *> state_map = {[m
         {StateId::STANDBY, &standby},[m
