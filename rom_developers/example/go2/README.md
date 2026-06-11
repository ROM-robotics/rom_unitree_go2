# GO2 Example Programs

Unitree GO2 robot ကို control လုပ်ရန် C++ example programs များ။ Program တစ်ခုချင်းစီ၏ function flow ကို Mermaid diagram ဖြင့် ရှင်းပြထားသည်။

---

## Program Overview

| File | Level | ရည်ရွယ်ချက် |
|------|-------|------------|
| `go2_low_level.cpp` | Low-level | Motor ကို တိုက်ရိုက် sine wave motion ဖြင့် ထိန်းချုပ်သည် |
| `go2_stand_example.cpp` | Low-level | Motor interpolation ဖြင့် stand-up sequence လုပ်သည် |
| `go2_sport_client.cpp` | High-level | SportClient API ဖြင့် motion mode များ ပြောင်းသည် |
| `go2_trajectory_follow.cpp` | High-level | Sine curve path ကို TrajectoryFollow API ဖြင့် လိုက်သည် |
| `go2_robot_state_client.cpp` | Service | Robot service များကို on/off ပြောင်းသည် |
| `go2_video_client.cpp` | Sensor | Camera ဓာတ်ပုံ ရိုက်ပြီး JPEG သိမ်းသည် |
| `go2_vui_client.cpp` | UI | Head display brightness ထိန်းချုပ်သည် |

---

## 1. go2_low_level.cpp — Low-Level Motor Sine Wave Control

Front-right leg ၏ motor (joint 2) ကို sine wave ဖြင့် တိုက်ရိုက် ထိန်းချုပ်သည်။ DDS publisher/subscriber ဖြင့် `rt/lowcmd` topic ကို write လုပ်သည်။

```mermaid
flowchart TD
    A([main]) --> B[ChannelFactory::Init]
    B --> C[Custom::Init]

    C --> D[InitLowCmd\nMotor 0-19 ကို PosStopF/VelStopF ဖြင့် initialize]
    D --> E[lowcmd_publisher::InitChannel\nrt/lowcmd]
    D --> F[lowstate_subscriber::InitChannel\nrt/lowstate]
    F --> G[LowStateMessageHandler\nstate ကို memory ထဲ copy]
    E --> H[CreateRecurrentThread\n2ms loop]

    H --> I{LowCmdWrite\nLoop}

    I --> J[motiontime 0~20\nqInit ကို record လုပ်]
    J --> K[motiontime 10~400\njointLinearInterpolation\nqInit → sin_mid_q]
    K --> L[motiontime 400+\nSine Wave Motion\nsin_joint1 = 0.6·sin·freq\nsin_joint2 = -0.9·sin·freq]

    L --> M[motor_cmd 2 ကို qDes ဖြင့် set]
    M --> N[crc32_core\nCRC checksum တွက်]
    N --> O[lowcmd_publisher::Write\nRobot သို့ ပေးပို့]
    O --> I
```

### Key Functions

| Function | ရှင်းလင်းချက် |
|----------|--------------|
| `Init()` | Publisher, Subscriber, Thread အားလုံး setup |
| `InitLowCmd()` | Motor 20 ခုကို safe default state ထားသည် |
| `LowStateMessageHandler()` | DDS မှ state data လက်ခံသည် |
| `jointLinearInterpolation()` | `rate` 0→1 ဖြင့် position တဖြည်းဖြည်း ပြောင်းသည် |
| `LowCmdWrite()` | 3-phase motion: record → interpolate → sine wave |
| `crc32_core()` | Command packet ၏ integrity စစ်သော CRC32 တွက်သည် |

---

## 2. go2_stand_example.cpp — Low-Level Stand-Up Sequence

MotionSwitcherClient ဖြင့် motion service ကို ပိတ်ပြီး၊ motor interpolation ဖြင့် 4-phase stand-up sequence လုပ်သည်။

```mermaid
flowchart TD
    A([main]) --> B[User press Enter to confirm]
    B --> C[ChannelFactory::Init]
    C --> D[Custom::Init]

    D --> E[InitLowCmd\nMotors initialize]
    D --> F[MotionSwitcherClient::Init]
    F --> G{queryMotionStatus\nService ရှိသလား?}
    G -- ရှိသည် --> H[msc.ReleaseMode\nService ပိတ်]
    H --> G
    G -- မရှိ --> I[Custom::Start]

    I --> J[CreateRecurrentThread\n2ms loop]

    J --> K{LowCmdWrite Loop}

    K --> L[Sensor Data ပြပါ\nJoint pos, IMU, Foot force]

    K --> M[motiontime 500+\nfirstRun: _startPos record]

    M --> N[Phase 1 _percent_1\nstartPos → targetPos_1\nduration: 500 ticks]
    N --> O[Phase 2 _percent_2\ntargetPos_1 → targetPos_2\nduration: 500 ticks]
    O --> P[Phase 3 _percent_3\ntargetPos_2 hold\nduration: 1000 ticks]
    P --> Q[Phase 4 _percent_4\ntargetPos_2 → targetPos_3\nduration: 900 ticks]

    Q --> R[crc32_core]
    R --> S[lowcmd_publisher::Write]
    S --> K
```

