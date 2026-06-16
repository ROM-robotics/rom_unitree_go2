# unitree_sdk2 Go2 header analysis

ဒီ note က `unitree_sdk2/include/unitree` အောက်က Go2 နဲ့တိုက်ရိုက်ဆိုင်တဲ့ header တွေကို layer အလိုက်ဖတ်ပြီး summary လုပ်ထားတာပါ။ အဓိက architecture က layer 3 ခုနဲ့မြင်ရတယ်:

1. `idl/go2/*.hpp`
Generated DDS message types. Wire format, field layout, topic payload structure တွေကို define လုပ်တယ်။

2. `dds_wrapper/robots/go2/*`
Real-time DDS topic publish/subscribe helper layer. `LowCmd_`, `LowState_`, `SportModeState_` လို message types တွေကို convenience wrapper အဖြစ်ပေးတယ်။

3. `robot/go2/*`
High-level RPC/service client layer. `sport`, `robot_state`, `config`, `video`, `vui`, `utrack`, `obstacles_avoid` စတဲ့ subsystem တွေကို `Client` base class ပေါ်မှာ wrap လုပ်ထားတယ်။

## 1. DDS generated message layer: `include/unitree/idl/go2`

ဒီ folder က Cyclone DDS IDL generator ထုတ်ထားတဲ့ C++ classes တွေပါ။ Go2 SDK မှာ topic payload contract အနေနဲ့သုံးတယ်။ Directory inventory ကိုကြည့်ရင် core message groups တွေက:

- Low-level control/state: `LowCmd_.hpp`, `LowState_.hpp`, `MotorCmd_.hpp`, `MotorCmds_.hpp`, `MotorState_.hpp`, `MotorStates_.hpp`
- Sport/high-level locomotion: `SportModeCmd_.hpp`, `SportModeState_.hpp`, `PathPoint_.hpp`
- Sensors and auxiliary data: `IMUState_.hpp`, `LidarState_.hpp`, `HeightMap_.hpp`, `UwbState_.hpp`, `WirelessController_.hpp`, `Go2FrontVideoData_.hpp`
- Service/RPC payload helpers: `Req_.hpp`, `Res_.hpp`, `ConfigChangeStatus_.hpp`, `InterfaceConfig_.hpp`, `Error_.hpp`

### LowCmd / SportModeState မှာ တွေ့ရတဲ့ structure

- `LowCmd_` ထဲမှာ `head`, `level_flag`, `motor_cmd[20]`, `wireless_remote[40]`, `gpio`, `crc` ပါတယ်။ Low-level servo command packet တိုက်ရိုက်ပို့ဖို့ layout ပြည့်စုံအောင် define လုပ်ထားတာဖြစ်တယ်။
- `SportModeState_` ထဲမှာ `imu_state`, `gait_type`, `position[3]`, `velocity[3]`, `yaw_speed`, `range_obstacle`, `foot_force`, `path_point[10]` လို high-level locomotion state fields ပါတယ်။

အဓိက takeaway က Go2 SDK ရဲ့ DDS topic layer ဟာ message schema များကို auto-generated class အဖြစ်တိတိကျကျ export လုပ်ပေးပြီး၊ အပေါ် layer တွေက အဲဒီ schema များကို wrapper ပဲလုပ်တာဖြစ်တယ်။

## 2. DDS wrapper layer: `include/unitree/dds_wrapper/robots/go2`

ဒီ layer က topic names, message initialization, CRC, joystick decoding/encoding တို့လို real-time transport convenience ကို handle လုပ်တယ်။

### `defines.h`

- `JointIndex` enum ကို front-right, front-left, rear-right, rear-left order နဲ့ 12 joints အတွက် define လုပ်ထားတယ်။
- Joint ordering standardization အတွက် important ဖြစ်တယ်။ Low-level controller code က magic number မသုံးဘဲ enum သုံးလို့ရတယ်။

### `go2_pub.h`

Publisher helpers တွေရှိတယ်:

- `publisher::LowCmd`
	`rt/lowcmd` topic ကို publish လုပ်ဖို့ wrapper. Constructor မှာ packet head ကို `0xFE 0xEF`, `level_flag=0xFF` သတ်မှတ်ပေးပြီး motor mode ကို default servo mode (`1`) ထားပေးတယ်။ `pre_communication()` ထဲမှာ CRC ကို auto fill လုပ်တယ်။
- `publisher::LowState`
	`rt/lowstate` topic publisher. Optional joystick object ရှိရင် `wireless_remote` buffer ထဲ encode လုပ်ပေးပြီး CRC ပြန်တွက်တယ်။
