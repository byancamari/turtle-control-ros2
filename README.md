# Turtle Control — ROS 2

## Objetivo

Sistema em ROS 2 que recebe os comandos `right`, `left`, `up` e `down`. Cada comando atualiza a posição lógica da tartaruga em uma unidade e movimenta a tartaruga no `turtlesim`.

A posição lógica começa em `(0, 0)`:

| Comando | Alteração |
|---|---|
| `right` | `x = x + 1` |
| `left` | `x = x - 1` |
| `up` | `y = y + 1` |
| `down` | `y = y - 1` |

## Arquitetura

O projeto possui dois nós implementados em C++:

- `send_msg`: lê o comando e publica no tópico `/move_command`;
- `turtle`: recebe o comando, atualiza a posição lógica e controla o movimento.

O `turtlesim_node` é o terceiro nó em execução. Ele fornece a interface gráfica e realiza o movimento visual.

```text
send_msg
    |
    | /move_command (std_msgs/msg/String)
    v
turtle
    |
    | publica: /turtle1/cmd_vel
    | recebe:  /turtle1/pose
    v
turtlesim
```

## Dependências

- Ubuntu 24.04 LTS;
- ROS 2 Jazzy;
- C++;
- `rclcpp`;
- `std_msgs`;
- `geometry_msgs`;
- `turtlesim`;
- `colcon`.

## Estrutura

```text
turtle_control/
├── src/
│   ├── send_msg.cpp
│   └── turtle.cpp
├── CMakeLists.txt
├── package.xml
├── LICENSE
└── README.md
```

## Instalação

Crie o workspace e clone o repositório:

```bash
mkdir -p ~/projeto_trainee_ws/src
cd ~/projeto_trainee_ws/src
git clone https://github.com/byancamari/turtle-control-ros2 turtle_control
```

Instale as dependências e compile:

```bash
cd ~/projeto_trainee_ws
source /opt/ros/jazzy/setup.bash
rosdep install --from-paths src --ignore-src -r -y
colcon build --packages-select turtle_control
source install/setup.bash
```

## Execução

São necessários três terminais.

### Terminal 1 — turtlesim

```bash
source /opt/ros/jazzy/setup.bash
ros2 run turtlesim turtlesim_node
```

### Terminal 2 — nó turtle

```bash
cd ~/projeto_trainee_ws
source /opt/ros/jazzy/setup.bash
source install/setup.bash
ros2 run turtle_control turtle
```

### Terminal 3 — nó send_msg

```bash
cd ~/projeto_trainee_ws
source /opt/ros/jazzy/setup.bash
source install/setup.bash
ros2 run turtle_control send_msg
```

Digite um comando por vez:

```text
right
left
up
down
```

Espere a mensagem `Movimento concluído` antes de enviar o próximo comando. Digite `exit` para encerrar o `send_msg`.

## Verificação

Com os três nós em execução, abra outro terminal:

```bash
source /opt/ros/jazzy/setup.bash
source ~/projeto_trainee_ws/install/setup.bash
```

Verifique os nós:

```bash
ros2 node list
```

Resultado esperado:

```text
/send_msg
/turtle
/turtlesim
```

Verifique os tópicos:

```bash
ros2 topic list
```

Entre os tópicos devem aparecer:

```text
/move_command
/turtle1/cmd_vel
/turtle1/pose
```

Observe a comunicação entre os dois nós implementados:

```bash
ros2 topic echo /move_command
```

Ao enviar `right`, por exemplo, deverá aparecer:

```yaml
data: right
---
```

## Funcionamento

O nó `turtle` mantém uma posição lógica iniciada em `(0, 0)` e recebe a posição física pelo tópico `/turtle1/pose`.

Para cada comando, ele:

1. Atualiza a posição lógica;
2. Gira para a direção correta;
3. Avança uma unidade;
4. Para ao chegar ao destino.

O controle utiliza os estados:

```text
IDLE → ROTATING → MOVING → IDLE
```

## Cor opcional

Para usar o fundo `#469c57`:

```bash
source /opt/ros/jazzy/setup.bash
ros2 param set /turtlesim background_r 70
ros2 param set /turtlesim background_g 156
ros2 param set /turtlesim background_b 87
ros2 service call /clear std_srvs/srv/Empty
```

