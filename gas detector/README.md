# gas detector（Geant4 气体探测器模拟）

本目录提供一套基于 Geant4 的模拟程序，用于模拟 **100 MeV 入射**离子在气体探测器中的能量沉积。程序参数通过 YAML 文件统一配置，可设置：

- 单粒子或多粒子混合发射（如 `60Fe` + `60Ni`）
- 各粒子电荷态（`charge_state`）
- 各粒子发射比例（`ratio`）
- 探测器气体种类与气压
- 每一轮发射粒子个数与轮数

## 探测器规格（按需求实现）

- 四个阳极板：长度分别 `100 mm`、`80 mm`、`60 mm`、`130 mm`，对应信号 `E1~E4`
- 一个阴极板：长度 `373 mm`，对应总信号 `Etotal`
- 阴、阳极间设置一层栅极（简化建模）
- 气体：默认丙烷 `G4_PROPANE`，默认气压 `38.7 mbar`
- 入射窗尺寸：`65 mm x 40 mm`
- 入射窗材料：`Mylar`，厚度 `2.5 um`

## 编译

前提：已安装 Geant4 与 yaml-cpp。

```bash
mkdir -p build
cd build
cmake ../
make -j
```

## 运行

```bash
./gas_detector ../config/sim.yaml
# 或
./gas_detector ../config/sim_ni60.yaml
```

输出：ROOT 文件（默认由 `output_root` 指定），包含 `gas` ntuple，每个事件写入 `E1,E2,E3,E4,Etotal`（单位 MeV）。

## YAML 参数说明

### 1）混合束写法（推荐）

> `ratio` 仅用于相对比例，不要求和为 1。程序会自动归一化。

```yaml
particles:
  - name: "Fe60"
    z: 26
    a: 60
    charge_state: 20
    kinetic_energy_mev: 100.0
    ratio: 0.7
  - name: "Ni60"
    z: 28
    a: 60
    charge_state: 22
    kinetic_energy_mev: 100.0
    ratio: 0.3

detector:
  gas_material: "G4_PROPANE"
  gas_pressure_mbar: 38.7

run:
  particles_per_round: 1000
  rounds: 5
  output_root: "events_mix_fe60_ni60.root"
```

### 2）单粒子写法（兼容）

```yaml
particle:
  name: "Ni60"
  z: 28
  a: 60
  charge_state: 22
  kinetic_energy_mev: 100.0
```

总事件数 = `particles_per_round * rounds`。