- `publisher::SportModeState`
	`rt/sportmodestate` topic publisher convenience wrapper.
- `publisher::WirelessController`
	`rt/wirelesscontroller` topic publisher. Joystick axis/buttons ကို DDS message field များထဲ map လုပ်ပေးတယ်။
- `publisher::MotorCmds`, `publisher::MotorStates`
	Topic-specific vector payload wrappers. Constructor မှာ vector size ကို resize လုပ်ပေးတယ်။

### `go2_sub.h`

Subscriber helpers တွေရှိတယ်:

- `subscription::LowState`
	`rt/lowstate` subscriber wrapper. `update()` မှာ joystick bytes 40-byte buffer ကို decode လုပ်ပြီး joystick timeout detection (`3000 ms`) ပါလုပ်ထားတယ်။
- `subscription::LowCmd`
	`rt/lowcmd` subscriber wrapper.
- `subscription::SportModeState`
	`rt/sportmodestate` subscriber wrapper. `gaitType()`, `position()`, `velocity()` ဆိုတဲ့ Eigen-based accessor helper တွေပါလာတယ်။
- `subscription::MotorStates`, `subscription::MotorCmds`
	Variable-length vector DDS payload subscriber wrappers.

### `go2.h`

- Aggregate include header တစ်ခုဖြစ်ပြီး `defines.h`, `go2_pub.h`, `go2_sub.h` ကိုစုထည့်ထားတယ်။
- `shutdown()` helper ပါတယ်။ ဒီ function က `unitree::robot::b2::MotionSwitcherClient` ကိုသုံးပြီး default controller/motion mode ကို release လုပ်ဖို့ကြိုးစားတယ်။ Meaning က low-level command မပို့ခင် default controller ကိုပိတ်ရန် convenience helper ဖြစ်တယ်။

Observation:

- Go2 wrapper header ထဲက `shutdown()` က B2 namespace အောက်က `MotionSwitcherClient` ကို import လုပ်ထားတာကြောင့် control-plane service အချို့ကို model အကြား shared infrastructure အဖြစ်သုံးထားတာ မြင်ရတယ်။

## 3. High-level service client layer: `include/unitree/robot/go2`

ဒီ layer က DDS raw topic ကို direct မကိုင်ဘဲ request/response style API ပေးတယ်။ Design pattern တူတာက:

1. `*_api.hpp` က service name, api version, api id constants နဲ့ JSON payload classes ကို define လုပ်တယ်။
2. `*_client.hpp` က `Client` base class ကို inherit လုပ်ပြီး method signatures ပေးတယ်။
3. `*_error.hpp` ရှိရင် subsystem-specific error code declarations ထည့်ထားတယ်။

### 3.1 `sport/`

Files:

- `sport_client.hpp`
- `sport_api.hpp`
- `sport_error.hpp`

`SportClient` က Go2 high-level motion control အတွက် အဓိက header ဖြစ်တယ်။ Header မှာပါတဲ့ methods တွေကို capability အလိုက်ခွဲရင်:

- posture/state transitions: `Damp()`, `BalanceStand()`, `StandUp()`, `StandDown()`, `RecoveryStand()`, `Sit()`, `RiseSit()`, `StopMove()`
- body or velocity command: `Euler(roll,pitch,yaw)`, `Move(vx,vy,vyaw)`, `SpeedLevel(level)`
- expressive motions: `Hello()`, `Stretch()`, `Scrape()`, `FrontFlip()`, `FrontJump()`, `FrontPounce()`, `Dance1()`, `Dance2()`, `LeftFlip()`, `BackFlip()`, `HandStand(flag)`
- gait/behavior toggles: `FreeWalk()`, `FreeBound(flag)`, `FreeJump(flag)`, `FreeAvoid(flag)`, `ClassicWalk(flag)`, `WalkUpright(flag)`, `CrossStep(flag)`, `StaticWalk()`, `TrotRun()`, `EconomicGait()`, `SwitchAvoidMode()`
- automation/control toggles: `SwitchJoystick(flag)`, `Pose(flag)`, `AutoRecoverSet(flag)`, `AutoRecoverGet(flag)`

`sport_api.hpp` မှာ service metadata:

- service name: `sport`
- api version: `1.0.0.1`
- api ids: `1001` ကနေ `2058` အထိ မျိုးစုံ define လုပ်ထားတယ်

`sport_error.hpp` မှာ notable errors:

- `UT_ROBOT_SPORT_ERR_CLIENT_POINT_PATH` = 4101
- `UT_ROBOT_SPORT_ERR_SERVER_OVERTIME` = 4201
- `UT_ROBOT_SPORT_ERR_SERVER_NOT_INIT` = 4205

