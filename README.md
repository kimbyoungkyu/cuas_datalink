# cuas_datalink

`cuas_datalink`는 ROS 2 Humble C++ 기반의 CUAS 전술 데이터 링크 패키지입니다.

C2(Command & Control) 시스템과 요격체(Interceptor) 사이의 명령, 상태, 텔레메트리, heartbeat 메시지를 ROS 2 멀티 노드 구조로 분리해 처리하며, NATS를 통해 외부 C2 시스템 또는 분산 시스템과 연동할 수 있도록 구성되어 있습니다.

---

## Architecture

```text
External C2 / NATS
    ↓
cuas.c2.command
    ↓
nats_bridge_node
    ↓
/cuas/c2/command
    ↓
datalink_router_node
    ↓
/cuas/interceptor/command
    ↓
Interceptor
```

```text
Interceptor
    ↓
/cuas/interceptor/status
    ↓
datalink_router_node
    ↓
/cuas/c2/status
    ↓
nats_bridge_node
    ↓
cuas.c2.status
    ↓
External C2 / NATS
```

---

## Nodes

### datalink_router_node

ROS 2 내부 라우팅 담당 노드입니다.

```text
/cuas/c2/command            -> /cuas/interceptor/command
/cuas/interceptor/status    -> /cuas/c2/status
```

---

### nats_bridge_node

NATS와 ROS 2 topic을 연결하는 브릿지 노드입니다.

NATS to ROS 2:

```text
cuas.c2.command             -> /cuas/c2/command
cuas.interceptor.status     -> /cuas/interceptor/status
```

ROS 2 to NATS:

```text
/cuas/interceptor/command   -> cuas.interceptor.command
/cuas/c2/status             -> cuas.c2.status
/cuas/datalink/heartbeat    -> cuas.datalink.heartbeat
```

---

### heartbeat_node

데이터링크 상태 확인용 heartbeat를 publish합니다.

```text
/cuas/datalink/heartbeat
```

---

### c2_command_node

테스트용 C2 명령 publish 노드입니다.

---

### interceptor_status_node

테스트용 요격체 상태 publish 노드입니다.

---

## Requirements

- ROS 2 Humble
- C++17
- nats.c client library

Ubuntu 예시:

```bash
sudo apt update
sudo apt install ros-humble-rclcpp ros-humble-std-msgs
```

NATS C client는 환경에 따라 직접 빌드가 필요할 수 있습니다.

```bash
git clone https://github.com/nats-io/nats.c.git
cd nats.c
mkdir build
cd build
cmake ..
make -j
sudo make install
sudo ldconfig
```

---

## Build

```bash
cd ~/ros2_ws/src
git clone <repository_url> cuas_datalink

cd ~/ros2_ws
colcon build --packages-select cuas_datalink

source install/setup.bash
```

NATS 없이 ROS 2 노드만 빌드하려면:

```bash
colcon build --packages-select cuas_datalink --cmake-args -DCUAS_DATALINK_ENABLE_NATS=OFF
```

---

## Run

### ROS 2 internal only

```bash
ros2 launch cuas_datalink cuas_datalink.launch.py
```

### With NATS bridge

```bash
ros2 launch cuas_datalink cuas_datalink_with_nats.launch.py
```

### Demo nodes

```bash
ros2 launch cuas_datalink cuas_datalink_demo.launch.py
```

---

## NATS Test

NATS server 실행:

```bash
nats-server
```

C2 command publish:

```bash
nats pub cuas.c2.command "INTERCEPT target=drone_001"
```

C2 status subscribe:

```bash
nats sub cuas.c2.status
```

Interceptor command subscribe:

```bash
nats sub cuas.interceptor.command
```

Interceptor status publish:

```bash
nats pub cuas.interceptor.status "interceptor_001 status=READY"
```

Heartbeat subscribe:

```bash
nats sub cuas.datalink.heartbeat
```

---

## ROS 2 Topic Check

```bash
ros2 topic list
```

```bash
ros2 topic echo /cuas/c2/command
ros2 topic echo /cuas/interceptor/command
ros2 topic echo /cuas/interceptor/status
ros2 topic echo /cuas/c2/status
ros2 topic echo /cuas/datalink/heartbeat
```

---

## Design Goals

- ROS 2 Humble C++ 기반
- 멀티 노드 구조
- NATS 연동
- C2와 요격체 간 데이터 링크 역할
- 명령, 상태, heartbeat 흐름 분리
- 실제 전술 데이터 링크로 확장 가능한 구조
- 커스텀 메시지 및 외부 통신 프로토콜 연동 가능
- CUAS 시뮬레이터 및 실증 환경 모두 고려
