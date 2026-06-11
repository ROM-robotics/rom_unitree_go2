# Unitree Go2 Programming & Control Methodologies

Unitree Go2 Quadruped Robot ကို Programming ရေးသားပြီး ထိန်းချုပ်ခိုင်းစေနိုင်သည့် အဓိကနည်းလမ်း (၄) ခု၏ ဖွဲ့စည်းပုံနှင့် အသုံးချမှုများ ဖြစ်ပါသည်။

---

## 1. High-Level Programming (အပေါ်ယံ အမိန့်ပေးစနစ်)
ရိုဘော့၏ ခန္ဓာကိုယ်ဟန်ချက်ထိန်းခြင်းနှင့် ခြေလှမ်းလှုပ်ရှားမှုများကို Unitree ၏ မူရင်း Motion Controller က အလိုအလျောက် တာဝန်ယူပေးပြီး၊ အပေါ်မှနေ၍ အခြေခံလှုပ်ရှားမှု အမိန့်များကိုသာ ပေးပို့သည့် နည်းလမ်းဖြစ်ပါသည်။

* **အသုံးပြုသည့် ဘာသာစကား/SDK:** `Python` သို့မဟုတ် `C++` (Unitree SDK2 မှတစ်ဆင့်)
* **ထိန်းချုပ်နိုင်သည့်အရာများ:** အရှေ့/အနောက်/ဘယ်/ညာ သွားနှုန်း ($v_x, v_y$)၊ ကိုယ်ထည်လှည့်နှုန်း ($\omega_z$)၊ ထိုင်ခြင်း၊ ထခြင်း၊ လှဲနေရာမှ ပြန်ထခြင်း နှင့် မူရင်းပါဝင်ပြီးသား Dynamic Gaits များ။
* **သင့်တော်သည့် Project များ:** AI Computer Vision (Object Tracking)၊ Voice Control နှင့် အခြေခံ Autonomous Navigation Application များ။

---

## 2. Low-Level Programming (အောက်ခြေ မော်တာထိန်းချုပ်စနစ်)
ရိုဘော့၏ မူရင်း ဟန်ချက်ထိန်းစနစ်ကို ပိတ်ကာ၊ ခြေဆစ်မော်တာ (Joint Motors) ၁၂ ခုလုံး၏ ရုန်းအားနှင့် တုံ့ပြန်မှုများကို သင်္ချာနည်းအရ ကိုယ်တိုင်တိုက်ရိုက် တွက်ချက်ထိန်းချုပ်သည့် နည်းလမ်းဖြစ်ပါသည်။

* **အသုံးပြုသည့် ဘာသာစကား/Middleware:** `C++` နှင့် `DDS` (Data Distribution Service)
* **ထိန်းချုပ်နိုင်သည့်အရာများ:** မော်တာတစ်ခုချင်းစီ၏ ပစ်မှတ်ထောင့်စွန်း (`q_des`)၊ လည်ပတ်နှုန်း (`dq_des`) နှင့် ရုန်းအား (`tau_des`) တို့ကို 1ms to 2ms (500Hz - 1000Hz) Loop rate ဖြင့် တိုက်ရိုက် Feed လုပ်ခြင်း။
* **သင့်တော်သည့် Project များ:** Custom Gait Generation (ခြေလှမ်းပုံစံအသစ် ထွင်ခြင်း)၊ ကျွမ်းထိုးခြင်း (Backflip) နှင့် အဆင့်မြင့် Dynamics & Control သုတေသနလုပ်ငန်းများ။

---

## 3. ROS 2 (Robot Operating System) Integration
Unitree SDK2 ကို ROS 2 Node များအဖြစ် ပြောင်းလဲကာ စက်ရုပ်လောက၏ Standard Open-source Libraries များနှင့် ပေါင်းစပ်အသုံးပြုသည့် နည်းလမ်းဖြစ်ပါသည်။

* **အသုံးပြုသည့် Platform:** `ROS 2 Humble / Iron` (DDS Communication)
* **ချိတ်ဆက်နိုင်သည့် နည်းပညာများ:** 2D/3D LiDAR၊ Depth Camera၊ `SLAM` (မြေပုံဆွဲခြင်း) နှင့် `Nav2` (Navigation Stack) တည်းဟူသော အတားအဆီးရှောင်ကွင်းခြင်းစနစ်များ။
* **သင့်တော်သည့် Project များ:** Warehouse AMRs ( waiter robots / delivery robots)၊ စက်ရုံတွင်း ကင်းလှည့်ခြင်း (Autonomous Patrol) နှင့် Multi-robot Fleet Management များ။

---

## 4. Simulation & Sim-to-Real (DDRL Learning)
NVIDIA Isaac Sim ကဲ့သို့ ဓာတ်ခွဲခန်း virtual ကမ္ဘာထဲတွင် Deep Reinforcement Learning (DRL) စနစ်သုံးကာ ရိုဘော့ကို အကြိမ်ပေါင်း သန်း/ဘီလီယံချီ လေ့ကျင့်ပေးပြီးမှ ပြင်ပရိုဘော့အစစ် (Real Robot) ဆီသို့ AI Policy ပြောင်းရွှေ့အသုံးချသည့် နည်းလမ်းဖြစ်ပါသည်။

* **အသုံးပြုသည့် Tools:** `NVIDIA Isaac Sim` / `Isaac Lab`၊ `PyTorch` (RL Frameworks ဖြစ်သော PPO, GRPO စသည်)
* **အဓိက နည်းပညာ:** **Domain Randomization** နှင့် **Actuator Network Modeling** (Simulation နှင့် ပြင်ပကမ္ဘာကြားရှိ Reality Gap ကို ကျော်ဖြတ်ရန်)။
* **သင့်တော်သည့် Project များ:** ကြမ်းတမ်းလွန်းသော မြေပြင်များ (ရွှံ့နွံ၊ ကျောက်ခဲတောင်ကုန်း) ပေါ်တွင် သွားလာခြင်း၊ လှေကားတက်ခြင်း၊ ပြင်ပမှ ရိုက်နှက်တွန်းထိုးသော်လည်း လဲမကျဘဲ သူ့အလိုလို ဟန်ချက်ပြန်ထိန်းနိုင်သည့် Physical AI သုတေသနများ။

---

## Summary Comparison Table

| နည်းလမ်း (Methodology) | ခက်ခဲမှုအဆင့် | Loop Rate | တုံ့ပြန်မှုနှုန်း (Latency) | အဓိက အားသာချက် |
| :--- | :---: | :---: | :---: | :--- |
| **High-Level Control** | Easy | ~20Hz - 50Hz | High | ရေးသားရလွယ်ကူခြင်း၊ ဘေးကင်းခြင်း |
| **Low-Level Control** | Hard | 500Hz - 1000Hz | Ultra-Low | မော်တာများကို စိတ်ကြိုက်ထိန်းချုပ်နိုင်ခြင်း |
| **ROS 2 Navigation** | Medium | ~50Hz - 100Hz | Medium | Sensor များနှင့် Open-source library ပေါများခြင်း |
| **Sim-to-Real (RL)** | Expert | ~50Hz - 200Hz | Low-Medium | သင်္ချာနည်းဖြင့် တွက်ရန်ခက်သော ပတ်ဝန်းကျင်ကို ကျော်ဖြတ်နိုင်ခြင်း |