Interpretation:

- ဒီ module က Go2 body motion preset library + behavior switchboard လိုပုံစံရှိတယ်။
- RPC method naming က firmware-side motion primitives ကိုတော်တော်လေး direct expose လုပ်ထားတယ်။

### 3.2 `robot_state/`

Files:

- `robot_state_client.hpp`
- `robot_state_api.hpp`
- `robot_state_error.hpp`

`RobotStateClient` methods:

- `ServiceList(std::vector<ServiceState>&)`
- `ServiceSwitch(const std::string& name, int32_t swit, int32_t& status)`
- `SetReportFreq(int32_t interval, int32_t duration)`

Role:

- Runtime service manager လိုအလုပ်လုပ်တယ်။ Go2 firmware/services တွေရဲ့ on/off status ကို query/switch လုပ်နိုင်တယ်။
- `ServiceState` ထဲမှာ `name`, `status`, `protect` fields ပါတယ်။ `protect` ရှိတာကြောင့် service အချို့ကို protected state နဲ့ မပိတ်နိုင်တဲ့ logic ရှိနိုင်တယ်။

`robot_state_api.hpp` က service metadata:

- service name: `robot_state`
- api version: `1.0.0.1`
- ids: `SERVICE_SWITCH=1001`, `SET_REPORT_FREQ=1002`, `SERVICE_LIST=1003`

Error header မှာ:

- `UT_ROBOT_STATE_CLIENT_ERR_SERVICE_SWITCH` = 5201
- `UT_ROBOT_STATE_CLIENT_ERR_SERVICE_PROTECTED` = 5202

### 3.3 `config/`

Files:

- `config_client.hpp`
- `config_api.hpp`
- `config_error.hpp`

`ConfigClient` methods:

- `Set(name, content)`
- `Get(name, content)`
- `Del(name)`
- `Meta(name, ConfigMeta&)`
- `Meta(name, std::string&)`
- `SubscribeChangeStatus(name, callback)`

Design meaning:

- Go2 onboard config store ကို key/content storage ပုံစံနဲ့ expose လုပ်ထားတယ်။
- `ConfigMeta` မှာ `name`, `lastModified`, `size`, `epoch` ပါတယ်။ file-like metadata store ဖြစ်ပုံရတယ်။
- `SubscribeChangeStatus` က DDS topic subscriber တစ်ခု (`ConfigChangeStatus_`) ဖြင့် config update notifications ကို async callback ပေးတယ်။ ဒါက config subsystem ဟာ RPC + event hybrid design ဖြစ်တယ်လို့ပြတယ်။

`config_api.hpp` က JSON payload classes များ:

- `JsonizeConfigMeta`
- `ConfigSetParameter`
- `ConfigGetParameter`
- `ConfigGetData`
- `ConfigDelParameter`
- `ConfigMetaParameter`
- `ConfigMetaData`

Error range ကပိုကျယ်တယ်။ Typical issues:

- invalid parameter/name/content
- not found
- lock/meta/data load-save-formalize-remove failures

ဒါကြောင့် config service က internal persistence layer တစ်ခုနဲ့ချိတ်နေပြီး robustness concerns အတွက် dedicated error taxonomy ထည့်ထားတယ်။

### 3.4 `video/`

Files:

- `video_client.hpp`
- `video_api.hpp`
- `video_error.hpp`

`VideoClient` API ကရိုးရိုးရှင်းရှင်းတစ်ခုပဲရှိတယ်:

- `GetImageSample(std::vector<uint8_t>&)`

Metadata:

- service name: `videohub`
- api version: `1.0.0.0`
- api id: `GETIMAGESAMPLE = 1001`

Meaning:

- Video service ကို full stream abstraction မပေးဘဲ sampled frame fetch RPC အနေနဲ့ expose လုပ်ထားတယ်။
- Example code က returned bytes ကို JPG file အဖြစ် save လုပ်နေတာကြောင့် payload က compressed image blob ဖြစ်တယ်လို့ယူဆနိုင်တယ်။

### 3.5 `vui/`

Files:

- `vui_client.hpp`
- `vui_api.hpp`
- `vui_error.hpp`

`VuiClient` methods:

- `SetSwitch(int enable)` / `GetSwitch(int&)`
- `SetVolume(int level)` / `GetVolume(int&)`
- `SetBrightness(int level)` / `GetBrightness(int&)`

Metadata:

- service name: `vui`
- api version: `1.0.0.0`
- api ids 1001..1006

