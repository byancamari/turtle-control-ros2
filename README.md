# Turtle Control — ROS 2

Projeto desenvolvido para o **2º Processo Seletivo Antecipado do RobôCIn**, na categoria Humanoid Soccer League. A aplicação possui dois nós ROS 2 em C++ que se comunicam por um tópico para controlar uma tartaruga no `turtlesim`.

## Objetivo

O sistema recebe os comandos `right`, `left`, `up` e `down`. Cada comando desloca a posição lógica da tartaruga em uma unidade no plano cartesiano e movimenta a tartaruga exibida no `turtlesim`.

A posição lógica começa em `(0, 0)`:

| Comando | Alteração |
|---|---|
| `right` | `x = x + 1` |
| `left` | `x = x - 1` |
| `up` | `y = y + 1` |
| `down` | `y = y - 1` |

## Arquitetura

O pacote possui dois nós próprios:

- `send_msg`: recebe um comando digitado no terminal e publica uma mensagem no tópico `/move_command`;
- `turtle`: recebe o comando, atualiza a posição lógica e controla a movimentação da tartaruga no simulador.

O nó `turtlesim` fornece a interface gráfica e informa a posição física da tartaruga.

```text
send_msg
    |
    | /move_command (std_msgs/msg/String)
    v
turtle
    | publica: /turtle1/cmd_vel
    | recebe:  /turtle1/pose
    v
turtlesim
```

## Tecnologias e dependências

- Ubuntu 24.04 LTS;
- ROS 2 Jazzy Jalisco;
- C++;
- `rclcpp`;
- `std_msgs`;
- `geometry_msgs`;
- `turtlesim`;
- `colcon`.

## Estrutura do pacote

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

## Como preparar o workspace

Crie um workspace ROS 2 e clone este repositório dentro da pasta `src`:

```bash
mkdir -p ~/projeto_trainee_ws/src
cd ~/projeto_trainee_ws/src
git clone URL_DO_REPOSITORIO turtle_control
```

Substitua `URL_DO_REPOSITORIO` pela URL deste repositório.

Instale as dependências necessárias:

```bash
cd ~/projeto_trainee_ws
rosdep install --from-paths src --ignore-src -r -y
```

Compile o pacote:

```bash
colcon build --packages-select turtle_control
```

Carregue o workspace:

```bash
source install/setup.bash
```

Esse comando deve ser executado em cada terminal novo usado para iniciar os nós do pacote.

## Como executar

São necessários três terminais.

### Terminal 1 — simulador

```bash
ros2 run turtlesim turtlesim_node
```

### Terminal 2 — nó receptor e controlador

```bash
cd ~/projeto_trainee_ws
source install/setup.bash
ros2 run turtle_control turtle
```

### Terminal 3 — nó que envia os comandos

```bash
cd ~/projeto_trainee_ws
source install/setup.bash
ros2 run turtle_control send_msg
```

No terceiro terminal, digite um dos comandos:

```text
right
left
up
down
```

Espere a mensagem `Movimento concluído` no terminal do nó `turtle` antes de enviar o próximo comando.

Digite `exit` para encerrar o nó `send_msg`.

## Como verificar os requisitos

Com todos os nós em execução, abra outro terminal.

### Verificar os nós

```bash
ros2 node list
```

Resultado esperado:

```text
/send_msg
/turtle
/turtlesim
```

### Verificar os tópicos

```bash
ros2 topic list
```

Entre os tópicos exibidos devem estar:

```text
/move_command
/turtle1/cmd_vel
/turtle1/pose
```

### Observar os comandos publicados

```bash
ros2 topic echo /move_command
```

Ao digitar `right` no nó `send_msg`, por exemplo, deve aparecer:

```yaml
data: right
---
```

Também é possível observar os comandos de velocidade enviados ao simulador:

```bash
ros2 topic echo /turtle1/cmd_vel
```

## Funcionamento do controle

O nó `turtle` mantém duas referências de posição:

- **Posição lógica:** começa em `(0, 0)` e é atualizada em uma unidade a cada comando;
- **Posição física:** recebida pelo tópico `/turtle1/pose` e usada para controlar o movimento no simulador.

Ao receber um comando, o controlador:

1. Define a nova posição lógica e o destino físico;
2. Gira a tartaruga para a direção correspondente;
3. Publica velocidade linear em `/turtle1/cmd_vel`;
4. Compara continuamente a posição atual com o destino;
5. Publica velocidade zero quando o deslocamento termina.

Enquanto um movimento está sendo executado, novos comandos são recusados para evitar sobreposição.

## Cor opcional do simulador

Com o `turtlesim` em execução, o fundo pode ser alterado para a cor `#469c57`(verde da equipe):

```bash
ros2 param set /turtlesim background_r 70
ros2 param set /turtlesim background_g 156
ros2 param set /turtlesim background_b 87
ros2 service call /clear std_srvs/srv/Empty
```

## Principais dificuldades e aprendizados

As principais dificuldades foram compreender a organização de um workspace ROS 2, configurar corretamente os executáveis no `CMakeLists.txt` e converter comandos cartesianos em rotação e deslocamento físico no `turtlesim`.

O desenvolvimento permitiu praticar:

- Criação e compilação de pacotes ROS 2 em C++;
- Comunicação assíncrona com publisher e subscriber;
- Inspeção de nós e tópicos pela linha de comando;
- Uso de mensagens `String`, `Twist` e `Pose`;
- Controle de orientação, distância e estado de movimento;
- Versionamento do projeto com Git e GitHub.

## Autora

**Byanca Maria da Silva**
