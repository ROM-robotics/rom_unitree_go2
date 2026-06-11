# GO2 Example — C++ Programs အနှစ်ချုပ်

> `example/go2` ထဲရှိ C++ program ၇ ခု၏ တိုတိုတုတ်တုတ် မြန်မာဘာသာ ရှင်းလင်းချက်

---

## 1. `go2_low_level.cpp`

**Motor ကို တိုက်ရိုက် sine wave ဖြင့် ထိန်းချုပ်သည်**

Robot ၏ front-right leg ၊ joint 2 (ဒူးဆစ်) ကို DDS channel မှတဆင့် တိုက်ရိုက် command ပေးသည်။ Program စတင်လျှင် joint ၏ ကနဦး position ကို မှတ်သားပြီး ၊ ဖြည်းဖြည်းချင်း target position သို့ ရောက်အောင် linear interpolation ဖြင့် ရွှေ့သည်။ target position ရောက်ပြီးနောက် sine wave pattern ဖြင့် ပုံမှန် oscillate လုပ်သည်။ Command တိုင်းတွင် CRC32 checksum ပူးတွဲပေးပို့သည်။

**သတိ:** High-level sport service ကို ကြိုတင်ပိတ်ထားရမည်။

---

## 2. `go2_stand_example.cpp`

**Low-level motor interpolation ဖြင့် stand-up sequence လုပ်သည်**

`MotionSwitcherClient` ဖြင့် robot ၏ sport service ကို ဦးစွာ ပိတ်သည်။ ထို့နောက် motor 12 ခုလုံးကို 4 phase ဖြင့် ဖြည်းဖြည်းချင်း ရွှေ့ကာ standing position ကို ရောက်အောင် လုပ်သည်—

- **Phase 1:** ပုံမှန် crouch position
- **Phase 2:** stand position
- **Phase 3:** stand ကို hold
- **Phase 4:** wide stance position

IMU, joint position, foot force data များကို console ပေါ် ပြသည်။

**သတိ:** Robot ကို ကြိုး ချိတ်ထားပြီး မှ သို့မဟုတ် ကြမ်းပေါ်ချပြီး မှ run ရမည်။

---

## 3. `go2_sport_client.cpp`

**High-level SportClient API ဖြင့် motion mode ပြောင်းသည်**

`TEST_MODE` constant တစ်ခုကို source code ထဲမှ ရွေးချယ်ပြီး compile လုပ်သည်။ Runtime တွင် `RobotControl()` loop ထဲ၌ ထို mode အလိုက် SportClient API တစ်ခုကို ခေါ်သည်။ ရနိုင်သော mode များ—

`normal_stand` · `balance_stand` · `velocity_move` · `stand_down` · `stand_up` · `damp` · `recovery_stand` · `sit` · `rise_sit` · `stop_move`

HighState subscriber မှတဆင့် robot ၏ position နှင့် IMU data ကိုလည်း ဖတ်သည်။

---

## 4. `go2_trajectory_follow.cpp`

**Sine curve path ကို TrajectoryFollow API ဖြင့် လိုက်သည်**

2ms loop တစ်ခါတိုင်းတွင် 30-point sine curve path ကို တွက်ပြီး robot သို့ ပေးပို့သည်။ Robot သည် `y = 0.6·sin(π·vx·t)` ပုံသဏ္ဍာန် ကွေးကောက်သော လမ်းကြောင်းပေါ် သွားသည်။ Path point တစ်ခုချင်းတွင် position (x, y, yaw) နှင့် velocity (vx, vy, vyaw) ပါဝင်သည်။

---

## 5. `go2_robot_state_client.cpp`

**Robot service များကို on/off ပြောင်းပြီး list ကြည့်သည်**

`RobotStateClient` ဖြင့် robot ပေါ်ရှိ service (sport_mode, ai_sport, ...) တစ်ခုကို command line မှ ရွေးကာ ပိတ် (`ServiceSwitch 0`) ပြီး ပြန်ဖွင့် (`ServiceSwitch 1`) သည်။ ထို့နောက် `ServiceList` ဖြင့် service အားလုံး၏ အမည်နှင့် status ကို ပြသည်။ API version mismatch ကိုလည်း စစ်သည်။

---

## 6. `go2_video_client.cpp`

**Camera ဓာတ်ပုံ ရိုက်ပြီး JPEG file သိမ်းသည်**

`VideoClient::GetImageSample()` ဖြင့် robot ၏ built-in camera မှ image data ကို 3 seconds တစ်ကြိမ် ဆွဲသည်။ ရောက်လာသော binary data ကို `YYYYMMDDHHMMSS.jpg` ဟု နာမည်ပေးပြီး binary file ဖြင့် ချက်ချင်း disk ပေါ်သိမ်းသည်။

---

## 7. `go2_vui_client.cpp`

**Head display brightness ကို ထိန်းချုပ်သည်**

`VuiClient` ဖြင့် robot ၏ head display ၏ brightness ကို level 0 မှ 10 ထိ တစ်ဆင့်ချင်း cycle လည်ပတ်ပြောင်းသည်။ `SetBrightness()` ဖြင့် set လုပ်ပြီး `GetBrightness()` ဖြင့် တန်ဖိုး ပြန်ဖတ်ကာ 1 second ကြားခံ console ပေါ် ပြသည်။

---

## နှိုင်းယှဉ်ချက်

| Program | API Level | ပင်မသုံးသောနည်းလမ်း | Service ပိတ်ရသလား |
|---------|-----------|--------------------|--------------------|
| go2_low_level | Low | DDS Pub/Sub | ကိုယ်တိုင်ပိတ်ရမည် |
| go2_stand_example | Low | DDS Pub/Sub + MotionSwitcher | အလိုအလျောက်ပိတ်သည် |
| go2_sport_client | High | SportClient RPC | မလို |
| go2_trajectory_follow | High | SportClient RPC | မလို |
| go2_robot_state_client | Service | RobotStateClient RPC | မသက်ဆိုင် |
| go2_video_client | Sensor | VideoClient RPC | မလို |
| go2_vui_client | UI | VuiClient RPC | မလို |