Meaning:

- ဒီ module က voice UI / screen UI / user-facing multimedia controls ကိုတစ်စုတစ်စည်းတည်းထားတယ်။
- `brightness` ပါတာကြောင့် device-side display or indicator system တစ်ခုရှိကြောင်းပြတယ်။

### 3.6 `utrack/`

Files:

- `utrack_client.hpp`
- `utrack_api.hpp`

`UtrackClient` methods:

- `SwitchSet(bool enable)`
- `SwitchGet(bool& enable)`
- `IsTracking(bool& enable)`

Metadata:

- service name: `uwbswitch`
- api version: `1.0.0.1`
- ids 1001..1003

Meaning:

- UWB-based tracking feature on/off and current tracking state ကို separate service အနေနဲ့ထုတ်ထားတယ်။
- `service name` က `uwbswitch` ဖြစ်ပြီး class name က `UtrackClient` ဖြစ်တာကြောင့် user-facing naming နဲ့ firmware-side service naming မတူနိုင်တာသတိထားရမယ်။

### 3.7 `obstacles_avoid/`

Files:

- `obstacles_avoid_client.hpp`
- `obstacles_avoid_api.hpp`

`ObstaclesAvoidClient` methods:

- `SwitchSet(bool enable)` / `SwitchGet(bool& enable)`
- `Move(float x, float y, float yaw)`
- `UseRemoteCommandFromApi(bool isRemoteCommandsFromApi)`
- `MoveToAbsolutePosition(float x, float y, float yaw)`
- `MoveToIncrementPosition(float x, float y, float yaw)`

`obstacles_avoid_api.hpp` ထဲမှာ `ObstaclesAvoidMoveParameter` က `mode` field ပါတယ်:

- `0` = velocity
- `1` = increment pose
- `2` = absolute pose

Meaning:

- ဒီ module က obstacle avoidance enabled state အပေါ်မှာ motion command abstraction တင်ထားတယ်။
- Velocity command နဲ့ position-goal command နှစ်မျိုးကို service တစ်ခုတည်းထဲ multiplex လုပ်ထားတယ်။
- `UseRemoteCommandFromApi()` က remote joystick source နဲ့ API source ဘယ်က command authority ယူမလဲဆိုတာ control လုပ်ဖို့သုံးတယ်။ control arbitration use-case အတွက်အရေးကြီးတယ်။

### 3.8 `public/jsonize_type.hpp`

ဒီ file က Go2 subsystem များစွာ cross-use လုပ်တဲ့ JSON payload helper types တွေ define လုပ်ထားတယ်:

- scalar wrappers: `JsonizeFlagBool`, `JsonizeDataBool`, `JsonizeDataInt`, `JsonizeDataFloat`, `JsonizeDataDouble`, `JsonizeDataString`
- geometry wrappers: `JsonizeVec3`, `JsonizeQuat`
- motion wrapper: `JsonizePathPoint`
- generic name/value helper: `JsonizeCommObjInt`

Observation:

- `a2`, `as2`, `g1`, `h1`, `h2`, `r1` modules တချို့ကတောင် ဒီ `go2/public/jsonize_type.hpp` ကို include လုပ်ထားတယ်။ ဆိုလိုတာက Go2 folder ထဲက helper type file ဟာ robot-family shared utility အဖြစ် evolve လုပ်သွားပြီး namespace placement က historical artifact ဖြစ်နိုင်တယ်။

## 4. Example files ကပြတဲ့ usage pattern

`unitree_sdk2/example/go2` ထဲက examples တွေကြည့်ရင် Go2 SDK usage pattern နှစ်မျိုးကွဲထွက်တယ်:

### Pattern A: direct DDS topic access

- `go2_low_level.cpp`

ဒီ example က `ChannelPublisher<LowCmd_>` နဲ့ `ChannelSubscriber<LowState_>` ကို direct သုံးတယ်။ Header helper layer မသုံးဘဲ raw DDS messages ကို manipulate လုပ်တယ်။

Key points:

- topic names: `rt/lowcmd`, `rt/lowstate`
- `LowCmd_` packet header, mode, q/kp/dq/kd/tau, crc ကို manual ဖြည့်တယ်
- low-level control loop ကို `CreateRecurrentThreadEx` နဲ့ run တယ်

ဒီ pattern က servo/joint-level control လုပ်ချင်တဲ့ user အတွက်ဖြစ်တယ်။

### Pattern B: service client access + topic monitoring

- `go2_sport_client.cpp`
- `go2_robot_state_client.cpp`
- `go2_video_client.cpp`
- `go2_vui_client.cpp`

