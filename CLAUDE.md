# CLAUDE.md

This file provides guidance to Claude Code (claude.ai/code) when working with code in this repository.

## Build and Run

```bash
make          # builds ./program (links with -lSDL -lSDL_image)
./program     # requires a display and all .png assets in the working directory
make clean    # not defined — delete .o/.d files manually if needed
```

The binary must be run from the project root so it can find the PNG assets (pieces, board frame, alert images).

## Architecture

This is a C++ chess game with an SDL-based GUI and a built-in AI opponent.

**`grafika.cpp/h`** — SDL wrapper layer. `Obrazovka` is a singleton for the screen. `Obrazek` wraps an SDL surface with position/velocity and `kresli()` to blit. All rendering primitives (`barva`, `obdelnik`, `kruh`, etc.) use a global `pixel` state.

**`figurka.cpp/h`** — Abstract base class `Figurka` for all chess pieces. Key members:
- `barva` — `BILAF` (0) = white, `CERNAF` (1) = black
- `tah` — move counter (used for castling/en-passant eligibility)
- `validniTah()` — piece-specific movement rules (overridden per piece)
- `validniTahSach()` — in `Figurka` base: temporarily makes the move and checks if it leaves own king in check
- `hodnotaFigurky(y,x)` — returns material + positional score for evaluation
- `kdoJsi()` — returns piece-type index 0–11 (white 0–5, black 6–11) used for Zobrist hashing

**Per-piece files** (`pesak`, `vez`, `kun`, `strelec`, `kralovna`, `kral`) each override the above virtuals. `Kral` also handles castling in `pohlase()` (called after a move commits).

**`sachovnice.cpp/h`** — `Sachovnice` owns the 8×8 board (`pozice[row][col]`), the game loop logic, and the AI.
- Row 0 = black back rank, row 7 = white back rank
- `klik(kx, ky)` — translates pixel click to board coords and drives the human move flow
- `pohni()` / `tahniZpet()` — commit/undo a move; update Zobrist hash incrementally; push/pop `tahZpet` (a `std::list<Tah>`)
- `jeSach()` / `jeMat()` — check/checkmate detection
- `robot()` — AI entry point: iterates all legal moves, calls `negaMax()` depth-5 with alpha-beta, picks best (randomly among ties)
- `negaMax()` — negamax with alpha-beta; consults transposition table (`pole`) before searching
- Transposition table: `pole` is heap-allocated at `1<<25` entries (`~32 MB`), indexed by `hash % 2000000` — note the modulus is smaller than the allocation, so most of the table is unused
- Zobrist table: `table[8][8][12]` of `Uint64`, plus `blackToMove` toggle

**`menu.cpp/h`** — Simple button list (`Menu` + `Tlacitko`) rendered as clickable rectangles. Callbacks are stored as `std::function`.

**`main.cpp`** — SDL event loop. Black always plays as the AI (`robot()` called when `barvicka == CERNAF`). Buttons for reset, fullscreen toggle, and undo (`tahniZpetuser()` undoes two half-moves).

## Key Invariants

- `Tah::vyhozena` stores a raw pointer to the captured piece — `tahniZpet()` restores it without deleting; the piece was never freed during `pohni()`.
- Pawn promotion always promotes to `Kralovna`; the original pawn is deleted and a new queen allocated.
- `validniTahSach()` does a temporary board mutation (not a full `pohni`/`tahniZpet` cycle) — it does **not** update the Zobrist hash, so it must not be called inside the AI search path where the hash matters.
