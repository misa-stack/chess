# ♟️ Chess

A graphical chess game written in C++ using SDL, featuring a single-player mode against an AI opponent powered by the Negamax algorithm.

---

## Features

- **Graphical board** rendered with SDL — custom piece sprites for both colors
- **Human vs. AI** — play as White against an automated Black opponent
- **Move validation** per piece type, including rook slide detection (`validniTahVez`)
- **Check detection** — board evaluates whether the king is in check after each move
- **Undo (tah zpět)** — step back through the move history at any time
- **Board reset** — restart the game without relaunching
- **Fullscreen toggle** — switch between windowed and fullscreen at runtime
- **Score display** — live board evaluation value shown in the sidebar
- **Negamax AI** — recursive minimax with negamax formulation for Black's moves

---

## Architecture

```
main.cpp           – SDL event loop, UI buttons, game entry point
sachovnice.h/.cpp  – Board state, move logic, AI (Negamax), check/threat detection
figurka.h/.cpp     – Base piece class
kral.h/.cpp        – King (král)
kralovna.h/.cpp    – Queen (královna)
vez.h/.cpp         – Rook (věž)
strelec.h/.cpp     – Bishop (střelec)
kun.h/.cpp         – Knight (kůň)
pesak.h/.cpp       – Pawn (pěšák)
grafika.h/.cpp     – SDL rendering helpers (Obrazovka singleton, Obrazek)
menu.h/.cpp        – Button/menu system (Tlacitko, Menu)
```

### Key Classes

| Class | Responsibility |
|---|---|
| `Sachovnice` | 8×8 board, move history (`std::list<Tah>`), AI call, evaluation |
| `Figurka` | Abstract base for all pieces; holds color, position, sprite |
| `Obrazovka` | SDL window singleton — init, clear, flip |
| `Menu` / `Tlacitko` | Clickable sidebar buttons with lambda callbacks |

### AI

Black's moves are selected via `negaMax(hloubka, barva)` — a standard recursive Negamax search. Board state is scored by `hodnotaSachovnice()` using piece-value heuristics. The search depth (`hloubka`) determines difficulty.

---

## Dependencies

| Library | Purpose |
|---|---|
| `SDL 1.2` | Window, rendering, input |
| `SDL_image` | Loading PNG piece sprites |
| Standard C++17 | `<list>`, `<functional>` (lambdas) |

> **Note:** The project uses SDL 1.2 (legacy), not SDL2. Make sure you have the correct version installed.

### Arch Linux

```bash
sudo pacman -S sdl sdl_image
```

### Debian/Ubuntu

```bash
sudo apt install libsdl1.2-dev libsdl-image1.2-dev
```

---

## Build

```bash
make
```

The `Makefile` compiles all `.cpp` files and links against SDL. The resulting binary is `program`.

```bash
./program
```

The window opens at **1920×1080**. Use the sidebar buttons to control the game.

---

## Controls

| Action | How |
|---|---|
| Select & move a piece | Click on it, then click the target square |
| Undo last move | Click **tah zpět** |
| Reset board | Click **reset figurek** |
| Toggle fullscreen | Click **fullscreen** |
| Quit | `Escape` |

---

## Project Structure (notable files)

```
chess/
├── main.cpp
├── sachovnice.h / sachovnice.cpp
├── figurka.h / figurka.cpp
├── kral.h / kral.cpp
├── kralovna.h / kralovna.cpp
├── vez.h / vez.cpp
├── strelec.h / strelec.cpp
├── kun.h / kun.cpp
├── pesak.h / pesak.cpp
├── grafika.h / grafika.cpp
├── menu.h / menu.cpp
├── Makefile
└── *.png          # Piece sprites
```

Build artifacts (`.o`, `.d`, `.cflags`, etc.) are generated in the project root — these can be safely ignored or added to `.gitignore`.

---

## Known Limitations

- SDL 1.2 only; not portable to SDL2 without refactoring `Obrazovka`
- AI does not implement castling, en passant, or pawn promotion
- No draw/checkmate end-screen — the game loops indefinitely
- Build artifacts are committed to the repo (`.o`, `.d` files)

---

## License

No license specified. All rights reserved by the author.