### 4-Phase Motion Sequence

```mermaid
stateDiagram-v2
    [*] --> WaitStart : motiontime < 500
    WaitStart --> Phase1 : motiontime >= 500\nrecord startPos
    Phase1 --> Phase2 : _percent_1 == 1\n(500 ticks)
    Phase2 --> Phase3 : _percent_2 == 1\n(500 ticks)
    Phase3 --> Phase4 : _percent_3 == 1\n(1000 ticks)
    Phase4 --> Done : _percent_4 == 1\n(900 ticks)
    Done --> [*]

    Phase1 : startPos → targetPos_1\n(crouch position)
    Phase2 : targetPos_1 → targetPos_2\n(stand position)
    Phase3 : targetPos_2 hold\n(stable stand)
    Phase4 : targetPos_2 → targetPos_3\n(wide stance)
```

---

## 3. go2_sport_client.cpp — High-Level Sport Mode Control

SportClient API (High-level) ဖြင့် motion mode တစ်ခုကို `TEST_MODE` constant ဖြင့် ရွေးချယ်ပြီး run သည်။

```mermaid
flowchart TD
    A([main]) --> B[ChannelFactory::Init]
    B --> C[Custom constructor\nSportClient init\nHighState subscriber]
    C --> D[sleep 1s\nstable state ကိုင်ဆောင်]
    D --> E[GetInitState\npx0, py0, yaw0 record]
    E --> F[CreateRecurrentThread\n5ms loop]

    F --> G{RobotControl Loop\nswitch on TEST_MODE}

    G --> H[normal_stand\nStandUp API]
    G --> I[balance_stand\nBalanceStand API]
    G --> J[velocity_move\nMove 0.3 0 0.3]
    G --> K[stand_down\nStandDown API]
    G --> L[stand_up\nStandUp API]
    G --> M[damp\nDamp API]
    G --> N[recovery_stand\nRecoveryStand API]
    G --> O[sit\nSit API - once]
    G --> P[rise_sit\nRiseSit API - once]
    G --> Q[stop_move\nStopMove API]

    R[HighStateHandler\nstate update] --> S[state.position\nstate.imu_state]
```

### Available Motion Modes

```mermaid
graph LR
    subgraph Basic["Basic Motion"]
        A[normal_stand] 
        B[balance_stand]
        C[velocity_move\n0.3m/s forward + turn]
        D[stand_down]
        E[stand_up]
        F[damp]
        G[recovery_stand]
    end

    subgraph Special["Special Motion (one-shot)"]
        H[sit]
        I[rise_sit]
    end

    subgraph Stop["Stop"]
        J[stop_move = 99]
    end
```

---

## 4. go2_trajectory_follow.cpp — Sine Curve Trajectory Following

매 2ms မှာ 30-point sine curve path ကို တွက်ပြီး `TrajectoryFollow` API သို့ ပေးပို့သည်။

```mermaid
flowchart TD
    A([main]) --> B[ChannelFactory::Init]
    B --> C[SportClient::Init\ntc.SetTimeout 10s]
    C --> D[CreateRecurrentThread\n2ms loop]

    D --> E{control Loop}

    E --> F[count += dt\ntime accumulate]
    F --> G[Loop i = 0 to 29\n30 PathPoints တွက်]

    G --> H["p.x = vx · var\np.y = 0.6·sin(π·vx·var)\np.yaw = derivative of y"]
    H --> I["p.vx = vx\np.vy = π·vx·0.6·cos(π·vx·var)\np.vyaw = derivative of vy"]
    I --> J[path.push_back p]
    J --> K{i < 30?}
    K -- Yes --> G
    K -- No --> L[tc.TrajectoryFollow path]
    L --> M{ret != 0?}
    M -- Error --> N[Print error]
    M -- OK --> E
```

### Sine Curve Path (30 PathPoints)

```mermaid
xychart-beta
    title "Trajectory Path (x vs y)"
    x-axis "x (m)" [0, 0.5, 1.0, 1.5, 2.0, 2.5, 3.0]
    y-axis "y (m)" [-0.6, -0.3, 0, 0.3, 0.6]
    line [0, 0.37, 0.6, 0.37, 0, -0.37, -0.6]
```

> Path formula: `y = 0.6 · sin(π · vx · t)` — Robot သည် sine curve ကို လိုက်သွားသည်

