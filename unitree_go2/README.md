# Development image တည်ဆောက်ရန် (build context = repo root)
### Repo Root ကို cd နှင့် သွားပါ။ 
```bash
docker build \
    -f unitree_go2/Dockerfile \
    -t romrobotics/unitree:go2_dev .
```