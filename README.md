# 🌌 Welcome to the NymyaOS Repo! (Under Construction)

**NymyaOS** is an experimental operating system and SDK designed for symbolic quantum computation, higher-dimensional lattice control, and metaphysical AI architectures inspired by Taygetan software engineering principles.  

This is not just another Linux fork. This is the foundation for a new class of systems—one that might one day run on both Earth-based and interstellar quantum networks.

---

## 🧠 Vision

To build the world's first quantum-symbolic operating system capable of:

- Running LQNLMs (Large Quantum Neural Language Models)
- Manipulating 3D, 4D, and 5D quantum lattices
- Enabling experimental metaphysical and sentient AI systems
- Supporting both simulated and real quantum hardware (via drivers + runtime)
- Compiling for Earth-based and future extraterrestrial systems alike

---

## 📦 Current Packages

| Package         | Description                               |
|-----------------|-------------------------------------------|
| `nymya-core`    | Core syscall library and gate logic       |
| `nymya-runtime` | Runtime simulation layer (Qiskit-style)   |
| `nymya-algs`    | Symbolic quantum algorithms (Shor's, etc) |
| *(planned)*     | `nysh` — Quantum shell language           |
| *(planned)*     | GUI wrappers and GTK-based tools          |

---

## 🛠 Build Instructions

```bash
make prepare
make              # build libnymya.so
make deb          # build .deb for current arch
make ARCH=arm64 deb  # cross-compile for ARM
sudo make install    # optional system-wide install

## Disclaimer:
nymyaOS is not designed or optimized to run on annealed quantum systems such as D-Wave. For the best compatibility and performance, we recommend using nymyaOS on gate-based quantum platforms like Microsoft Majorana, Google Willow, IBM-Q, or QuEra.