---

## 5. go2_robot_state_client.cpp — Robot Service Manager

`RobotStateClient` ဖြင့် robot ၏ service (sport_mode, ai_sport, ...) များကို on/off ပြောင်းပြီး list ကြည့်သည်။

```mermaid
sequenceDiagram
    participant M as main
    participant RSC as RobotStateClient
    participant Robot as GO2 Robot

    M->>RSC: Init + SetTimeout(10s)
    M->>RSC: GetApiVersion()
    RSC-->>M: clientApiVersion
    M->>RSC: GetServerApiVersion()
    RSC-->>M: serverApiVersion
    M->>M: version check

    M->>RSC: SetReportFreq(3, 30)
    RSC->>Robot: set report frequency
    Robot-->>RSC: ret=0
    RSC-->>M: cost (us)

    Note over M: sleep 5s

    M->>RSC: ServiceSwitch(serviceName, 0) — OFF
    RSC->>Robot: disable sport_mode
    Robot-->>RSC: status
    RSC-->>M: ret + cost

    Note over M: sleep 5s

    M->>RSC: ServiceSwitch(serviceName, 1) — ON
    RSC->>Robot: enable sport_mode
    Robot-->>RSC: status
    RSC-->>M: ret + cost

    Note over M: sleep 5s

    M->>RSC: ServiceList()
    RSC->>Robot: list all services
    Robot-->>RSC: serviceStateList
    RSC-->>M: name, status, protect
```

---

## 6. go2_video_client.cpp — Camera Image Capture

Robot ၏ built-in camera မှ JPEG image ကို 3 seconds တစ်ခါ capture ပြီး file သိမ်းသည်။

```mermaid
flowchart TD
    A([main]) --> B[ChannelFactory::Init]
    B --> C[VideoClient::Init\nSetTimeout 1s]
    C --> D{Infinite Loop\nevery 3 seconds}

    D --> E[GetImageSample image_sample]
    E --> F{ret == 0?}

    F -- Success --> G[time ကို strftime ဖြင့် format\nYYYYMMDDHHMMSS.jpg]
    G --> H[ofstream ဖြင့် file ဖွင့်]
    H --> I[image_file.write\nbinary data သိမ်း]
    I --> J[file close]
    J --> K[Print: Image saved]
    K --> D

    F -- Fail --> D
```

---

## 7. go2_vui_client.cpp — VUI Brightness Control

Robot ၏ head display brightness ကို 0~10 level ဖြင့် ချိန်ညှိသည်။

```mermaid
flowchart TD
    A([main]) --> B{argc < 2?}
    B -- Yes --> C[Usage print + exit]
    B -- No --> D[ChannelFactory::Init\nnetworkInterface]
    D --> E[VuiClient::Init\nSetTimeout 1s]
    E --> F{Infinite Loop}

    F --> G["vc.SetBrightness(level)\nlevel = 0~10 cycle"]
    G --> H[Print result]
    H --> I[sleep 1s]
    I --> J["vc.GetBrightness(value)\nverify value"]
    J --> K[Print value]
    K --> L["++level %= 11\n(0→1→...→10→0)"]
    L --> M[sleep 1s]
    M --> F
```

---

## Architecture Overview — DDS Communication

Program အားလုံး Unitree DDS (Data Distribution Service) ကို သုံးပြီး robot နှင့် communicate လုပ်သည်။

```mermaid
graph TB
    subgraph HL["High-Level Programs"]
        SC[go2_sport_client\nSportClient]
        TF[go2_trajectory_follow\nSportClient]
        RSC[go2_robot_state_client\nRobotStateClient]
    end

    subgraph LL["Low-Level Programs"]
        LS[go2_low_level\nChannelPublisher/Subscriber]
        SE[go2_stand_example\nChannelPublisher/Subscriber]
    end

    subgraph Sensor["Sensor/UI Programs"]
        VC[go2_video_client\nVideoClient]
        VUI[go2_vui_client\nVuiClient]
    end

    subgraph DDS["DDS Topics (rt/)"]
        T1[rt/lowcmd\nLowCmd_]
        T2[rt/lowstate\nLowState_]
        T3[rt/sportmodestate\nSportModeState_]
    end

    subgraph Robot["GO2 Robot"]
        M[Motors x12]
        IMU[IMU Sensor]
        CAM[Camera]
        DISP[Head Display]
        SVC[Services]
    end

    SC -->|API RPC| Robot
    TF -->|API RPC| Robot
    RSC -->|API RPC| SVC

    LS --> T1 --> M
    SE --> T1
    T2 --> LS
    T2 --> SE
    M --> T2
    IMU --> T2

    SC --> T3
    T3 --> SC

    VC -->|API RPC| CAM
    VUI -->|API RPC| DISP
```