ဒီ examples တွေက `SetTimeout() -> Init() -> API call` pattern ကိုပြတယ်။

`go2_sport_client.cpp` မှာ interesting point က:

- `SportClient` နဲ့ command ပို့တယ်
- တပြိုင်နက် `ChannelSubscriber<SportModeState_>` နဲ့ `rt/sportmodestate` ကို subscribe လုပ်ပြီး feedback state ဖတ်တယ်

ဒါက service client + DDS state topic combine လုပ်တာဟာ expected usage pattern ဖြစ်ကြောင်းပြတယ်။

## 5. Practical map: Go2 header ကို ဘယ် use-case မှာသုံးမလဲ

| Use-case | Recommended headers |
| --- | --- |
| Joint-level command, servo tuning, raw low-level loop | `idl/go2/LowCmd_.hpp`, `idl/go2/LowState_.hpp`, or `dds_wrapper/robots/go2/go2_pub.h`, `go2_sub.h` |
| High-level walking / preset motions | `robot/go2/sport/sport_client.hpp` |
| Runtime service on/off and telemetry report interval | `robot/go2/robot_state/robot_state_client.hpp` |
| Onboard config CRUD and config update callbacks | `robot/go2/config/config_client.hpp` |
| Snapshot image fetch | `robot/go2/video/video_client.hpp` |
| Voice/display user interface control | `robot/go2/vui/vui_client.hpp` |
| UWB tracking enable/state | `robot/go2/utrack/utrack_client.hpp` |
| Obstacle avoidance behavior + authority switching | `robot/go2/obstacles_avoid/obstacles_avoid_client.hpp` |

## 6. Important findings and caveats

### Finding 1: Go2 API layers are intentionally split

- Real-time deterministic control path ကို DDS topic layer မှာထားတယ်
- Human-friendly service calls ကို `Client`-based RPC layer မှာထားတယ်

ဒီ split ကြောင့် low-level loop နဲ့ high-level behavior API ကို project တစ်ခုထဲမှာပေါင်းသုံးနိုင်တယ်။

### Finding 2: `go2.h` is not the whole Go2 SDK

`dds_wrapper/robots/go2/go2.h` က DDS convenience layer aggregate ပဲဖြစ်တယ်။ `sport/config/video/vui/...` service clients ကို auto include မလုပ်ဘူး။ Go2 API အားလုံးရချင်ရင် `robot/go2/...` subtree ကိုသီးသန့် include လုပ်ရမယ်။

### Finding 3: Some Go2 helpers are reused by other robot families

`robot/go2/public/jsonize_type.hpp` ကို အခြား robot family headers တော်တော်များများ include လုပ်နေတယ်။ Namespace/path naming ကို strictly semantic meaning အနေနဲ့မယူသင့်ဘူး။ Utility reuse location အနေနဲ့ပဲမြင်ရမယ်။

### Finding 4: Example/header mismatch exists around trajectory follow

`example/go2/go2_trajectory_follow.cpp` က `SportClient::TrajectoryFollow(path)` ကိုခေါ်တယ်။ ဒါပေမယ့် Go2 ရဲ့ `sport_client.hpp` နဲ့ `sport_api.hpp` ထဲမှာ:

- `TrajectoryFollow(...)` declaration မတွေ့ရ
- trajectory follow API id မတွေ့ရ

အဲဒီ capability က B2 headers မှာတော့ရှိတယ်။ ဒါကြောင့် ဒီ repository snapshot အရ `go2_trajectory_follow.cpp` ဟာ stale example, unsupported API, သို့မဟုတ် missing header implementation တစ်ခုခုဖြစ်နိုင်တယ်။ Build မတိုင်ခင် verify လုပ်သင့်တယ်။

## 7. Summary

Go2-related headers တွေကို architecture angle နဲ့ကြည့်ရင်:

- `idl/go2` = message schema contract
- `dds_wrapper/robots/go2` = real-time topic helper layer
- `robot/go2` = high-level RPC/service client layer

Practical development အတွက် rule of thumb က:

- deterministic joint/servo control ဆိုရင် DDS message/topic layer ကိုသုံး
- behavior/service style command ဆိုရင် `robot/go2/*_client.hpp` ကိုသုံး
- feedback/state monitoring ကို DDS subscriber နဲ့တွဲသုံး

ဒီ design က Go2 SDK ကို low-level robotics control နဲ့ high-level product features နှစ်မျိုးစလုံးထောက်ပံ့နိုင်အောင် modular ခွဲထားတာကိုပြတယ်။
